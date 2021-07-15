#include "./audio.hpp"

#include "../audio/al-check.hpp"
#include "../resource/vfs.hpp"
#include "../utility/setup-file.hpp"
#include "../utility/logger.hpp"

namespace {
	constexpr arch_t kSoundChannels = 12;
}

audio_t::~audio_t() {
	tasks.clear();
	channels.clear();
	if (context) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(reinterpret_cast<ALCcontext*>(context));
		context = nullptr;
	}
	if (device) {
		alcCloseDevice(reinterpret_cast<ALCdevice*>(device));
		device = nullptr;
	}
}

bool audio_t::init(const setup_file_t& config) {
	if (device) {
		synao_log("OpenAL device already exists!\n");
		return false;
	}
	if (context) {
		synao_log("OpenAL context already exists!\n");
		return false;
	}

	device = alcOpenDevice(nullptr);
	if (!device) {
		synao_log("OpenAL device creation failed!\n");
		return false;
	}

	context = alcCreateContext(reinterpret_cast<ALCdevice*>(device), nullptr);
	if (!context) {
		synao_log("OpenAL context creation failed!\n");
		return false;
	}
	if (alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(context)) == 0) {
		synao_log("OpenAL context currency failed!\n");
		return false;
	}

	real_t volume = 1.0f;
	config.get("Audio", "Volume", volume);

	channels.resize(kSoundChannels);
	for (auto&& channel : channels) {
		channel.create(volume);
	}

	synao_log("Audio system initialized.\n");
	return true;
}

void audio_t::flush() {
	if (!tasks.empty()) {
		for (auto&& task : tasks) {
			auto& channel = channels[task.first];
			if (channel.attach(task.second)) {
				task.second = nullptr;
				channel.play();
			}
		}
		auto remover = [](audio_task_t& task) {
			if (!task.second) {
				return true;
			}
			if (task.second->error()) {
				return true;
			}
			return false;
		};
		tasks.erase(std::remove_if(tasks.begin(), tasks.end(), remover), tasks.end());
	}
}

void audio_t::play(const entt::hashed_string& entry, arch_t index) {
	if (index < channels.size()) {
		tasks.emplace_back(index, vfs_t::noise(entry));
	}
}

void audio_t::play(const entt::hashed_string& entry) {
	for (auto&& channel : channels) {
		if (!channel.playing()) {
			tasks.emplace_back(
				std::distance(&channels[0], &channel),
				vfs_t::noise(entry)
			);
			break;
		}
	}
}

void audio_t::play(const std::string& id, arch_t index) {
	const entt::hashed_string entry{id.c_str()};
	this->play(entry, index);
}

void audio_t::play(const std::string& id) {
	const entt::hashed_string entry{id.c_str()};
	this->play(entry);
}

void audio_t::pause(arch_t index) {
	if (index < channels.size()) {
		channels[index].pause();
	}
}

void audio_t::resume(arch_t index) {
	if (index < channels.size()) {
		if (channels[index].paused()) {
			channels[index].play();
		}
	}
}

void audio_t::set_volume(real_t volume) {
	for (auto&& channel : channels) {
		channel.set_volume(volume);
	}
}

real_t audio_t::get_volume() const {
	if (channels.size() > 0) {
		return channels[0].get_volume();
	}
	return 0.0f;
}
