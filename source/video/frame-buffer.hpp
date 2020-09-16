#ifndef LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP

#include "./texture.hpp"

struct color_buffer_t : public not_copyable_t, public sampler_t {
public:
	color_buffer_t() :
		handle(0),
		dimensions(0),
		layers(0),
		format(pixel_format_t::Invalid) {}
	color_buffer_t(color_buffer_t&& that) noexcept;
	color_buffer_t& operator=(color_buffer_t&& that) noexcept;
	~color_buffer_t();
public:
	bool create(glm::ivec2 dimensions, uint_t layers, pixel_format_t format);
	void destroy();
	bool valid();
	uint_t get_layers() const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_inverse_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
private:
	friend struct gfx_t;
	uint_t handle;
	glm::ivec2 dimensions;
	uint_t layers;
	pixel_format_t format;
};

struct depth_buffer_t : public not_copyable_t, public sampler_t {
public:
	depth_buffer_t() :
		handle(0),
		dimensions(0),
		compress(false) {}
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

namespace __enum_frame_buffer_binding {
	enum type : arch_t {
		Main,
		Read,
		Write
	};
}

using frame_buffer_binding_t = __enum_frame_buffer_binding::type;

struct frame_buffer_t : public not_copyable_t {
public:
	frame_buffer_t() :
		ready(false),
		handle(0),
		color_buffer(),
		depth_buffer() {}
	frame_buffer_t(frame_buffer_t&& that) noexcept;
	frame_buffer_t& operator=(frame_buffer_t&& that) noexcept;
	~frame_buffer_t();
public:
	void color(glm::ivec2 dimensions, uint_t layers, pixel_format_t format);
	void depth(glm::ivec2 dimensions, bool_t compress);
	bool create();
	void destroy();
	static void bind(const frame_buffer_t* frame_buffer, frame_buffer_binding_t binding, arch_t index);
	static void bind(const frame_buffer_t* frame_buffer);
	static void blit(glm::ivec2 source_position, glm::ivec2 source_dimensions, glm::ivec2 destination_position, glm::ivec2 destination_dimensions);
	static void blit(glm::ivec2 source_dimensions, glm::ivec2 destination_dimensions);
	static void clear(glm::ivec2 dimensions, glm::vec4 color);
	static void clear(glm::ivec2 dimensions);
	static void clear(const frame_buffer_t* frame_buffer, glm::vec4 color);
	static void clear(const frame_buffer_t* frame_buffer);
	static void viewport(glm::ivec2 dimensions);
	static void viewport(const frame_buffer_t* frame_buffer);
	static void bucket(glm::vec4 color);
	bool valid() const;
	glm::vec2 get_dimensions() const;
	glm::ivec2 get_integral_dimensions() const;
	const color_buffer_t* get_color_buffer() const;
	const depth_buffer_t* get_depth_buffer() const;
private:
	friend struct gfx_t;
	bool_t ready;
	uint_t handle;
	color_buffer_t color_buffer;
	depth_buffer_t depth_buffer;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP
