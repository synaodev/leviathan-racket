#ifndef SYNAO_OVERLAY_DRAW_FRAMERATE_HPP
#define SYNAO_OVERLAY_DRAW_FRAMERATE_HPP

#include "./draw_count.hpp"

struct draw_framerate_t : public not_copyable_t, public not_moveable_t {
public:
	draw_framerate_t();
	~draw_framerate_t() = default;
public:
	void init(const texture_t* texture);
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void force() const;
	void set_position(real_t x, real_t y);
	void set_position(glm::vec2 position);
private:
	real64_t timer;
	draw_count_t count;
};

#endif // SYNAO_OVERLAY_DRAW_FRAMERATE_HPP