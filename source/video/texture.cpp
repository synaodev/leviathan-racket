#include "./texture.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"
#include "../utility/thread-pool.hpp"

static constexpr sint_t kMaxLayers = 64;

bool sampler_t::has_immutable_option() {
	return glTexStorage2D != nullptr;
}

bool sampler_t::has_azdo() {
	return glCreateTextures != nullptr;
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
		std::swap(handles, that.handles);
	}
}

sampler_allocator_t& sampler_allocator_t::operator=(sampler_allocator_t&& that) noexcept {
	if (this != &that) {
		std::swap(handles, that.handles);
	}
	return *this;
}

sampler_data_t& sampler_allocator_t::get(const glm::ivec2& dimensions, pixel_format_t format) {
	if (handles.find(dimensions) != handles.end()) {
		return handles[dimensions];
	}
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
	auto& ref = handles[dimensions];
	ref.id = handle;
	ref.type = GL_TEXTURE_2D_ARRAY;
	ref.count = 0;
	return ref;
}

const sampler_data_t& sampler_allocator_t::get(const glm::ivec2& dimensions) const {
	auto it = handles.find(dimensions);
	if (it != handles.end()) {
		return it->second;
	}
	synao_log("Warning! Sampler allocator did not retrieve the sampler! This message shouldn't print!\n");
	static const sampler_data_t kNullHandle = sampler_data_t{};
	return kNullHandle;
}

texture_t::texture_t(texture_t&& that) noexcept : texture_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(format, that.format);
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
		std::swap(format, that.format);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);
	}
	return *this;
}

texture_t::~texture_t() {
	this->destroy();
}

void texture_t::load(const std::string& full_path, pixel_format_t format, sampler_allocator_t& allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = &allocator;
	this->format = format;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void texture_t::destroy() {
	ready = false;
	if (future.valid()) {
		auto result = future.get();
	}
	allocator = nullptr;
	format = pixel_format_t::Invalid;
	name = 0;
	dimensions = glm::zero<glm::ivec2>();
}

void texture_t::assure() {
	if (!ready and future.valid()) {
		// Load texture now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->get(dimensions, format);
			this->dimensions = dimensions;
			this->name = handle.count++;

			if (name < kMaxLayers) {
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle.id));
				glCheck(glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 0, 0, 0,
					name, dimensions.x, dimensions.y, 1,
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
		return allocator->get(dimensions).id;
	}
	return 0;
}

sint_t texture_t::get_name() const {
	this->assure();
	return name;
}

real_t texture_t::get_convert(real_t index) const {
	this->assure();
	if (dimensions.y > 0) {
		return index / static_cast<real_t>(dimensions.y);
	}
	return 0.0f;
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

/*palette_t::palette_t(palette_t&& that) noexcept : palette_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(format, that.format);
	}
}

palette_t& palette_t::operator=(palette_t&& that) noexcept {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(format, that.format);
	}
	return *this;
}

palette_t::~palette_t() {
	this->destroy();
}

void palette_t::load(const std::string& full_path, pixel_format_t format, thread_pool_t& thread_pool) {
	assert(!ready);
	this->format = format;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

bool palette_t::create(glm::ivec2 dimensions, pixel_format_t format) {
	if (!handle) {
		this->dimensions = dimensions;
		this->format = format;

		uint_t gl_enum = gfx_t::get_pixel_format_gl_enum(format);

		glCheck(glGenTextures(1, &handle));
		glCheck(glBindTexture(GL_TEXTURE_2D, handle));

		if (sampler_t::has_immutable_option()) {
			glCheck(glTexStorage2D(GL_TEXTURE_2D, 1, gl_enum, dimensions.x, dimensions.y));
		} else {
			glCheck(glTexImage2D(GL_TEXTURE_2D, 0, gl_enum, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		return true;
	}
	synao_log("Warning! Tried to overwrite existing palette!\n");
	return false;
}

void palette_t::destroy() {
	if (future.valid()) {
		auto result = future.get();
	}
	ready = false;
	if (handle != 0) {
		glCheck(glDeleteTextures(1, &handle));
		handle = 0;
	}
	dimensions	= glm::zero<glm::ivec2>();
	format		= pixel_format_t::Invalid;
}

void palette_t::assure() {
	if (!ready and future.valid()) {
		// Do palette loading now
		const image_t image = future.get();
		if (!image.empty()) {
			if (this->create(image.get_dimensions(), format)) {
				glCheck(glTexSubImage2D(
					GL_TEXTURE_2D, 0, 0, 0,
					dimensions.x, dimensions.y,
					GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
				));
				glCheck(glGenerateMipmap(GL_TEXTURE_2D));
			}
			glCheck(glBindTexture(GL_TEXTURE_2D, 0));
		}
		ready = true;
	}
}

void palette_t::assure() const {
	if (!ready) {
		const_cast<palette_t*>(this)->assure();
	}
}

glm::vec2 palette_t::get_dimensions() const {
	this->assure();
	return glm::vec2(dimensions);
}

glm::vec2 palette_t::get_inverse_dimensions() const {
	this->assure();
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 palette_t::get_integral_dimensions() const {
	this->assure();
	return dimensions;
}

real_t palette_t::convert(real_t index) const {
	this->assure();
	if (dimensions.y > 0) {
		return index / static_cast<real_t>(dimensions.y);
	}
	return 0.0f;
}*/
