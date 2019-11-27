#include "./weapon.hpp"
#include "./kontext.hpp"

#include "../cnt/location.hpp"
#include "../cnt/kinematics.hpp"
#include "../cnt/health.hpp"

SYNAO_CTOR_TABLE_CREATE(routine_generator_t) {
	SYNAO_CTOR_TABLE_PUSH(ai::frontier::type, 		ai::frontier::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::toxitier::type, 		ai::toxitier::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::weak_hammer::type, 	ai::weak_hammer::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::strong_hammer::type, 	ai::strong_hammer::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::holy_lance::type, 	ai::holy_lance::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::holy_tether::type, 	ai::holy_tether::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::kannon::type, 		ai::kannon::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::nail_ray::type, 		ai::nail_ray::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::wolf_vulcan::type, 	ai::wolf_vulcan::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::austere::type, 		ai::austere::ctor);
}

entt::entity ai::weapon::find_closest(entt::entity s, kontext_t& ktx) {
	std::vector<std::pair<entt::entity, real_t> > closelist;
	glm::vec2 center = ktx.get<location_t>(s).center();
	ktx.slice<actor_header_t, location_t, health_t>().each([&closelist, &center](entt::entity actor, const actor_header_t&, const location_t& location, const health_t& health) {
		if (health.flags[health_flags_t::Leviathan]) {
			auto element = std::make_pair(
				actor, glm::distance(center, location.center())
			);
			closelist.push_back(element);
		}
	});
	if (!closelist.empty()) {
		std::sort(closelist.cbegin(), closelist.cend(), [](const auto& a, const auto& b) {
			return std::get<real32>(a) < std::get<real32>(b);
		});
		return std::get<entt::entity>(closelist.front());
	}
	return entt::null;
}

entt::entity ai::weapon::find_hooked(entt::entity s, kontext_t& ktx) {
	entt::entity result = entt::null;
	rect_t zone = ktx.get<location_t>(s).hitbox();
	ktx.slice<actor_header_t, location_t, health_t>().each([&result, &zone](entt::entity actor, const actor_header_t&, const location_t& location, health_t& health) {
		if (result != entt::null) {
			if ((health.flags[health_flags_t::Hookable]) and location.overlap(zone)) {
				health.flags[health_flags_t::Grappled] = true;
				result = actor;
			}
		}
	});
	return result;
}

bool ai::weapon::damage_check(entt::entity s, kontext_t& ktx) {
	entt::entity result = entt::null;
	rect_t zone = ktx.get<location_t>(s).hitbox();
	auto& attacker = ktx.get<health_t>(s);
	ktx.slice<actor_header_t, location_t, health_t>().each([&result, &attacker, &zone](entt::entity actor, const actor_header_t&, const location_t& location, health_t& health) {
		if (health.flags[health_flags_t::Leviathan] and !health.flags[health_flags_t::Invincible]) {
			if (location.overlap(zone)) {
				health.flags[health_flags_t::Hurt] = true;
				attacker.attack(health);
				result = actor;
			}
		}
	});
	return result != entt::null;
}

bool ai::weapon::damage_range(entt::entity s, kontext_t& ktx, glm::vec2 center, glm::vec2 dimensions) {
	entt::entity result = entt::null;
	rect_t zone = rect_t(center - dimensions / 2.0f, dimensions);
	auto& attacker = ktx.get<health_t>(s);
	ktx.slice<actor_header_t, location_t, health_t>().each([&result, &zone, &attacker](entt::entity actor, const actor_header_t&, const location_t& location, health_t& health) {
		if (health.flags[health_flags_t::Leviathan] and !health.flags[health_flags_t::Invincible]) {
			if (location.overlap(zone)) {
				health.flags[health_flags_t::Hurt] = true;
				attacker.attack(health);
				result = actor;
			}
		}
	});
	return result != entt::null;
}

bool ai::weapon::reverse_range(entt::entity s, kontext_t& ktx) {
	entt::entity result = entt::null;
	rect_t zone = ktx.get<location_t>(s).hitbox();
	ktx.slice<actor_header_t, location_t, kinematics_t, health_t>().each([&result, &zone](entt::entity actor, const actor_header_t&, const location_t& location, kinematics_t& movement, health_t& health) {	
		if (health.flags[health_flags_t::Deflectable] and location.overlap(zone)) {
			health.flags[health_flags_t::Leviathan] = false;
			movement.velocity = -movement.velocity;
			result = actor;
		}
	});
	return result != entt::null;
}