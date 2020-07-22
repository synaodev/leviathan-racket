#include "./weapons.hpp"
#include "./particles.hpp"
#include "./naomi.hpp"

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/kinematics.hpp"
#include "../component/sprite.hpp"
#include "../component/health.hpp"
#include "../component/liquid.hpp"
#include "../system/audio.hpp"
#include "../field/camera.hpp"
#include "../field/collision.hpp"

#include "../resource/id.hpp"

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

entt::entity ai::weapons::find_closest(entt::entity s, kontext_t& ktx) {
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
		std::sort(closelist.begin(), closelist.end(), [](const auto& a, const auto& b) {
			return std::get<real_t>(a) < std::get<real_t>(b);
		});
		return std::get<entt::entity>(closelist.front());
	}
	return entt::null;
}

entt::entity ai::weapons::find_hooked(entt::entity s, kontext_t& ktx) {
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

bool ai::weapons::damage_check(entt::entity s, kontext_t& ktx) {
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

bool ai::weapons::damage_range(entt::entity s, kontext_t& ktx, glm::vec2 center, glm::vec2 dimensions) {
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

bool ai::weapons::reverse_range(entt::entity s, kontext_t& ktx) {
	entt::entity result = entt::null;
	rect_t zone = ktx.get<location_t>(s).hitbox();
	ktx.slice<actor_header_t, location_t, kinematics_t, health_t>().each([&result, &zone](entt::entity actor, const actor_header_t&, const location_t& location, kinematics_t& kinematics, health_t& health) {	
		if (health.flags[health_flags_t::Deflectable] and location.overlap(zone)) {
			health.flags[health_flags_t::Leviathan] = false;
			kinematics.velocity = -kinematics.velocity;
			result = actor;
		}
	});
	return result != entt::null;
}

void ai::frontier::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position += glm::vec2(
		rng::next(-2.0f, 2.0f) - 8.0f,
		rng::next(-2.0f, 2.0f) - 8.0f
	);
	location.bounding = rect_t(6.0f, 6.0f, 4.0f, 4.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	real_t variation = rng::next(-0.261799f, 0.261799f);
	if (location.direction & direction_t::Down) {
		kinematics.accel_angle(glm::half_pi<real_t>() + variation, 5.0f);
	} else if (location.direction & direction_t::Up) {
		kinematics.accel_angle(1.5f * glm::pi<real_t>() + variation, 5.0f);
	} else if (location.direction & direction_t::Left) {
		kinematics.accel_angle(glm::pi<real_t>() + variation, 5.0f);
	} else {
		kinematics.accel_angle(variation, 5.0f);
	}
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Frontier);
	sprite.layer = 0.6f;
	sprite.position = location.position;
	sprite.pivot = glm::vec2(8.0f);
	sprite.angle = rng::next(0.0f, glm::two_pi<real_t>());

	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 3;
	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 35;

	ktx.assign_if<liquid_listener_t>(s);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::frontier::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	auto& listener = rtp.ktx.get<liquid_listener_t>(s);
	if (listener.liquid != entt::null and rtp.ktx.valid(listener.liquid)) {
		auto& location = rtp.ktx.get<location_t>(s);
		location.position.y = rtp.ktx.get<liquid_body_t>(listener.liquid).hitbox.y;
	}
	if (timer[0]-- <= 0 or weapons::damage_check(s, rtp.ktx)) {
		rtp.ktx.dispose(s);
	} else {
		auto& sprite = rtp.ktx.get<sprite_t>(s);	
		sprite.amend = true;
		sprite.angle = glm::mod(sprite.angle + 0.035f, glm::two_pi<real_t>());
	}
}

void ai::toxitier::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position += glm::vec2(
		rng::next(-2.0f, 2.0f) - 8.0f,
		rng::next(-2.0f, 2.0f) - 8.0f
	);
	location.bounding = rect_t(6.0f, 6.0f, 4.0f, 4.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	real_t variation = rng::next(-0.261799f, 0.261799f);
	if (location.direction & direction_t::Down) {
		kinematics.accel_angle(glm::half_pi<real_t>() + variation, 3.0f);
	} else if (location.direction & direction_t::Up) {
		kinematics.accel_angle(1.5f * glm::pi<real_t>() + variation, 3.0f);
	} else if (location.direction & direction_t::Left) {
		kinematics.accel_angle(glm::pi<real_t>() + variation, 3.0f);
	} else {
		kinematics.accel_angle(variation, 3.0f);
	}
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Frontier);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 3;
	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 60;
	ktx.assign_if<liquid_listener_t>(s);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::toxitier::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	auto& sprite = rtp.ktx.get<sprite_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	auto& listener = rtp.ktx.get<liquid_listener_t>(s);
	if (listener.liquid == entt::null or 
		!rtp.ktx.valid(listener.liquid) or 
		timer[0]-- <= 0 or 
		weapons::damage_check(s, rtp.ktx)) {
		rtp.ktx.dispose(s);
	} else {
		kinematics.accel_y(-0.3f, 0.6f);
		sprite.amend = true;
		sprite.alpha = glm::clamp(sprite.alpha - 0.016f, 0.0f, 1.0f);
	}
}

void ai::weak_hammer::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 8.0f;
	location.bounding = rect_t(2.0f, 2.0f, 12.0f, 12.0f);
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Hammer);
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	if (location.direction & direction_t::Down) {
		kinematics.velocity.y += 5.0f;
		sprite.oriented = oriented_t::Top;
		sprite.mirroring = mirroring_t::Vertical;
	} else if (location.direction & direction_t::Up) {
		kinematics.velocity.y -= 5.0f;
		sprite.oriented = oriented_t::Top;
	} else if (location.direction & direction_t::Left) {
		kinematics.velocity.x -= 5.0f;
		sprite.mirroring = mirroring_t::Horizontal;
	} else {
		kinematics.velocity.x += 5.0f;
	}
	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 9;
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 1;
	ktx.assign_if<routine_t>(s, tick);
}

void ai::weak_hammer::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	auto& routine = rtp.ktx.get<routine_t>(s);
	if (timer[0]-- <= 0) {
		rtp.ktx.dispose(s);
	} else if (!routine.state and weapons::damage_check(s, rtp.ktx)) {
		routine.state = 1;
		rtp.aud.play(res::sfx::Fan, 5);
		rtp.ktx.shrapnel(rtp.ktx.get<location_t>(s).center(), 3);
		rtp.cam.quake(1.5f, 0.15);
	}
}

void ai::strong_hammer::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 12.0f;
	location.bounding = rect_t(4.0f, 4.0f, 16.0f, 16.0f);
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Hammer);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	if (location.direction & direction_t::Down) {
		kinematics.velocity.y += 6.0f;
		sprite.oriented = oriented_t::Top;
		sprite.mirroring = mirroring_t::Vertical;
	} else if (location.direction & direction_t::Up) {
		kinematics.velocity.y -= 6.0f;
		sprite.oriented = oriented_t::Top;
	} else if (location.direction & direction_t::Left) {
		kinematics.velocity.x -= 6.0f;
		sprite.mirroring = mirroring_t::Horizontal;
	} else {
		kinematics.velocity.x += 6.0f;
	}
	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 12;
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 4;
	ktx.assign_if<routine_t>(s, tick);
}

void ai::strong_hammer::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	auto& routine = rtp.ktx.get<routine_t>(s);
	if (timer[0]-- <= 0) {
		rtp.ktx.dispose(s);
	} else if (!routine.state and weapons::damage_check(s, rtp.ktx)) {
		routine.state = 1;
		rtp.aud.play(res::sfx::Fan, 5);
		rtp.ktx.shrapnel(rtp.ktx.get<location_t>(s).center(), 4);
		rtp.cam.quake(2.0f, 0.18);
	}
}

void ai::holy_lance::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= glm::vec2(8.0f);
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 8.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::HolyLance);
	sprite.layer = 0.6f;
	sprite.position = location.position;

	ktx.assign_if<kinematics_t>(s);
	ktx.assign_if<actor_timer_t>(s);
	ktx.assign_if<routine_t>(s, tick);

	ktx.spawn(ai::holy_tether::type, location.position);
}

