#include "./texture.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"
#include "../utility/thread-pool.hpp"

static constexpr sint_t kMaxLayers = 256;

bool sampler_t::has_immutable_option() {
	return glTexStorage2D != nullptr;
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
		std::swap(textures, that.textures);
		std::swap(palettes, that.palettes);
		std::swap(texture_format, that.texture_format);
		std::swap(palette_format, that.palette_format);
	}
}

sampler_allocator_t& sampler_allocator_t::operator=(sampler_allocator_t&& that) noexcept {
	if (this != &that) {
		std::swap(textures, that.textures);
		std::swap(palettes, that.palettes);
		std::swap(texture_format, that.texture_format);
		std::swap(palette_format, that.palette_format);
	}
	return *this;
}

sampler_data_t& sampler_allocator_t::texture(const glm::ivec2& dimensions) {
	if (textures.find(dimensions) != textures.end()) {
		return textures[dimensions];
	}
	auto& ref = textures[dimensions];
	ref = this->generate(dimensions, texture_format);
	return ref;
}

const sampler_data_t& sampler_allocator_t::texture_const(const glm::ivec2& dimensions) const {
#ifndef LEVIATHAN_BUILD_DEBUG
	return textures.at(dimensions);
#else
	auto it = textures.find(dimensions);
	if (it != textures.end()) {
		return it->second;
	}
	synao_log("Warning! Sampler allocator did not retrieve the texture! This message shouldn't print!\n");
	static const sampler_data_t kNullHandle = sampler_data_t{};
	return kNullHandle;
#endif
}

sampler_data_t& sampler_allocator_t::palette(const glm::ivec2& dimensions) {
	if (palettes.find(dimensions) != palettes.end()) {
		return palettes[dimensions];
	}
	auto& ref = palettes[dimensions];
	ref = this->generate(dimensions, palette_format);
	return ref;
}

const sampler_data_t& sampler_allocator_t::palette_const(const glm::ivec2& dimensions) const {
#ifndef LEVIATHAN_BUILD_DEBUG
	return palettes.at(dimensions);
#else
	auto it = palettes.find(dimensions);
	if (it != palettes.end()) {
		return it->second;
	}
	synao_log("Warning! Sampler allocator did not retrieve the palette! This message shouldn't print!\n");
	static const sampler_data_t kNullHandle = sampler_data_t{};
	return kNullHandle;
#endif
}

sampler_data_t sampler_allocator_t::generate(const glm::ivec2& dimensions, pixel_format_t format) {
	uint_t handle = 0;
	glCheck(glGenTextures(1, &handle));
	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle));
	if (sampler_t::has_immutable_option()) {
		glCheck(glTexStorage3D(
			GL_TEXTURE_2D_ARRAY, 4, gfx_t::get_pixel_format_gl_enum(format),
			dimensions.x, dimensions.y, kMaxLayers
		));
	} else {
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, gfx_t::get_pixel_format_gl_enum(format),
			dimensions.x, dimensions.y, kMaxLayers,
			0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
		));
	}
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	return sampler_data_t(
		handle,
		GL_TEXTURE_2D_ARRAY,
		0
	);
}

texture_t::texture_t(texture_t&& that) noexcept : texture_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);
	}
}

texture_t& texture_t::operator=(texture_t&& that) noexcept {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);
	}
	return *this;
}

texture_t::~texture_t() {
	if (future.valid()) {
		auto result = future.get();
	}
}

void texture_t::load(const std::string& full_path, sampler_allocator_t& allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = &allocator;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void texture_t::assure() {
	if (!ready and future.valid()) {
		// Load texture now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->texture(dimensions);
			this->dimensions = dimensions;
			this->name = handle.count;
			if (handle.count < kMaxLayers) {
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle.id));
				glCheck(glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 0, 0, 0,
					handle.count++, dimensions.x, dimensions.y, 1,
					GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
				));
				glCheck(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
			} else {
				synao_log("Warning! This texture handle has no space left! This message should not print!\n");
			}
		}
		ready = true;
	}
}

void texture_t::assure() const {
	if (!ready) {
		const_cast<texture_t*>(this)->assure();
	}
}

bool texture_t::valid() const {
	return dimensions != glm::zero<glm::ivec2>();
}

uint_t texture_t::get_handle() const {
	this->assure();
	if (allocator != nullptr) {
		return allocator->texture_const(dimensions).id;
	}
	return 0;
}

sint_t texture_t::get_name() const {
	this->assure();
	return name;
}

glm::vec2 texture_t::get_dimensions() const {
	this->assure();
	return glm::vec2(dimensions);
}

glm::vec2 texture_t::get_inverse_dimensions() const {
	this->assure();
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 texture_t::get_integral_dimensions() const {
	this->assure();
	return dimensions;
}

palette_t::palette_t(palette_t&& that) noexcept : palette_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);
	}
}

palette_t& palette_t::operator=(palette_t&& that) noexcept {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);
	}
	return *this;
}

palette_t::~palette_t() {
	if (future.valid()) {
		auto result = future.get();
	}
}

void palette_t::load(const std::string& full_path, sampler_allocator_t& allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = &allocator;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void palette_t::assure() {
	if (!ready and future.valid()) {
		// Load palette now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->palette(dimensions);
			this->dimensions = dimensions;
			this->name = handle.count;
			if ((handle.count + dimensions.y) < kMaxLayers) {
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle.id));
				for (sint_t it = 0; it < dimensions.y; it++) {
					glCheck(glTexSubImage3D(
						GL_TEXTURE_2D_ARRAY, 0, 0, 0,
						handle.count++, dimensions.x, dimensions.y, 1,
						GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
					));
					glCheck(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
				}
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
			} else {
				synao_log("Warning! This palette handle has no space left! This message should not print!\n");
			}
		}
		ready = true;
	}
}

void palette_t::assure() const {
	if (!ready) {
		const_cast<palette_t*>(this)->assure();
	}
}

bool palette_t::valid() const {
	return dimensions != glm::zero<glm::ivec2>();
}

uint_t palette_t::get_handle() const {
	this->assure();
	if (allocator != nullptr) {
		return allocator->palette_const(dimensions).id;
	}
	return 0;
}

sint_t palette_t::get_name() const {
	this->assure();
	return name;
}

real_t palette_t::convert(real_t index) const {
	this->assure();
	if (dimensions.y > 0) {
		return index / static_cast<real_t>(dimensions.y);
	}
	return 0.0f;
}
