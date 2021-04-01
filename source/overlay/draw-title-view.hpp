#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP

#include "./draw-text.hpp"

struct dialogue_gui_t;

struct draw_title_view_t : public not_copyable_t {
public:
	draw_title_view_t() = default;
	draw_title_view_t(draw_title_view_t&&) = default;
	draw_title_view_t& operator=(draw_title_view_t&&) = default;
	~draw_title_view_t() = default;
public:
	void handle(const dialogue_gui_t& dialogue_gui);
	void render(renderer_t& renderer) const;
	void invalidate() const;
	void push(const std::string& string, arch_t font_index);
	void clear();
	void set_font(const font_t* font);
	void set_persistent(const glm::vec2& position, const glm::vec4& color);
	void set_persistent(real_t x, real_t y, const glm::vec4& color);
	void set_position(arch_t index, const glm::vec2& position);
	void set_position(arch_t index, real_t x, real_t y);
	void set_centered(arch_t index, bool horizontal, bool vertical);
	void set_head(const std::string& string);
	void set_head();
	bool has_field() const;
	bool has_drawable() const;
private:
	real64_t timer { 0.0 };
	draw_text_t head {};
	draw_text_t lite {};
	std::vector<draw_text_t> cards {};
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP
