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
		if constexpr (DEBUG_WINDOW) {
			oam_window = SDL_CreateWindow(
				"OAM Sprite Viewer",
				oam_view_width,
				oam_view_height,
				SDL_WINDOW_RESIZABLE
			);

			oam_renderer = SDL_CreateRenderer(oam_window, nullptr);

			oam_texture = SDL_CreateTexture(
				oam_renderer,
				SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_STREAMING,
				oam_view_width,
				oam_view_height
			);

			SDL_SetTextureScaleMode(
				oam_texture,
				SDL_SCALEMODE_NEAREST
			);
		}

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);

		if (count > 0) {
		    joystick = SDL_OpenJoystick(joysticks[0]);

		    if (joystick) {
		        std::cout << "Opened joystick: "
		                  << SDL_GetJoystickName(joystick)
		                  << '\n';
		    }
		}

		SDL_free(joysticks);
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

	void display_oam_view(std::vector<uint32_t>& oam_buffer) {
		if constexpr (DEBUG_WINDOW) {
			int window_width;
			int window_height;

			SDL_GetWindowSize(oam_window, &window_width, &window_height);

			int scale = std::max(1, std::min(
				window_width  / oam_view_width,
				window_height / oam_view_height
			));

			float render_width  = static_cast<float>(oam_view_width  * scale);
			float render_height = static_cast<float>(oam_view_height * scale);

			SDL_UpdateTexture(
				oam_texture,
				nullptr,
				oam_buffer.data(),
				oam_view_width * sizeof(uint32_t)
			);

			SDL_SetRenderDrawColor(oam_renderer, 0, 0, 0, 255);
			SDL_RenderClear(oam_renderer);

			oam_dst = {
				(window_width  - render_width)  * 0.5f,
				(window_height - render_height) * 0.5f,
				render_width,
				render_height
			};

			SDL_RenderTexture(
				oam_renderer,
				oam_texture,
				nullptr,
				&oam_dst
			);

			SDL_RenderPresent(oam_renderer);
		}
		return;
	}

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
		    switch (event.type) {
		    case SDL_EVENT_QUIT:
		    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		        running = false;
		        break;

		    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
		        gamepad_buttons[event.jbutton.button] = true;
		        break;

		    case SDL_EVENT_JOYSTICK_BUTTON_UP:
		        gamepad_buttons[event.jbutton.button] = false;
		        break;

		    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
		        if (event.jaxis.axis == 0) {
		            axis_x = event.jaxis.value;
		        }
		        else if (event.jaxis.axis == 1) {
		            axis_y = event.jaxis.value;
		        }
		        break;
		    }
		}

	    const bool* keys = SDL_GetKeyboardState(nullptr);
		
		bool up    = keys[SDL_SCANCODE_UP]    || axis_y < -AXIS_THRESHOLD;
		bool down  = keys[SDL_SCANCODE_DOWN]  || axis_y >  AXIS_THRESHOLD;
		bool left  = keys[SDL_SCANCODE_LEFT]  || axis_x < -AXIS_THRESHOLD;
		bool right = keys[SDL_SCANCODE_RIGHT] || axis_x >  AXIS_THRESHOLD;

		bool select = keys[SDL_SCANCODE_A]      || gamepad_buttons[8];
		bool start  = keys[SDL_SCANCODE_RETURN] || gamepad_buttons[9];

		bool x = keys[SDL_SCANCODE_D] || gamepad_buttons[0];
		bool a = keys[SDL_SCANCODE_S] || gamepad_buttons[1];
		bool b = keys[SDL_SCANCODE_Z] || gamepad_buttons[2];
		bool y = keys[SDL_SCANCODE_X] || gamepad_buttons[3];

		bool l = keys[SDL_SCANCODE_Q] || gamepad_buttons[4];
		bool r = keys[SDL_SCANCODE_W] || gamepad_buttons[5];

		joy1l =
		      (a << 7)
		    | (x << 6)
		    | (l << 5)
		    | (r << 4);

		joy1h =
		      (b      << 7)
		    | (y      << 6)
		    | (select << 5)
		    | (start  << 4)
		    | (up     << 3)
		    | (down   << 2)
		    | (left   << 1)
		    | (right  << 0);

	}

	void close_window() {
		if (joystick) {
		    SDL_CloseJoystick(joystick);
		    joystick = nullptr;
		}
		SDL_DestroyWindow(window);
		if constexpr (DEBUG_WINDOW) {
			SDL_DestroyWindow(debug_window);
		}
		SDL_DestroyWindow(oam_window);
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

	// OAM sprite viewer window (always on, independent of DEBUG_WINDOW)
	SDL_Window* oam_window = nullptr;
	SDL_Renderer* oam_renderer = nullptr;
	SDL_Texture* oam_texture = nullptr;
	SDL_FRect oam_dst;
	
	static constexpr int oam_cell_size = 64;
	static constexpr int oam_view_width  = 16 * oam_cell_size;
	static constexpr int oam_view_height = 8  * oam_cell_size;

	bool closed = true;
	Byte joy1l, joy1h = 0x00;

	SDL_Joystick* joystick = nullptr;

	bool gamepad_buttons[10]{};

	int16_t axis_x = 0;
	int16_t axis_y = 0;

	static constexpr int AXIS_THRESHOLD = 16000;

	int screen_width;
	int screen_height;
	int scale;
};

