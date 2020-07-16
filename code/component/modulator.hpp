#ifndef SYNAO_COMPONENT_MODULATOR_HPP
#define SYNAO_COMPONENT_MODULATOR_HPP

#include "../types.hpp"

struct sprite_t;

struct modulator_t {
public:
	modulator_t(sprite_t* sprite_ptr);
	modulator_t();
	modulator_t(const modulator_t&) = default;
	modulator_t& operator=(const modulator_t&) = default;
	modulator_t(modulator_t&&) = default;
	modulator_t& operator=(modulator_t&&) = default;
	~modulator_t() = default;
public:
	void reset(sprite_t* sprite_ptr, real_t current, real_t offset);
	void reset(real_t current, real_t offset);
	void reset();
	void update();
private:
	sprite_t* sprite_ptr;
public:
	real_t current, offset;
};

#endif // SYNAO_COMPONENT_MODULATOR_HPP
