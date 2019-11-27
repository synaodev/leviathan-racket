#include "./light.hpp"

#include <utility>

light_t::light_t() :
	drawn(false),
	write(false),
	__padding0(false),
	__padding1(false),
	position(0.0f),
	depth(0.0f),
	diameter(0.0f),
	attenuate(0.0f),
	__padding2(0.0f),
	color(1.0f)
{

}

light_t::light_t(light_t&& that) noexcept : light_t() {
	if (this != &that) {
		std::swap(drawn, that.drawn);
		std::swap(write, that.write);
		std::swap(position, that.position);
		std::swap(depth, that.depth);
		std::swap(diameter, that.diameter);
		std::swap(attenuate, that.attenuate);
		std::swap(color, that.color);
	}
}

light_t& light_t::operator=(light_t&& that) noexcept {
	if (this != &that) {
		std::swap(drawn, that.drawn);
		std::swap(write, that.write);
		std::swap(position, that.position);
		std::swap(depth, that.depth);
		std::swap(diameter, that.diameter);
		std::swap(attenuate, that.attenuate);
		std::swap(color, that.color);
	}
	return *this;
}

bool light_t::query(arch_t* count) {
	assert(count != nullptr);
	if (drawn) {
		drawn = false;
		if (write) {
			write = false;
			return true;
		}
		*count++;
	}
	return false;
}

void light_t::ftr_write(glm::vec2 position, real_t depth, real_t radius, glm::vec3 attenuate, glm::vec4 color) {
	this->drawn = true;
	if (this->position != position or
		this->depth != depth or
		this->diameter != (radius * 2.0f) or
		this->attenuate != attenuate or
		this->color != color
	) {
		this->write = true;
		this->position = position;
		this->depth = depth;
		this->diameter = radius * 2.0f;
		this->attenuate = attenuate;
		this->color = color;
	}
}