#ifndef SYNAO_FIELD_PARALLAX_BACKGROUND_HPP
#define SYNAO_FIELD_PARALLAX_BACKGROUND_HPP

#include <memory>
#include <tmxlite/Layer.hpp>

#include "../utl/rect.hpp"

struct texture_t;
struct renderer_t;

struct parallax_background_t : public not_copyable_t {
public:
	parallax_background_t();
	parallax_background_t(parallax_background_t&&);
	parallax_background_t& operator=(parallax_background_t&&);
	~parallax_background_t() = default;
public:
	void init(const std::unique_ptr<tmx::Layer>& layer, glm::vec2 dimensions);
	void handle(rect_t viewport);
	void render(renderer_t& renderer, rect_t viewport, const texture_t* texture) const;
private:
	mutable arch_t indices;
	glm::vec2 position, scrolling, offsets, speed, dimensions;
	rect_t bounding;
};

#endif // SYNAO_FIELD_PARALLAX_BACKGROUND_HPP