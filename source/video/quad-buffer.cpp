#include "./quad-buffer.hpp"
#include "./const-buffer.hpp"
#include "./gl-check.hpp"

#include <limits>
#include <glm/common.hpp>

quad_allocator_t::quad_allocator_t() :
	primitive(primitive_t::Triangles),
	handle(0),
	length(0)
{

}

quad_allocator_t::quad_allocator_t(quad_allocator_t&& that) noexcept : quad_allocator_t() {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(handle, that.handle);
		std::swap(length, that.length);
	}
}

quad_allocator_t& quad_allocator_t::operator=(quad_allocator_t&& that) noexcept {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(handle, that.handle);
		std::swap(length, that.length);
	}
	return *this;
}

quad_allocator_t::~quad_allocator_t() {
	this->destroy();
}

bool quad_allocator_t::create(primitive_t primitive, arch_t length) {
	if (handle != 0) {
		return false;
	}
	if (primitive != primitive_t::Triangles and primitive != primitive_t::TriangleStrip) {
		return false;
	}
	if (length == 0 or length > UINT16_MAX) {
		return false;
	}
	this->primitive = primitive;
	this->length = length;

	auto indices = quad_allocator_t::generate(length, 0, primitive);

	glCheck(glGenBuffers(1, &handle));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle));

	if (const_buffer_t::has_immutable_option()) {
		glCheck(glBufferStorage(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(decltype(indices)::value_type) * indices.size(),
			indices.data(), 0
		));
	} else {
		glCheck(glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(decltype(indices)::value_type) * indices.size(),
			indices.data(),
			GL_STATIC_DRAW
		));
	}

	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	return true;
}

void quad_allocator_t::destroy() {
	if (handle != 0) {
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &handle));
		handle = 0;
	}
	primitive = primitive_t::Triangles;
	length = 0;
}

void quad_allocator_t::bind(bool_t value) const {
	if (handle != 0) {
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, value ? handle : 0));
	}
}

bool quad_allocator_t::valid() const {
	return handle != 0;
}

arch_t quad_allocator_t::get_length() const {
	return length;
}

primitive_t quad_allocator_t::get_primitive() const {
	return primitive;
}

arch_t quad_allocator_t::convert(arch_t length) {
	return (length * 6) / 4;
}

std::vector<uint16_t> quad_allocator_t::generate(arch_t length, arch_t offset, primitive_t primitive) {
	std::vector<uint16_t> result(quad_allocator_t::convert(length));
	arch_t it = 0;
	uint16_t ut = static_cast<uint16_t>(offset);
	while (it < result.size()) {
		auto ptr = &result[it];
		if (primitive != primitive_t::TriangleStrip) {
			ptr[0] = ut;
			ptr[1] = ut + 1;
			ptr[2] = ut + 2;
		} else {
			ptr[0] = glm::max((uint16_t)(ut - 1), (uint16_t)0);
			ptr[1] = ut;
			ptr[2] = ut;
		}
		ptr[3] = ut + 1;
		ptr[4] = ut + 2;
		ptr[5] = ut + 3;
		it += 6;
		ut += 4;
	}
	return result;
}

quad_buffer_t::quad_buffer_t() :
	allocator(nullptr),
	usage(buffer_usage_t::Static),
	specify(),
	arrays(0),
	buffer(0),
	length(0)
{

}

quad_buffer_t::quad_buffer_t(quad_buffer_t&& that) noexcept : quad_buffer_t() {
	if (this != &that) {
		std::swap(allocator, that.allocator);
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
}

quad_buffer_t& quad_buffer_t::operator=(quad_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(allocator, that.allocator);
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
	return *this;
}

quad_buffer_t::~quad_buffer_t() {
	this->destroy();
}

void quad_buffer_t::setup(const quad_allocator_t* allocator, buffer_usage_t usage, vertex_spec_t specify) {
	if (allocator and allocator->valid()) {
		this->destroy();
		this->allocator = allocator;
		this->usage = usage;
		this->specify = specify;
		if (!arrays) {
			glCheck(glGenVertexArrays(1, &arrays));
		}
		glCheck(glBindVertexArray(arrays));

		if (!buffer) {
			glCheck(glGenBuffers(1, &buffer));
		}
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		allocator->bind(true);

		if (specify.detail) {
			specify.detail();
		}
		glCheck(glBindVertexArray(0));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		allocator->bind(false);
	}
}

void quad_buffer_t::create(arch_t length) {
	if (allocator and allocator->valid() and arrays != 0) {
		this->length = length;

		uint_t gl_enum = gfx_t::get_buffer_usage_gl_enum(usage);

		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		glCheck(glBufferData(GL_ARRAY_BUFFER, specify.length * length, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}

void quad_buffer_t::destroy() {
	allocator = nullptr;
	if (arrays != 0) {
		glCheck(glBindVertexArray(0));
		glCheck(glDeleteVertexArrays(1, &arrays));
		arrays = 0;
	}
	if (buffer != 0) {
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &buffer));
		buffer = 0;
	}
	length = 0;
}

bool quad_buffer_t::update(const vertex_t* vertices, arch_t count, arch_t offset) {
	if (!allocator) {
		return false;
	} else if (!allocator->valid() or !arrays) {
		return false;
	} else if (!vertices) {
		return false;
	} else if (count > length) {
		return false;
	} else if (offset and (count + offset > length)) {
		return false;
	}
	glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	glCheck(glBufferSubData(GL_ARRAY_BUFFER, specify.length * offset, specify.length * count, vertices));
	glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	return true;
}

bool quad_buffer_t::update(const vertex_t* vertices, arch_t count) {
	return this->update(vertices, count, 0);
}

bool quad_buffer_t::update(const vertex_t* vertices) {
	return this->update(vertices, length, 0);
}

void quad_buffer_t::draw(arch_t count) const {
	if (allocator and allocator->valid() and arrays != 0) {
		count = quad_allocator_t::convert(count);
		glCheck(glBindVertexArray(arrays));
		glCheck(glDrawElements(
			gfx_t::get_primitive_gl_enum(allocator->get_primitive()),
			static_cast<uint_t>(count),
			GL_UNSIGNED_SHORT,
			nullptr
		));
		glCheck(glBindVertexArray(0));
	}
}

void quad_buffer_t::draw() const {
	this->draw(length);
}

buffer_usage_t quad_buffer_t::get_usage() const {
	return usage;
}

arch_t quad_buffer_t::get_length() const {
	return length;
}
