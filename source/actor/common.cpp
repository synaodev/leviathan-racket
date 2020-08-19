#include "./common.hpp"
#include "./naomi.hpp"

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/sprite.hpp"
#include "../component/health.hpp"
#include "../utility/enums.hpp"

#include "../resource/id.hpp"

LEVIATHAN_CTOR_TABLE_CREATE(routine_generator_t) {
	LEVIATHAN_CTOR_TABLE_PUSH(ai::null::type, 			ai::null::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::hv_trigger::type, 	ai::hv_trigger::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::chest_full::type, 	ai::chest_full::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::chest_empty::type, 	ai::chest_empty::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::door::type, 			ai::door::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::death_spikes::type, 	ai::death_spikes::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::bed::type, 			ai::bed::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::ammo_station::type, 	ai::ammo_station::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::computer::type, 		ai::computer::ctor);
	LEVIATHAN_CTOR_TABLE_PUSH(ai::fireplace::type, 		ai::fireplace::ctor);
}

void ai::null::ctor(entt::entity, kontext_t&) {

}

void ai::hv_trigger::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	if (location.direction & direction_t::Up or location.direction & direction_t::Down) {
		location.bounding = rect_t(0.0f, -48.0f, 16.0f, 112.0f);
	} else {
		location.bounding = rect_t(-48.0f, 0.0f, 112.0f, 16.0f);
	}
}

void ai::hv_trigger::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& trigger = rtp.ktx.get<actor_trigger_t>(s);
	if (trigger.bitmask[trigger_flags_t::InteractionEvent]) {
		auto& this_location = rtp.ktx.get<location_t>(s);
		auto& that_location = rtp.ktx.get<location_t>(rtp.nao.actor);
		if (this_location.overlap(that_location)) {
			rtp.ktx.run(trigger);
		}
	}
}

void ai::chest_full::ctor(entt::entity s, kontext_t& ktx) {
	ktx.assign_if<sprite_t>(s, res::anim::Chest);
}

void ai::chest_empty::ctor(entt::entity s, kontext_t& ktx) {
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Chest);
	sprite.variation = 1;
}

void ai::door::ctor(entt::entity s, kontext_t& ktx) {
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Door);
	sprite.position = ktx.get<location_t>(s).position;
}

void ai::spikes::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 8.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Death);
	sprite.position = location.position;
	sprite.pivot = glm::vec2(8.0f);
}

void ai::death_spikes::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(4.0f, 4.0f, 24.0f, 12.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Death);
	sprite.position = location.position;

	auto& health = ktx.assign_if<health_t>(s);
	health.flags[health_flags_t::Instant] = true;
}

void ai::bed::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(8.0f, 0.0f, 16.0f, 16.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Helpful);
	sprite.state = 0;
	sprite.position = location.position;
}

void ai::ammo_station::ctor(entt::entity s, kontext_t& ktx) {
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Helpful);
	sprite.state = 1;
	sprite.position = ktx.get<location_t>(s).position;
}

void ai::computer::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= glm::vec2(2.0f, 0.0f);
	location.bounding = rect_t(5.0f, 0.0f, 16.0f, 16.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Computer);
	sprite.position = location.position;
}

void ai::fireplace::ctor(entt::entity s, kontext_t& ktx) {
	ktx.assign_if<sprite_t>(s, res::anim::Fireplace);
}
