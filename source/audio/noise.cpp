#include "./noise.hpp"
#include "./al-check.hpp"
#include "./channel.hpp"
#include "../utility/logger.hpp"
#include "../utility/thread-pool.hpp"

#include <cstring>
#include <SDL2/SDL_audio.h>

static ALenum get_format_enum(const SDL_AudioSpec* audio_spec) {
	assert(audio_spec);
	ALenum type;
	if (audio_spec->channels == 1) {
		if (audio_spec->format == AUDIO_U8 or audio_spec->format == AUDIO_S8) {
			type = AL_FORMAT_MONO8;
		} else {
			type = AL_FORMAT_MONO16;
		}
	} else {
		if (audio_spec->format == AUDIO_U8 or audio_spec->format == AUDIO_S8) {
			type = AL_FORMAT_STEREO8;
		} else {
			type = AL_FORMAT_STEREO16;
		}
	}
	return type;
}

void noise_t::load(const std::string& full_path) {
	if (this->create()) {
		uint8_t* data = nullptr;
		uint_t length = 0;
		SDL_AudioSpec aospec;
		if (!SDL_LoadWAV(full_path.c_str(), &aospec, &data, &length)) {
			synao_log("Failed to load noise from \"{}\"! SDL Error: {}\n", full_path, SDL_GetError());
			ready = false;
			return;
		}
		alCheck(alBufferData(
			handle,
			get_format_enum(&aospec),
			data,
			length,
			aospec.freq
		));
		SDL_FreeWAV(data);
		ready = true;
	} else {
		ready = false;
	}
}

void noise_t::load(const std::string& full_path, thread_pool_t& thread_pool) {
	assert(!ready);
	this->future = thread_pool.push([this](const std::string& full_path) -> void {
		this->load(full_path);
	}, full_path);
}

bool noise_t::create() {
	if (!handle) {
		alCheck(alGenBuffers(1, &handle));
		return handle != 0;
	}
	synao_log("Warning! Tried to overwrite existing noise!\n");
	return true;
}

void noise_t::destroy() {
	if (future.valid()) {
		future.wait();
	}
	ready = false;
	if (binder.size() > 0) {
		std::set<channel_t*> channels;
		channels.swap(binder);
		for (auto&& channel : channels) {
			channel->attach();
		}
	}
	if (handle != 0) {
		alCheck(alDeleteBuffers(1, &handle));
		handle = 0;
	}
}

bool noise_t::error() const {
	if (!ready) {
		return !future.valid();
	}
	return false;
}

void noise_t::assure() const {
	if (!ready and future.valid()) {
		future.wait();
	}
}
