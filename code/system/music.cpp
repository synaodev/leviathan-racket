#include "./music.hpp"

#include "../utility/setup_file.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"
#include "../audio/alcheck.hpp"

// Needed in order to quell annoying warnings...
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include "../pxtone/pxtnService.h"
#include "../pxtone/pxtnError.h"

#include <SDL2/SDL.h>

static constexpr arch_t kHeightLength = 10485760;
static constexpr sint_t kAudioBitrate = pxtnBITPERSAMPLE / 8;
static constexpr sint_t kTotalChannel = 2;
static constexpr sint_t kSamplingRate = 44100;
static constexpr real_t kBufferedTime = 0.1f;
static constexpr real_t kWaitConstant = 750.0f;

music_t::music_t() :
	service(nullptr),
	title(),
	playing(false),
	looping(false),
	thread(),
	channels(kTotalChannel),
	sampling_rate(kSamplingRate),
	buffered_time(kBufferedTime),
	volume(1.0f),
	source(0),
	buffers{}
{
	for (auto&& buffer : buffers) {
		buffer = 0;
	}
}

music_t::~music_t() {
	this->clear();
	if (source != 0) {
		sint_t state = 0;
		alCheck(alGetSourcei(source, AL_SOURCE_STATE, &state));
		if (state == AL_PLAYING) {
			alCheck(alSourceStop(source));
		}
		alCheck(alSourcei(source, AL_BUFFER, 0));
		alCheck(alDeleteBuffers(buffers.size(), &buffers[0]));
		alCheck(alDeleteSources(1, &source));
	}
}

bool music_t::init(const setup_file_t& config) {
	config.get("Music", "Channels",	channels);
	channels = glm::clamp(channels, 1, 2);
	config.get("Music", "SamplingRate",	sampling_rate);
	sampling_rate = glm::clamp(sampling_rate, 11025, 44100);
	config.get("Music", "kBufferedTime", buffered_time);
	buffered_time = glm::clamp(buffered_time, 0.1f, 0.75f);
	config.get("Music", "Volume", volume);
	volume = glm::clamp(volume, 0.0f, 1.0f);
	if (playing or !title.empty()) {
		SYNAO_LOG("Music device is already running!\n");
		return false;
	}
	if (service != nullptr) {
		SYNAO_LOG("Pxtone service already exists!\n");
		return false;
	}
	service = std::make_unique<pxtnService>();
	if (service == nullptr) {
		SYNAO_LOG("Pxtone service creation failed!\n");
		return false;
	}
	pxtnERR result = service->init();
	if (result != pxtnERR::pxtnOK) {
		SYNAO_LOG("Pxtone service initialization failed! PxtnErr: %d\n", result);
		return false;
	}
	if (!service->set_destination_quality(channels, sampling_rate)) {
		SYNAO_LOG("Pxtone quality setting failed!\n");
		return false;
	}
	// Setup OpenAL stuff
	if (source == 0) {
		alCheck(alGenSources(1, &source));
		alCheck(alGenBuffers(buffers.size(), &buffers[0]));
	} else {
		SYNAO_LOG("Music OpenAL resources already exist!\n");
		return false;
	}
	SYNAO_LOG("Music service is ready.\n");
	return true;
}

bool music_t::load(const std::string& title) {
	if (this->title == title) {
		return true;
	}
	if (service == nullptr) {
		return false;
	}
	this->clear();
	const std::string tune_path = vfs::resource_path(vfs_resource_path_t::Tune);
	std::vector<byte_t> buffer = vfs::byte_buffer(tune_path + title + ".ptcop");
	arch_t length = buffer.size();
	if (!length) {
		SYNAO_LOG("Pxtone file loading failed!\n");
		return false;
	} else if (length >= kHeightLength) {
		SYNAO_LOG("Pxtone file too large!\n");
		return false;
	}
	pxtnDescriptor descriptor;
	if (!descriptor.set_memory_r(&buffer[0], static_cast<sint_t>(length))) {
		SYNAO_LOG("Pxtone descriptor creation failed!\n");
		return false;
	}
	pxtnERR result = service->read(&descriptor);
	if (result != pxtnERR::pxtnOK) {
		SYNAO_LOG("Pxtone descriptor reading failed! PxtnErr: %d\n", result);
		service->clear();
		return false;
	}
	result = service->tones_ready();
	if (result != pxtnERR::pxtnOK) {
		SYNAO_LOG("Pxtone tone readying failed! PxtnErr: %d\n", result);
		service->clear();
		return false;
	}
	this->title = title;
	return true;
}

