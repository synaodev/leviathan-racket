#include "./draw-text.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include "../system/renderer.hpp"
#include "../utility/vfs.hpp"
#include "../utility/utf32.hpp"
#include "../video/font.hpp"

#include <algorithm>

draw_text_t::draw_text_t() :
	amend(false),
	font(nullptr),
	position(0.0f),
	origin(0.0f),
	layer(layer_value::Persistent),
	color(1.0f),
	current(0),
	buffer(),
	quads()
{
	auto specify = vertex_spec_t::from(vtx_fonts_t::name());
	quads.setup(specify);
}

void draw_text_t::invalidate() const {
	amend = true;
}

void draw_text_t::clear() {
	amend = true;
	current = 0;
	buffer.clear();
	quads.clear();
}

void draw_text_t::increment() {
	++current;
	this->generate();
}

void draw_text_t::render(renderer_t& renderer) const {
	if (!quads.empty() and font != nullptr) {
		auto& list = renderer.display_list(
			layer,
			blend_mode_t::Alpha,
			program_t::Strings
		);
		if (amend) {
			amend = false;
			list.begin(quads.size())
				.vtx_pool_write(quads)
			.end();
		} else {
			list.skip(quads.size());
		}
	}
}

void draw_text_t::set_font(const font_t* font) {
	amend = true;
	this->font = font;
}

void draw_text_t::set_string(std::string words, bool immediate) {
	buffer.clear();
	utf8_to_utf32(
		words.begin(),
		words.end(),
		std::back_inserter(buffer)
	);
	if (immediate or current > buffer.size()) {
		current = buffer.size();
	}
	this->generate();
}

void draw_text_t::append_string(std::string words, bool immediate) {
	utf8_to_utf32(
		words.begin(),
		words.end(),
		std::back_inserter(buffer)
	);
	if (immediate or current > buffer.size()) {
		current = buffer.size();
	}
	this->generate();
}

void draw_text_t::set_color(glm::vec4 color) {
	this->color = color;
	this->generate();
}

void draw_text_t::set_position(glm::vec2 position) {
	this->position = glm::round(position);
	this->generate();
}

void draw_text_t::set_position(real_t x, real_t y) {
	this->set_position(glm::vec2(x, y));
}

void draw_text_t::set_origin(glm::vec2 origin) {
	this->origin = glm::round(origin);
	this->generate();
}

void draw_text_t::set_origin(real_t x, real_t y) {
	this->set_origin(glm::vec2(x, y));
}

void draw_text_t::set_layer(layer_t layer) {
	amend = true;
	this->layer = layer;
}

bool draw_text_t::finished() const {
	return current >= buffer.size();
}

bool draw_text_t::empty() const {
	return buffer.empty();
}

rect_t draw_text_t::bounds() const {
	if (!quads.empty()) {
		const vtx_fonts_t* verts = quads.at<vtx_fonts_t>(0);
		real_t left = verts[0].position.x;
		real_t top = verts[0].position.y;
		real_t right = verts[0].position.x;
		real_t bottom = verts[0].position.y;
		for (arch_t it = 1; it < quads.size(); ++it) {
			glm::vec2 marked = verts[it].position;
			if (marked.x < left) {
				left = marked.x;
			} else if (marked.x > right) {
				right = marked.x;
			}
			if (marked.y < top) {
				top = marked.y;
			} else if (marked.y > bottom) {
				bottom = marked.y;
			}
		}
		return rect_t(
			left, top,
			right - left,
			bottom - top
		);
	}
	return rect_t();
}

const font_t* draw_text_t::get_font() const {
	return font;
}

glm::vec2 draw_text_t::get_position() const {
	return position;
}

glm::vec2 draw_text_t::get_origin() const {
	return origin;
}

layer_t draw_text_t::get_layer() const {
	return layer;
}

glm::vec2 draw_text_t::get_font_size() const {
	if (font != nullptr) {
		return font->get_dimensions();
	}
	return glm::zero<glm::vec2>();
}

void draw_text_t::generate() {
	amend = true;
	quads.clear();
	if (font != nullptr and !buffer.empty()) {
		arch_t spaces_chars = std::count_if(
			buffer.begin(),
			buffer.begin() + current,
			[](auto c) { return c == U'\n' or c == U'\t'; }
		);
		arch_t drawable_chars = buffer.size() - spaces_chars;
		quads.resize(drawable_chars * display_list_t::SingleQuad);
		glm::vec2 start_pos = position - origin;
		glm::vec2 start_dim = font->get_dimensions();
		glm::vec2 start_inv = font->get_inverse_dimensions();
		sint_t atlas_name = font->get_atlas_name();
		char32_t p = U'\0';
		for (arch_t it = 0, qindex = 0; it < current; ++it, ++qindex) {
			char32_t& c = buffer[it];
			switch (c) {
			case U'\t': {
				const font_glyph_t& glyph = font->glyph(U' ');
				p = U' ';

				start_pos.x += glyph.w * 4.0f;
				--qindex;
				break;
			}
			case U'\n': {
				p = U'\0';

				start_pos.x = (position - origin).x;
				start_pos.y += start_dim.y;
				--qindex;
				break;
			}
			default: {
				const font_glyph_t& glyph = font->glyph(c);
				const real_t kerning = font->kerning(p, c);
				p = c;

				vtx_fonts_t* quad = quads.at<vtx_fonts_t>(qindex * display_list_t::SingleQuad);
				quad[0].position = glm::vec2(start_pos.x + glyph.x_offset, start_pos.y + glyph.y_offset);
				quad[0].uvcoords = glm::vec2(glyph.x, glyph.y) * start_inv;
				quad[0].color = color;
				quad[0].atlas = atlas_name;
				quad[0].table = glyph.table;

				quad[1].position = glm::vec2(start_pos.x + glyph.x_offset, start_pos.y + glyph.y_offset + glyph.h);
				quad[1].uvcoords = glm::vec2(glyph.x, glyph.y + glyph.h) * start_inv;
				quad[1].color = color;
				quad[1].atlas = atlas_name;
				quad[1].table = glyph.table;

				quad[2].position = glm::vec2(start_pos.x + glyph.x_offset + glyph.w, start_pos.y + glyph.y_offset);
				quad[2].uvcoords = glm::vec2(glyph.x + glyph.w, glyph.y) * start_inv;
				quad[2].color = color;
				quad[2].atlas = atlas_name;
				quad[2].table = glyph.table;

				quad[3].position = glm::vec2(start_pos.x + glyph.x_offset + glyph.w, start_pos.y + glyph.y_offset + glyph.h);
				quad[3].uvcoords = glm::vec2(glyph.x + glyph.w, glyph.y + glyph.h) * start_inv;
				quad[3].color = color;
				quad[3].atlas = atlas_name;
				quad[3].table = glyph.table;

				start_pos.x += (glyph.x_advance + kerning);
				break;
			}
			}
		}
	}
}
