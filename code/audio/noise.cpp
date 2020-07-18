#include "./noise.hpp"
#include "./alcheck.hpp"
#include "./channel.hpp"

#include "../utility/logger.hpp"
#include "../utility/thread_pool.hpp"

#include <cstring>
#include <SDL2/SDL_audio.h>

noise_t::noise_t() :
	ready(false),
	// aospec(),
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
		
		// std::swap(aospec, that.aospec);
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
		
		// std::swap(aospec, that.aospec);
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
	assert(audio_spec != nullptr);
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
			SYNAO_LOG("Failed to load noise from %s!\nSDL Error: %s\n", full_path.c_str(), SDL_GetError());
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
	}
}

void noise_t::load(const std::string& full_path, thread_pool_t& thread_pool) {
	assert(!ready);
	this->future = thread_pool.push([this](const std::string& full_path) -> void {
		this->load(full_path);
	}, full_path);
	// this->future = thread_pool.push([this](const std::string& full_path) -> std::vector<uint8_t> {
	// 	std::vector<uint8_t> result;
	// 	uint8_t* data = nullptr;
	// 	uint_t length = 0;
	// 	if (SDL_LoadWAV(full_path.c_str(), &aospec, &data, &length)) {
	// 		result.resize(length);
	// 		std::memcpy(&result[0], data, result.size());
	// 		SDL_FreeWAV(data);
	// 	} else {
	// 		SYNAO_LOG("Failed to load noise from %s!\n", full_path.c_str());
	// 		SYNAO_LOG("SDL Error: %s\n", SDL_GetError());
	// 	}
	// 	return result;
	// }, full_path);
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
		// auto result = future.get();
		future.wait();
	}
	ready = false;
	// std::memset(&aospec, 0, sizeof(aospec));
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

// void noise_t::assure() {
// 	if (!ready and future.valid()) {
// 		// Do noise loading now
// 		std::vector<uint8_t> buffer = future.get();
// 		if (this->create()) {
// 			uint_t length = static_cast<uint_t>(buffer.size());
// 			sint_t format = get_format_enum(&aospec);
// 			alCheck(alBufferData(
// 				handle, 
// 				format, 
// 				buffer.data(),
// 				length, 
// 				aospec.freq
// 			));
// 		}
// 		ready = true;
// 	}
// }

void noise_t::assure() const {
	if (!ready and future.valid()) {
		future.wait();
		// const_cast<noise_t*>(this)->assure();
	}
}