#ifndef LEVIATHAN_INCLUDED_FIELD_TILESET_VIEWER_HPP
#define LEVIATHAN_INCLUDED_FIELD_TILESET_VIEWER_HPP

#include <vector>
#include <string>

#include "../utility/rect.hpp"

struct texture_t;

struct input_t;
struct renderer_t;

struct tileset_viewer_t : public not_copyable_t {
public:
	tileset_viewer_t();
	tileset_viewer_t(tileset_viewer_t&&) = default;
	tileset_viewer_t& operator=(tileset_viewer_t&&) = default;
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
	mutable bool_t amend;
	bool_t select, flash;
	rect_t cursor;
	arch_t index;
	real64_t timer;
	const texture_t* texture;
	std::vector<uint_t> bitmasks;
	std::string file;
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILESET_VIEWER_HPP