void ai::holy_lance::tick(entt::entity s, routine_tuple_t& rtp) {
	static constexpr real_t kMaxSpeed = 16.0f;
	static constexpr real_t kLowSpeed = 6.0f;

	auto& header = rtp.ktx.get<actor_header_t>(s);
	auto& location = rtp.ktx.get<location_t>(s);
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	auto& routine = rtp.ktx.get<routine_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	switch (routine.state) {
	case 0: /* Search Hooked */ {
		if (timer[0]++ < 10) {
			glm::vec2 reticule = rtp.nao.get_reticule();
			real_t angle = glm::atan(
				reticule.y - location.position.y,
				reticule.x - location.position.x
			);
			kinematics.accel_angle(angle, 10.0f);
			glm::vec2 naomi_center = rtp.ktx.get<location_t>(rtp.nao.actor).center();
			glm::vec2 actor_center = location.center();
			if (kinematics.flags[phy_t::Hooked]) {
				kinematics.flags[phy_t::Noclip] = true;
				kinematics.velocity = glm::zero<glm::vec2>();
				timer[0] = 0;
				routine.state = 3;
				auto& naomi_kinematics = rtp.ktx.get<kinematics_t>(rtp.nao.actor);
				naomi_kinematics.anchor = actor_center;
				naomi_kinematics.tether = glm::distance(actor_center, naomi_center);
				return;
			}
			entt::entity actor = weapons::find_hooked(s, rtp.ktx);
			if (actor != entt::null) {
				header.attach = actor;
				kinematics.flags[phy_t::Noclip] = true;
				kinematics.velocity = glm::zero<glm::vec2>();
				timer[0] = 0;
				routine.state = 1;
				auto& naomi_kinematics = rtp.ktx.get<kinematics_t>(rtp.nao.actor);
				naomi_kinematics.anchor = actor_center;
				naomi_kinematics.tether = glm::distance(actor_center, naomi_center);
			}
		} else {
			rtp.ktx.dispose(s);
		}
		break;
	}
	case 1: /* Actor Hooked */ {
		auto& naomi_kinematics = rtp.ktx.get<kinematics_t>(rtp.nao.actor);
		if (rtp.ktx.valid(header.attach)) {
			auto& attach_location = rtp.ktx.get<location_t>(header.attach);
			auto& attach_health = rtp.ktx.get<health_t>(header.attach);
			if (attach_health.flags[health_flags_t::Grappled] and attach_health.flags[health_flags_t::Hookable]) {
				location.position = attach_location.center() + location.bounding.center();
				naomi_kinematics.anchor = location.center();
				return;
			}
		}
		routine.state = 2;
		naomi_kinematics.tether = 0.0f;
		break;
	}
	case 2: /* Return */ {
		auto& naomi_location = rtp.ktx.get<location_t>(rtp.nao.actor);
		auto& naomi_kinematics = rtp.ktx.get<kinematics_t>(rtp.nao.actor);
		if (location.overlap(naomi_location)) {
			rtp.ktx.dispose(s);
		} else {
			glm::vec2 actor_center = location.center();
			glm::vec2 naomi_center = naomi_location.center();
			real_t magnitude = glm::length(naomi_kinematics.velocity);
			real_t angle = glm::atan(
				naomi_center.y - actor_center.y, 
				naomi_center.x - actor_center.x
			);
			real_t speed = glm::clamp(
				magnitude + (kLowSpeed / 2.0f), 
				kLowSpeed, kMaxSpeed
			);
			kinematics.accel_angle(angle, speed);
		}
		header.attach = entt::null;
		break;
	}
	default: {
		break;
	}
	}
}

void ai::holy_tether::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(0.0f, 0.0f, 1.0f, 1.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::HolyLance);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;
	sprite.pivot = glm::vec2(0.5f);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::holy_tether::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& location = rtp.ktx.get<location_t>(s);
	auto& sprite = rtp.ktx.get<sprite_t>(s);
	entt::entity lance = rtp.ktx.search_type(ai::holy_lance::type);
	if (lance != entt::null) {
		glm::vec2 naomi_center = rtp.ktx.get<location_t>(rtp.nao.actor).center();
		glm::vec2 actor_center = location.center();
		real_t angle = glm::atan(
			naomi_center.y - actor_center.y,
			naomi_center.x - actor_center.x
		);
		location.position = naomi_center;
		sprite.amend = true;
		sprite.angle = angle;
		sprite.scale = glm::vec2(
			glm::distance(naomi_center, actor_center), 1.0f
		);
	} else {
		rtp.ktx.dispose(s);
	}
}

