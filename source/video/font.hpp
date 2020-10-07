#ifndef LEVIATHAN_INCLUDED_VIDEO_FONT_HPP
#define LEVIATHAN_INCLUDED_VIDEO_FONT_HPP

#include <string>
#include <unordered_map>

#include "../types.hpp"

struct texture_t;
struct palette_t;

struct font_glyph_t {
	real_t x, y, w, h, x_offset, y_offset, x_advance;
	glm::ivec4 table;
public:
	font_glyph_t(real_t x, real_t y, real_t w, real_t h, real_t x_offset, real_t y_offset, real_t x_advance, glm::ivec4 table) :
		x(x), y(y),
		w(w), h(h),
		x_offset(x_offset), y_offset(y_offset),
		x_advance(x_advance),
		table(table) {}
	font_glyph_t() :
		x(0.0f), y(0.0f),
		w(0.0f), h(0.0f),
		x_offset(0.0f),
		y_offset(0.0f),
		x_advance(0.0f),
		table(0) {}
	font_glyph_t(const font_glyph_t&) = default;
	font_glyph_t(font_glyph_t&&) = default;
	font_glyph_t& operator=(const font_glyph_t&) = default;
	font_glyph_t& operator=(font_glyph_t&& that) = default;
	~font_glyph_t() = default;
};

struct font_t : public not_copyable_t {
public:
	font_t();
	font_t(font_t&& that) noexcept;
	font_t& operator=(font_t&& that) noexcept;
	~font_t() = default;
public:
	void load(const std::string& directory, const std::string& full_path);
	const font_glyph_t& glyph(char32_t code_point) const;
	const texture_t* get_texture() const;
	sint_t get_texture_name() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
private:
	std::unordered_map<char32_t, font_glyph_t> glyphs;
	glm::vec2 dimensions;
	const texture_t* texture;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_FONT_HPP
