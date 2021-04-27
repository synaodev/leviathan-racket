#pragma once

#include "../utility/rect.hpp"

struct renderer_t;

enum class fade_state_t : arch_t {
	DoneIn,
	DoneOut,
	FadingIn,
	FadingOut
};

struct draw_fade_t : public not_copyable_t {
public:
	draw_fade_t() = default;
	draw_fade_t(draw_fade_t&&) noexcept = default;
	draw_fade_t& operator=(draw_fade_t&&) noexcept = default;
	~draw_fade_t() = default;
public:
	void init();
	void reset();
	void handle();
	void render(renderer_t& renderer) const;
	void invalidate() const;
	void fade_in();
	void fade_out();
	bool is_done() const;
	bool is_moving() const;
	bool is_visible() const;
private:
	mutable bool_t amend { false };
	fade_state_t state { fade_state_t::DoneOut };
	rect_t bounding {};
};
