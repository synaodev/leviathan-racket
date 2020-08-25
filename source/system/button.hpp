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

struct demo_player_t : public not_copyable_t {
public:
	demo_player_t();
	demo_player_t(bool_t record);
	demo_player_t(demo_player_t&&) = default;
	demo_player_t& operator=(demo_player_t&&) = default;
	~demo_player_t() = default;
public:
	bool load(const std::string& name);
	bool write(const std::string& output);
	void read(std::bitset<btn_t::Total>& pressed, std::bitset<btn_t::Total>& holding);
	void store(const std::bitset<btn_t::Total>& pressed, const std::bitset<btn_t::Total>& holding);
	bool recording() const;
	bool playing() const;
private:
	bool_t record;
	arch_t index;
	std::vector<std::bitset<btn_t::Total> > buttons;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_BUTTON_HPP
