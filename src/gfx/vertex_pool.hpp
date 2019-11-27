#ifndef SYNAO_GRAPHICS_VERTEX_POOL_HPP
#define SYNAO_GRAPHICS_VERTEX_POOL_HPP

#include <type_traits>
#include <cassert>
#include <vector>

#include "./vertex.hpp"

struct vertex_pool_t {
public:
	vertex_pool_t() = default;
	vertex_pool_t(const vertex_pool_t&) = default;
	vertex_pool_t(vertex_pool_t&&) = default;
	vertex_pool_t& operator=(const vertex_pool_t&) = default;
	vertex_pool_t& operator=(vertex_pool_t&&) = default;
	~vertex_pool_t() = default;
public:
	template<typename V>
	void setup() {
		static_assert(std::is_base_of<vtx_minor_t, V>::value);
		vertex_spec_t specify = vertex_spec_t::from(typeid(V));
		this->setup(specify);
	}
	void setup(vertex_spec_t specify);
	void clear();
	void resize(arch_t length);
	void copy(arch_t from, arch_t count, const vertex_pool_t* that);
	bool empty() const;
	arch_t size() const;
	vertex_spec_t get_specify() const;
	template<typename V = vtx_minor_t> V* at(arch_t index) {
		assert(specify.detail != nullptr);
		return reinterpret_cast<V*>(&memory[index * specify.length]);
	}
	template<typename V = vtx_minor_t> const V* at(arch_t index) const {
		assert(specify.detail != nullptr);
		return reinterpret_cast<const V*>(&memory[index * specify.length]);
	}
private:
	vertex_spec_t specify;
	std::vector<byte_t> memory;
};

#endif // SYNAO_GRAPHICS_VERTEX_POOL_HPP