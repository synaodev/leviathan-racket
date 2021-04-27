#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <array>

#include <pxtone/pxtnService.h>

#include "../types.hpp"

struct setup_file_t;

struct music_t : public not_copyable_t, public not_moveable_t {
public:
	music_t() = default;
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
	std::unique_ptr<pxtnService> service { nullptr };
	std::string title {};
	std::atomic<bool> playing { false };
	std::atomic<bool> looping { false };
	std::thread thread {};
	sint_t channels { 0 };
	sint_t sampling_rate { 0 };
	real_t buffered_time { 0.0f };
	real_t volume { 1.0f };
private: // For playing
	uint_t source { 0 };
	std::array<uint_t, 3> buffers {
		0, 0, 0
	};
};
