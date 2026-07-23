#include "ppu.hpp"
#include "dma_controller.hpp"
#include <iostream>

// Register handling defined in header
// This just contains code related to the rendering itself

constexpr int DOTS_PER_LINE = 1364;
constexpr Address HVBJOY = 0x4212;
constexpr CycleCount PPU_CYCLE = 4;

void PPU::window_mask(std::array<Pixel, 512>& scanline, bool window1_enabled, bool window2_enabled, bool window1_inverted, bool window2_inverted, Byte mask_logic, bool colour_math) {
	int x = 0;
	for (int dot = 0; dot < 512; dot += 2) {
		bool window1_mask = window1_dots[dot];
		bool window2_mask = window2_dots[dot];
		if (window1_inverted) { window1_mask = !window1_mask; }
		if (window2_inverted) { window2_mask = !window2_mask; }
				
		bool mask = false;
		if (window1_enabled && !window2_enabled) { mask = window1_mask; }
		if (!window1_enabled && window2_enabled) { mask = window2_mask; }
		if (window1_enabled && window2_enabled) {
			switch (mask_logic) {
			case 0: mask =   window1_mask || window2_mask;  break;
			case 1: mask =   window1_mask && window2_mask;  break;
			case 2: mask =   window1_mask ^  window2_mask;  break;
			case 3: mask = !(window1_mask ^  window2_mask); break;
			}
		}

		if (mask) {
			scanline[dot].transparent = true;
			scanline[dot + 1].transparent = true;
		}
	}
}


