#ifndef LEVIATHAN_INCLUDED_MENU_ATTRIBUTE_GUI_HPP
#define LEVIATHAN_INCLUDED_MENU_ATTRIBUTE_GUI_HPP

#include <vector>
#include <string>
#include <bitset>

#include "../types.hpp"

struct tileset_viewer_t;
struct input_t;
struct renderer_t;

namespace __enum_attrgui_bits {
	enum type : arch_t {
		Enable,
		Load,
		Save,
		Total
	};
}

using attrgui_bits_t = __enum_attrgui_bits::type;

struct attribute_gui_t : public not_copyable_t {
public:
	attribute_gui_t();
	attribute_gui_t(attribute_gui_t&&) = default;
	attribute_gui_t& operator=(attribute_gui_t&&) = default;
	~attribute_gui_t() = default;
public:
	void reset();
	void handle(tileset_viewer_t& tileset_viewer, renderer_t& renderer);
	bool active() const;
private:
	std::bitset<attrgui_bits_t::Total> bitmask;
	sint_t index;
	std::vector<std::string> files;
};

#endif // LEVIATHAN_INCLUDED_MENU_ATTRIBUTE_GUI_HPP
