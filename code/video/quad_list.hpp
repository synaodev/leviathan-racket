#ifndef SYNAO_VIDEO_QUAD_LIST_HPP
#define SYNAO_VIDEO_QUAD_LIST_HPP

#include <vector>

#include "./vertex_buffer.hpp"

struct quad_list_t : public not_copyable_t {
public:
	quad_list_t();
	quad_list_t(quad_list_t&& that) noexcept;
	quad_list_t& operator=(quad_list_t&& that) noexcept;
	~quad_list_t();
public:
	static bool allocate_indexer(arch_t length, primitive_t primitive);
	static bool release_indexer();
	template<typename V>
	inline void setup(buffer_usage_t usage) {
		static_assert(std::is_base_of<vertex_t, V>::value);
		vertex_spec_t specify = vertex_spec_t::from(typeid(V));
		this->setup(usage, specify);
	}
	void setup(buffer_usage_t usage, vertex_spec_t specify);
	void create(arch_t length);
	void destroy();
	bool update(const vertex_t* vertices, arch_t count, arch_t offset);
	bool update(const vertex_t* vertices, arch_t count);
	bool update(const vertex_t* vertices);
	void draw(arch_t count) const;
	void draw() const;
	buffer_usage_t get_usage() const;
	arch_t get_length() const;
private:
	static arch_t convert(arch_t length);
	static std::vector<uint16_t> generate(arch_t length, arch_t offset, primitive_t primitive);
private:
	friend struct gfx_t;
	static primitive_t primitive;
	static uint_t elemts;
	buffer_usage_t usage;
	vertex_spec_t specify;
	uint_t arrays, buffer;
	arch_t length;
};

#endif // SYNAO_VIDEO_QUAD_LIST_HPP