#ifndef SYNAO_OVERLAY_DRAW_METER_HPP
#define SYNAO_OVERLAY_DRAW_METER_HPP

#include <entt/entity/registry.hpp>

#include "./draw_scheme.hpp"

#include "../utl/rect.hpp"

struct kontext_t;
struct health_t;

struct draw_meter_t : public not_copyable_t {
public:
	draw_meter_t();
	draw_meter_t(draw_meter_t&&) = default;
	draw_meter_t& operator=(draw_meter_t&&) = default;
	~draw_meter_t() = default;
public:
	void init(const animation_t* animation);
	void reset();
	void handle(const kontext_t& kontext);
	void update(real64_t delta);
	void force() const;
	void render(renderer_t& renderer) const;
	void set_enemy(sint_t identity);
	bool is_active() const;
private:
	mutable bool_t write;
	sint_t current, identity;
	rect_t varying;
	draw_scheme_t graphed;
};

#endif // SYNAO_OVERLAY_DRAW_METER_HPP