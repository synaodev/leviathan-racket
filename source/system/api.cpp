#include "./api.hpp"

#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/music.hpp"
#include "../system/kernel.hpp"
#include "../menu/stack-gui.hpp"
#include "../menu/dialogue-gui.hpp"
#include "../menu/headsup-gui.hpp"
#include "../field/camera.hpp"
#include "../actor/naomi.hpp"
#include "../component/kontext.hpp"
#include "../utility/rng.hpp"
#include "../resource/vfs.hpp"

static input_t* input_ptr { nullptr };
static audio_t* audio_ptr { nullptr };
static music_t* music_ptr { nullptr };
static kernel_t* kernel_ptr { nullptr };
static stack_gui_t* stack_gui_ptr { nullptr };
static dialogue_gui_t* dialogue_gui_ptr { nullptr };
static headsup_gui_t* headsup_gui_ptr { nullptr };
static camera_t* camera_ptr { nullptr };
static naomi_state_t* naomi_state_ptr { nullptr };
static kontext_t* kontext_ptr { nullptr };

bool api_t::init(input_t& input, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext) {
	input_ptr = &input;
	audio_ptr = &audio;
	music_ptr = &music;
	kernel_ptr = &kernel;
	stack_gui_ptr = &stack_gui;
	dialogue_gui_ptr = &dialogue_gui;
	headsup_gui_ptr = &headsup_gui;
	camera_ptr = &camera;
	naomi_state_ptr = &naomi_state;
	kontext_ptr = &kontext;
	return true;
}

#include <pybind11/embed.h>
#include <pybind11/functional.h>
namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(lvrk, m) {
	m.def("get_flag", [&](int index) {
		return kernel_ptr->get_flag((arch_t)index);
	});
	m.def("get_flag_range", [&](int from, int to) {
		return kernel_ptr->get_flag_range((arch_t)from, (arch_t)to);
	});
	m.def("set_flag", [&](int index, bool value) {
		kernel_ptr->set_flag((arch_t)index, value);
	});
	m.def("set_flag_range", [&](int from, int to, bool value) {
		kernel_ptr->set_flag_range((arch_t)from, (arch_t)to, value);
	});
	m.def("set_item", [&](int type, int count, int limit, int option) {
		kernel_ptr->set_item(type, count, limit, option);
	});
	m.def("set_item_limit", [&](int type, int limit) {
		kernel_ptr->set_item_limit(type, limit);
	});
	m.def("set_item_option", [&](int type, int option) {
		kernel_ptr->set_item_option(type, option);
	});
	m.def("add_item", [&](int type, int count, int limit) {
		kernel_ptr->add_item(type, count, limit);
	});
	m.def("sub_item", [&](int type, int count) {
		kernel_ptr->sub_item(type, count);
	});
	m.def("rid_item", [&](int type) {
		kernel_ptr->rid_item(type);
	});
	m.def("get_item_count", [&](int type) {
		kernel_ptr->get_item_count(type);
	});
	m.def("set_item_ptr_index", [&](int index) {
		kernel_ptr->set_item_ptr_index((arch_t)index);
	});
	m.def("get_item_ptr_index", [&] {
		return (int)kernel_ptr->get_item_ptr_index();
	});
	m.def("get_max_items", [&] {
		return (int)kernel_ptr->get_max_items();
	});
	m.def("lock", [&] {
		kernel_ptr->lock();
	});
	m.def("freeze", [&] {
		kernel_ptr->freeze();
	});
	m.def("unlock", [&] {
		kernel_ptr->unlock();
	});
	m.def("reboot", [&] {
		kernel_ptr->boot();
	});
	m.def("quit", [&] {
		kernel_ptr->quit();
	});
	m.def("buffer_field", [&](const std::string& name, int id) {
		kernel_ptr->buffer_field(name, id);
	});
	// m.def("eval_field", [&](const std::function<void()>& fn, int id) {
	// 	kernel_ptr->buffer_field(fn, id);
	// });
	m.def("load_progress", [&] {
		kernel_ptr->load_progress();
	});
	m.def("save_progress", [&] {
		kernel_ptr->save_progress();
	});
	m.def("set_file_index", [&](int index) {
		kernel_ptr->set_file_index((arch_t)index);
	});
	m.def("get_file_index", [&] {
		return (int)kernel_ptr->get_file_index();
	});
	m.def("random_int", [](int lowest, int highest) {
		return rng::next(lowest, highest);
	});
	m.def("random_real", [](float lowest, float highest) {
		return rng::next(lowest, highest);
	});
	m.def("key_pressed", [&](int action) {
		if (action < 16) {
			return input_ptr->pressed.test(action);
		}
		return false;
	});
	m.def("key_holding", [&](int action) {
		if (action < 16) {
			return input_ptr->holding.test(action);
		}
		return false;
	});
	m.def("get_locales", [](const std::string& key, int index) {
		return vfs_t::i18n_find(key, (arch_t)index);
	});
	m.def("get_locale", [](const std::string& key, int from, int to) {
		return vfs_t::i18n_find(key, (arch_t)from, (arch_t)to);
	});
	m.def("get_locale_size", [](const std::string& key) {
		return (int)vfs_t::i18n_size(key);
	});
	m.def("push_widget", [&](int type, int flags) {
		stack_gui_ptr->push((menu_t)type, (arch_t)flags);
	});
	m.def("pop_widget", [&] {
		stack_gui_ptr->pop();
	});
	m.def("fade_in", [&] {
		headsup_gui_ptr->fade_in();
	});
	m.def("fade_out", [&] {
		headsup_gui_ptr->fade_out();
	});
	m.def("set_field_text", [&](const std::string& text) {
		headsup_gui_ptr->set_field_text(text);
	});
	m.def("set_face", [&](int index, int type) {
		dialogue_gui_ptr->set_face((arch_t)index, (direction_t)type);
	});
	m.def("set_delay", [&](float delay) {
		dialogue_gui_ptr->set_delay(delay);
	});
	m.def("push_card", [&](const std::string& text, int font) {
		headsup_gui_ptr->push_card(text, (arch_t)font);
	});
	m.def("clear_cards", [&] {
		headsup_gui_ptr->clear_cards();
	});
	m.def("set_card_position", [&](int index, float x, float y) {
		headsup_gui_ptr->set_card_position((arch_t)index, x, y);
	});
	m.def("set_card_centered", [&](int index, bool horizontal, bool vertical) {
		headsup_gui_ptr->set_card_centered((arch_t)index, horizontal, vertical);
	});
	m.def("top_box", [&] {
		dialogue_gui_ptr->open_textbox_high();
	});
	m.def("low_box", [&] {
		dialogue_gui_ptr->open_textbox_low();
	});
	m.def("say", [&](const std::string& words) {
		dialogue_gui_ptr->write_textbox(words);
	});
	m.def("clear", [&] {
		dialogue_gui_ptr->clear_textbox();
	});
	m.def("close", [&] {
		dialogue_gui_ptr->close_textbox();
	});
	// m.def("ask", [&](std::vector<std::string>& question) {
	// 	dialogue_gui_ptr->ask_question(question);
	// });
	m.def("get_answer", [&] {
		return (int)dialogue_gui_ptr->get_answer();
	});
}
