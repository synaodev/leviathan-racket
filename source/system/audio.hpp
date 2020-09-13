#ifndef LEVIATHAN_INCLUDED_SYSTEM_AUDIO_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_AUDIO_HPP

#include <string>
#include <vector>
#include <entt/core/hashed_string.hpp>

#include "../audio/channel.hpp"

struct setup_file_t;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

using audio_task_t = std::pair<arch_t, const noise_t*>;

struct audio_t : public not_copyable_t {
public:
	audio_t();
	audio_t(audio_t&&) = default;
	audio_t& operator=(audio_t&&) = default;
	~audio_t();
public:
	bool init(const setup_file_t& config);
	void flush();
	void play(const entt::hashed_string& entry, arch_t index);
	void play(const entt::hashed_string& entry);
	void play(const std::string& id, arch_t index);
	void play(const std::string& id);
	void pause(arch_t index);
	void resume(arch_t index);
	void set_volume(real_t volume);
	real_t get_volume() const;
private:
	std::vector<audio_task_t> tasks;
	std::vector<channel_t> channels;
	ALCdevice* engine;
	ALCcontext* context;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_AUDIO_HPP
