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
		scale = 3;

		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow(
			"SNES Emulator",
			screen_width * scale,
			screen_height * scale,
			0
		);

		framebuffer.clear();
		framebuffer.assign(screen_width * screen_height, 0xFF000000);
		closed = false;

		renderer = SDL_CreateRenderer(window, nullptr);
		
		texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_width,
			screen_height
		);

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

	void display_framebuffer(std::vector<uint32_t>& framebuffer) {
		SDL_UpdateTexture(
			texture,
			nullptr,
			framebuffer.data(),
			screen_width * sizeof(uint32_t)
		);

		SDL_RenderClear(renderer);

		dst = {
			0.0f,
			0.0f,
			static_cast<float>(screen_width * scale),
			static_cast<float>(screen_height * scale)
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

	void loop() {
		SDL_Event event;
	    while (SDL_PollEvent(&event)) {
	        if (event.type == SDL_EVENT_QUIT) {
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
		SDL_Quit();
		closed = true;
	}

	bool running = true;

	std::vector<uint32_t> framebuffer;

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_FRect dst;
	bool closed = true;
	Byte joy1l, joy1h = 0x00;

	int screen_width;
	int screen_height;
	int scale;
};