Pixel PPU::fetch_bg_pixel(BG& bg, uint16_t xcounter) {
	// hcounter is not being used as the scanline is calculated at the start of hblank
	// using temporary 'xcounter' instead

	int bg_x, bg_y;

	if (bg.mosaic) {
		int mosaic_x = xcounter - (xcounter % (mosaic_size + 1));
		int mosaic_y = vcounter - (vcounter % (mosaic_size + 1));

		bg_x = (mosaic_x + bg.bghofs) & 0x3FF;
		bg_y = (mosaic_y + bg.bgvofs) & 0x3FF;
	} else {
		bg_x = (xcounter + bg.bghofs) & 0x3FF;
		bg_y = (vcounter + bg.bgvofs) & 0x3FF;
	}
	int tile_x = bg_x >> 3;
	int tile_y = bg_y >> 3;

	int pixel_x = bg_x & 7;
	int pixel_y = bg_y & 7;

	int map_width_tiles  = bg.horizontal_tilemap_count ? 64 : 32;
	int map_height_tiles = bg.vertical_tilemap_count   ? 64 : 32;

	tile_x = tile_x % map_width_tiles;
	tile_y = tile_y % map_height_tiles;

	int screen_x = tile_x >> 5;
	int screen_y = tile_y >> 5;
	int screen = 0;

	if (bg.horizontal_tilemap_count) {
		screen = screen | screen_x;
	}

	if (bg.vertical_tilemap_count) {
		screen = screen | (screen_y << 1);
	}

	int local_x = tile_x & 31;
	int local_y = tile_y & 31;

	Word tilemap_address = (bg.tilemap_vram_address + (screen * 0x400) + (local_y * 32) + local_x) & 0x7FFF;
	Word entry = vram.data[tilemap_address];

	int tile_number = entry & 0x3FF;
	bool hflip = entry & 0x4000;
	bool vflip = entry & 0x8000;

	int palette = (entry >> 10) & 0x7;
	int priority = (entry >> 13) & 0x1;

	if (bg.character_size) {
	    int sub_x = (bg_x >> 3) & 1;
	    int sub_y = (bg_y >> 3) & 1;

	    if (hflip) { sub_x ^= 1; }
	    if (vflip) { sub_y ^= 1; }

	    tile_number += sub_x;
	    tile_number += sub_y * 16;
	}

	if (hflip) {
	    pixel_x = pixel_x ^ 7;
	}
	if (vflip) {
	    pixel_y = pixel_y ^ 7;
	}

	Word tile_address = (bg.word_address + tile_number * (4 * bg.bpp)) & 0x7FFF;

	Word plane01, plane23, plane45, plane67;

	if (bg.bpp >= 2) { plane01 = vram.data[(tile_address +      pixel_y) & 0x7FFF]; }
	if (bg.bpp >= 4) { plane23 = vram.data[(tile_address + 8  + pixel_y) & 0x7FFF]; }
	if (bg.bpp == 8) { plane45 = vram.data[(tile_address + 16 + pixel_y) & 0x7FFF]; }
	if (bg.bpp == 8) { plane67 = vram.data[(tile_address + 24 + pixel_y) & 0x7FFF]; }

	Byte p0, p1, p2, p3, p4, p5, p6, p7;

	int bit = 7 - pixel_x;
	p0 = get_lo(plane01);
	p1 = get_hi(plane01);
	if (bg.bpp >= 4) {
		p2 = get_lo(plane23);
		p3 = get_hi(plane23);
		if (bg.bpp == 8) {
			p4 = get_lo(plane45);
			p5 = get_hi(plane45);
			p6 = get_lo(plane67);
			p7 = get_hi(plane67);
		}
	}
	
	Byte colour = 0x00;
	if (bg.bpp == 2) {
		colour = ((p0 >> bit) & 1) | (((p1 >> bit) & 1) << 1);
	} else if (bg.bpp == 4) {
		colour = ((p0 >> bit) & 1)       | (((p1 >> bit) & 1) << 1) |
				(((p2 >> bit) & 1) << 2) | (((p3 >> bit) & 1) << 3);
	} else if (bg.bpp == 8) { // Direct colour not considered yet!
		colour = ((p0 >> bit) & 1)       | (((p1 >> bit) & 1) << 1) |
				(((p2 >> bit) & 1) << 2) | (((p3 >> bit) & 1) << 3) |
				(((p4 >> bit) & 1) << 4) | (((p5 >> bit) & 1) << 5) |
				(((p6 >> bit) & 1) << 6) | (((p7 >> bit) & 1) << 7);
	}

	// Direct colour not implemented yet!
	int cgram_index;

	if (bg.bpp == 2) { cgram_index = palette * 4   + colour; }
	if (bg.bpp == 4) { cgram_index = palette * 16  + colour; }
	if (bg.bpp == 8) { cgram_index =                 colour; } // THIS IS WRONG!
	
	bool transparent = (colour == 0);

	Word snes_colour = cgram.data[cgram_index];

	Pixel px;
	px.transparent = transparent;

	if (priority) {
		if (bg.layer == 1) { px.priority = priority_order.H1; }
		if (bg.layer == 2) { px.priority = priority_order.H2; }
		if (bg.layer == 3) { px.priority = priority_order.H3; }
		if (bg.layer == 4) { px.priority = priority_order.H4; }
	} else {
		if (bg.layer == 1) { px.priority = priority_order.L1; }
		if (bg.layer == 2) { px.priority = priority_order.L2; }
		if (bg.layer == 3) { px.priority = priority_order.L3; }
		if (bg.layer == 4) { px.priority = priority_order.L4; }
	}

	px.colour = snes_colour;
	px.layer = bg.layer;

	return px;
}

Pixel PPU::fetch_mode7_pixel(BG& bg, uint16_t xcounter) {
	int screen_x = xcounter;
	int screen_y = vcounter;

	int rel_x = screen_x + mode7.m7hofs - mode7.m7x;
	int rel_y = screen_y + mode7.m7vofs - mode7.m7y;

	int source_x = ((mode7.m7a * rel_x) + (mode7.m7b * rel_y)) >> 8; 
	int source_y = ((mode7.m7c * rel_x) + (mode7.m7d * rel_y)) >> 8; 

	source_x += mode7.m7x;
	source_y += mode7.m7y;

	source_x = source_x & 0x3FF;
	source_y = source_y & 0x3FF;

	int tile_x = source_x >> 3;
	int tile_y = source_y >> 3;

	int pixel_x = source_x & 0x7;
	int pixel_y = source_y & 0x7;

	uint32_t tile_index = (tile_x + (tile_y * 128)) & 0x3FFF;
	Byte tile_number = get_lo(vram.data[tile_index]);

	uint32_t pixel_offset = (pixel_y * 8) + pixel_x;
	uint32_t char_word_addr = ((tile_number * 64) + pixel_offset) & 0x3FFF;
	Byte colour = get_hi(vram.data[char_word_addr]);

	Word snes_colour = cgram.data[colour];
	Pixel px;
	px.transparent = (colour == 0);
	px.colour = snes_colour;
	px.layer = 1;
	px.priority = priority_order.H1;

	return px;
}

