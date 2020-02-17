#ifndef SYNAO_OVERLAY_DRAW_SCHEME_HPP
#define SYNAO_OVERLAY_DRAW_SCHEME_HPP

#include "../utl/enums.hpp"

struct renderer_t;
struct animation_t;

struct draw_scheme_t : public not_copyable_t {
public:
	draw_scheme_t();
	draw_scheme_t(draw_scheme_t&&) = default;
	draw_scheme_t& operator=(draw_scheme_t&&) = default;
	~draw_scheme_t() = default;
public:
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void force() const;
	void set_file(const animation_t* file);
	void set_visible(bool_t visible);
	void set_state(arch_t state);
	void set_direction(direction_t direction);
	void set_frame(arch_t frame);
	void set_index(real_t index);
	void set_position(real_t x, real_t y);
	void set_position(glm::vec2 position);
	void mut_position(real_t x, real_t y);
	void mut_position(glm::vec2 offset);
	bool is_finished() const;
	real64_t get_timer() const;
	arch_t get_state() const;
	direction_t get_direction() const;
	arch_t get_frame() const;
	real_t get_index() const;
	glm::vec2 get_position() const;
public:
	static constexpr arch_t NonState = (arch_t)-1;
private:
	mutable bool_t write;
	const animation_t* file;
	bool_t visible;
	real64_t timer;
	arch_t state;
	direction_t direction;
	arch_t frame;
	real_t index;
	glm::vec2 position;
};

#endif // SYNAO_OVERLAY_DRAW_SCHEME_HPP