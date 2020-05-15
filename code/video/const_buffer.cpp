#include "./const_buffer.hpp"
#include "./glcheck.hpp"

#include <utility>

const_buffer_t::const_buffer_t() :
	usage(buffer_usage_t::Static),
	handle(0),
	length(0)
{

}

const_buffer_t::const_buffer_t(const_buffer_t&& that) noexcept : const_buffer_t() {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(handle, that.handle);
		std::swap(length, that.length);
	}
}

const_buffer_t& const_buffer_t::operator=(const_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(usage, that.usage);
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
	if (handle != 0) {
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, handle));
		glCheck(glBufferData(GL_UNIFORM_BUFFER, length, nullptr, usage));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		this->length = length;
	}
}

void const_buffer_t::destroy() {
	if (handle != 0) {
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &handle));
		handle = 0;
		length = 0;
	}
}

bool const_buffer_t::update(const optr_t pointer, arch_t count, arch_t offset) {
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

bool const_buffer_t::update(const optr_t pointer, arch_t count) {
	return this->update(pointer, count, 0);
}

bool const_buffer_t::update(const optr_t pointer) {
	return this->update(pointer, length, 0);
}

bool const_buffer_t::valid() const {
	return handle != 0;
}

buffer_usage_t const_buffer_t::get_usage() const {
	return usage;
}

arch_t const_buffer_t::get_length() const {
	return length;
}

bool const_buffer_t::has_immutable_storage() {
#ifndef __EMSCRIPTEN__
	return glBufferStorage != nullptr;
#endif // __EMSCRIPTEN__
	return false;
}