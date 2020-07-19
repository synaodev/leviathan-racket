#include "./particles.hpp"
#include "./naomi.hpp"

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/kinematics.hpp"
#include "../component/sprite.hpp"
#include "../utility/enums.hpp"

#include "../resource/id.hpp"

SYNAO_CTOR_TABLE_CREATE(routine_generator_t) {
	SYNAO_CTOR_TABLE_PUSH(ai::smoke::type, 			ai::smoke::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::shrapnel::type, 		ai::shrapnel::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::dust::type, 			ai::dust::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::splash::type, 		ai::splash::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::blast_small::type, 	ai::blast_small::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::blast_medium::type, 	ai::blast_medium::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::blast_large::type, 	ai::blast_large::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::energy_trail::type, 	ai::energy_trail::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::dash_flash::type, 	ai::dash_flash::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::barrier::type, 		ai::barrier::ctor);
}

void ai::particles::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (timer[0]-- <= 0) {
		rtp.ktx.dispose(s);
	}
}

void ai::smoke::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 8.0f;
	location.bounding = rect_t(6.0f, 6.0f, 4.0f, 4.0f);

	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.accel_angle(
		rng::next(0.0f, glm::two_pi<real_t>()), 
		rng::next(0.3f, 3.0f)
	);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Smoke);
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 35;

	ktx.assign_if<routine_t>(s, tick);
}

void ai::smoke::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (timer[0]-- <= 0) {
		rtp.ktx.dispose(s);
	} else if (kinematics.hori_sides()) {
		kinematics.decel_y(0.05f);
	} else if (kinematics.vert_sides()) {
		kinematics.decel_x(0.05f);
	}
}

void ai::shrapnel::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position += glm::vec2(
		rng::next(-3.0f, 3.0f) - 8.0f,
		rng::next(-3.0f, 3.0f) - 8.0f
	);

	auto& kinematics = ktx.assign_if<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	kinematics.accel_angle(
		rng::next(-2.44346f, -0.698132f),
		rng::next(1.0f, 6.0f)
	);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Shrapnel);
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 25;

	ktx.assign_if<routine_t>(s, tick);
}

void ai::shrapnel::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (timer[0] <= 0) {
		rtp.ktx.dispose(s);
	} else {
		auto& kinematics = rtp.ktx.get<kinematics_t>(s);
		kinematics.accel_y(0.2f, 6.0f);
	}
}

void ai::dust::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding =  rect_t(0.0f, 0.0f, 1.0f, 1.0f);
	auto& kinematics = ktx.get<kinematics_t>(s);
	kinematics.flags[phy_t::Noclip] = true;
	real_t variation = rng::next(-0.08f, 0.08f);
	real_t speed = rng::next(3.0f, 4.0f);
	if (location.direction & direction_t::Down) {
		location.position += glm::vec2(
			rng::next(-6.0f, 6.0f), 8.0f
		);
		kinematics.accel_angle(
			glm::half_pi<real_t>() + variation, speed
		);
	} else if (location.direction & direction_t::Up) {
		location.position += glm::vec2(
			rng::next(-6.0f, 6.0f), -8.0f
		);
		kinematics.accel_angle(
			1.5f * glm::pi<real_t>() + variation, speed
		);
	} else if (location.direction & direction_t::Left) {
		location.position += glm::vec2(
			-8.0f, rng::next(-6.0f, 6.0f)
		);
		kinematics.accel_angle(
			glm::pi<real_t>() + variation, speed
		);
	} else {
		location.position += glm::vec2(
			8.0f, rng::next(-6.0f, 6.0f)
		);
		kinematics.accel_angle(
			variation, speed
		);
	}
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::HolyLance);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;

	ktx.assign_if<routine_t>(s, tick);
}

void ai::dust::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& sprite = rtp.ktx.get<sprite_t>(s);
	sprite.amend = true;
	sprite.alpha -= 0.02f;
	if (sprite.alpha <= 0.0f) {
		rtp.ktx.dispose(s);
	}
}

void ai::splash::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= glm::vec2(8.0f, 16.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Splash);
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 7;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::blast_small::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 8.0f;

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Blast);
	sprite.state = 0;
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 4;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::blast_medium::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 16.0f;

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Blast);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 8;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::blast_large::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 24.0f;

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Blast);
	sprite.state = 2;
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 9;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::energy_trail::ctor(entt::entity s, kontext_t& ktx) {
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Kannon);
	sprite.state = 1;
	sprite.layer = 0.6f;
	sprite.position = ktx.get<location_t>(s).position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 7;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::dash_flash::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.position -= 16.0f;

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::DashFlash);
	sprite.layer = 0.6f;
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 6;

	ktx.assign_if<routine_t>(s, particles::tick);
}

void ai::barrier::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Barrier);

	sprite.variation = location.direction & direction_t::Left;
	sprite.layer = 0.6f;
	sprite.scale = glm::vec2(2.0f);
	sprite.position = location.position;

	auto& timer = ktx.assign_if<actor_timer_t>(s);
	timer[0] = 12;

	ktx.assign_if<routine_t>(s, tick);
}

void ai::barrier::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& location = rtp.ktx.get<location_t>(s);
	auto& sprite = rtp.ktx.get<sprite_t>(s);
	auto& timer = rtp.ktx.get<actor_timer_t>(s);
	if (timer[0]-- <= 0) {
		rtp.ktx.dispose(s);
	} else {
		glm::vec2 scale = sprite.scale / 2.0f;
		glm::vec2 dimensions = glm::vec2(32.0f, 22.0f) * scale;
		location.position = rtp.ktx.get<location_t>(rtp.nao.actor).center() - dimensions;
	}
}
