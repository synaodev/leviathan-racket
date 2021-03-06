#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_TEXT_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_TEXT_HPP

#include <string>

#include "../utility/rect.hpp"
#include "../utility/enums.hpp"
#include "../video/vertex-pool.hpp"
#include "../video/gfx.hpp"

struct font_t;
struct renderer_t;

struct draw_text_t : public not_copyable_t {
public:
	draw_text_t();
	draw_text_t(draw_text_t&&) = default;
	draw_text_t& operator=(draw_text_t&&) = default;
	~draw_text_t() = default;
public:
	void invalidate() const;
	void clear();
	void increment();
	void render(renderer_t& renderer) const;
	void set_font(const font_t* font);
	void set_string(std::string words, bool immediate = true);
	void append_string(std::string words, bool immediate = true);
	void set_color(glm::vec4 color);
	void set_position(glm::vec2 position);
	void set_position(real_t x, real_t y);
	void set_origin(glm::vec2 origin);
	void set_origin(real_t x, real_t y);
	void set_layer(layer_t layer);
	bool finished() const;
	bool empty() const;
	rect_t bounds() const;
	const font_t* get_font() const;
	glm::vec2 get_position() const;
	glm::vec2 get_origin() const;
	layer_t get_layer() const;
	glm::vec2 get_font_size() const;
private:
	void generate();
private:
	mutable bool_t amend;
	const font_t* font;
	glm::vec2 position, origin;
	layer_t layer;
	glm::vec4 color;
	arch_t current;
	std::u32string buffer;
	vertex_pool_t quads;
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_TEXT_HPP
