#include "./quad_list.hpp"
#include "./glcheck.hpp"

primitive_t quad_list_t::primitive = primitive_t::Triangles;
uint_t quad_list_t::elemts = 0;

bool quad_list_t::allocate_indexer(arch_t length, primitive_t primitive) {
	assert(primitive == primitive_t::Triangles or primitive == primitive_t::TriangleStrip);
	if (!quad_list_t::elemts and length != 0) {
		quad_list_t::primitive = primitive;
		auto indices = quad_list_t::generate(length, 0, primitive);
		glCheck(glGenBuffers(1, &quad_list_t::elemts));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_list_t::elemts));
		glCheck(glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(decltype(indices)::value_type) * indices.size(),
			indices.data(),
			buffer_usage_t::Static
		));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		return true;
	}
	return false;
}

bool quad_list_t::release_indexer() {
	if (quad_list_t::elemts != 0) {
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &quad_list_t::elemts));
		quad_list_t::elemts = 0;
		quad_list_t::primitive = primitive_t::Triangles;
		return true;
	}
	return false;
}

quad_list_t::quad_list_t() :
	usage(buffer_usage_t::Static),
	specify(),
	arrays(0),
	buffer(0),
	length(0)
{

}

quad_list_t::quad_list_t(quad_list_t&& that) noexcept : quad_list_t() {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
}

quad_list_t& quad_list_t::operator=(quad_list_t&& that) noexcept {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
	return *this;
}

quad_list_t::~quad_list_t() {
	this->destroy();
}

void quad_list_t::setup(buffer_usage_t usage, vertex_spec_t specify) {
	if (quad_list_t::elemts != 0) {
		this->destroy();
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
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_list_t::elemts));

		if (specify.detail != nullptr) {
			specify.detail();
		}
		glCheck(glBindVertexArray(0));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}

void quad_list_t::create(arch_t length) {
	if (quad_list_t::elemts != 0 and arrays != 0) {
		this->length = length;
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		glCheck(glBufferData(GL_ARRAY_BUFFER, specify.length * length, nullptr, usage));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}

void quad_list_t::destroy() {
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

bool quad_list_t::update(const vertex_t* vertices, arch_t count, arch_t offset) {
	if (!quad_list_t::elemts or !arrays) {
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

bool quad_list_t::update(const vertex_t* vertices, arch_t count) {
	return this->update(vertices, count, 0);
}

bool quad_list_t::update(const vertex_t* vertices) {
	return this->update(vertices, length, 0);
}

void quad_list_t::draw(arch_t count) const {
	if (quad_list_t::elemts != 0 and arrays != 0) {
		count = quad_list_t::convert(count);
		glCheck(glBindVertexArray(arrays));
		glCheck(glDrawElements(quad_list_t::primitive, static_cast<uint_t>(count), GL_UNSIGNED_SHORT, nullptr));
		glCheck(glBindVertexArray(0));
	}
}

void quad_list_t::draw() const {
	this->draw(length);
}

buffer_usage_t quad_list_t::get_usage() const {
	return usage;
}

arch_t quad_list_t::get_length() const {
	return length;
}

arch_t quad_list_t::convert(arch_t length) {
	return (length * 6) / 4;
}

std::vector<uint16_t> quad_list_t::generate(arch_t length, arch_t offset, primitive_t primitive) {
	std::vector<uint16_t> result(quad_list_t::convert(length));
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