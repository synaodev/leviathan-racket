#ifndef SYNAO_OVERLAY_DRAW_HIDDEN_HPP
#define SYNAO_OVERLAY_DRAW_HIDDEN_HPP

#include <functional>

#include "./draw_count.hpp"
#include "./draw_text.hpp"

namespace __enum_draw_hidden_state {
	enum type : arch_t {
		None,
		Framerate,
		DrawCalls,
		ActorCount
	};
}

using draw_hidden_state_t = __enum_draw_hidden_state::type;

struct draw_hidden_t : public not_copyable_t {
public:
	draw_hidden_t();
	draw_hidden_t(draw_hidden_t&&) = default;
	draw_hidden_t& operator=(draw_hidden_t&&) = default;
	~draw_hidden_t() = default;
public:
	void init(const texture_t* texture, const palette_t* palette, const font_t* font);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void invalidate() const;
	void set_state(draw_hidden_state_t state, std::function<sint_t()> radio);
private:
	draw_hidden_state_t state;
	std::function<sint_t()> radio;
	real64_t timer;
	draw_count_t count;
	draw_text_t text;
};

#endif // SYNAO_OVERLAY_DRAW_HIDDEN_HPP