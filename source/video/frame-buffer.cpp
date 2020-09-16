#include "./frame-buffer.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"

static constexpr uint_t kDrawTotal = 32;

static uint_t* kDrawAttach() {
	static uint_t array[kDrawTotal] = {};
	static bool_t empty = true;
	if (empty) {
		empty = false;
		for (uint_t i = 0; i < kDrawTotal; ++i) {
			array[i] = GL_COLOR_ATTACHMENT0 + i;
		}
	}
	return array;
}

color_buffer_t::color_buffer_t(color_buffer_t&& that) noexcept : color_buffer_t() {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(layers, that.layers);
		std::swap(format, that.format);
	}
}

color_buffer_t& color_buffer_t::operator=(color_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(layers, that.layers);
		std::swap(format, that.format);
	}
	return *this;
}

color_buffer_t::~color_buffer_t() {
	this->destroy();
}

bool color_buffer_t::create(glm::ivec2 dimensions, uint_t layers, pixel_format_t format) {
	if (!handle) {
		assert(layers > 0);
		this->dimensions = dimensions;
		this->layers = layers;
		this->format = format;

		uint_t gl_enum = gfx_t::get_pixel_format_gl_enum(format);

		glCheck(glGenTextures(1, &handle));
		if (layers > 1) {
			glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle));
			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, gl_enum, dimensions.x, dimensions.y, layers));
			} else {
				glCheck(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, gl_enum, dimensions.x, dimensions.y, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));

			glCheck(glBindTexture(GL_TEXTURE_2D, 0));
			for (uint_t it = 0; it < layers; ++it) {
				glCheck(glFramebufferTextureLayer(
					GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + it,
					handle, 0, it
				));
			}
		} else {
			glCheck(glBindTexture(GL_TEXTURE_2D, handle));
			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage2D(GL_TEXTURE_2D, 4, gl_enum, dimensions.x, dimensions.y));
			} else {
				glCheck(glTexImage2D(GL_TEXTURE_2D, 0, gl_enum, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));

			glCheck(glBindTexture(GL_TEXTURE_2D, 0));
			glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				handle, 0
			));
		}
		return true;
	}
	synao_log("Warning! Tried to overwrite existing color buffer!\n");
	return false;
}

void color_buffer_t::destroy() {
	if (handle != 0) {
		glCheck(glDeleteTextures(1, &handle));
		handle = 0;
	}
	dimensions = glm::zero<glm::ivec2>();
	layers = 0;
	format = pixel_format_t::Invalid;
}

bool color_buffer_t::valid() {
	return handle != 0;
}

uint_t color_buffer_t::get_layers() const {
	return layers;
}

glm::vec2 color_buffer_t::get_dimensions() const {
	return glm::vec2(dimensions);
}

glm::vec2 color_buffer_t::get_inverse_dimensions() const {
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 color_buffer_t::get_integral_dimensions() const {
	return dimensions;
}

depth_buffer_t::depth_buffer_t(depth_buffer_t&& that) noexcept : depth_buffer_t() {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(compress, that.compress);
	}
}

depth_buffer_t& depth_buffer_t::operator=(depth_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(handle, that.handle);
		std::swap(dimensions, that.dimensions);
		std::swap(compress, that.compress);
	}
	return *this;
}

depth_buffer_t::~depth_buffer_t() {
	this->destroy();
}

void depth_buffer_t::destroy() {
	if (!handle) {
		if (compress) {
			glCheck(glDeleteRenderbuffers(1, &handle));
		} else {
			glCheck(glDeleteTextures(1, &handle));
		}
		handle = 0;
	}
	dimensions = glm::zero<glm::ivec2>();
	compress = false;
}