void ai::kannon::ctor(entt::entity s, kontext_t& ktx) {
	static constexpr real_t kBegSpeed = 0.05f;
	static constexpr real_t kLowSpeed = 0.000001f;

	auto& location = ktx.get<location_t>(s);
	location.position -= 8.0f;
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 8.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	if (location.direction & direction_t::Down) {
		kinematics.velocity.x = 0.0f;
		if (kinematics.velocity.y > 0.0f) {
			kinematics.velocity.y += kBegSpeed;
		} else {
			kinematics.velocity.y = kBegSpeed;
		}
	} else if (location.direction & direction_t::Up) {
		kinematics.velocity.x = 0.0f;
		if (kinematics.velocity.y < 0.0f) {
			kinematics.velocity.y -= kBegSpeed;
		} else {
			kinematics.velocity.y = -kBegSpeed;
		}
	} else if (location.direction & direction_t::Left) {
		kinematics.velocity.y = kLowSpeed;
		if (kinematics.velocity.x < 0.0f) {
			kinematics.velocity.x -= kBegSpeed;
		} else {
			kinematics.velocity.x = -kBegSpeed;
		}
	} else {
		kinematics.velocity.y = kLowSpeed;
		if (kinematics.velocity.x > 0.0f) {
			kinematics.velocity.x += kBegSpeed;
		} else {
			kinematics.velocity.x = kBegSpeed;
		}
	}
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Kannon);
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 9;
	ktx.assign_if<actor_timer_t>(s);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::kannon::tick(entt::entity s, routine_tuple_t& rtp) {
	static constexpr real_t kTopSpeed = 7.0f;

	auto& location = rtp.ktx.get<location_t>(s);
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	if (kinematics.velocity.x != 0.0f) {
		kinematics.accel_x(
			kinematics.velocity.x > 0.0f ? 
			0.1f : -0.1f, kTopSpeed
		);
	} else if (kinematics.velocity.y != 0.0f) {
		kinematics.accel_y(
			kinematics.velocity.y > 0.0f ? 
			0.1f : -0.1f, kTopSpeed
		);
	}
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (kinematics.any_side() or 
		weapons::damage_check(s, rtp.ktx) or 
		timer[0]++ > 45) {
		rtp.ktx.spawn(ai::blast_large::type, location.center());
		rtp.aud.play(res::sfx::Explode2, 5);
		rtp.ktx.dispose(s);
	} else if (timer[1]++ > 8) {
		timer[1] = 0;
		rtp.ktx.spawn(ai::energy_trail::type, location.position);
	}
}

void ai::nail_ray::ctor(entt::entity s, kontext_t& ktx) {
	static constexpr real_t kTopSpeed = 3.0f;
	static constexpr real_t kLowSpeed = 0.000001f;

	auto& location = ktx.get<location_t>(s);
	location.position -= 16.0f;
	location.bounding = rect_t(8.0f, 8.0f, 16.0f, 16.0f);
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::NailRay);
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	if (location.direction & direction_t::Down) {
		kinematics.velocity.y = kTopSpeed;
		sprite.mirroring = mirroring_t::Horizontal;
	} else if (location.direction & direction_t::Up) {
		kinematics.velocity.y = -kTopSpeed;
	} else if (location.direction & direction_t::Left) {
		kinematics.velocity = glm::vec2(-kTopSpeed, kLowSpeed);
		sprite.mirroring = mirroring_t::Horizontal;
	} else {
		kinematics.velocity = glm::vec2(kTopSpeed, kLowSpeed);
	}
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 4;
	ktx.assign_if<actor_timer_t>(s);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::nail_ray::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& location = rtp.ktx.get<location_t>(s);
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (kinematics.any_side() or timer[0]++ > 42) {
		rtp.aud.play(res::sfx::Bwall, 5);
		rtp.ktx.spawn(ai::blast_small::type, location.center());
		rtp.ktx.dispose(s);
	} else if (timer[1]++ >= 6 and weapons::damage_check(s, rtp.ktx)) {
		timer[1] = 0;
		rtp.aud.play(res::sfx::Razor, 5);
		rtp.ktx.shrapnel(location.center(), 1);
	}
}

