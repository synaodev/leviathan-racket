#include "./sampler.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"

namespace {
	constexpr sint_t kWorkingUnit = GL_TEXTURE4;
	constexpr sint_t kMipMapTexs  = 4;
	constexpr sint_t kTotalTexs   = 80;
	constexpr sint_t kMipMapPals  = 1;
	constexpr sint_t kTotalPals   = 128;
	constexpr sint_t kDimensions  = 256;
	constexpr sint_t kColorsMax   = 32;
	constexpr sint_t kTotalAtlas  = 5;
}

sint_t sampler_t::get_working_unit() {
	return kWorkingUnit;
}

sint_t sampler_t::get_maximum_textures() {
	return kTotalTexs;
}

sint_t sampler_t::get_maximum_palettes() {
	return kTotalPals;
}

sint_t sampler_t::get_maximum_colors() {
	return kColorsMax;
}

sint_t sampler_t::get_maximum_atlases() {
	return kTotalAtlas;
}

bool sampler_t::has_immutable_option() {
	return opengl_version[0] == 4 and opengl_version[1] >= 2;
}

void sampler_data_t::destroy() {
	if (id != 0) {
		glCheck(glBindTexture(type, 0));
		glCheck(glDeleteTextures(1, &id));
		id = 0;
		type = 0;
		count = 0;
	}
}

sampler_data_t sampler_allocator_t::kNullHandle {};

bool sampler_allocator_t::create(pixel_format_t highest, pixel_format_t lowest) {
	const glm::ivec2 texture_dims { kDimensions, kDimensions };
	const glm::ivec2 palette_dims { kColorsMax, kTotalPals };

	this->highest = highest;
	this->lowest = lowest;
	auto& t = this->texture(texture_dims);
	auto& p = this->palette(palette_dims);
	auto& s = this->atlas(texture_dims);
	return true;
}

sampler_data_t& sampler_allocator_t::texture(const glm::ivec2& dimensions) {
	if (dimensions.x == kDimensions and dimensions.y == kDimensions) {
		if (textures.id == 0) {
			// Save previous unit and set to working unit
			sint_t previous = 0;
			glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
			glCheck(glActiveTexture(kWorkingUnit));

			uint_t handle = 0;
			glCheck(glGenTextures(1, &handle));
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage3D(
					GL_TEXTURE_2D_ARRAY, kMipMapTexs,
					gfx_t::get_pixel_format_gl_enum(highest),
					dimensions.x, dimensions.y, kTotalTexs
				));
			} else {
				glCheck(glTexImage3D(
					GL_TEXTURE_2D_ARRAY, 0,
					gfx_t::get_pixel_format_gl_enum(highest),
					dimensions.x, dimensions.y, kTotalTexs,
					0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
				));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
			} else {
				glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			}
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

			// Restore previous unit
			glCheck(glActiveTexture(previous));

			textures.id = handle;
			textures.type = GL_TEXTURE_2D_ARRAY;
			textures.count = 0;
		}
		return textures;
	}
	synao_log("Error! Texture size must be 256x256! This one is {}x{}!\n", dimensions.x, dimensions.y);
	return kNullHandle;
}

const sampler_data_t& sampler_allocator_t::texture() const {
	return textures;
}

sampler_data_t& sampler_allocator_t::palette(const glm::ivec2& dimensions) {
	if (dimensions.x == kColorsMax) {
		if (palettes.id == 0) {
			// Save previous unit and set to working unit
			sint_t previous = 0;
			glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
			glCheck(glActiveTexture(kWorkingUnit));

			uint_t handle = 0;
			glCheck(glGenTextures(1, &handle));
			glCheck(glBindTexture(GL_TEXTURE_1D_ARRAY, handle));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage2D(
					GL_TEXTURE_1D_ARRAY, kMipMapPals,
					gfx_t::get_pixel_format_gl_enum(lowest),
					dimensions.x, kTotalPals
				));
			} else {
				glCheck(glTexImage2D(
					GL_TEXTURE_1D_ARRAY, 0,
					gfx_t::get_pixel_format_gl_enum(lowest),
					dimensions.x, kTotalPals,
					0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
				));
			}
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
			} else {
				glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			}
			glCheck(glBindTexture(GL_TEXTURE_1D_ARRAY, 0));

			// Restore previous unit
			glCheck(glActiveTexture(previous));

			palettes.id = handle;
			palettes.type = GL_TEXTURE_1D_ARRAY;
			palettes.count = 0;
		}
		return palettes;
	}
	synao_log("Error! Palette size must be 32 colors! This one is {}!\n", dimensions.x);
	return kNullHandle;
}

const sampler_data_t& sampler_allocator_t::palette() const {
	return palettes;
}

sampler_data_t& sampler_allocator_t::atlas(const glm::ivec2& dimensions) {
	if (dimensions.x == kDimensions and dimensions.y == kDimensions) {
		if (atlases.id == 0) {
			// Save previous unit and set to working unit
			sint_t previous = 0;
			glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
			glCheck(glActiveTexture(kWorkingUnit));

			uint_t handle = 0;
			glCheck(glGenTextures(1, &handle));
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage3D(
					GL_TEXTURE_2D_ARRAY, kMipMapTexs,
					gfx_t::get_pixel_format_gl_enum(lowest),
					dimensions.x, dimensions.y, kTotalAtlas
				));
			} else {
				glCheck(glTexImage3D(
					GL_TEXTURE_2D_ARRAY, 0,
					gfx_t::get_pixel_format_gl_enum(lowest),
					dimensions.x, dimensions.y, kTotalAtlas,
					0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
				));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			if (sampler_t::has_immutable_option()) {
				glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
			} else {
				glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			}
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

			// Restore previous unit
			glCheck(glActiveTexture(previous));

			atlases.id = handle;
			atlases.type = GL_TEXTURE_2D_ARRAY;
			atlases.count = 0;
		}
		return atlases;
	}
	synao_log("Error! Atlas size must be 256x256! This one is {}x{}!\n", dimensions.x, dimensions.y);
	return kNullHandle;
}

const sampler_data_t& sampler_allocator_t::atlas() const {
	return atlases;
}
