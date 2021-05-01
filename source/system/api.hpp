#pragma once

#include "../types.hpp"

#include <pybind11/embed.h>

struct input_t;
struct audio_t;
struct music_t;
struct kernel_t;
struct stack_gui_t;
struct dialogue_gui_t;
struct headsup_gui_t;
struct camera_t;
struct naomi_state_t;
struct kontext_t;

struct api_t : public not_copyable_t, public not_moveable_t {
public:
	api_t() = default;
	~api_t() = default;
public:
	bool init(input_t& input, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext);
public:
	pybind11::scoped_interpreter interpeter {};
};
