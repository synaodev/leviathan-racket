#include "./draw-title-view.hpp"

#include "../menu/dialogue-gui.hpp"
#include "../resource/vfs.hpp"
#include "../utility/constants.hpp"

namespace {
	constexpr real64_t kFadeTime = 0.048;
}

void draw_title_view_t::handle(const dialogue_gui_t& dialogue_gui) {
	if (!head.empty()) {
		timer -= constants::MinInterval();
		if (dialogue_gui.get_flag(dialogue_gui_t::Textbox) or timer <= 0.0) {
			this->set_head();
		}
	}
}

void draw_title_view_t::render(renderer_t& renderer) const {
	if (timer != 0.0) {
		lite.render(renderer);
		if (timer >= kFadeTime) {
			head.render(renderer);
		}
	}
	if (!cards.empty()) {
		for (auto&& card : cards) {
			card.render(renderer);
		}
	}
}

void draw_title_view_t::invalidate() const {
	head.invalidate();
	lite.invalidate();
	for (auto&& card : cards) {
		card.invalidate();
	}
}

void draw_title_view_t::push(const std::string& string, arch_t font_index) {
	auto& recent = cards.emplace_back();
	recent.set_font(vfs_t::font(font_index));
	recent.set_string(string);
}

void draw_title_view_t::clear() {
	cards.clear();
}

void draw_title_view_t::set_font(const font_t* font) {
	head.set_font(font);
	lite.set_font(font);
	cards.clear();
}

void draw_title_view_t::set_persistent(const glm::vec2& position, const glm::vec4& color) {
	head.set_position(position);
	lite.set_position(position);
	lite.set_color(color);
}

void draw_title_view_t::set_persistent(real_t x, real_t y, const glm::vec4& color) {
	const glm::vec2 p { x, y };
	this->set_persistent(p, color);
}

void draw_title_view_t::set_position(arch_t index, const glm::vec2& position) {
	if (index < cards.size()) {
		auto& card = cards[index];
		card.set_position(position);
	}
}

void draw_title_view_t::set_position(arch_t index, real_t x, real_t y) {
	const glm::vec2 p { x, y };
	this->set_position(index, p);
}

void draw_title_view_t::set_centered(arch_t index, bool x, bool y) {
	if (index < cards.size()) {
		auto& card = cards[index];
		const rect_t bounds = card.bounds();
		card.set_origin(
			x ? bounds.w / 2.0f : 0.0f,
			y ? bounds.h / 2.0f : 0.0f
		);
	}
}

void draw_title_view_t::set_head(const std::string& string) {
	timer = 2.0;
	head.set_string(string);
	const rect_t bounds = head.bounds();
	head.set_origin(bounds.w / 2.0f, 0.0f);
	lite.set_string(string);
	lite.set_origin(bounds.w / 2.0f - 1.0f, -1.0f);
}

void draw_title_view_t::set_head() {
	timer = 0.0;
	head.clear();
	lite.clear();
}

bool draw_title_view_t::has_field() const {
	return !head.empty();
}

bool draw_title_view_t::has_drawable() const {
	return timer != 0.0 or !cards.empty();
}
