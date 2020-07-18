#include "./audio.hpp"

#include "../audio/alcheck.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

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
	real_t volume = 1.0f;
	config.get("Audio", "Volume", volume);
	if (engine != nullptr) {
		SYNAO_LOG("OpenAL engine already exists!\n");
		return false;
	}
	if (context != nullptr) {
		SYNAO_LOG("OpenAL context already exists!\n");
		return false;
	}

	engine = alcOpenDevice(nullptr);

	if (engine == nullptr) {
		SYNAO_LOG("OpenAL engine creation failed!\n");
		return false;
	}

	context = alcCreateContext(reinterpret_cast<ALCdevice*>(engine), nullptr);

	if (context == nullptr) {
		SYNAO_LOG("OpenAL context creation failed!\n");
		return false;
	}
	if (alcMakeContextCurrent(reinterpret_cast<ALCcontext*>(context)) == 0) {
		SYNAO_LOG("OpenAL context relevance failed!\n");
		return false;
	}

	for (auto&& channel : channels) {
		channel.create(volume);
	}
	SYNAO_LOG("Audio system initialized.\n");
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
		tasks.erase(
			std::remove_if(tasks.begin(), tasks.end(), [](auto& task) { return task.second == nullptr; }),
			tasks.end()
		);
	}
}

void audio_t::play(const std::string& id, arch_t index) {
	if (index < channels.size()) {
		tasks.emplace_back(index, vfs::noise(id));
	}
}

void audio_t::play(const std::string& id) {
	for (auto&& channel : channels) {
		if (!channel.playing()) {
			tasks.emplace_back(
				std::distance(&channels[0], &channel),
				vfs::noise(id)
			);
			break;
		}
	}
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
