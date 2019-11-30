#include "./depth_buffer.hpp"
#include "./glcheck.hpp"

depth_buffer_t::depth_buffer_t() :
	handle(0),
	dimensions(0),
	compress(false)
{

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

			if (sampler_t::has_immutable_storage()) {
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