#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_ITEM_VIEW_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_ITEM_VIEW_HPP

#include "./draw-scheme.hpp"
#include "./draw-count.hpp"

struct kernel_t;

struct draw_item_view_t : public not_copyable_t {
public:
	draw_item_view_t() = default;
	draw_item_view_t(draw_item_view_t&&) = default;
	draw_item_view_t& operator=(draw_item_view_t&&) = default;
	~draw_item_view_t() = default;
public:
	void init(const texture_t* texture, const palette_t* palette, const animation_t* heads_animation, const animation_t* items_animation);
	void handle(const kernel_t& kernel);
	void render(renderer_t& renderer) const;
	void invalidate() const;
private:
	draw_scheme_t view {};
	draw_scheme_t scheme {};
	draw_count_t ammo {};
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_ITEM_VIEW_HPP
