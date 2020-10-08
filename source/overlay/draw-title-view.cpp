#include "./draw-title-view.hpp"

#include "../utility/constants.hpp"
#include "../utility/vfs.hpp"

static constexpr real64_t kFadeTime = 0.048;

draw_title_view_t::draw_title_view_t() :
	timer(0.0),
	head(),
	lite(),
	cards()
{

}

void draw_title_view_t::handle() {
	if (!head.empty()) {
		timer -= constants::MinInterval();
		if (timer <= 0.0) {
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
	recent.set_font(vfs::font(font_index));
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

void draw_title_view_t::set_persistent(glm::vec2 position, glm::vec4 color) {
	head.set_position(position);
	lite.set_position(position);
	lite.set_color(color);
}

void draw_title_view_t::set_persistent(real_t x, real_t y, glm::vec4 color) {
	glm::vec2 position = glm::vec2(x, y);
	this->set_persistent(position, color);
}

void draw_title_view_t::set_position(arch_t index, glm::vec2 position) {
	if (index < cards.size()) {
		auto& card = cards[index];
		card.set_position(position);
	}
}

void draw_title_view_t::set_position(arch_t index, real_t x, real_t y) {
	glm::vec2 position = glm::vec2(x, y);
	this->set_position(index, position);
}

void draw_title_view_t::set_centered(arch_t index, bool x, bool y) {
	if (index < cards.size()) {
		auto& card = cards[index];
		rect_t bounds = card.bounds();
		card.set_origin(
			x ? bounds.w / 2.0f : 0.0f,
			y ? bounds.h / 2.0f : 0.0f
		);
	}
}

void draw_title_view_t::set_head(const std::string& string) {
	timer = 2.0;
	head.set_string(string);
	head.set_origin(head.bounds().w / 2.0f, 0.0f);
	lite.set_string(string);
	lite.set_origin(lite.bounds().w / 2.0f - 1.0f, -1.0f);
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
