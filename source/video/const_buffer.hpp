#ifndef LEVIATHAN_INCLUDED_VIDEO_CONST_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_CONST_BUFFER_HPP

#include "./gfx.hpp"

struct const_buffer_t : public not_copyable_t {
public:
	const_buffer_t();
	const_buffer_t(const_buffer_t&& that) noexcept;
	const_buffer_t& operator=(const_buffer_t&& that) noexcept;
	~const_buffer_t();
public:
	void setup(buffer_usage_t usage);
	void create(arch_t length);
	void create_immutable(arch_t length);
	void destroy();
	bool update(const void_t pointer);
	bool update(const void_t pointer, arch_t count);
	bool update(const void_t pointer, arch_t count, arch_t offset);
	bool valid() const;
	bool immutable() const;
	buffer_usage_t get_usage() const;
	arch_t get_length() const;
	static bool has_immutable_option();
private:
	friend struct gfx_t;
	buffer_usage_t usage;
	bool_t immuts;
	uint_t handle;
	arch_t length;
};

#endif // LEVIATHAN_INCLUDED_VIDEO_CONST_BUFFER_HPP
