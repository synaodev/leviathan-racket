#include "./sampler.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"

static constexpr sint_t kWorkingUnit = GL_TEXTURE4;
static constexpr sint_t kMipMapTexs  = 4;
static constexpr sint_t kTotalTexs   = 64;
static constexpr sint_t kMipMapPals  = 1;
static constexpr sint_t kTotalPals   = 64;
static constexpr sint_t kDimensions  = 256;
static constexpr sint_t kColorsMax   = 32;

bool sampler_t::has_immutable_option() {
	return glTexStorage2D != nullptr;
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

sampler_data_t::sampler_data_t(sampler_data_t&& that) noexcept : sampler_data_t() {
	if (this != &that) {
		std::swap(id, that.id);
		std::swap(type, that.type);
		std::swap(count, that.count);
	}
}

sampler_data_t& sampler_data_t::operator=(sampler_data_t&& that) noexcept {
	if (this != &that) {
		std::swap(id, that.id);
		std::swap(type, that.type);
		std::swap(count, that.count);
	}
	return *this;
}

sampler_data_t::~sampler_data_t() {
	if (id != 0) {
		glCheck(glBindTexture(type, 0));
		glCheck(glDeleteTextures(1, &id));
		id = 0;
		type = 0;
		count = 0;
	}
}

sampler_allocator_t::sampler_allocator_t(sampler_allocator_t&& that) noexcept : sampler_allocator_t() {
	if (this != &that) {
		std::swap(highest, that.highest);
		std::swap(lowest, that.lowest);
		std::swap(textures, that.textures);
		std::swap(palettes, that.palettes);
	}
}

sampler_allocator_t& sampler_allocator_t::operator=(sampler_allocator_t&& that) noexcept {
	if (this != &that) {
		std::swap(highest, that.highest);
		std::swap(lowest, that.lowest);
		std::swap(textures, that.textures);
		std::swap(palettes, that.palettes);
	}
	return *this;
}

bool sampler_allocator_t::create(pixel_format_t highest, pixel_format_t lowest) {
	this->highest = highest;
	this->lowest = lowest;
	auto& t = this->texture(glm::ivec2(kDimensions, kDimensions));
	auto& p = this->palette(glm::ivec2(kColorsMax, kTotalPals));
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
					GL_TEXTURE_2D_ARRAY, kMipMapTexs,
					gfx_t::get_pixel_format_gl_enum(highest),
					dimensions.x, dimensions.y, kTotalTexs,
					0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
				));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
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
	static sampler_data_t kNullHandle = sampler_data_t{};
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
					GL_TEXTURE_1D_ARRAY, kMipMapPals,
					gfx_t::get_pixel_format_gl_enum(lowest),
					dimensions.x, kTotalPals,
					0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
				));
			}
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
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
	static sampler_data_t kNullHandle = sampler_data_t{};
	return kNullHandle;
}

const sampler_data_t& sampler_allocator_t::palette() const {
	return palettes;
}
