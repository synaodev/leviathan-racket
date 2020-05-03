#ifndef SYNAO_VIDEO_CONST_BUFFER_HPP
#define SYNAO_VIDEO_CONST_BUFFER_HPP

#include "../types.hpp"

namespace __enum_buffer_usage {
	enum type : uint_t {
		Stream  = 0x88E0,
		Static  = 0x88E4,
		Dynamic = 0x88E8
	};
}

using buffer_usage_t = __enum_buffer_usage::type;

struct const_buffer_t : public not_copyable_t {
public:
	const_buffer_t();
	const_buffer_t(const_buffer_t&& that) noexcept;
	const_buffer_t& operator=(const_buffer_t&& that) noexcept;
	~const_buffer_t();
public:
	void setup(buffer_usage_t usage);
	void create(arch_t length);
	void destroy();
	bool update(const optr_t pointer);
	bool update(const optr_t pointer, arch_t count);
	bool update(const optr_t pointer, arch_t count, arch_t offset);
	bool valid() const;
	buffer_usage_t get_usage() const;
	arch_t get_length() const;
	static bool has_immutable_storage();
private:
	friend struct gfx_t;
	buffer_usage_t usage;
	uint_t handle;
	arch_t length;
};

#endif // SYNAO_VIDEO_CONST_BUFFER_HPP