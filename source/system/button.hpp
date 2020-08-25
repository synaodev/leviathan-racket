#ifndef LEVIATHAN_INCLUDED_SYSTEM_BUTTON_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_BUTTON_HPP

#include <bitset>
#include <vector>
#include <string>

#include "../utility/enums.hpp"

namespace __enum_btn {
	enum type : arch_t {
		Jump, Yes = Jump,
		Hammer, No = Hammer,
		Item,
		Dash,
		Context,
		Strafe,
		Inventory,
		Options,
		Up,
		Down,
		Left,
		Right,
		Click,
		Total = 16
	};
}

using btn_t = __enum_btn::type;

struct demo_buffer_t : public not_copyable_t {
public:
	// Demo buffer elements should be 8 bytes large.
	static_assert(sizeof(std::bitset<btn_t::Total>) == sizeof(uint64_t));
	demo_buffer_t();
	demo_buffer_t(demo_buffer_t&&) = default;
	demo_buffer_t& operator=(demo_buffer_t&&) = default;
	~demo_buffer_t() = default;
public:
	bool load(const std::string& name);
	bool valid() const;
	bool done() const;
	std::pair<std::bitset<btn_t::Total>, std::bitset<btn_t::Total> > next();
private:
	arch_t index;
	std::vector<uint64_t> buttons;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_BUTTON_HPP
