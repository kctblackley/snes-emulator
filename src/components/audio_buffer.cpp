#include "audio_buffer.hpp"
#include <array>
#include <algorithm>
#include <cstdio>

AudioBuffer::AudioBuffer() {
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec spec {};

	spec.format = SDL_AUDIO_S16;
	spec.channels = 2;
	spec.freq = 32000;

	stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_callback, this);
	if (!stream) {
		SDL_Log("Audio device failed to open: %s", SDL_GetError());
		return;
	}

	SDL_ResumeAudioStreamDevice(stream);
}

AudioBuffer::~AudioBuffer() {
	if (stream) {
		SDL_PauseAudioStreamDevice(stream);
		SDL_DestroyAudioStream(stream);
		stream = nullptr;
	}
}

bool AudioBuffer::push(const StereoSample& sample) {
	return buffer.push(sample);
}

void AudioBuffer::audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
	AudioBuffer* audio = static_cast<AudioBuffer*>(userdata);
	const int sample_count = std::min<int>(additional_amount / sizeof(StereoSample), 512);

	std::array<StereoSample, 512> samples;

	static int callback_count = 0;
	static long total_requested = 0;
	static long total_underruns = 0;
	int underruns_this_call = 0;

	for (int i = 0; i < sample_count; i++) {
		if (!audio->buffer.pop(samples[i])) {
			samples[i].left = 0;
			samples[i].right = 0;
			underruns_this_call++;
		}
	}

	callback_count++;
	total_requested += sample_count;
	total_underruns += underruns_this_call;

	SDL_PutAudioStreamData(stream, samples.data(), sample_count * sizeof(StereoSample));
}