bool music_t::load(const std::string& title, real_t start_point, real_t fade_length) {
	if (this->load(title)) {
		return this->play(start_point, fade_length);
	}
	return false;
}

bool music_t::play(real_t start_point, real_t fade_length) {
	if (service == nullptr) {
		return false;
	}
	if (playing) {
		return true;
	}
	pxtnVOMITPREPARATION preparation = { 0 };
	preparation.flags |= looping ? pxtnVOMITPREPFLAG_loop : 0;
	preparation.start_pos_float = start_point / 1000.0f;
	preparation.fadein_sec = fade_length / 1000.0f;
	preparation.master_volume = volume;
	if (!service->moo_preparation(&preparation)) {
		SYNAO_LOG("Pxtone couldn't prepare tune!\n");
		return false;
	}
	playing = true;
	thread = std::thread(music_t::process, this);
	return true;
}

void music_t::pause() {
	playing = false;
	if (thread.joinable()) {
		thread.join();
	}
}

bool music_t::resume(real_t fade_length) {
	if (service == nullptr) {
		return false;
	}
	if (!playing and service->moo_is_valid_data()) {
		return this->play(0.0f, fade_length);
	}
	return true;
}

void music_t::clear() {
	this->pause();
	if (service != nullptr) {
		service->clear();
	}
	title.clear();
}

bool music_t::running() const {
	return playing;
}

void music_t::fade_out(real_t fade_length) {
	if (service != nullptr and playing) {
		service->moo_set_fade(-1, fade_length);
	}
}

void music_t::set_looping(bool looping) {
	this->looping = looping;
	if (service != nullptr and playing) {
		service->moo_set_loop(looping);
	}
}

bool music_t::get_looping() const {
	return looping;
}

void music_t::set_volume(real_t volume) {
	this->volume = volume;
	if (service != nullptr and playing) {
		service->moo_set_master_volume(volume);
	}
}

real_t music_t::get_volume() const {
	return volume;
}

void music_t::process(music_t* music) {
	if (music == nullptr) {
		SYNAO_LOG("Music thread should not print this message.\n");
		return;
	}
	// Initialize Necessary Data
	sint_t amount = static_cast<sint_t>(music->buffered_time * static_cast<real_t>(music->channels * music->sampling_rate * kAudioBitrate));
	uint_t waiter = static_cast<uint_t>(music->buffered_time * kWaitConstant);
	std::vector<byte_t> vector = std::vector<byte_t>(static_cast<arch_t>(amount), 0);
	// Generate OpenAL streaming data
	if (!vector.empty()) {
		sint_t state = 0;
		sint_t procs = 0;
		// Queue Tune Beginning
		for (auto&& buffer : music->buffers) {
			if (music->service->Moo(&vector[0], amount)) {
				alCheck(alBufferData(
					buffer, 
					music->channels != 2 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, 
					&vector[0], 
					amount, 
					music->sampling_rate
				));
				alCheck(alSourceQueueBuffers(music->source, 1, &buffer));
			} else {
				music->playing = false;
			}
		}
		// Main Loop
		while (music->playing) {
			alCheck(alGetSourcei(music->source, AL_SOURCE_STATE, &state));
			if (state != AL_PLAYING) {
				alCheck(alSourcePlay(music->source));
			}
			alCheck(alGetSourcei(music->source, AL_BUFFERS_PROCESSED, &procs));
			while (procs > 0) {
				uint_t buffer = 0;
				alCheck(alSourceUnqueueBuffers(music->source, 1, &buffer));
				if (music->service->Moo(&vector[0], amount)) {
					alCheck(alBufferData(
						buffer,
						music->channels != 2 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, 
						&vector[0], 
						amount, 
						music->sampling_rate
					));
					alCheck(alSourceQueueBuffers(music->source, 1, &buffer));
					procs--;
				} else {
					music->playing = false;
					break;
				}
			}
			SDL_Delay(waiter);
		}
		// Clean Up
		alCheck(alGetSourcei(music->source, AL_SOURCE_STATE, &state));
		if (state == AL_PLAYING) {
			alCheck(alSourceStop(music->source));
		}
		alCheck(alSourcei(music->source, AL_BUFFER, 0));
	}
}