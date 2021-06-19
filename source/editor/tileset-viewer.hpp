#pragma once

#ifdef LEVIATHAN_USES_META

#include <vector>
#include <string>

#include "../utility/rect.hpp"

struct texture_t;

struct input_t;
struct renderer_t;

struct tileset_viewer_t : public not_copyable_t, public not_moveable_t {
public:
	tileset_viewer_t();
	~tileset_viewer_t() = default;
public:
	void reset();
	void handle(const input_t& input);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	bool load(const std::string& path, renderer_t& renderer);
	bool save();
	bool selected() const;
	void set_bitmask(uint_t mask);
	uint_t get_bitmask() const;
private:
	mutable bool_t amend { false };
	bool_t select { false };
	bool_t flash { false };
	rect_t cursor {};
	arch_t index { 0 };
	real64_t timer { 0.0 };
	const texture_t* texture { nullptr };
	std::vector<uint_t> bitmasks {};
	std::string file {};
};

#endif
