#include "./vertex_buffer.hpp"
#include "./glcheck.hpp"

#include <utility>

vertex_buffer_t::vertex_buffer_t() :
	primitive(primitive_t::Points),
	usage(buffer_usage_t::Static),
	specify(),
	arrays(0),
	buffer(0),
	elemts(0),
	vtx_count(0),
	idx_count(0)
{

}

vertex_buffer_t::vertex_buffer_t(vertex_buffer_t&& that) noexcept : vertex_buffer_t() {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(elemts, that.elemts);
		std::swap(vtx_count, that.vtx_count);
		std::swap(idx_count, that.idx_count);
	}
}

vertex_buffer_t& vertex_buffer_t::operator=(vertex_buffer_t&& that) noexcept {
	if (this != &that) {
		std::swap(primitive, that.primitive);
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(elemts, that.elemts);
		std::swap(vtx_count, that.vtx_count);
		std::swap(idx_count, that.idx_count);
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

void vertex_buffer_t::create(arch_t vtx_count, arch_t idx_count) {
	if (arrays != 0) {
		this->vtx_count = vtx_count;
		this->idx_count = idx_count;

		uint_t gl_enum = gfx_t::get_buffer_usage_gl_enum(usage);

		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		glCheck(glBufferData(GL_ARRAY_BUFFER, specify.length * vtx_count, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemts));
		glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * idx_count, nullptr, gl_enum));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
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
	vtx_count = 0;
	idx_count = 0;
}

bool vertex_buffer_t::update(const vertex_t* vertices, arch_t count, arch_t offset) {
	if (!arrays) {
		return false;
	} else if (!vertices) {
		return false;
	} else if (count > vtx_count) {
		return false;
	} else if (offset and (count + offset > vtx_count)) {
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
	return this->update(vertices, vtx_count, 0);
}

bool vertex_buffer_t::update(const uint16_t* indices, arch_t count, arch_t offset) {
	if (!arrays) {
		return false;
	} else if (!indices) {
		return false;
	} else if (count > idx_count) {
		return false;
	} else if (offset and (count + offset > idx_count)) {
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
	return this->update(indices, idx_count, 0);
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
	this->draw(idx_count);
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

arch_t vertex_buffer_t::get_vtx_count() const {
	return vtx_count;
}

arch_t vertex_buffer_t::get_idx_count() const {
	return idx_count;
}
