#pragma once
#include "common.hpp"
#include <vector>

#define JOY1L_ADDRESS 0x4218
#define JOY1H_ADDRESS 0x4219

class Renderer {
public:
	Renderer() { 
		closed = true;
	}

	~Renderer() {
		if (!closed && window) {
			close_window();
		}
	}

	void create_window(int width, int height) {
		screen_width = width;
		screen_height = height;

		SDL_Init(SDL_INIT_VIDEO);

		// Main window
		window = SDL_CreateWindow(
			"SNES Emulator",
			screen_width,
			screen_height,
			SDL_WINDOW_RESIZABLE
		);

		//framebuffer.clear();
		//framebuffer.assign(screen_width * screen_height, 0xFF000000);
		closed = false;

		renderer = SDL_CreateRenderer(window, nullptr);
		
		texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_width,
			screen_height
		);

		// Debug window 1 (window with separate layers)
		if constexpr (DEBUG_WINDOW) {
			debug_window = SDL_CreateWindow(
				"Separate Layers",
				screen_width * 2,
				screen_height * 3,
				SDL_WINDOW_RESIZABLE
			);

			debug_renderer = SDL_CreateRenderer(debug_window, nullptr);

			bg1_tex = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
			bg2_tex = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
			bg3_tex = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
			bg4_tex = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
			obj_tex = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
		}

		SDL_SetTextureScaleMode(
			texture,
			SDL_SCALEMODE_NEAREST
		);
	}

	Byte get_joypad(uint16_t offset) {
		if (offset == JOY1L_ADDRESS) {
			return joy1l;
		} else if (offset == JOY1H_ADDRESS) {
			return joy1h;
		}
		return 0xFF;
	}

	// This displays the main screen
	void display_framebuffer(std::vector<uint32_t>& framebuffer) {
		int window_width;
		int window_height;

		SDL_GetWindowSize(window, &window_width, &window_height);

		int scale = std::max(1, std::min(
			window_width / screen_width,
			window_height / screen_height
		));

		float render_width  = static_cast<float>(screen_width * scale);
		float render_height = static_cast<float>(screen_height * scale);

		SDL_UpdateTexture(
			texture,
			nullptr,
			framebuffer.data(),
			screen_width * sizeof(uint32_t)
		);

		SDL_RenderClear(renderer);

		dst = {
			(window_width - render_width) * 0.5f,
			(window_height - render_height) * 0.5f,
			render_width,
			render_height
		};

		SDL_RenderTexture(
			renderer,
			texture,
			nullptr,
			&dst
		);

		SDL_RenderPresent(renderer);
		return;
	}

	// This displays the layers separately
	void display_separate_framebuffers(std::vector<uint32_t>& bg1,
									   std::vector<uint32_t>& bg2,
									   std::vector<uint32_t>& bg3,
									   std::vector<uint32_t>& bg4,
									   std::vector<uint32_t>& obj) {

		//
		if constexpr (DEBUG_WINDOW) {
			int window_width;
			int window_height;

			SDL_GetWindowSize(debug_window, &window_width, &window_height);

			float cell_width  = window_width  / 2.0f;
			float cell_height = window_height / 3.0f;

			float scale = std::min(
				cell_width  / screen_width,
				cell_height / screen_height
			);

			float w = screen_width  * scale;
			float h = screen_height * scale;

			SDL_UpdateTexture(bg1_tex, nullptr, bg1.data(), screen_width * sizeof(uint32_t));
			SDL_UpdateTexture(bg2_tex, nullptr, bg2.data(), screen_width * sizeof(uint32_t));
			SDL_UpdateTexture(bg3_tex, nullptr, bg3.data(), screen_width * sizeof(uint32_t));
			SDL_UpdateTexture(bg4_tex, nullptr, bg4.data(), screen_width * sizeof(uint32_t));
			SDL_UpdateTexture(obj_tex, nullptr, obj.data(), screen_width * sizeof(uint32_t));
			
			SDL_SetRenderDrawColor(debug_renderer, 0, 0, 0, 255);
			SDL_RenderClear(debug_renderer);

			float x_offset = (cell_width - w) * 0.5f;
			float y_offset = (cell_height - h) * 0.5f;

			bg1_r = {x_offset,              y_offset,              w, h};
			bg2_r = {cell_width + x_offset, y_offset,              w, h};
			bg3_r = {x_offset,              cell_height + y_offset,w, h};
			bg4_r = {cell_width + x_offset, cell_height + y_offset,w, h};
			obj_r = {x_offset,              cell_height * 2.0f + y_offset, w, h};

			SDL_RenderTexture(debug_renderer, bg1_tex, nullptr, &bg1_r);
			SDL_RenderTexture(debug_renderer, bg2_tex, nullptr, &bg2_r);
			SDL_RenderTexture(debug_renderer, bg3_tex, nullptr, &bg3_r);
			SDL_RenderTexture(debug_renderer, bg4_tex, nullptr, &bg4_r);
			SDL_RenderTexture(debug_renderer, obj_tex, nullptr, &obj_r);
			
			SDL_RenderPresent(debug_renderer);
		}
	}

	void loop() {
		SDL_Event event;
	    while (SDL_PollEvent(&event)) {
	        if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
	            running = false;
	        }
	    }

	    const bool* keys = SDL_GetKeyboardState(nullptr);
		
		joy1l = (keys[SDL_SCANCODE_S]     << 7)  // A
              | (keys[SDL_SCANCODE_D]     << 6)  // X
              | (keys[SDL_SCANCODE_Q]     << 5)  // L
              | (keys[SDL_SCANCODE_W]     << 4); // R

        joy1h = (keys[SDL_SCANCODE_Z]     << 7)  // B
		      | (keys[SDL_SCANCODE_X]     << 6)  // Y
		      | (keys[SDL_SCANCODE_A]     << 5)  // Select
			  | (keys[SDL_SCANCODE_RETURN]<< 4)  // Start
			  | (keys[SDL_SCANCODE_UP]    << 3)  // D-Pad
			  | (keys[SDL_SCANCODE_DOWN]  << 2)
			  | (keys[SDL_SCANCODE_LEFT]  << 1)
			  | (keys[SDL_SCANCODE_RIGHT] << 0);
	}

	void close_window() {
		SDL_DestroyWindow(window);
		if constexpr (DEBUG_WINDOW) {
			SDL_DestroyWindow(debug_window);
		}
		SDL_Quit();
		closed = true;
	}

	bool running = true;

	std::vector<uint32_t> framebuffer;

private:
	// Main window
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_FRect dst;

	// Separate layer debug window
	SDL_Window* debug_window = nullptr;
	SDL_Renderer* debug_renderer = nullptr;

	SDL_Texture* bg1_tex = nullptr;
	SDL_Texture* bg2_tex = nullptr;
	SDL_Texture* bg3_tex = nullptr;
	SDL_Texture* bg4_tex = nullptr;
	SDL_Texture* obj_tex = nullptr;

	SDL_FRect bg1_r, bg2_r, bg3_r, bg4_r, obj_r;
	bool closed = true;
	Byte joy1l, joy1h = 0x00;

	int screen_width;
	int screen_height;
	int scale;
};