void PPU::render_bg_scanline(BG& bg) {
	Pixel fetched_pixel;

	int i = 0;
	for (int dot = 0; dot < 512; dot++) {

		if (!hires_mode && (dot & 1)) {
			bg.main_scanline[i]  = fetched_pixel;
			bg.sub_scanline[i++] = fetched_pixel;
			continue;
		}

		uint16_t screen_x = hires_mode ? dot : (dot >> 1);
		if (bg_mode == 7) {
			fetched_pixel = fetch_mode7_pixel(bg, screen_x);
		} else {
			fetched_pixel = fetch_bg_pixel(bg, screen_x);
		}

		fetched_pixel.colour_math = bg.enable_colour_math;

		bg.main_scanline[i]  = fetched_pixel;
		bg.sub_scanline[i++] = fetched_pixel;
	}

	if (bg.windows_on_subscreen) {
		window_mask(bg.sub_scanline, bg.window1_enabled, bg.window2_enabled, bg.window1_inverted, bg.window2_inverted, bg.mask_logic, bg.enable_colour_math);
	}
	if (bg.windows_on_main_screen) {
		window_mask(bg.main_scanline, bg.window1_enabled, bg.window2_enabled, bg.window1_inverted, bg.window2_inverted, bg.mask_logic, bg.enable_colour_math);
	}
}

void PPU::fetch_objects() {

	object_buffer.clear();

	for (int i = 0; i < 128 && object_buffer.size() < MAX_OBJECTS; i++) {
		Word x_coordinate = oam.data[(4 * i) + 0];
		Word y_coordinate = oam.data[(4 * i) + 1];
		Word tile_number  = oam.data[(4 * i) + 2];
		Word attributes   = oam.data[(4 * i) + 3];

		tile_number       = ((attributes & 1) << 8) | tile_number;
		Byte palette      = (attributes >> 1) & 0x7;
		Byte priority     = 0;

		switch ((attributes >> 4) & 0x3) {
		case 0:
			priority = priority_order.S0;
			break;
		case 1:
			priority = priority_order.S1;
			break;
		case 2:
			priority = priority_order.S2;
			break;
		case 3:
			priority = priority_order.S3;
			break;
		}

		bool horizontal_flip = (attributes >> 6) & 1;
		bool vertical_flip   = (attributes >> 7) & 1;

		Byte high_byte = oam.data[512 + (int)(i / 4)];
		Byte high_byte_pair = (high_byte >> (2 * (i % 4))) & 0x3;
		
		x_coordinate = ((high_byte_pair & 1) << 8) | x_coordinate;
		int signed_x = x_coordinate;
		if (signed_x > 255) {
			signed_x -= 512;
		}

		bool size = (high_byte_pair >> 1) & 1;

		int width  = size ? oam.obj_size.large_width  : oam.obj_size.small_width;
		int height = size ? oam.obj_size.large_height : oam.obj_size.small_height;

		int render_width  = hires_mode ? width : (2 * width);
		int render_height = hires_mode ? height : (2 * height);

		int line_in_sprite = (vcounter - y_coordinate) & 0xFF;

		if (line_in_sprite < height) {
			Object obj;

			obj.x_coordinate = signed_x;
			obj.y_coordinate = y_coordinate;
			obj.tile_number = tile_number;
			obj.attributes = attributes;
			obj.palette = palette;
			obj.priority = priority;
			obj.horizontal_flip = horizontal_flip;
			obj.vertical_flip = vertical_flip;
			obj.width = width;
			obj.height = height;
			obj.render_width = render_width;
			obj.render_height = render_height;
			obj.line_in_sprite = line_in_sprite;

			object_buffer.push_back(obj);
		} 
	}

}

