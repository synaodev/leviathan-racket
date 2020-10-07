#ifndef LEVIATHAN_INCLUDED_FIELD_TILEMAP_LAYER_HPP
#define LEVIATHAN_INCLUDED_FIELD_TILEMAP_LAYER_HPP

#include <memory>
#include <tmxlite/Layer.hpp>

#include "../utility/enums.hpp"
#include "../video/vertex-pool.hpp"

struct texture_t;
struct palette_t;
struct renderer_t;

struct tilemap_layer_t : public not_copyable_t {
public:
	tilemap_layer_t(glm::ivec2 map_size);
	tilemap_layer_t();
	tilemap_layer_t(tilemap_layer_t&& that) noexcept/*= default */;
	tilemap_layer_t& operator=(tilemap_layer_t&& that) noexcept/*= default */;
	~tilemap_layer_t() = default;
public:
	void init(const std::unique_ptr<tmx::Layer>& layer, glm::vec2 inverse_dimensions, std::vector<uint_t>& attributes, const std::vector<uint_t>& attribute_key);
	void handle(arch_t range, glm::ivec2 first, glm::ivec2 last, glm::ivec2 map_size, const texture_t* texture, const palette_t* palette);
	void render(renderer_t& renderer, bool_t amend, const palette_t* palette) const;
private:
	layer_t priority;
	arch_t indices;
	glm::vec2 inverse_dimensions;
	std::vector<glm::ivec2> tiles;
	vertex_pool_t quads;
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILELAYER_HPP
