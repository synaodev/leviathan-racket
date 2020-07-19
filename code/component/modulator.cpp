#include "./modulator.hpp"
#include "./sprite.hpp"

modulator_t::modulator_t(sprite_t* sprite_ptr) :
	sprite_ptr(sprite_ptr),
	current(0.0f),
	offset(0.0f)
{

}

modulator_t::modulator_t() :
	sprite_ptr(nullptr),
	current(0.0f),
	offset(0.0f)
{

}

void modulator_t::reset(sprite_t* sprite_ptr, real_t current, real_t offset) {
	this->sprite_ptr = sprite_ptr;
	this->reset(current, offset);
}

void modulator_t::reset(real_t current, real_t offset) {
	this->current = current;
	this->offset = offset;
	this->update();
}

void modulator_t::reset() {
	this->reset(nullptr, 0.0f, 0.0f);
}

void modulator_t::update() {
	if (sprite_ptr != nullptr) {
		real_t table = current + offset;
		if (sprite_ptr->table != table) {
			sprite_ptr->table = table;
			sprite_ptr->amend = true;
		}
	}
}
