#include "./shoshi.hpp"
#include "./particles.hpp"
#include "./naomi.hpp"

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/kinematics.hpp"
#include "../component/sprite.hpp"
#include "../component/blinker.hpp"
#include "../component/liquid.hpp"
#include "../resource/id.hpp"
#include "../system/audio.hpp"

// Functions

static bool test_shoshi_sprite_mirroring(mirroring_t mirroring, direction_t direction) {
	return (
		(mirroring == mirroring_t::None and (direction & direction_t::Left) == direction_t::Right) or
		(mirroring == mirroring_t::Horizontal and (direction & direction_t::Left) == direction_t::Left)
	);
}

void ai::shoshi::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 12.0f);

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Shoshi);
	sprite.table = 0.0f;
	sprite.layer = 0.2f;
	sprite.position = location.position;

	kontext.assign_if<blinker_t>(s, 0, 4);
	kontext.assign_if<routine_t>(s, tick);
	kontext.sort<sprite_t>(sprite_t::compare);
}

void ai::shoshi::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& location = rtp.kontext.get<location_t>(s);
	auto& kinematics = rtp.kontext.get<kinematics_t>(s);
	auto& sprite = rtp.kontext.get<sprite_t>(s);
	auto& routine = rtp.kontext.get<routine_t>(s);
	if (!routine.state) {
		if (kinematics.velocity.x != 0.0f) {
			location.direction = kinematics.velocity.x > 0.0f ? direction_t::Right : direction_t::Left;
			if (test_shoshi_sprite_mirroring(sprite.mirroring, location.direction)) {
				sprite.amend = true;
				if (location.direction & direction_t::Left) {
					sprite.mirroring = mirroring_t::Horizontal;
				} else {
					sprite.mirroring = mirroring_t::None;
				}
			}
			sprite.new_state(kinematics.flags[phy_t::Bottom] ? 1 : 2);
		} else if (!kinematics.flags[phy_t::Bottom]) {
			sprite.new_state(2);
		} else if (sprite.state != 4) {
			sprite.new_state(0);
		}
	}
	kinematics.accel_y(0.1f, 6.0f);
}

void ai::shoshi_carry::ctor(entt::entity s, kontext_t& kontext) {
	auto& location = kontext.get<location_t>(s);
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 12.0f);

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Shoshi);
	sprite.state = 3;
	sprite.table = 0.0f;
	sprite.layer = 0.22f;
	sprite.position = location.position;

	kontext.assign_if<blinker_t>(s, 3, 5);
	kontext.assign_if<routine_t>(s, tick);
	kontext.sort<sprite_t>(sprite_t::compare);
}

void ai::shoshi_carry::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& naomi_location = rtp.kontext.get<location_t>(rtp.naomi.get_actor());
	auto& location = rtp.kontext.get<location_t>(s);
	location.position = naomi_location.position;
	location.hori(
		naomi_location.direction & direction_t::Left ?
		direction_t::Left : direction_t::Right
	);

	auto& naomi_sprite = rtp.kontext.get<sprite_t>(rtp.naomi.get_actor());
	auto& sprite = rtp.kontext.get<sprite_t>(s);
	if (sprite.mirroring != naomi_sprite.mirroring) {
		sprite.amend = true;
		sprite.mirroring = naomi_sprite.mirroring;
	}
}


void ai::shoshi_follow::ctor(entt::entity s, kontext_t& kontext) {
	kontext.assign_if<shoshi_state_t>(s);

	auto& location = kontext.get<location_t>(s);
	location.bounding = rect_t(4.0f, 4.0f, 8.0f, 12.0f);

	auto& sprite = kontext.assign_if<sprite_t>(s, res::anim::Shoshi);
	sprite.table = 0.0f;
	sprite.layer = 0.2f;
	sprite.position = location.position;

	kontext.assign_if<kinematics_t>(s);
	kontext.assign_if<blinker_t>(s, 0, 4);
	kontext.assign_if<routine_t>(s, tick);
	kontext.assign_if<liquid_listener_t>(s, ai::splash::type, res::sfx::Splash);
	kontext.sort<sprite_t>(sprite_t::compare);
}

