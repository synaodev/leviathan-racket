#include "./blinker.hpp"
#include "./sprite.hpp"
#include "./kontext.hpp"

blinker_t::blinker_t(arch_t first_state, arch_t blink_state) :
	enable(true),
	timer(0.0),
	first_state(first_state),
	blink_state(blink_state)
{

}

blinker_t::blinker_t() :
	enable(true),
	timer(0.0),
	first_state(0),
	blink_state(0)
{

}

void blinker_t::update(kontext_t& kontext, real64_t delta) {
	kontext.slice<blinker_t, sprite_t>().each([delta](entt::entity, blinker_t& blinker, sprite_t& sprite) {
		if (blinker.enable) {
			blinker.timer -= delta;
			if (sprite.state == blinker.first_state) {
				if (blinker.timer <= 0.0) {
					blinker.timer = static_cast<real64_t>(rng::next(0.5f, 3.0f));
					sprite.new_state(blinker.blink_state);
				}
			} else if (sprite.state == blinker.blink_state) {
				if (sprite.finished()) {
					sprite.new_state(blinker.first_state);
				}
			}
		}
	});
}