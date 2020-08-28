#include "./health.hpp"
#include "./location.hpp"
#include "./kontext.hpp"

#include "../actor/naomi.hpp"
#include "../system/receiver.hpp"

void health_t::reset(sint_t current, sint_t maximum, sint_t leviathan, sint_t damage) {
	flags.reset();
	this->current = current;
	this->maximum = maximum;
	this->leviathan = leviathan;
	this->damage = damage;
}

void health_t::reset() {
	this->reset(2, 2, 0, 0);
}

void health_t::attack(health_t& victim) const {
	if (flags[health_flags_t::Instant]) {
		victim.maximum = 0;
	} else if (!victim.flags[health_flags_t::OnceMore] and victim.maximum < damage) {
		victim.maximum = 0;
	}
	victim.current -= damage;
	victim.leviathan += leviathan;
}

void health_t::handle(audio_t& audio, receiver_t& receiver, naomi_state_t& naomi_state, kontext_t& kontext) {
	const auto& naomi_location = kontext.get<location_t>(naomi_state.actor);
	kontext.slice<actor_header_t, health_t, location_t>().each([&audio, &receiver, &naomi_state, &kontext, &naomi_location](entt::entity actor, const actor_header_t&, health_t& health, const location_t& location) {
		if (health.current <= 0) {
			if (kontext.has<actor_trigger_t>(actor)) {
				auto& trigger = kontext.get<actor_trigger_t>(actor);
				if (trigger.bitmask[trigger_flags_t::DeathEvent]) {
					trigger.bitmask[trigger_flags_t::DeathEvent] = false;
					receiver.run_event(trigger.identity);
				}
			}
			if (health.flags[health_flags_t::MajorFight]) {
				health.reset();
				kontext.meter(0, 0);
			} else {
				kontext.dispose(actor);
			}
		} else if (health.flags[health_flags_t::Attack]) {
			if (health.damage > 0 and location.overlap(naomi_location)) {
				naomi_state.damage(actor, audio, kontext);
			}
		} else if (health.flags[health_flags_t::MajorFight]) {
			kontext.meter(health.current, health.maximum);
		}
	});
}
