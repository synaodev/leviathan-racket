#pragma once

#include <tuple>

#include "../utility/watch.hpp"

struct setup_file_t;

struct screen_params_t {
public:
	screen_params_t(bool_t vsync, bool_t full, sint_t scaling, real_t framerate) :
		vsync(vsync),
		full(full),
		scaling(scaling),
		framerate(framerate) {}
	screen_params_t() = default;
	screen_params_t(const screen_params_t&) = default;
	screen_params_t& operator=(const screen_params_t&) = default;
	screen_params_t(screen_params_t&&) = default;
	screen_params_t& operator=(screen_params_t&&) = default;
	~screen_params_t() = default;
public:
	static constexpr sint_t 	kDefaultScaling 	= 1;
	static constexpr sint_t 	kHighestScaling		= 12; // 4K
	static constexpr real64_t 	kDefaultFramerate 	= 60.0;
public:
	bool_t vsync { false };
	bool_t full { false };
	sint_t scaling { kDefaultScaling };
	real64_t framerate { kDefaultFramerate };
};

typedef struct SDL_Window SDL_Window;
typedef void_t SDL_GLContext;

struct video_t : public not_copyable_t, public not_moveable_t {
public:
	video_t() = default;
	~video_t();
public:
	bool init(const setup_file_t& config, bool_t tileset_editor = false);
	void flush() const;
	void set_parameters(screen_params_t parameters);
	const screen_params_t& get_parameters() const;
	glm::vec2 get_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
	bool get_meta_menu() const;
	bool get_tileset_editor() const;
	auto get_device() const {
		return std::make_tuple(window, context);
	}
private:
	SDL_Window* window { nullptr };
	SDL_GLContext context { nullptr };
	screen_params_t parameters {};
	bool_t meta_menu { false };
	bool_t tileset_editor { false };
};
