#ifndef LEVIATHAN_INCLUDED_RESOURCE_FONT_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_FONT_HPP

#include <string>
#include <map>
#include <glm/vec2.hpp>

#include "../types.hpp"

struct texture_t;
struct atlas_t;
struct palette_t;

struct font_glyph_t {
	real_t x { 0.0f };
	real_t y { 0.0f };
	real_t w { 0.0f };
	real_t h { 0.0f };
	real_t x_offset { 0.0f };
	real_t y_offset { 0.0f };
	real_t x_advance { 0.0f };
	sint_t table { 0 };
public:
	font_glyph_t(real_t x, real_t y, real_t w, real_t h, real_t x_offset, real_t y_offset, real_t x_advance, sint_t table) :
		x(x),
		y(y),
		w(w),
		h(h),
		x_offset(x_offset),
		y_offset(y_offset),
		x_advance(x_advance),
		table(table) {}
	font_glyph_t() = default;
	font_glyph_t(const font_glyph_t&) = default;
	font_glyph_t& operator=(const font_glyph_t&) = default;
	font_glyph_t(font_glyph_t&&) noexcept = default;
	font_glyph_t& operator=(font_glyph_t&&) noexcept = default;
	~font_glyph_t() = default;
};

struct font_t : public not_copyable_t {
public:
	font_t() = default;
	font_t(font_t&& that) noexcept = default;
	font_t& operator=(font_t&& that) noexcept = default;
	~font_t() = default;
public:
	void load(const std::string& directory, const std::string& full_path);
	const font_glyph_t& glyph(char32_t code_point) const;
	real_t kerning(char32_t first, char32_t second) const;
	const atlas_t* get_atlas() const;
	sint_t get_atlas_name() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
private:
	static const font_glyph_t kNullGlyph;
private:
	std::map<char32_t, font_glyph_t> glyphs {};
	std::map<std::pair<char32_t, char32_t>, real_t> kernings {};
	glm::vec2 dimensions {};
	const atlas_t* atlas { nullptr };
};

#endif // LEVIATHAN_INCLUDED_RESOURCE_FONT_HPP