void PPU::render_obj_scanline(ObjectLayer& obj) {

	Pixel transparent_pixel;
	transparent_pixel.transparent = true;
	transparent_pixel.colour = 0x00;
	transparent_pixel.priority = 0;

	std::array<Pixel, 512>& scanline = obj.scanline;
	scanline.fill(transparent_pixel);

	fetch_objects();

	// Render each object into scanline here
	for (auto& o : object_buffer) {
		int x = hires_mode ? o.x_coordinate : (o.x_coordinate * 2);

		for (int i = 0; i < o.width; i++) {
			
			int sprite_x = i;
			int sprite_y = o.line_in_sprite;

			if (o.horizontal_flip) {
				sprite_x = o.width - 1 - sprite_x;
			}
			if (o.vertical_flip) {
				if (o.width == o.height) {
					sprite_y = o.height - 1 - sprite_y;
				} else if (sprite_y < o.width) {
					sprite_y = o.width - 1 - sprite_y;
				} else {
					sprite_y = o.width + (o.width - 1) - (sprite_y - o.width);
				}
			}

			int tile_col = sprite_x / 8;
			int tile_row = sprite_y / 8;

			int pixel_x = sprite_x & 7;
			int pixel_y = sprite_y & 7;

			int base_col = o.tile_number & 0xF;
			int base_row = (o.tile_number >> 4) & 0xF;

			int col = (base_col + tile_col) & 0xF;
			int row = (base_row + tile_row) & 0xF;

			int tile_index = (row << 4) | col;

			bool second_base = (o.tile_number & 0x100) != 0;
			Word tile_base = second_base ? oam.second_base : oam.first_base;
			Word tile_address = (tile_base + (tile_index * 16)) & 0x7FFF;
			Word p01 = vram.data[(tile_address + 0 + pixel_y) & 0x7FFF];
			Word p23 = vram.data[(tile_address + 8 + pixel_y) & 0x7FFF];

			Byte p0 = get_lo(p01);
			Byte p1 = get_hi(p01);
			Byte p2 = get_lo(p23);
			Byte p3 = get_hi(p23);

			int bit = 7 - pixel_x;
			Byte colour = (((p0 >> bit) & 1) << 0) |
			 			  (((p1 >> bit) & 1) << 1) |
			 			  (((p2 >> bit) & 1) << 2) |
			 			  (((p3 >> bit) & 1) << 3);

			Pixel px;

			px.transparent = (colour == 0);

			Byte cgram_index = 128 + (o.palette * 16) + colour;
			Word snes_colour = cgram.data[cgram_index];

			px.priority = o.priority;
			px.colour = snes_colour;

			px.colour_math =  obj.enable_colour_math && (o.palette >= 4);

			// Sprites are not affected by hires mode
			if (x >= 0 && x < 512) {
				if (scanline[x].transparent && !px.transparent) {
					scanline[x] = px;
				}
			}
			if (((x + 1) >= 0 && (x + 1) < 512)) {
				if (scanline[x + 1].transparent && !px.transparent) {
					scanline[x + 1] = px;
				}
			}

			x += 2;
		}
	}

	obj.main_scanline = obj.scanline;
	obj.sub_scanline = obj.scanline;

	if (obj.windows_on_subscreen) {
		window_mask(obj.sub_scanline, obj.window1_enabled, obj.window2_enabled, obj.window1_inverted, obj.window2_inverted, obj.mask_logic, obj.enable_colour_math);
	}
	if (obj.windows_on_main_screen) {
		window_mask(obj.main_scanline, obj.window1_enabled, obj.window2_enabled, obj.window1_inverted, obj.window2_inverted, obj.mask_logic, obj.enable_colour_math);
	}
}

bool PPU::should_resolve(bool is_window, int value) {
	bool resolve = false;
	switch (value) {
	case 0: resolve = false; break;
	case 1: resolve = !is_window; break;
	case 2: resolve = is_window; break;
	case 3: resolve = true; break;
	}
	return resolve;
}

bool PPU::is_colour_math_window(int x) {
	bool window1_mask = (x >= window1.left_position) && (x <= window1.right_position);
	bool window2_mask = (x >= window2.left_position) && (x <= window2.right_position);
	if (col.window1_inverted) { window1_mask = !window1_mask; }
	if (col.window2_inverted) { window2_mask = !window2_mask; }
			
	bool mask = false;
	if (col.window1_enabled && !col.window2_enabled) { mask = window1_mask; }
	if (!col.window1_enabled && col.window2_enabled) { mask = window2_mask; }
	if (col.window1_enabled && col.window2_enabled) {
		switch (col.mask_logic) {
		case 0: mask =   window1_mask || window2_mask;  break;
		case 1: mask =   window1_mask && window2_mask;  break;
		case 2: mask =   window1_mask ^  window2_mask;  break;
		case 3: mask = !(window1_mask ^  window2_mask); break;
		}
	}

	return mask;
}

