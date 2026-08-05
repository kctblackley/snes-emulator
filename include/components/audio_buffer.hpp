#pragma once

#include <SDL3/SDL.h>
#include "ring_buffer.hpp"
#include "stereo_sample.hpp"

class AudioBuffer {
public:
	AudioBuffer();
	~AudioBuffer();

	bool push(const StereoSample& sample);
	
	size_t samples_available() {
		return buffer.current_size();
	}

	bool above_half_capacity() {
		return buffer.current_size() >= (BUFFER_CAPACITY / 2.0);
	}

	void close_audio () { // RAII for SDL audio buffer
		if (stream) {
			SDL_PauseAudioStreamDevice(stream);
			SDL_DestroyAudioStream(stream);
			stream = nullptr;
		}
	}

private:

	static void audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);
	SDL_AudioStream* stream = nullptr;

	static constexpr size_t BUFFER_CAPACITY = 8192;
	RingBuffer<StereoSample, BUFFER_CAPACITY> buffer;
};