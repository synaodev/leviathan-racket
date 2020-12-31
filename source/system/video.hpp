#ifndef LEVIATHAN_INCLUDED_SYSTEM_VIDEO_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_VIDEO_HPP

#include <tuple>

#include "../utility/watch.hpp"

struct setup_file_t;

struct screen_params_t {
public:
	bool_t vsync, full;
	sint_t scaling;
	real64_t framerate;
	static constexpr sint_t 	kDefaultScaling 	= 1;
	static constexpr sint_t 	kHighestScaling		= 12; // 4K
	static constexpr real64_t 	kDefaultFramerate 	= 60.0;
public:
	screen_params_t(bool_t vsync, bool_t full, sint_t scaling, real_t framerate) :
		vsync(vsync),
		full(full),
		scaling(scaling),
		framerate(framerate) {}
	screen_params_t() :
		vsync(false),
		full(false),
		scaling(kDefaultScaling),
		framerate(kDefaultFramerate) {}
	screen_params_t(const screen_params_t&) = default;
	screen_params_t(screen_params_t&&) = default;
	screen_params_t& operator=(const screen_params_t&) = default;
	screen_params_t& operator=(screen_params_t&&) = default;
	~screen_params_t() = default;
};

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

struct video_t : public not_copyable_t {
public:
	video_t();
	video_t(video_t&&) = default;
	video_t& operator=(video_t&&) = default;
	~video_t();
public:
	bool init(const setup_file_t& config, bool editor = false);
	void flush() const;
	void set_parameters(screen_params_t params);
	const screen_params_t& get_parameters() const;
	glm::vec2 get_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
	glm::ivec2 get_editor_dimensions() const;
	bool get_meta_option() const;
	auto get_device() const {
		return std::make_tuple(window, context);
	}
private:
	SDL_Window* window;
	SDL_GLContext context;
	screen_params_t params;
	bool_t meta;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_VIDEO_HPP