bool PPU::resolve_main_screen_px(Pixel& px, bool is_window) {
	bool resolve = should_resolve(is_window, col.main_screen_black_region);
	if (resolve) {
		px.colour = 0;
		px.transparent = false;
	}
	return resolve;
}

void PPU::resolve_sub_screen_px(Pixel& px, bool is_window) {
	bool resolve = should_resolve(is_window, col.sub_screen_transparent_region);
	if (resolve) {
		px.transparent = true;
	}
}

inline int clamp(int value, int min, int max) {
	if (value < min) { return min; }
	if (value > max) { return max; }
	return value;
}

Pixel PPU::colour_math(Pixel main, Pixel sub, bool ignore_half) {
	if (!main.colour_math || (col.addend == 1 && sub.transparent)) {
		return main;
	}

	Byte main_red   = (main.colour >> 0)  & 0x1F;
	Byte main_green = (main.colour >> 5)  & 0x1F;
	Byte main_blue  = (main.colour >> 10) & 0x1F;

	Byte sub_red, sub_green, sub_blue;
	if (!col.addend) {
		sub_red   = col.red;
		sub_green = col.green;
		sub_blue  = col.blue;
	} else {
		sub_red   = (sub.colour >> 0)   & 0x1F;
		sub_green = (sub.colour >> 5)   & 0x1F;
		sub_blue  = (sub.colour >> 10)  & 0x1F;
	}
	
	Byte red, green, blue;

	Byte divide = (col.half_colour_math && !ignore_half) ? 2 : 1;
	if (col.operator_type) {
		red   = clamp( (main_red   - sub_red) / divide,   0, 31);
		green = clamp( (main_green - sub_green) / divide, 0, 31);
		blue  = clamp( (main_blue  - sub_blue) / divide,  0, 31);
	} else {
		red   = clamp( (main_red   + sub_red) / divide,   0, 31);
		green = clamp( (main_green + sub_green) / divide, 0, 31);
		blue  = clamp( (main_blue  + sub_blue) / divide,  0, 31);
	}

	Pixel result = main;
	result.colour = (blue << 10) | (green << 5) | red;

	return result;
}

void PPU::composite(std::array<Pixel, 512>& final_scanline) {
	// JUST RESOLVES PRIORITIES, NO COLOUR MATH JUST YET!
	std::array<Pixel, 5> candidates;

	Pixel main_default_pixel;
	main_default_pixel.transparent = false;
	main_default_pixel.colour = cgram.data[0];
	main_default_pixel.colour_math = col.backdrop_colour_math_enabled;
	main_default_pixel.priority = 0;

	Pixel sub_default_pixel;
	sub_default_pixel.transparent = false;
	sub_default_pixel.colour = (col.blue << 10) | (col.green << 5) | col.red;
	sub_default_pixel.colour_math = col.backdrop_colour_math_enabled;
	sub_default_pixel.priority = 0;
	// THIS WILL NEED TO CHANGE TO GET A SUB AND MAIN SCREEN PIXEL ON WHICH TO DO COLOUR MATH
	for (int dot = 0; dot < 512; dot++) {
		int n = 0;
		if (bg1.main_screen) { candidates[n++] = bg1.main_scanline[dot]; }
		if (bg2.main_screen) { candidates[n++] = bg2.main_scanline[dot]; }
		if (bg3.main_screen) { candidates[n++] = bg3.main_scanline[dot]; }
		if (bg_mode == 0) {
			if (bg4.main_screen) { candidates[n++] = bg4.main_scanline[dot]; }
		}
		if (obj.main_screen) { candidates[n++] = obj.main_scanline[dot]; }
		
	
		Pixel* winner = nullptr;

		for (int c = 0; c < n; c++) {
			Pixel& px = candidates[c];

		    if (px.transparent) {
		        continue;
		    }

		    if (!winner || px.priority > winner->priority || (px.priority == winner->priority && px.layer > winner->layer)) {
		        winner = &px;
		    }
		}

		Pixel main_screen_px = winner ? *winner : main_default_pixel;

		// Repeat for sub_screen (could create function out of this...)
		n = 0;
		if (bg1.sub_screen) { candidates[n++] = bg1.sub_scanline[dot]; }
		if (bg2.sub_screen) { candidates[n++] = bg2.sub_scanline[dot]; }
		if (bg3.sub_screen ) { candidates[n++] = bg3.sub_scanline[dot]; }
		if (bg_mode == 0) {
			if (bg4.sub_screen) { candidates[n++] = bg4.sub_scanline[dot]; }
		}
		if (obj.sub_screen) { candidates[n++] = obj.sub_scanline[dot]; }
		
	
		winner = nullptr;

		for (int c = 0; c < n; c++) {
			Pixel& px = candidates[c];

		    if (px.transparent) {
		        continue;
		    }

		    if (!winner || px.priority > winner->priority || (px.priority == winner->priority && px.layer > winner->layer)) {
		        winner = &px;
		    }
		}

		Pixel sub_screen_px = winner ? *winner : sub_default_pixel;
		bool sub_is_backdrop = (winner == nullptr);

		uint16_t screen_x = hires_mode ? dot : (dot >> 1);
		bool is_window = is_colour_math_window(screen_x);
		bool main_forced_black = resolve_main_screen_px(main_screen_px, is_window);
		resolve_sub_screen_px(sub_screen_px, is_window);

		bool ignore_half = main_forced_black || (col.addend && sub_is_backdrop);
		final_scanline[dot] = colour_math(main_screen_px, sub_screen_px, ignore_half);
	}
}

