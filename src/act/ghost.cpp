#include "./ghost.hpp"

#include "../cnt/kontext.hpp"
#include "../cnt/location.hpp"
#include "../cnt/health.hpp"
#include "../cnt/sprite.hpp"

#include "../res.hpp"

SYNAO_CTOR_TABLE_CREATE(routine_generator_t) {
	SYNAO_CTOR_TABLE_PUSH(ai::ghost::type, ai::ghost::ctor);
}

void ai::ghost::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(4.0f, 0.0f, 8.0f, 16.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Ghost);
	sprite.layer = layer_value::Automatic;
	sprite.position = location.position;

	auto& health = ktx.assign_if<health_t>(s);
	health.reset(8, 8, 0, 1);
	health.flags[health_flags_t::Leviathan] = true;

	ktx.assign_if<routine_t>(s, ghost::tick);
}


void ai::ghost::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& health = rtp.ktx.get<health_t>(s);
	if (health.flags[health_flags_t::Hurt]) {
		auto& sprite = rtp.ktx.get<sprite_t>(s);
		sprite.shake = 4.0f;
		sprite.new_state(1);
	}
}
