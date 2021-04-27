#pragma once

#include "../types.hpp"

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
typedef union SDL_Event SDL_Event;

struct input_t;
struct video_t;
struct renderer_t;

#ifdef LEVIATHAN_USES_META

struct meta_state_t : public not_copyable_t {
public:
	meta_state_t() = default;
	meta_state_t(meta_state_t&&) noexcept = default;
	meta_state_t& operator=(meta_state_t&&) noexcept = default;
	~meta_state_t();
public:
	bool init(const video_t& video);
	void handle(const input_t& input);
	void update(real64_t delta);
	void flush() const;
public:
	typedef bool(*event_callback_t)(const SDL_Event*);
	static event_callback_t get_event_callback();
	static bool_t Hitboxes, Framerate;
private:
	static bool_t Ready;
	mutable bool_t active { false };
	mutable bool_t amend { false };
	SDL_Window* window { nullptr };
	SDL_GLContext context { nullptr };
};

#else // LEVIATHAN_USES_META

struct meta_state_t : public not_copyable_t {
public:
	meta_state_t() = default;
	meta_state_t(meta_state_t&&) = default;
	meta_state_t& operator=(meta_state_t&&) = default;
	~meta_state_t() = default;
public:
	typedef bool(*event_callback_t)(const SDL_Event*);
	static event_callback_t get_event_callback() { return nullptr; }
};

#endif // LEVIATHAN_USES_META
