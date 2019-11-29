#ifndef SYNAO_MENU_DIALOGUE_GUI_HPP
#define SYNAO_MENU_DIALOGUE_GUI_HPP

#include <bitset>
#include <entt/signal/delegate.hpp>

#include "../oly/draw_text.hpp"
#include "../oly/draw_scheme.hpp"

class CScriptArray;

struct input_t;
struct audio_t;
struct receiver_t;

namespace __enum_dialogue_flag {
	enum type : arch_t {
		Textbox,
		Facebox,
		Question,
		Sound,
		Writing,
		Total
	};
}

using dialogue_flag_t = __enum_dialogue_flag::type;

struct dialogue_gui_t : public not_copyable_t {
public:
	dialogue_gui_t();
	dialogue_gui_t(dialogue_gui_t&&) = default;
	dialogue_gui_t& operator=(dialogue_gui_t&&) = default;
	~dialogue_gui_t() = default;
public:
	bool init(receiver_t& receiver);
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
	void ask_question(const CScriptArray* array);
	bool get_flag(dialogue_flag_t flag) const;
	arch_t get_answer() const;
private:
	mutable bool_t write;
	std::bitset<dialogue_flag_t::Total> flags;
	arch_t cursor_index, cursor_total;
	real64_t timer, delay;
	rect_t rect;
	draw_text_t text;
	draw_scheme_t faces, arrow;
	entt::delegate<void(void)> suspender;
};

#endif // SYNAO_MENU_DIALOGUE_GUI_HPP