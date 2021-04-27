#ifndef LEVIATHAN_INCLUDED_VIDEO_VERTEX_POOL_HPP
#define LEVIATHAN_INCLUDED_VIDEO_VERTEX_POOL_HPP

#include <type_traits>
#include <vector>

#include "./vertex.hpp"

struct vertex_pool_t : public not_copyable_t {
public:
	vertex_pool_t() = default;
	vertex_pool_t(vertex_pool_t&&) noexcept = default;
	vertex_pool_t& operator=(vertex_pool_t&&) noexcept = default;
	~vertex_pool_t() = default;
public:
	void setup(vertex_spec_t specify);
	void clear();
	void resize(arch_t length);
	void copy(arch_t from, arch_t count, const vertex_pool_t& that);
	bool empty() const;
	arch_t size() const;
	vertex_t* operator[](size_t index);
	const vertex_t* operator[](size_t index) const;
	template<typename V> V* at(arch_t index);
	template<typename V> const V* at(arch_t index) const;
	const vertex_spec_t& get_specify() const;
private:
	vertex_spec_t specify {};
	std::vector<byte_t> memory {};
};

template<typename V>
inline V* vertex_pool_t::at(arch_t index) {
	if (!specify.detail) {
		return nullptr;
	}
	static_assert(std::is_base_of<vertex_t, V>::value);
	return reinterpret_cast<V*>(&memory[index * specify.length]);
}

template<typename V>
inline const V* vertex_pool_t::at(arch_t index) const {
	if (!specify.detail) {
		return nullptr;
	}
	static_assert(std::is_base_of<vertex_t, V>::value);
	return reinterpret_cast<const V*>(&memory[index * specify.length]);
}

#endif // LEVIATHAN_INCLUDED_VIDEO_VERTEX_POOL_HPP
