#include "./noise.hpp"
#include "./alcheck.hpp"
#include "./channel.hpp"

#include "../utility/logger.hpp"
#include "../utility/thread_pool.hpp"

#ifndef __EMSCRIPTEN__
	#include <SDL2/SDL.h>
#else
	#include <SDL/SDL.h>
#endif // __EMSCRIPTEN__

noise_t::noise_t() :
	ready(false),
	future(),
	handle(0),
	binder()
{
	
}

noise_t::noise_t(noise_t&& that) noexcept : noise_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());
		
		std::swap(future, that.future);
		std::swap(handle, that.handle);
		std::swap(binder, that.binder);
	}
}

noise_t& noise_t::operator=(noise_t&& that) noexcept {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());
		
		std::swap(future, that.future);
		std::swap(handle, that.handle);
		std::swap(binder, that.binder);
	}
	return *this;
}

noise_t::~noise_t() {
	this->destroy();
}

static ALenum get_format_enum(const SDL_AudioSpec* audio_spec) {
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
		SDL_AudioSpec audio_spec;
		uint8_t* audio_data = nullptr;
		uint_t length = 0;
		if (!SDL_LoadWAV(full_path.c_str(), &audio_spec, &audio_data, &length)) {
			SYNAO_LOG("Error! Couldn't load %s!\n", full_path.c_str());
			SYNAO_LOG("%s\n", SDL_GetError());
			return;
		}
		alCheck(alBufferData(
			handle, 
			get_format_enum(&audio_spec), 
			audio_data, 
			length, 
			audio_spec.freq
		));
		SDL_FreeWAV(audio_data);
		ready = true;
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
		return true;
	}
	SYNAO_LOG("Warning! Tried to overwrite existing noise!\n");
	return false;
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

void noise_t::assure() const {
	if (!ready and future.valid()) {
		future.wait();
	}
}