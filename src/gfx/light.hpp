#ifndef SYNAO_GRAPHICS_LIGHT_HPP
#define SYNAO_GRAPHICS_LIGHT_HPP

#include "../types.hpp"

struct light_t : public not_copyable_t {
public:
	light_t();
	light_t(light_t&& that) noexcept;
	light_t& operator=(light_t&& that) noexcept;
	~light_t() = default;
public:
	bool query(arch_t* count);
	void ftr_write(glm::vec2 position, real_t depth, real_t radius, glm::vec3 attenuate, glm::vec4 color);
private:
	bool_t drawn, write;
	bool_t __padding0, __padding1;
	glm::vec2 position;
	real_t depth, diameter;
	glm::vec3 attenuate;
	real_t __padding2;
	glm::vec4 color;
};

#endif // SYNAO_GRAPHICS_LIGHT_HPP