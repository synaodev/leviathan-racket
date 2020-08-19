#ifndef LEVIATHAN_INCLUDED_VIDEO_DEPTH_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_DEPTH_BUFFER_HPP

#include "./texture.hpp"

struct depth_buffer_t : public not_copyable_t, public sampler_t {
public:
	depth_buffer_t();
	depth_buffer_t(depth_buffer_t&& that) noexcept;
	depth_buffer_t& operator=(depth_buffer_t&& that) noexcept;
	~depth_buffer_t();
public:
	bool create(glm::ivec2 dimensions, bool_t compress);
	void destroy();
	bool valid() const;
private:
	friend struct gfx_t;
	uint_t handle;
	glm::ivec2 dimensions;
	bool_t compress;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_DEPTH_BUFFER_HPP
