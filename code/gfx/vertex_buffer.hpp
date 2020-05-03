#ifndef SYNAO_GRAPHICS_VERTEX_BUFFER_HPP
#define SYNAO_GRAPHICS_VERTEX_BUFFER_HPP

#include "./vertex.hpp"
#include "./const_buffer.hpp"

namespace __enum_primitive {
	enum type : uint_t {
		Points, 
		Lines, 
		LineLoop, 
		LineStrip,
		Triangles, 
		TriangleStrip, 
		TriangleFan,
		Quads, 
		QuadStrip, 
		Polygon,
		LinesAdjacency, 
		LineStripAdjacency,
		TrianglesAdjacency, 
		TriangleStripAdjacency
	};
}

using primitive_t = __enum_primitive::type;

struct vertex_buffer_t : public not_copyable_t {
public:
	vertex_buffer_t();
	vertex_buffer_t(vertex_buffer_t&& that) noexcept;
	vertex_buffer_t& operator=(vertex_buffer_t&& that) noexcept;
	~vertex_buffer_t();
public:
	template<typename V>
	inline void setup(primitive_t primitive, buffer_usage_t usage) {
		static_assert(std::is_base_of<vertex_t, V>::value);
		vertex_spec_t specify = vertex_spec_t::from(typeid(V));
		this->setup(primitive, usage, specify);
	}
	void setup(primitive_t primitive, buffer_usage_t usage, vertex_spec_t specify);
	void create(arch_t vtx_count, arch_t idx_count);
	void destroy();
	bool update(const vertex_t* vertices, arch_t count, arch_t offset);
	bool update(const vertex_t* vertices, arch_t count);
	bool update(const vertex_t* vertices);
	bool update(const uint16_t* indices, arch_t count, arch_t offset);
	bool update(const uint16_t* indices, arch_t count);
	bool update(const uint16_t* indices);
	void draw(arch_t count) const;
	void draw() const;
	bool valid() const;
	primitive_t get_primitive() const;
	buffer_usage_t get_usage() const;
	arch_t get_vtx_count() const;
	arch_t get_idx_count() const;
private:
	friend struct gfx_t;
	primitive_t primitive;
	buffer_usage_t usage;
	vertex_spec_t specify;
	uint_t arrays, buffer, elemts;
	arch_t vtx_count, idx_count;
};


#endif // SYNAO_GRAPHICS_VERTEX_BUF_HPP