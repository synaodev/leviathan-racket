#ifndef LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP
#define LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP

#include <memory>
#include <glm/vec2.hpp>
#include <tmxlite/Layer.hpp>

#include "../utility/rect.hpp"

struct texture_t;
struct renderer_t;

struct tilemap_parallax_t : public not_copyable_t {
public:
	tilemap_parallax_t() = default;
	tilemap_parallax_t(tilemap_parallax_t&& that) noexcept = default;
	tilemap_parallax_t& operator=(tilemap_parallax_t&& that) noexcept = default;
	~tilemap_parallax_t() = default;
public:
	void init(const std::unique_ptr<tmx::Layer>& layer, const glm::vec2& dimensions);
	void handle(const rect_t& viewport);
	void render(renderer_t& renderer, const rect_t& viewport, const texture_t* texture) const;
private:
	mutable arch_t indices { 0 };
	glm::vec2 position {};
	glm::vec2 scrolling {};
	glm::vec2 dimensions { 1.0f };
	rect_t bounding {};
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP
