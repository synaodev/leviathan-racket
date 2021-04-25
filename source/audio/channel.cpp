#include "./channel.hpp"
#include "./al-check.hpp"
#include "./noise.hpp"

void channel_t::create() {
	if (!handle) {
		alCheck(alGenSources(1, &handle));
	}
	ready = false;
}

void channel_t::create(real_t volume) {
	this->create();
	this->set_volume(volume);
}

void channel_t::destroy() {
	this->attach();
	if (handle != 0){
		alCheck(alDeleteSources(1, &handle));
		handle = 0;
	}
	ready = false;
}

bool channel_t::attach() {
	if (current) {
		ready = false;
		this->stop();
		current->binder.erase(this);
		current = nullptr;
		alCheck(alSourcei(handle, AL_BUFFER, 0));
	}
	return ready;
}

bool channel_t::attach(const noise_t* noise) {
	if (!ready or current != noise) {
		this->stop();
		if (current) {
			current->binder.erase(this);
			current = nullptr;
		}
		if (noise and noise->ready) {
			// noise->assure();
			ready = true;
			current = noise;
			noise->binder.insert(this);
			alCheck(alSourcei(handle, AL_BUFFER, noise->handle));
		} else {
			ready = false;
		}
	}
	return ready;
}

void channel_t::set_volume(real_t volume) {
	alCheck(alSourcef(handle, AL_GAIN, volume));
}

void channel_t::play() {
	if (ready) {
		alCheck(alSourcePlay(handle));
	}
}

void channel_t::stop() {
	sint_t state = 0;
	alCheck(alGetSourcei(handle, AL_SOURCE_STATE, &state));
	if (state != AL_STOPPED) {
		alCheck(alSourceStop(handle));
	}
}

void channel_t::pause() {
	sint_t state = 0;
	alCheck(alGetSourcei(handle, AL_SOURCE_STATE, &state));
	if (state != AL_PAUSED) {
		alCheck(alSourcePause(handle));
	}
}

bool channel_t::playing() const {
	sint_t state = 0;
	alCheck(alGetSourcei(handle, AL_SOURCE_STATE, &state));
	return state == AL_PLAYING;
}

bool channel_t::stopped() const {
	sint_t state = 0;
	alCheck(alGetSourcei(handle, AL_SOURCE_STATE, &state));
	return state == AL_STOPPED;
}

bool channel_t::paused() const {
	sint_t state = 0;
	alCheck(alGetSourcei(handle, AL_SOURCE_STATE, &state));
	return state == AL_PAUSED;
}

real_t channel_t::get_volume() const {
	real_t volume = 0.0f;
	alCheck(alGetSourcef(handle, AL_GAIN, &volume));
	return volume;
}
