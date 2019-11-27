#include "./frame_buffer.hpp"
#include "./glcheck.hpp"

static const uint_t kDrawAttach[] = {
	GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, 
	GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, 
	GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9
};

frame_buffer_t::frame_buffer_t() :
	ready(false),
	handle(0),
	color_buffer(),
	depth_buffer()
{

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

void frame_buffer_t::push(glm::ivec2 dimensions, arch_t length, pixel_format_t format) {
	if (!ready and !color_buffer.get_layers()) {
		if (!handle) {
			glCheck(glGenFramebuffers(1, &handle));
		}
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, handle));		
		color_buffer.color_buffer(dimensions, length, format);
	}
}

void frame_buffer_t::depth(glm::ivec2 dimensions, bool_t compress) {
	if (!ready and !depth_buffer.valid()) {
		if (!handle) {
			glCheck(glGenFramebuffers(1, &handle));
		}
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, handle));
		depth_buffer.create(dimensions, compress);
	}
}

bool frame_buffer_t::create() {
	if (!ready) {
		if (handle != 0) {
			uint_t state = 0;
			uint_t count = color_buffer.get_layers();
			glCheck(glDrawBuffers(count, kDrawAttach));
			glCheck(state = glCheckFramebufferStatus(GL_FRAMEBUFFER));
			glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			if (state == GL_FRAMEBUFFER_COMPLETE) {
				ready = true;
			} else {
				this->destroy();
			}
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
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		glCheck(glDeleteFramebuffers(1, &handle));
		handle = 0;
	}
	ready = false;
}

void frame_buffer_t::bind(const frame_buffer_t* frame_buffer) {
	if (frame_buffer != nullptr and frame_buffer->ready) {
		uint_t layers = frame_buffer->color_buffer.get_layers();
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle));
		glCheck(glDrawBuffers(layers, kDrawAttach));
	} else {
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}

void frame_buffer_t::read(const frame_buffer_t* frame_buffer, arch_t index) {
	if (frame_buffer != nullptr and frame_buffer->ready) {
		glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer->handle));
		glCheck(glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<uint_t>(index)));
	} else {
		glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	}
}

void frame_buffer_t::write(const frame_buffer_t* frame_buffer, arch_t index) {
	if (frame_buffer != nullptr and frame_buffer->ready) {
		glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer->handle));
		glCheck(glDrawBuffer(GL_COLOR_ATTACHMENT0 + static_cast<uint_t>(index)));
	} else {
		glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	}
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
	glCheck(glViewport(0, 0, dimensions.x, dimensions.y));
	glCheck(glClearColor(color.x, color.y, color.z, color.w));
	glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void frame_buffer_t::clear(glm::ivec2 dimensions) {
	frame_buffer_t::clear(dimensions, glm::zero<glm::vec4>());
}

void frame_buffer_t::clear(const frame_buffer_t* frame_buffer, glm::vec4 color) {
	if (frame_buffer != nullptr and frame_buffer->ready) {
		frame_buffer_t::bind(frame_buffer);
		const glm::ivec2 dimensions = frame_buffer->color_buffer.get_integral_dimensions();
		glCheck(glViewport(0, 0, dimensions.x, dimensions.y));
		glCheck(glClearColor(color.x, color.y, color.z, color.w));
		glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
}

void frame_buffer_t::viewport(glm::ivec2 dimensions) {
	glCheck(glViewport(0, 0, dimensions.x, dimensions.y));
}

void frame_buffer_t::viewport(const frame_buffer_t* frame_buffer) {
	if (frame_buffer != nullptr) {
		frame_buffer_t::viewport(
			frame_buffer->color_buffer.get_integral_dimensions()
		);
	}
}

void frame_buffer_t::clear(const frame_buffer_t* frame_buffer) {
	frame_buffer_t::clear(frame_buffer, glm::zero<glm::vec4>());
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

const texture_t* frame_buffer_t::get_color_buffer() const {
	return &color_buffer;
}

const depth_buffer_t* frame_buffer_t::get_depth_buffer() const {
	return &depth_buffer;
}