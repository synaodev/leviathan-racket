#ifndef LEVIATHAN_INCLUDED_SYSTEM_EDITOR_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_EDITOR_HPP

#include "../field/tileset-viewer.hpp"
#include "../menu/attribute-gui.hpp"

struct video_t;
struct renderer_t;

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

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

#endif // LEVIATHAN_INCLUDED_SYSTEM_EDITOR_HPP