static int dump_timer = 0;

void PPU::clear_framebuffer(std::vector<uint32_t>& f) {
	f.assign(screen_width * framebuffer_height, 0x000000FF);
}

void PPU::add_to_framebuffer(std::vector<uint32_t>& f, std::array<Pixel, 512>& line) {
	int idx1 = screen_width * (2 * vcounter);

	for (int i = 0; i < screen_width; i++) {
		f[idx1 + i] = convert_to_rgba(line[i].colour);
	}

	std::copy(f.begin() + idx1, f.begin() + idx1 + screen_width, f.begin() + idx1 + screen_width);
}

static const auto& rgba_lut() {
	static auto lut = []{
		std::array<std::array<uint32_t, 32768>, 16> t{};
		for (int b = 0; b < 16; b++) {
			for (int c = 0; c < 32768; c++) {
				Byte r5 = c & 0x1F;
				Byte g5 = (c >> 5) & 0x1F;
				Byte b5 = (c >> 10) & 0x1F;

				r5 = (r5 * (b + 1)) >> 4;
				g5 = (g5 * (b + 1)) >> 4;
				b5 = (b5 * (b + 1)) >> 4;

				Byte r8 = (r5 << 3) | (r5 >> 2);
				Byte g8 = (g5 << 3) | (g5 >> 2);
				Byte b8 = (b5 << 3) | (b5 >> 2);

				uint32_t rgba = (r8 << 24) | (g8 << 16) | (b8 << 8) | 0xFF;

				t[b][c] = rgba;
			}
		}
		return t;
	}();
	return lut;
}

uint32_t PPU::convert_to_rgba(uint16_t colour) {
	return rgba_lut()[brightness][colour];
}

