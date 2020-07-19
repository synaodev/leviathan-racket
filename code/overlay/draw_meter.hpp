#ifndef SYNAO_OVERLAY_DRAW_METER_HPP
#define SYNAO_OVERLAY_DRAW_METER_HPP

#include "./draw_scheme.hpp"

#include "../utility/rect.hpp"

struct draw_meter_t : public not_copyable_t {
public:
	draw_meter_t();
	draw_meter_t(draw_meter_t&&) = default;
	draw_meter_t& operator=(draw_meter_t&&) = default;
	~draw_meter_t() = default;
public:
	void init(const animation_t* animation);
	void reset();
	void update(real64_t delta);
	void invalidate() const;
	void render(renderer_t& renderer) const;
	void set_values(sint_t current, sint_t maximum);
private:
	mutable bool_t amend;
	sint_t current;
	rect_t varying;
	draw_scheme_t graphed;
};

#endif // SYNAO_OVERLAY_DRAW_METER_HPP