void ai::wolf_vulcan::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(0.0f, 0.0f, 1.0f, 1.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::HolyLance);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;
	sprite.pivot = glm::vec2(0.5f);
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 4;
	ktx.assign_if<actor_timer_t>(s);
	ktx.assign_if<routine_t>(s, tick);
}

void ai::wolf_vulcan::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& location = rtp.ktx.get<location_t>(s);
	auto& sprite = rtp.ktx.get<sprite_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (!timer[0]) {
		timer[0]++;
		real_t angle = 0.0f;
		if (location.direction & direction_t::Down) {
			angle = glm::half_pi<real_t>();
		} else if (location.direction & direction_t::Up) {
			angle = glm::pi<real_t>() * 1.5f;
		} else if (location.direction & direction_t::Left) {
			angle = glm::pi<real_t>();
		}
		glm::vec2 end_point = collision::trace_ray(
			rtp.map, 320.0f, 
			location.position, angle
		);
		sprite.amend = true;
		sprite.scale = glm::vec2(
			glm::distance(location.position, end_point), 1.0f
		);
		sprite.angle = glm::atan(
			end_point.y - location.position.y,
			end_point.x - location.position.x
		);
	} else if (timer[0]++ > 20) {
		rtp.ktx.dispose(s);
	} else if (weapons::damage_check(s, rtp.ktx)) {
		rtp.aud.play(res::sfx::Drill, 5);
		rtp.ktx.spawn(ai::blast_medium::type, location.center());
	} if (rtp.ktx.valid(s)) {
		sprite.alpha = glm::clamp(sprite.alpha - 0.075f, 0.0f, 1.0f);
	}
}

void ai::austere::ctor(entt::entity s, kontext_t& ktx) {
	static constexpr real_t kTopSpeed = 3.0f;
	static constexpr real_t kLowSpeed = 0.000001f;

	auto& location = ktx.get<location_t>(s);
	location.position -= 8.0f;
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 8.0f);
	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	if (location.direction & direction_t::Down) {
		location.direction = direction_t::Left;
		kinematics.velocity.y = -kTopSpeed;
	} else if (location.direction & direction_t::Up) {
		location.direction = direction_t::Right;
		kinematics.velocity.y = kTopSpeed;
	} else if (location.direction & direction_t::Left) {
		kinematics.velocity = glm::vec2(-kTopSpeed, kLowSpeed);
	} else {
		kinematics.velocity = glm::vec2(kTopSpeed, kLowSpeed);
	}
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::WolfVulcan);
	sprite.layer = 0.6f;
	sprite.position = location.position;
	auto& health = ktx.assign_if<health_t>(s);
	health.damage = 3;
	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 60;
	ktx.assign_if<routine_t>(s, tick);
	auto& header = ktx.get<actor_header_t>(s);
	header.attach = weapons::find_closest(s, ktx);
}

void ai::austere::tick(entt::entity s, routine_tuple_t& rtp) {
	static constexpr real_t kSpeed = 3.0f;

	auto& header = rtp.ktx.get<actor_header_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	auto& location = rtp.ktx.get<location_t>(s);
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	if (header.attach != entt::null and rtp.ktx.valid(header.attach)) {
		glm::vec2 actor_center = location.center();
		glm::vec2 attach_center = rtp.ktx.get<location_t>(header.attach).center();
		real_t angle = glm::atan(
			actor_center.y - attach_center.y,
			actor_center.x - attach_center.x
		);
		kinematics.accel_angle(angle, -kSpeed);
	}
	if (kinematics.any_side() or 
		weapons::damage_check(s, rtp.ktx) or
		timer[0]-- <= 0) {
		rtp.aud.play(res::sfx::Bwall, 5);
		rtp.ktx.spawn(ai::blast_small::type, location.center());
		rtp.ktx.dispose(s);
	}
}
