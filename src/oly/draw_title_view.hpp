#ifndef SYNAO_OVERLAY_DRAW_TITLE_VIEW_HPP
#define SYNAO_OVERLAY_DRAW_TITLE_VIEW_HPP

#include "./draw_text.hpp"

struct draw_title_view_t : public not_copyable_t, public not_moveable_t {
public:
	draw_title_view_t();
	~draw_title_view_t() = default;
public:
	bool init();
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void set_card(const std::string& string, arch_t font, bool center_x, bool center_y, glm::vec2 position);
	void set_card(const std::string& string, arch_t font, bool center_x, bool center_y, real_t x, real_t y);
	void set_card();
	void set_head(const std::string& string);
	void set_head();
	bool has_field() const;
	bool has_drawable() const;
private:
	real64_t timer;
	draw_text_t head, lite;
	std::vector<draw_text_t> cards;
};

#endif // SYNAO_OVERLAY_DRAW_TITLE_VIEW_HPP