#include "./vertex.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"

#include <utility>
#include <cstddef>

bool vertex_spec_t::compare(const uint_t* lhv, const uint_t* rhv) {
	if (!lhv or !rhv) {
		synao_log("Warning! vertex_spec_t::compare has null inputs!\n");
		return false;
	}
	while (*lhv) {
		if (*lhv != *rhv) {
			break;
		}
		lhv++; rhv++;
	}
	return (*(const uint_t*)lhv - *(const uint_t*)rhv) == 0;
}

vertex_spec_t vertex_spec_t::from(const uint_t* list) {
	static const uint_t kMinor[]  = { GL_FLOAT_VEC2, 0 };
	static const uint_t kBlank[]  = { GL_FLOAT_VEC2, GL_INT, GL_FLOAT_VEC4, 0 };
	static const uint_t kMajor[] = { GL_FLOAT_VEC2, GL_INT, GL_FLOAT_VEC2, GL_FLOAT, GL_INT, 0 };
	static const uint_t kFonts[]  = { GL_FLOAT_VEC2, GL_FLOAT_VEC2, GL_FLOAT_VEC4, GL_INT, GL_INT, 0 };
	vertex_spec_t result;
	if (vertex_spec_t::compare(list, kMinor)) {
		result = vertex_spec_t::from(vtx_minor_t::name());
	} else if (vertex_spec_t::compare(list, kBlank)) {
		result = vertex_spec_t::from(vtx_blank_t::name());
	} else if (vertex_spec_t::compare(list, kMajor)) {
		result = vertex_spec_t::from(vtx_major_t::name());
	} else if (vertex_spec_t::compare(list, kFonts)) {
		result = vertex_spec_t::from(vtx_fonts_t::name());
	}
	return result;
}

vertex_spec_t vertex_spec_t::from(arch_t name) {
	vertex_spec_t result;
	if (name == vtx_minor_t::name()) {
		result.length = sizeof(vtx_minor_t);
		result.detail = [] {
			glCheck(glEnableVertexAttribArray(0));
			glCheck(glVertexAttribPointer(
				0, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_minor_t),
				(const void_t)offsetof(vtx_minor_t, position)
			));
		};
	} else if (name == vtx_blank_t::name()) {
		result.length = sizeof(vtx_blank_t);
		result.detail = [] {
			glCheck(glEnableVertexAttribArray(0));
			glCheck(glVertexAttribPointer(
				0, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_blank_t),
				(const void_t)offsetof(vtx_blank_t, position)
			));
			glCheck(glEnableVertexAttribArray(1));
			glCheck(glVertexAttribIPointer(
				1, 1,
				GL_INT, sizeof(vtx_blank_t),
				(const void_t)offsetof(vtx_blank_t, matrix)
			));
			glCheck(glEnableVertexAttribArray(2));
			glCheck(glVertexAttribPointer(
				2, glm::vec4::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_blank_t),
				(const void_t)offsetof(vtx_blank_t, color)
			));
		};
	} else if (name == vtx_major_t::name()) {
		result.length = sizeof(vtx_major_t);
		result.detail = [] {
			glCheck(glEnableVertexAttribArray(0));
			glCheck(glVertexAttribPointer(
				0, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_major_t),
				(const void_t)offsetof(vtx_major_t, position)
			));
			glCheck(glEnableVertexAttribArray(1));
			glCheck(glVertexAttribIPointer(
				1, 1,
				GL_INT, sizeof(vtx_major_t),
				(const void_t)offsetof(vtx_major_t, matrix)
			));
			glCheck(glEnableVertexAttribArray(2));
			glCheck(glVertexAttribPointer(
				2, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_major_t),
				(const void_t)offsetof(vtx_major_t, uvcoords)
			));
			glCheck(glEnableVertexAttribArray(3));
			glCheck(glVertexAttribPointer(
				3, 1,
				GL_FLOAT, GL_FALSE, sizeof(vtx_major_t),
				(const void_t)offsetof(vtx_major_t, alpha)
			));
			glCheck(glEnableVertexAttribArray(4));
			glCheck(glVertexAttribIPointer(
				4, 1,
				GL_INT, sizeof(vtx_major_t),
				(const void_t)offsetof(vtx_major_t, texID)
			));
		};
	} else if (name == vtx_fonts_t::name()) {
		result.length = sizeof(vtx_fonts_t);
		result.detail = [] {
			glCheck(glEnableVertexAttribArray(0));
			glCheck(glVertexAttribPointer(
				0, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_fonts_t),
				(const void_t)offsetof(vtx_fonts_t, position)
			));
			glCheck(glEnableVertexAttribArray(1));
			glCheck(glVertexAttribPointer(
				1, glm::vec2::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_fonts_t),
				(const void_t)offsetof(vtx_fonts_t, uvcoords)
			));
			glCheck(glEnableVertexAttribArray(2));
			glCheck(glVertexAttribPointer(
				2, glm::vec4::length(),
				GL_FLOAT, GL_FALSE, sizeof(vtx_fonts_t),
				(const void_t)offsetof(vtx_fonts_t, color)
			));
			glCheck(glEnableVertexAttribArray(3));
			glCheck(glVertexAttribIPointer(
				3, 1,
				GL_INT, sizeof(vtx_fonts_t),
				(const void_t)offsetof(vtx_fonts_t, atlas)
			));
			glCheck(glEnableVertexAttribArray(4));
			glCheck(glVertexAttribIPointer(
				4, 1,
				GL_INT, sizeof(vtx_fonts_t),
				(const void_t)offsetof(vtx_fonts_t, table)
			));
		};
	}
	if (result.length == 0) {
		synao_log("Warning! vertex_spec_t result has a length of zero!\n");
	}
	return result;
}
