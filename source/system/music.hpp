#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <array>

#include "../types.hpp"

struct setup_file_t;

class pxtnService;

struct music_t : public not_copyable_t {
public:
	music_t();
	music_t(music_t&&) = delete;
	music_t& operator=(music_t&&) = delete;
	~music_t();
public:
	bool init(const setup_file_t& config);
	bool load(const std::string& title);
	bool load(const std::string& title, real_t start_point, real_t fade_length);
	bool play(real_t start_point, real_t fade_length);
	void pause();
	bool resume(real_t fade_length);
	void clear();
	void fade_out(real_t fade_length);
	bool running() const;
	void set_looping(bool looping);
	bool get_looping() const;
	void set_volume(real_t volume);
	real_t get_volume() const;
private:
	static void process(music_t* music);
private:
	std::unique_ptr<pxtnService> service;
	std::string title;
	std::atomic<bool> playing, looping;
	std::thread thread;
	sint_t channels, sampling_rate;
	real_t buffered_time, volume;
private: // For playing
	uint_t source;
	std::array<uint_t, 3> buffers;
};