void PPU::render_scanline() {
	// Calculations go here
	for (int x = 0; x < 512; x ++) {
		int even_x = x / 2;
		window1_dots[x] = (even_x >= window1.left_position) && (even_x <= window1.right_position);
		window2_dots[x] = (even_x >= window2.left_position) && (even_x <= window2.right_position);
	}
	render_bg_scanline(bg1);
	if constexpr (DEBUG_WINDOW) {
		if (!forced_blank) {
			add_to_framebuffer(bg1.framebuffer, bg1.main_scanline);
		}
	}
	if (bg_mode != 6) {
		render_bg_scanline(bg2);
		if constexpr (DEBUG_WINDOW) {
			if (!forced_blank) {
				add_to_framebuffer(bg2.framebuffer, bg2.main_scanline);
			}
		}
	}
	if (bg_mode != 3 && bg_mode != 5 && bg_mode != 7) {
		render_bg_scanline(bg3);
		if constexpr (DEBUG_WINDOW) {
			if (!forced_blank) {
				add_to_framebuffer(bg3.framebuffer, bg3.main_scanline);
			}
		}
	}
	if (bg_mode == 0) {
		render_bg_scanline(bg4);
		if constexpr (DEBUG_WINDOW) {
			if (!forced_blank) {
				add_to_framebuffer(bg4.framebuffer, bg4.main_scanline);
			}
		}
	}
	render_obj_scanline(obj);
	if constexpr (DEBUG_WINDOW) {
		if (!forced_blank) {
			add_to_framebuffer(obj.framebuffer, obj.main_scanline);
		}
	}

	std::array<Pixel, 512> final_scanline;
	composite(final_scanline);
	
	// Pushing calculated scanline into framebuffer goes here
	// With considerations for interlace modes!

	int idx1 = screen_width * (2 * vcounter);
	int idx2 = screen_width * ((2 * vcounter) + 1);
	
	if (forced_blank) {
		for (int i = 0; i < screen_width; i++) {
			framebuffer[idx1 + i] = 0x000000FF;
			framebuffer[idx2 + i] = 0x000000FF;
		}

		return;
	}

	for (const auto& px : final_scanline) {
		uint32_t rgba = convert_to_rgba(px.colour);

		framebuffer[idx1] = rgba;
		framebuffer[idx2] = rgba;

		idx1++;
		idx2++;
	}
}

void PPU::call_irq() {
	bus->write(TIMEUP_ADDRESS, 0x80);
}

void PPU::call_nmi() {
	bus->write(RDNMI_ADDRESS, 0x80);
}

bool PPU::frame_ended() {
	return vcounter >= 262;
}

void PPU::next_frame() {
	vcounter = 0;

	if (screen_interlacing) {
		field = !field;
	}

	update_vblank();
}

void PPU::enter_hblank() {
	hvbjoy = hvbjoy | (0b1 << 6);
	bus->write(HVBJOY, hvbjoy);

	if (!vblank && dma_controller) {
		dma_controller->execute_hdma();
	}
}

void PPU::leave_hblank() {
	hvbjoy = hvbjoy & ~(0b1 << 6);
	bus->write(HVBJOY, hvbjoy);
}

void PPU::enter_vblank() {
	frame_finished = true;
	hvbjoy = hvbjoy | (0b1 << 7);
	bus->write(HVBJOY, hvbjoy);
	call_nmi();
}

void PPU::leave_vblank() {
	hvbjoy = hvbjoy & ~(0b1 << 7);
	bus->write(HVBJOY, hvbjoy);
	bus->write(RDNMI_ADDRESS, 0x00);

	if (dma_controller) {
		dma_controller->hdma_init();
	}
}

void PPU::update_hblank() {
	bool old_hblank = hblank;
	hblank = (hcounter >= 1096);

	if (!old_hblank && hblank) {
		enter_hblank();
	}
	if (old_hblank && !hblank) {
		leave_hblank();
	}
}

void PPU::update_vblank() {
	vblank_start = (overscan_mode ? 240 : 225);

	bool old_vblank = vblank;
	vblank = (vcounter >= vblank_start);

	if (!old_vblank && vblank) {
		enter_vblank();
	}
	if (old_vblank && !vblank) {
		leave_vblank();
	}
}

void PPU::end_scanline() {
	vcounter += 1;
	if (vcounter < (overscan_mode ? overscan_vcount : no_overscan_vcount)) {
		render_scanline();
	}
	update_vblank();

	if (frame_ended()) {
		next_frame();
	}
}

void PPU::tick_component() {
	// MAIN PPU STUFF

	hcounter += 1;

	update_hblank();

	if (hcounter == DOTS_PER_LINE) {
		hcounter = 0;
		end_scanline();
	}

	if (irq_mode == 1 && hcounter == h_time_target) {
		call_irq();
	}
	if (irq_mode == 2 && vcounter == v_time_target && hcounter == 0) {
		call_irq();
	}
	if (irq_mode == 3 && vcounter == v_time_target && hcounter == h_time_target) {
		call_irq();
	}

	cycle += PPU_CYCLE;
};

void PPU::connect_dma_controller(DMAController* dma_controller) {
	this->dma_controller = dma_controller;
}
