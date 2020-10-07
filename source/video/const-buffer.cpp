#include "./const-buffer.hpp"
#include "./gl-check.hpp"

#include <utility>

const_buffer_t::const_buffer_t() :
	usage(buffer_usage_t::Static),
	immuts(false),
	handle(0),
	length(0)
{

}

const_buffer_t::const_buffer_t(const_buffer_t&& that) noexcept : const_buffer_t() {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(immuts, that.immuts);
		std::swap(handle, that.handle);
		std::swap(length, that.length);
	}
}

const_buffer_t& const_buffer_t::operator=(const_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(immuts, that.immuts);
		std::swap(handle, that.handle);
		std::swap(length, that.length);
	}
	return *this;
}

const_buffer_t::~const_buffer_t() {
	this->destroy();
}

void const_buffer_t::setup(buffer_usage_t usage) {
	this->destroy();
	this->usage = usage;
	if (!handle) {
		glCheck(glGenBuffers(1, &handle));
	}
}

void const_buffer_t::create(arch_t length) {
	if (handle != 0 and !immuts) {
		uint_t gl_enum = gfx_t::get_buffer_usage_gl_enum(usage);

		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, handle));
		glCheck(glBufferData(GL_UNIFORM_BUFFER, length, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		this->length = length;
	}
}

void const_buffer_t::create_immutable(arch_t length) {
	if (const_buffer_t::has_immutable_option()) {
		if (handle != 0 and !immuts) {
			// uint_t flags = usage != buffer_usage_t::Static ? GL_DYNAMIC_STORAGE_BIT : 0;
			glCheck(glBindBuffer(GL_UNIFORM_BUFFER, handle));
			glCheck(glBufferStorage(GL_UNIFORM_BUFFER, length, nullptr, GL_DYNAMIC_STORAGE_BIT));
			glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
			this->immuts = true;
			this->length = length;
		}
	}
}

void const_buffer_t::destroy() {
	if (handle != 0) {
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &handle));
		handle = 0;
	}
	immuts = false;
	length = 0;
}

bool const_buffer_t::update(const void_t pointer, arch_t count, arch_t offset) {
	if (!handle) {
		return false;
	} else if (!pointer) {
		return false;
	} else if (count > length) {
		return false;
	} else if (offset and (count + offset > length)) {
		return false;
	}
	glCheck(glBindBuffer(GL_UNIFORM_BUFFER, handle));
	glCheck(glBufferSubData(GL_UNIFORM_BUFFER, offset, count, pointer));
	glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	return true;
}

bool const_buffer_t::update(const void_t pointer, arch_t count) {
	return this->update(pointer, count, 0);
}

bool const_buffer_t::update(const void_t pointer) {
	return this->update(pointer, length, 0);
}

bool const_buffer_t::valid() const {
	return handle != 0;
}

bool const_buffer_t::immutable() const {
	return immuts;
}

buffer_usage_t const_buffer_t::get_usage() const {
	return usage;
}

arch_t const_buffer_t::get_length() const {
	return length;
}

bool const_buffer_t::has_immutable_option() {
	return opengl_version[0] == 4 and opengl_version[1] >= 4;
}
