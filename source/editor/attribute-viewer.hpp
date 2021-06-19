#pragma once

#ifdef LEVIATHAN_USES_META

#include <vector>
#include <string>
#include <bitset>

#include "../types.hpp"

struct tileset_viewer_t;

struct input_t;
struct renderer_t;

struct attribute_viewer_t : public not_copyable_t, public not_moveable_t {
public:
	attribute_viewer_t();
	~attribute_viewer_t() = default;
public:
	void reset();
	void handle(tileset_viewer_t& tileset_viewer, renderer_t& renderer);
	bool active() const;
public:
	enum flags_t : arch_t {
		Enabled,
		Loading,
		Saving,
		TotalFlags
	};
private:
	std::bitset<flags_t::TotalFlags> bitmask { 0 };
	sint_t index { 0 };
	std::vector<std::string> files {};
};

#endif
