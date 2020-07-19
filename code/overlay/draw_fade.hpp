#ifndef SYNAO_OVERLAY_DRAW_FADE_HPP
#define SYNAO_OVERLAY_DRAW_FADE_HPP

#include "../utility/rect.hpp"

struct renderer_t;

namespace __enum_fade_state {
	enum type : arch_t {
		DoneIn,
		DoneOut,
		FadingIn,
		FadingOut
	};
}

using fade_state_t = __enum_fade_state::type;

struct draw_fade_t : public not_copyable_t {
public:
	draw_fade_t();
	draw_fade_t(draw_fade_t&&) = default;
	draw_fade_t& operator=(draw_fade_t&&) = default;
	~draw_fade_t() = default;
public:
	void init();
	void reset();
	void handle();
	void render(renderer_t& renderer) const;
	void fade_in();
	void fade_out();
	bool is_done() const;
	bool is_moving() const;
	bool is_visible() const;
private:
	mutable bool_t amend;
	fade_state_t state;
	rect_t bounding;
};

#endif // SYNAO_OVERLAY_DRAW_FADE_HPP