void ai::shoshi_follow::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& shoshi_state = rtp.kontext.get<shoshi_state_t>(s);
	auto& location = rtp.kontext.get<location_t>(s);
	auto& kinematics = rtp.kontext.get<kinematics_t>(s);
	auto& sprite = rtp.kontext.get<sprite_t>(s);
	auto& listener = rtp.kontext.get<liquid_listener_t>(s);
	auto& naomi_location = rtp.kontext.get<location_t>(rtp.naomi.get_actor());
	auto& naomi_kinematics = rtp.kontext.get<kinematics_t>(rtp.naomi.get_actor());

	bool should_jump = false;
	glm::vec2 naomi_center = naomi_location.center();
	glm::vec2 shoshi_center = location.center();
	glm::vec2 box_distance = glm::vec2(
		glm::abs(naomi_center.x - shoshi_center.x),
		glm::abs(naomi_center.y - shoshi_center.y)
	);

	real_t kMaxHsp = 2.0f;
	real_t kMaxVsp = 6.0f;
	real_t kAccelX = 0.1f;
	real_t kDecelX = 0.24f;
	real_t kJumpPw = 3.7f;
	real_t kJumpHd = 0.16f;
	real_t kGravSp = 0.28f;

	if (listener.liquid != entt::null and rtp.kontext.valid(listener.liquid)) {
		kMaxHsp /= 2.0f;
		kMaxVsp /= 2.0f;
		kAccelX /= 2.0f;
		kDecelX /= 2.0f;
		kJumpPw /= 1.5f;
		kJumpHd /= 1.5f;
		kGravSp /= 2.0f;
	}

	if (box_distance.y > 16.0f) {
		if (shoshi_center.y < naomi_center.y) {
			if (kinematics.flags[phy_t::FallThrough] and kinematics.flags[phy_t::Bottom]) {
				kinematics.flags[phy_t::WillDrop] = true;
				kinematics.flags[phy_t::Bottom] = false;
			}
		} else if (naomi_kinematics.flags[phy_t::FallThrough]) {
			should_jump = true;
		}
	}
	if (box_distance.x > 16.0f) {
		if (naomi_center.x > shoshi_center.x) {
			location.hori(direction_t::Right);
			kinematics.accel_x(kAccelX, kMaxHsp);
			if (kinematics.flags[phy_t::Right]) {
				should_jump = true;
			}
		} else {
			location.hori(direction_t::Left);
			kinematics.accel_x(-kAccelX, kMaxHsp);
			if (kinematics.flags[phy_t::Left]) {
				should_jump = true;
			}
		}
		if (kinematics.flags[phy_t::Bottom]) {
			sprite.new_state(1);
			shoshi_state.augment = true;
		} else if (shoshi_state.augment) {
			sprite.new_state(2);
			shoshi_state.augment = false;
			kinematics.velocity.y = -kJumpPw;
			should_jump = true;
			rtp.audio.play(res::sfx::Jump, 0);
		} else {
			sprite.new_state(2);
		}
		if (should_jump and kinematics.flags[phy_t::Bottom]) {
			kinematics.velocity.y = -kJumpPw;
		}
		if (kinematics.velocity.y < 0.0f) {
			kinematics.velocity.y -= kJumpHd;
		}
	} else if (kinematics.flags[phy_t::Bottom]) {
		kinematics.decel_x(kDecelX);
		sprite.new_state(0);
		shoshi_state.augment = true;
	}
	kinematics.accel_y(kGravSp, kMaxVsp);
	if (test_shoshi_sprite_mirroring(sprite.mirroring, location.direction)) {
		sprite.amend = true;
		if (location.direction & direction_t::Left) {
			sprite.mirroring = mirroring_t::Horizontal;
		} else {
			sprite.mirroring = mirroring_t::None;
		}
	}
}

// Tables

LEVIATHAN_CTOR_TABLE_CREATE(routine_ctor_generator_t) {
	LEVIATHAN_TABLE_PUSH(ai::shoshi::type, 			ai::shoshi::ctor);
	LEVIATHAN_TABLE_PUSH(ai::shoshi_carry::type, 	ai::shoshi_carry::ctor);
	LEVIATHAN_TABLE_PUSH(ai::shoshi_follow::type, 	ai::shoshi_follow::ctor);
}
