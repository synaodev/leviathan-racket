#include "./draw_title_view.hpp"

#include "../utl/vfs.hpp"

static constexpr real64_t kFadeTime = 0.048;

draw_title_view_t::draw_title_view_t() :
	timer(0.0),
	head(),
	lite(),
	cards()
{

}

bool draw_title_view_t::init() {
	const font_t* font = vfs::font(2);
	head.set_font(font);
	head.set_position(160.0f, 24.0f);
	lite.set_font(font);
	lite.set_position(160.0f, 24.0f);
	lite.set_params(1.0f);
	return font != nullptr;
}

void draw_title_view_t::update(real64_t delta) {
	if (!head.empty()) {
		timer -= delta;
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

void draw_title_view_t::set_card(const std::string& string, arch_t font, bool center_x, bool center_y, glm::vec2 position) {
	auto& recent = cards.emplace_back();
	recent.set_position(position);
	recent.set_font(vfs::font(font));
	recent.set_string(string);
	rect_t bounds = recent.bounds();
	
	glm::vec2 origin = glm::zero<glm::vec2>();
	if (center_x) {
		origin.x = bounds.w / 2.0f;
	}
	if (center_y) {
		origin.y = bounds.h / 2.0f;
	}
	recent.set_origin(origin);
}

void draw_title_view_t::set_card(const std::string& string, arch_t font, bool center_x, bool center_y, real_t x, real_t y) {
	glm::vec2 position = glm::vec2(x, y);
	this->set_card(string, font, center_x, center_y, position);
}

void draw_title_view_t::set_card() {
	cards.clear();
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
