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
	tilemap_layer_t(const glm::ivec2& dimensions);
	tilemap_layer_t();
	tilemap_layer_t(tilemap_layer_t&& that) noexcept = default;
	tilemap_layer_t& operator=(tilemap_layer_t&& that) noexcept = default;
	~tilemap_layer_t() = default;
public:
	void init(const std::unique_ptr<tmx::Layer>& layer, const glm::vec2& inverse_dimensions, std::vector<uint_t>& attributes, const std::vector<uint_t>& attribute_key);
	void handle(arch_t range, const glm::ivec2& first, const glm::ivec2& last, const glm::ivec2& dimensions, const texture_t* texture);
	void render(renderer_t& renderer, bool_t amend) const;
private:
	layer_t priority { layer_value::Background };
	arch_t indices { 0 };
	glm::vec2 inverse_dimensions { 1.0f };
	std::vector<glm::ivec2> tiles {};
	vertex_pool_t quads {};
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILELAYER_HPP
