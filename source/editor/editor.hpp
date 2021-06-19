#pragma once

#ifdef LEVIATHAN_USES_META

#include "./tileset-viewer.hpp"
#include "./attribute-viewer.hpp"

struct video_t;
struct renderer_t;

typedef struct SDL_Window SDL_Window;
typedef void_t SDL_GLContext;
typedef union SDL_Event SDL_Event;

struct editor_t : public not_copyable_t, public not_moveable_t {
public:
	editor_t() = default;
	~editor_t();
public:
	bool init(const video_t& video, renderer_t& renderer);
	void update(real64_t delta);
	bool handle(input_t& input, renderer_t& renderer);
	void render(const video_t& video, renderer_t& renderer) const;
	bool viable() const;
public:
	typedef bool(*event_callback_t)(const SDL_Event*);
	static event_callback_t get_event_callback();
private:
	real64_t accum { 0.0 };
	SDL_Window* window { nullptr };
	SDL_GLContext context { nullptr };
	attribute_viewer_t attribute_viewer {};
	tileset_viewer_t tileset_viewer {};
};

#endif
