#include "./common.hpp"
#include "./naomi.hpp"

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/sprite.hpp"
#include "../component/health.hpp"
#include "../resource/id.hpp"
#include "../utility/enums.hpp"

// Functions

void ai::null::ctor(entt::entity, kontext_t&) {}

void ai::hv_trigger::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	if (location.direction & direction_t::Up or location.direction & direction_t::Down) {
		location.bounding = { 0.0f, -48.0f, 16.0f, 112.0f };
	} else {
		location.bounding = { -48.0f, 0.0f, 112.0f, 16.0f };
	}
}

void ai::hv_trigger::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& trigger = rtp.kontext.get<actor_trigger_t>(s);
	if (trigger.bitmask[actor_trigger_t::InteractionEvent]) {
		auto& this_location = rtp.kontext.get<location_t>(s);
		auto& that_location = rtp.kontext.get<location_t>(rtp.naomi.get_actor());
		if (this_location.overlap(that_location)) {
			rtp.kontext.run(trigger);
		}
	}
}

void ai::full_chest::ctor(entt::entity s, kontext_t& kontext) {
	kontext.assign_if<sprite_t>(s, res::anim::Chest);
}

void ai::empty_chest::ctor(entt::entity s, kontext_t& kontext) {
	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Chest);
	sprite.variation = 1;
}

void ai::door::ctor(entt::entity s, kontext_t& kontext) {
	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Door);
}

void ai::spikes::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.bounding = { 4.0f, 4.0f, 8.0f, 8.0f };

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Death);
	sprite.pivot = { 8.0f, 8.0f };
}

void ai::death_spikes::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.bounding = { 4.0f, 4.0f, 24.0f, 12.0f };

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Death);

	auto& health = kontext.assign_if<health_t>(s);
	health.flags[health_t::Instant] = true;
}

void ai::bed::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.bounding = { 8.0f, 0.0f, 16.0f, 16.0f };

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Helpful);
	sprite.state = 0;
}

void ai::ammo_station::ctor(entt::entity s, kontext_t& kontext) {
	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Helpful);
	sprite.state = 1;
}

void ai::computer::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.position.x -= 2.0f;
	location.bounding = { 5.0f, 0.0f, 16.0f, 16.0f };

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Computer);
}

void ai::fire::ctor(entt::entity s, kontext_t& kontext) {
	kontext.assign_if<sprite_t>(s, res::anim::Fireplace);
}

// Tables

LEVIATHAN_CTOR_TABLE_CREATE(common) {
	LEVIATHAN_TABLE_PUSH(ai::null::type, 			ai::null::ctor);
	LEVIATHAN_TABLE_PUSH(ai::hv_trigger::type, 		ai::hv_trigger::ctor);
	LEVIATHAN_TABLE_PUSH(ai::full_chest::type, 		ai::full_chest::ctor);
	LEVIATHAN_TABLE_PUSH(ai::empty_chest::type, 	ai::empty_chest::ctor);
	LEVIATHAN_TABLE_PUSH(ai::door::type, 			ai::door::ctor);
	LEVIATHAN_TABLE_PUSH(ai::death_spikes::type, 	ai::death_spikes::ctor);
	LEVIATHAN_TABLE_PUSH(ai::bed::type, 			ai::bed::ctor);
	LEVIATHAN_TABLE_PUSH(ai::ammo_station::type, 	ai::ammo_station::ctor);
	LEVIATHAN_TABLE_PUSH(ai::computer::type, 		ai::computer::ctor);
	LEVIATHAN_TABLE_PUSH(ai::fire::type, 			ai::fire::ctor);
}
