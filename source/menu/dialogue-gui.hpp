#pragma once

#include <functional>
#include <bitset>

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

class CScriptArray;

struct input_t;
struct audio_t;
struct receiver_t;

struct dialogue_gui_t : public not_copyable_t {
public:
	enum flags_t : arch_t {
		Textbox,
		Facebox,
		Question,
		Writing,
		Delay,
		Sound,
		TotalFlags
	};
public:
	dialogue_gui_t() = default;
	dialogue_gui_t(dialogue_gui_t&&) noexcept = default;
	dialogue_gui_t& operator=(dialogue_gui_t&&) noexcept = default;
	~dialogue_gui_t() = default;
public:
	bool init(receiver_t& receiver);
	bool refresh();
	void reset();
	void handle(const input_t& input, audio_t& audio);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void open_textbox_high();
	void open_textbox_low();
	void write_textbox(const std::string& string);
	void clear_textbox();
	void close_textbox();
	void set_face(arch_t state, direction_t direction);
	void set_face();
	void set_delay(real_t delay);
	void set_delay();
	void set_color(sint_t red, sint_t green, sint_t blue);
	void ask_question(const CScriptArray* array);
	bool get_flag(flags_t flag) const;
	arch_t get_answer() const;
private:
	mutable bool_t amend { true };
	std::bitset<flags_t::TotalFlags> flags { 0 };
	arch_t cursor_index { 0 };
	arch_t cursor_total { 0 };
	real_t timer { 0.0f };
	real_t delay { 0.0f };
	rect_t rect {};
	draw_text_t text {};
	draw_scheme_t faces {};
	draw_scheme_t arrow {};
	std::function<void()> suspender {};
};
