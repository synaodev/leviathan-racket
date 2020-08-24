#include "./vertex_buffer.hpp"
#include "./const_buffer.hpp"
#include "./glcheck.hpp"

#include <utility>

vertex_buffer_t::vertex_buffer_t() :
	primitive(primitive_t::Points),
	usage(buffer_usage_t::Static),
	immuts(false),
	specify(),
	arrays(0),
	buffer(0),
	elemts(0),
	vertex_count(0),
	index_count(0)
{

}

vertex_buffer_t::vertex_buffer_t(vertex_buffer_t&& that) noexcept : vertex_buffer_t() {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(usage, that.usage);
		std::swap(immuts, that.immuts);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(elemts, that.elemts);
		std::swap(vertex_count, that.vertex_count);
		std::swap(index_count, that.index_count);
	}
}

vertex_buffer_t& vertex_buffer_t::operator=(vertex_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(usage, that.usage);
		std::swap(immuts, that.immuts);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(elemts, that.elemts);
		std::swap(vertex_count, that.vertex_count);
		std::swap(index_count, that.index_count);
	}
	return *this;
}

vertex_buffer_t::~vertex_buffer_t() {
	this->destroy();
}

void vertex_buffer_t::setup(primitive_t primitive, buffer_usage_t usage, vertex_spec_t specify) {
	this->destroy();
	this->primitive = primitive;
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

	if (!elemts) {
		glCheck(glGenBuffers(1, &elemts));
	}
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemts));

	if (specify.detail != nullptr) {
		specify.detail();
	}
	glCheck(glBindVertexArray(0));
	glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void vertex_buffer_t::create(arch_t vertex_count, arch_t index_count) {
	if (arrays != 0 and !immuts) {
		this->vertex_count = vertex_count;
		this->index_count = index_count;

		uint_t gl_enum = gfx_t::get_buffer_usage_gl_enum(usage);

		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		glCheck(glBufferData(GL_ARRAY_BUFFER, specify.length * vertex_count, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemts));
		glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * index_count, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}

void vertex_buffer_t::create_immutable(arch_t vertex_count, arch_t index_count) {
	if (const_buffer_t::has_immutable_option()) {
		if (arrays != 0 and !immuts) {
			this->vertex_count = vertex_count;
			this->index_count = index_count;

			uint_t gl_enum = gfx_t::get_buffer_usage_gl_enum(usage);

			glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
			glCheck(glBufferStorage(GL_ARRAY_BUFFER, specify.length * vertex_count, nullptr, GL_DYNAMIC_STORAGE_BIT));
			glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemts));
			glCheck(glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * index_count, nullptr, GL_DYNAMIC_STORAGE_BIT));
			glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
			glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}
	}
}

void vertex_buffer_t::destroy() {
	if (buffer != 0) {
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &buffer));
		buffer = 0;
	}
	if (elemts != 0) {
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &elemts));
		elemts = 0;
	}
	if (arrays != 0) {
		glCheck(glBindVertexArray(0));
		glCheck(glDeleteVertexArrays(1, &arrays));
		arrays = 0;
	}
	immuts = false;
	vertex_count = 0;
	index_count = 0;
}

bool vertex_buffer_t::update(const vertex_t* vertices, arch_t count, arch_t offset) {
	if (!arrays) {
		return false;
	} else if (!vertices) {
		return false;
	} else if (count > vertex_count) {
		return false;
	} else if (offset and (count + offset > vertex_count)) {
		return false;
	}
	glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	glCheck(glBufferSubData(GL_ARRAY_BUFFER, specify.length * offset, specify.length * count, vertices));
	glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	return true;
}

bool vertex_buffer_t::update(const vertex_t* vertices, arch_t count) {
	return this->update(vertices, count, 0);
}

bool vertex_buffer_t::update(const vertex_t* vertices) {
	return this->update(vertices, vertex_count, 0);
}

bool vertex_buffer_t::update(const uint16_t* indices, arch_t count, arch_t offset) {
	if (!arrays) {
		return false;
	} else if (!indices) {
		return false;
	} else if (count > index_count) {
		return false;
	} else if (offset and (count + offset > index_count)) {
		return false;
	}
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemts));
	glCheck(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * offset, sizeof(uint16_t) * count, indices));
	glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	return true;
}

bool vertex_buffer_t::update(const uint16_t* indices, arch_t count) {
	return this->update(indices, count, 0);
}

bool vertex_buffer_t::update(const uint16_t* indices) {
	return this->update(indices, index_count, 0);
}

void vertex_buffer_t::draw(arch_t count) const {
	if (arrays != 0) {
		glCheck(glBindVertexArray(arrays));
		glCheck(glDrawElements(
			gfx_t::get_primitive_gl_enum(primitive),
			static_cast<uint_t>(count),
			GL_UNSIGNED_SHORT,
			nullptr
		));
		glCheck(glBindVertexArray(0));
	}
}

void vertex_buffer_t::draw() const {
	this->draw(index_count);
}

bool vertex_buffer_t::valid() const {
	return arrays != 0;
}

primitive_t vertex_buffer_t::get_primitive() const {
	return primitive;
}

buffer_usage_t vertex_buffer_t::get_usage() const {
	return usage;
}

arch_t vertex_buffer_t::get_vertex_count() const {
	return vertex_count;
}

arch_t vertex_buffer_t::get_index_count() const {
	return index_count;
}
