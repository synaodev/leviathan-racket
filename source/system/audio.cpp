#include "./audio.hpp"

#include "../audio/al-check.hpp"
#include "../utility/setup-file.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"
#include "../resource/table-entry.hpp"

static constexpr arch_t kSoundChannels = 12;

audio_t::audio_t() :
	tasks(),
	channels(kSoundChannels),
	engine(nullptr),
	context(nullptr)
{

}

audio_t::~audio_t() {
	tasks.clear();
	channels.clear();
	if (context != nullptr) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(reinterpret_cast<ALCcontext*>(context));
		context = nullptr;
	}
	if (engine != nullptr) {
		alcCloseDevice(reinterpret_cast<ALCdevice*>(engine));
		engine = nullptr;
	}
}

bool audio_t::init(const setup_file_t& config) {
	if (engine != nullptr) {
		synao_log("OpenAL engine already exists!\n");
		return false;
	}
	if (context != nullptr) {
		synao_log("OpenAL context already exists!\n");
		return false;
	}

	engine = alcOpenDevice(nullptr);
	if (engine == nullptr) {
		synao_log("OpenAL engine creation failed!\n");
		return false;
	}

	context = alcCreateContext(reinterpret_cast<ALCdevice*>(engine), nullptr);
	if (context == nullptr) {
		synao_log("OpenAL context creation failed!\n");
		return false;
	}
	if (alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(context)) == 0) {
		synao_log("OpenAL context relevance failed!\n");
		return false;
	}

	real_t volume = 1.0f;
	config.get("Audio", "Volume", volume);
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
			if (task.second == nullptr) {
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

void audio_t::play(const table_entry_t& entry, arch_t index) {
	if (index < channels.size()) {
		tasks.emplace_back(index, vfs::noise(entry));
	}
}

void audio_t::play(const table_entry_t& entry) {
	for (auto&& channel : channels) {
		if (!channel.playing()) {
			tasks.emplace_back(
				std::distance(&channels[0], &channel),
				vfs::noise(entry)
			);
			break;
		}
	}
}

void audio_t::play(const std::string& id, arch_t index) {
	const table_entry_t entry = table_entry_t(id.c_str());
	this->play(entry, index);
}

void audio_t::play(const std::string& id) {
	const table_entry_t entry = table_entry_t(id.c_str());
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
