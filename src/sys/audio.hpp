#ifndef SYNAO_SYSTEM_AUDIO_HPP
#define SYNAO_SYSTEM_AUDIO_HPP

#include <string>
#include <vector>

#include "../sfx/channel.hpp"

struct setup_file_t;

struct audio_t : public not_copyable_t, public not_moveable_t {
public:
	audio_t();
	~audio_t();
public:
	bool init(const setup_file_t& config);
	void flush();
	void play(const std::string& id, arch_t index);
	void play(const std::string& id);
	void pause(arch_t index);
	void resume(arch_t index);
	void set_volume(real_t volume);
	real_t get_volume() const;
private:
	std::vector<std::pair<arch_t, const noise_t*> > tasks;
	std::vector<channel_t> channels;
	optr_t engine, context;
};

#endif // SYNAO_SYSTEM_AUDIO_HPP