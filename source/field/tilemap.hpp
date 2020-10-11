#ifndef LEVIATHAN_INCLUDED_FIELD_TILEMAP_HPP
#define LEVIATHAN_INCLUDED_FIELD_TILEMAP_HPP

#include "./tilemap-parallax.hpp"
#include "./tilemap-layer.hpp"

struct camera_t;

struct tilemap_t : public not_copyable_t {
public:
	tilemap_t();
	tilemap_t(tilemap_t&&) = default;
	tilemap_t& operator=(tilemap_t&&) = default;
	~tilemap_t() = default;
public:
	void reset();
	void handle(const camera_t& camera);
	void render(renderer_t& renderer, rect_t viewport) const;
	void push_properties(const tmx::Map& tmxmap);
	void push_layer(const std::unique_ptr<tmx::Layer>& layer);
	void push_parallax(const std::unique_ptr<tmx::Layer>& layer);
	uint_t get_attribute(sint_t x, sint_t y) const;
	uint_t get_attribute(glm::ivec2 index) const;
public:
	static sint_t round(real_t value);
	static sint_t ceiling(real_t value);
	static sint_t floor(real_t value);
	static real_t extend(sint_t value);
private:
	mutable bool_t amend;
	glm::ivec2 dimensions;
	std::vector<uint_t> attributes, attribute_key;
	rect_t previous_viewport;
	const texture_t* layer_texture;
	const texture_t* parallax_texture;
	std::vector<tilemap_parallax_t> tilemap_parallaxes;
	std::vector<tilemap_layer_t> tilemap_layers;
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILEMAP_HPP