bool depth_buffer_t::create(glm::ivec2 dimensions, bool_t compress) {
	if (!handle) {
		this->compress = compress;
		this->dimensions = dimensions;
		if (compress) {
			glCheck(glGenRenderbuffers(1, &handle));
			glCheck(glBindRenderbuffer(GL_RENDERBUFFER, handle));
			glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y));
			glCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));
			glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, handle));
		} else {
			glCheck(glGenTextures(1, &handle));
			glCheck(glBindTexture(GL_TEXTURE_2D, handle));
			if (sampler_t::has_immutable_option()) {
				glCheck(glTexStorage2D(GL_TEXTURE_2D, 4, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y));
			} else {
				glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_BYTE, nullptr));
			}
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

			glCheck(glBindTexture(GL_TEXTURE_2D, 0));
			glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, handle, 0));
		}
		return true;
	}
	return false;
}

bool depth_buffer_t::valid() const {
	return handle != 0;
}

glm::vec2 depth_buffer_t::get_dimensions() const {
	return glm::vec2(dimensions);
}

glm::vec2 depth_buffer_t::get_inverse_dimensions() const {
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 depth_buffer_t::get_integral_dimensions() const {
	return dimensions;
}

frame_buffer_t::frame_buffer_t(frame_buffer_t&& that) noexcept : frame_buffer_t() {
	if (this != &that) {
		std::swap(ready, that.ready);
		std::swap(handle, that.handle);
		std::swap(color_buffer, that.color_buffer);
		std::swap(depth_buffer, that.depth_buffer);
	}
}

frame_buffer_t& frame_buffer_t::operator=(frame_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(ready, that.ready);
		std::swap(handle, that.handle);
		std::swap(color_buffer, that.color_buffer);
		std::swap(depth_buffer, that.depth_buffer);
	}
	return *this;
}

frame_buffer_t::~frame_buffer_t() {
	this->destroy();
}

bool frame_buffer_t::create(glm::ivec2 dimensions, uint_t layers, pixel_format_t format) {
	if (!ready and layers < kDrawTotal) {
		if (!handle) {
			glCheck(glGenFramebuffers(1, &handle));
		}
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, handle));
		color_buffer.create(dimensions, layers, format);
		glCheck(glDrawBuffers(layers, kDrawAttach()));

		uint_t state = 0;
		glCheck(state = glCheckFramebufferStatus(GL_FRAMEBUFFER));
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		if (state == GL_FRAMEBUFFER_COMPLETE) {
			ready = true;
		} else {
			this->destroy();
		}
	}
	return ready;
}

bool frame_buffer_t::create(glm::ivec2 dimensions, uint_t layers, pixel_format_t format, bool_t compress) {
	if (!ready and layers < kDrawTotal) {
		if (!handle) {
			glCheck(glGenFramebuffers(1, &handle));
		}
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, handle));
		color_buffer.create(dimensions, layers, format);
		depth_buffer.create(dimensions, compress);
		glCheck(glDrawBuffers(layers, kDrawAttach()));

		uint_t state = 0;
		glCheck(state = glCheckFramebufferStatus(GL_FRAMEBUFFER));
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		if (state == GL_FRAMEBUFFER_COMPLETE) {
			ready = true;
		} else {
			this->destroy();
		}
	}
	return ready;
}

void frame_buffer_t::destroy() {
	if (depth_buffer.valid()) {
		depth_buffer.destroy();
	}
	if (color_buffer.valid() != 0) {
		color_buffer.destroy();
	}
	if (handle != 0) {
		frame_buffer_t::bind(nullptr);
		glCheck(glDeleteFramebuffers(1, &handle));
		handle = 0;
	}
	ready = false;
}

