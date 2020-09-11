#ifndef LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP
#define LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP

#include <memory>
#include <tmxlite/Layer.hpp>

#include "../utility/rect.hpp"

struct texture_t;
struct renderer_t;

struct tilemap_parallax_t : public not_copyable_t {
public:
	tilemap_parallax_t();
	tilemap_parallax_t(tilemap_parallax_t&& that) noexcept;
	tilemap_parallax_t& operator=(tilemap_parallax_t&& that) noexcept;
	~tilemap_parallax_t() = default;
public:
	void init(const std::unique_ptr<tmx::Layer>& layer, glm::vec2 dimensions);
	void handle(rect_t viewport);
	void render(renderer_t& renderer, rect_t viewport, const texture_t* texture) const;
private:
	mutable arch_t indices;
	glm::vec2 position, scrolling, dimensions;
	rect_t bounding;
};

#endif // LEVIATHAN_INCLUDED_FIELD_TILEMAP_PARALLAX_HPP
