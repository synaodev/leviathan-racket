#include "./vertex_pool.hpp"

void vertex_pool_t::setup(vertex_spec_t specify) {
	this->specify = specify;
	this->clear();
}

void vertex_pool_t::clear() {
	memory.clear();
}

bool vertex_pool_t::empty() const {
	return memory.empty();
}

arch_t vertex_pool_t::size() const {
	if (specify.length > 0) {
		return memory.size() / specify.length;
	}
	return 0;
}

void vertex_pool_t::resize(arch_t length) {
	memory.resize(length * specify.length);
}

void vertex_pool_t::copy(arch_t from, arch_t count, const vertex_pool_t* that) {
	assert(this->specify == that->specify);
	count *= this->specify.length;
	byte_t* this_vtx = &this->memory[from * this->specify.length];
	const byte_t* that_vtx = &that->memory[0];
	for (arch_t it = 0; it < count; ++it) {
		this_vtx[it] = that_vtx[it];
	}
}

vertex_spec_t vertex_pool_t::get_specify() const {
	return specify;
}