void frame_buffer_t::bind(const frame_buffer_t* frame_buffer, frame_buffer_binding_t binding, arch_t index) {
	static const frame_buffer_t* main = nullptr;
	static const frame_buffer_t* read = nullptr;
	static const frame_buffer_t* write = nullptr;
	switch (binding) {
	case frame_buffer_binding_t::Main: {
		if (main != frame_buffer) {
			main = frame_buffer;
			if (frame_buffer != nullptr and frame_buffer->ready) {
				uint_t layers = frame_buffer->color_buffer.get_layers();
				glCheck(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle));
				glCheck(glDrawBuffers(layers, kDrawAttach()));
			}
			else {
				glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			}
		}
		break;
	}
	case frame_buffer_binding_t::Read: {
		if (read != frame_buffer) {
			read = frame_buffer;
			if (frame_buffer != nullptr and frame_buffer->ready) {
				glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer->handle));
				glCheck(glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<uint_t>(index)));
			}
			else {
				glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
			}
		}
		break;
	}
	case frame_buffer_binding_t::Write: {
		if (write != frame_buffer) {
			write = frame_buffer;
			if (frame_buffer != nullptr and frame_buffer->ready) {
				glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer->handle));
				glCheck(glDrawBuffer(GL_COLOR_ATTACHMENT0 + static_cast<uint_t>(index)));
			}
			else {
				glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
			}
		}
		break;
	}
	default: {
		break;
	}
	}
}

void frame_buffer_t::bind(const frame_buffer_t* frame_buffer) {
	frame_buffer_t::bind(frame_buffer, frame_buffer_binding_t::Main, 0);
}

void frame_buffer_t::blit(glm::ivec2 source_position, glm::ivec2 source_dimensions, glm::ivec2 destination_position, glm::ivec2 destination_dimensions) {
	glCheck(glBlitFramebuffer(
		source_position.x, source_position.y,
		source_dimensions.x, source_dimensions.y,
		destination_position.x, destination_position.y,
		destination_dimensions.x, destination_dimensions.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	));
}

void frame_buffer_t::blit(glm::ivec2 source_dimensions, glm::ivec2 destination_dimensions) {
	frame_buffer_t::blit(
		glm::zero<glm::ivec2>(),
		source_dimensions,
		glm::zero<glm::ivec2>(),
		destination_dimensions
	);
}

void frame_buffer_t::clear(glm::ivec2 dimensions, glm::vec4 color) {
	frame_buffer_t::bind(nullptr);
	frame_buffer_t::viewport(dimensions);
	frame_buffer_t::bucket(color);
	glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void frame_buffer_t::clear(glm::ivec2 dimensions) {
	frame_buffer_t::clear(dimensions, glm::zero<glm::vec4>());
}

void frame_buffer_t::clear(const frame_buffer_t* frame_buffer, glm::vec4 color) {
	if (frame_buffer != nullptr and frame_buffer->ready) {
		frame_buffer_t::bind(frame_buffer);
		frame_buffer_t::viewport(frame_buffer);
		frame_buffer_t::bucket(color);
		glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
}

void frame_buffer_t::clear(const frame_buffer_t* frame_buffer) {
	frame_buffer_t::clear(frame_buffer, glm::zero<glm::vec4>());
}

void frame_buffer_t::viewport(glm::ivec2 dimensions) {
	static glm::ivec2 current = glm::zero<glm::ivec2>();
	if (current != dimensions) {
		current = dimensions;
		glCheck(glViewport(0, 0, dimensions.x, dimensions.y));
	}
}

void frame_buffer_t::viewport(const frame_buffer_t* frame_buffer) {
	if (frame_buffer != nullptr) {
		glm::ivec2 dimensions = frame_buffer->color_buffer.get_integral_dimensions();
		frame_buffer_t::viewport(dimensions);
	}
}

void frame_buffer_t::bucket(glm::vec4 color) {
	static glm::vec4 current = glm::zero<glm::vec4>();
	if (current != color) {
		current = color;
		glCheck(glClearColor(color.x, color.y, color.z, color.w));
	}
}

bool frame_buffer_t::valid() const {
	return ready;
}

glm::vec2 frame_buffer_t::get_dimensions() const {
	return color_buffer.get_dimensions();
}

glm::ivec2 frame_buffer_t::get_integral_dimensions() const {
	return color_buffer.get_integral_dimensions();
}

const color_buffer_t* frame_buffer_t::get_color_buffer() const {
	return &color_buffer;
}

const depth_buffer_t* frame_buffer_t::get_depth_buffer() const {
	return &depth_buffer;
}
