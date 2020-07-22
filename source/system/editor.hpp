#ifndef SYNAO_SYSTEM_EDITOR_HPP
#define SYNAO_SYSTEM_EDITOR_HPP

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

#include "../field/tileset_viewer.hpp"
#include "../menu/attribute_gui.hpp"

struct video_t;
struct renderer_t;

struct editor_t : public not_copyable_t {
public:
	editor_t();
	editor_t(editor_t&&) = default;
	editor_t& operator=(editor_t&&) = default;
	~editor_t();
public:
	bool init(const video_t& video, renderer_t& renderer);
	void update(real64_t delta);
	void handle(input_t& input, renderer_t& renderer);
	void render(const video_t& video, renderer_t& renderer) const;
	bool viable() const;
public:
	typedef bool(*event_callback_t)(const SDL_Event*);
	static event_callback_t get_event_callback();
private:
	real64_t accum;
	SDL_Window* window;
	SDL_GLContext context;
	attribute_gui_t attribute_gui;
	tileset_viewer_t tileset_viewer;
};

#endif // SYNAO_SYSTEM_EDITOR_HPP