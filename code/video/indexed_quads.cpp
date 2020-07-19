#include "./indexed_quads.hpp"
#include "./const_buffer.hpp"
#include "./glcheck.hpp"

primitive_t indexed_quads_t::primitive = primitive_t::Triangles;
uint_t indexed_quads_t::elemts = 0;

bool indexed_quads_t::allocate_indexer(arch_t length, primitive_t primitive) {
	if (primitive != primitive_t::Triangles and primitive != primitive_t::TriangleStrip) {
		return false;
	}
	if (!indexed_quads_t::elemts and length != 0) {
		indexed_quads_t::primitive = primitive;
		auto indices = indexed_quads_t::generate(length, 0, primitive);
		glCheck(glGenBuffers(1, &indexed_quads_t::elemts));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexed_quads_t::elemts));
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
				buffer_usage_t::Static
			));
		}
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		return true;
	}
	return false;
}

bool indexed_quads_t::release_indexer() {
	if (indexed_quads_t::elemts != 0) {
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glCheck(glDeleteBuffers(1, &indexed_quads_t::elemts));
		indexed_quads_t::elemts = 0;
		indexed_quads_t::primitive = primitive_t::Triangles;
		return true;
	}
	return false;
}

indexed_quads_t::indexed_quads_t() :
	usage(buffer_usage_t::Static),
	specify(),
	arrays(0),
	buffer(0),
	length(0)
{

}

indexed_quads_t::indexed_quads_t(indexed_quads_t&& that) noexcept : indexed_quads_t() {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
}

indexed_quads_t& indexed_quads_t::operator=(indexed_quads_t&& that) noexcept {
	if (this != &that) {
		std::swap(usage, that.usage);
		std::swap(specify, that.specify);
		std::swap(arrays, that.arrays);
		std::swap(buffer, that.buffer);
		std::swap(length, that.length);
	}
	return *this;
}

indexed_quads_t::~indexed_quads_t() {
	this->destroy();
}

void indexed_quads_t::setup(buffer_usage_t usage, vertex_spec_t specify) {
	if (indexed_quads_t::elemts != 0) {
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
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexed_quads_t::elemts));

		if (specify.detail != nullptr) {
			specify.detail();
		}
		glCheck(glBindVertexArray(0));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}

void indexed_quads_t::create(arch_t length) {
	if (indexed_quads_t::elemts != 0 and arrays != 0) {
		this->length = length;
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		glCheck(glBufferData(GL_ARRAY_BUFFER, specify.length * length, nullptr, usage));
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}

void indexed_quads_t::destroy() {
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

bool indexed_quads_t::update(const vertex_t* vertices, arch_t count, arch_t offset) {
	if (!indexed_quads_t::elemts or !arrays) {
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

bool indexed_quads_t::update(const vertex_t* vertices, arch_t count) {
	return this->update(vertices, count, 0);
}

bool indexed_quads_t::update(const vertex_t* vertices) {
	return this->update(vertices, length, 0);
}

void indexed_quads_t::draw(arch_t count) const {
	if (indexed_quads_t::elemts != 0 and arrays != 0) {
		count = indexed_quads_t::convert(count);
		glCheck(glBindVertexArray(arrays));
		glCheck(glDrawElements(indexed_quads_t::primitive, static_cast<uint_t>(count), GL_UNSIGNED_SHORT, nullptr));
		glCheck(glBindVertexArray(0));
	}
}

void indexed_quads_t::draw() const {
	this->draw(length);
}

buffer_usage_t indexed_quads_t::get_usage() const {
	return usage;
}

arch_t indexed_quads_t::get_length() const {
	return length;
}

arch_t indexed_quads_t::convert(arch_t length) {
	return (length * 6) / 4;
}

std::vector<uint16_t> indexed_quads_t::generate(arch_t length, arch_t offset, primitive_t primitive) {
	std::vector<uint16_t> result(indexed_quads_t::convert(length));
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