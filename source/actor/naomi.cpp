#include "./naomi.hpp"
#include "./particles.hpp"
#include "./weapons.hpp"
#include "./shoshi.hpp"

#include <entt/entity/registry.hpp>
#include <fmt/core.h>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include "../component/kontext.hpp"
#include "../component/location.hpp"
#include "../component/kinematics.hpp"
#include "../component/sprite.hpp"
#include "../component/health.hpp"
#include "../component/blinker.hpp"
#include "../component/liquid.hpp"
#include "../field/collision.hpp"
#include "../field/camera.hpp"
#include "../menu/headsup-gui.hpp"
#include "../resource/id.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/kernel.hpp"
#include "../system/receiver.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"

namespace kNao {
	static constexpr sint_t BoxGood = 0;
	static constexpr sint_t BoxOkay = 1;
	static constexpr sint_t BoxSick = 2;
	static constexpr sint_t BoxHeal = 3;
	static constexpr sint_t Oxygens = 1500;
}

bool naomi_state_t::init(kontext_t& kontext) {
	// First instance of entt::entity will always be zero.
	backend = kontext.backend();
	entt::entity actor = backend->create();
	if (actor == entt::null) {
		synao_log("First actor allocation failed!\n");
		return false;
	}
	auto& location = backend->emplace<location_t>(actor);

	auto& kinematics = backend->emplace<kinematics_t>(actor);
	kinematics.discrete = {
		4.0f,
		16.0f / 3.0f,
		8.0f,
		16.0f / 3.0f
	};

	auto& sprite = backend->emplace<sprite_t>(actor, res::anim::Naomi);
	backend->emplace<health_t>(actor);

	auto& blinker = backend->emplace<blinker_t>(actor);
	backend->emplace<liquid_listener_t>(actor, ai::splash::type, res::sfx::Splash);

	location.bounding = {
		4.0f,
		0.0f,
		8.0f,
		16.0f
	};
	sprite.layer = 0.35f;

	blinker.blink_state = naomi_anim_t::Blinking;
	blinker.first_state = naomi_anim_t::Idle;

	synao_log("Naomi is ready.\n");
	return true;
}

void naomi_state_t::reset(kontext_t& kontext) {
	entt::entity actor = this->get_actor();

	auto& location = kontext.get<location_t>(actor);
	auto& kinematics = kontext.get<kinematics_t>(actor);
	auto& sprite = kontext.get<sprite_t>(actor);
	auto& health = kontext.get<health_t>(actor);
	auto& listener = kontext.get<liquid_listener_t>(actor);

	location.direction = direction_t::Right;
	kinematics.reset();
	kinematics.flags[phy_t::Bottom] = true;
	sprite.reset();
	sprite.layer = 0.35f;
	health.reset(2, 2, 0, 0);
	listener.liquid = entt::null;
	chroniker = {
		0,				// Hammered
		180,			// Leviathan
		0,				// WallJumping
		kNao::Oxygens,	// Oxygen
		0,				// Reloading
		0,				// Invincibility
		0,				// Barrier
		0,				// Charging
		0,				// Flashing
		90,				// Blinked
		0				// Freefall
	};
	flags.reset();
	equips.reset();
	riding = glm::zero<glm::vec2>();
	view_point = glm::zero<glm::vec2>();
	reticule = glm::zero<glm::vec2>();
	last_direction = direction_t::Right;
	this->set_phys_const(false);
}

void naomi_state_t::reset(kontext_t& kontext, const glm::vec2& position, direction_t direction, sint_t current_barrier, sint_t maximum_barrier, sint_t leviathan, arch_t hexadecimal_equips) {
	entt::entity actor = this->get_actor();

	auto& location = kontext.get<location_t>(actor);
	auto& kinematics = kontext.get<kinematics_t>(actor);
	auto& sprite = kontext.get<sprite_t>(actor);
	auto& health = kontext.get<health_t>(actor);
	auto& listener = kontext.get<liquid_listener_t>(actor);

	location.position = position;
	location.direction = direction;

	kinematics.reset();
	kinematics.flags[phy_t::Bottom] = true;
	sprite.reset();
	sprite.layer = 0.35f;
	health.reset(current_barrier, maximum_barrier, leviathan, 0);
	listener.liquid = entt::null;
	chroniker = {
		0,				// Hammered
		180,			// Leviathan
		0,				// WallJumping
		kNao::Oxygens,	// Oxygen
		0,				// Reloading
		0,				// Invincibility
		0,				// Barrier
		0,				// Charging
		0,				// Flashing
		90,				// Blinked
		0				// Freefall
	};
	flags.reset();
	equips = std::bitset<naomi_equips_t::Total>(hexadecimal_equips);
	riding = glm::zero<glm::vec2>();
	view_point = glm::zero<glm::vec2>();
	reticule = glm::zero<glm::vec2>();
	last_direction = direction_t::Right;
	this->set_phys_const(false);
}

void naomi_state_t::setup(audio_t& audio, const kernel_t& kernel, camera_t& camera, kontext_t& kontext) {
	entt::entity actor = this->get_actor();

	last_direction = direction_t::Right;
	auto& location = kontext.get<location_t>(actor);
	auto& kinematics = kontext.get<kinematics_t>(actor);
	auto& sprite = kontext.get<sprite_t>(actor);
	auto& listener = kontext.get<liquid_listener_t>(actor);
	// Reset Collision Variables
	kinematics.flags[phy_t::WillDrop] = false;
	kinematics.flags[phy_t::Noclip] = false;
	kinematics.flags[phy_t::Outbounds] = false;
	kinematics.flags[phy_t::Bottom] = true;
	kinematics.tether = 0.0f;
	// Reset Certain Flags
	flags[naomi_flags_t::Reticule] = false;
	flags[naomi_flags_t::Slinging] = false;
	flags[naomi_flags_t::AimingGrapple] = false;
	flags[naomi_flags_t::CannotGrapple] = false;
	// Set Position
	entt::entity found = kontext.search_id(kernel.get_identity());
	if (found != entt::null) {
		location.position = kontext.get<location_t>(found).position;
		sprite.new_state(naomi_anim_t::Idle);
	}
	// Set Camera
	camera.set_focus(location.center());
	// Set Shoshi
	if (equips[naomi_equips_t::ShoshiCarry]) {
		kontext.spawn(ai::shoshi_carry::type, location.position);
	} else if (equips[naomi_equips_t::ShoshiFollow]) {
		kontext.spawn(ai::shoshi_follow::type, location.position);
	}
	// Check for Liquid
	liquid::handle(audio, kontext, location, listener);
	if (listener.liquid != entt::null) {
		this->set_phys_const(true);
	} else {
		chroniker[naomi_timer_t::Oxygen] = kNao::Oxygens;
		this->set_phys_const(true);
	}
}

void naomi_state_t::handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, headsup_gui_t& headsup_gui, kontext_t& kontext, const tilemap_t& tilemap) {
	entt::entity actor = this->get_actor();

	auto& location = kontext.get<location_t>(actor);
	auto& kinematics = kontext.get<kinematics_t>(actor);
	auto& sprite = kontext.get<sprite_t>(actor);
	auto& health = kontext.get<health_t>(actor);
	auto& listener = kontext.get<liquid_listener_t>(actor);

	this->do_begin(audio, kinematics);
	if (flags[naomi_flags_t::Killed]) {
		this->do_killed(location, kinematics);
	} else if (!kernel.has(kernel_t::Lock)) {
		flags[naomi_flags_t::Scripted] = false;
		if (flags[naomi_flags_t::Damaged]) {
			this->do_recovery(kinematics);
		} else {
			this->do_meter_leviathan(health);
			this->do_invincible(sprite);
			this->do_barrier(kontext, location, health);
			this->do_strafe(input, audio);
			this->do_grapple(input, audio, kontext, tilemap, location, kinematics);
			this->do_fire(input, audio, kernel, kontext, location, kinematics, sprite);
			this->do_hammer(input, audio, kontext, location, kinematics);
			this->do_move(input, location, kinematics, false);
			this->do_look(input, location, kinematics);
			this->do_jump(input, audio, kinematics, false);
			this->do_step(audio, sprite);
			this->do_interact(input, receiver, kontext, location, kinematics);
			this->do_light_dash(input, kontext, location, kinematics, sprite);
			this->do_wall_kick(input, audio, tilemap, location, kinematics);
		}
	} else {
		this->do_move(input, location, kinematics, true);
		this->do_jump(input, audio, kinematics, true);
		this->do_step(audio, sprite);
	}
	this->do_physics(kinematics);
	this->do_submerge(kontext, listener);
	this->do_camera(location, kinematics);
	this->do_death(receiver, kinematics, health);
	this->do_animation(location, sprite, health);
	this->do_headsup(headsup_gui, health);
}

void naomi_state_t::damage(entt::entity other, audio_t& audio, kontext_t& kontext) {
	entt::entity actor = this->get_actor();

	if (!flags[naomi_flags_t::Invincible] and !flags[naomi_flags_t::Damaged]) {
		flags[naomi_flags_t::Damaged] = true;
		auto& naomi_location = kontext.get<location_t>(actor);
		naomi_location.vert(direction_t::Neutral);
		auto& naomi_kinematics = kontext.get<kinematics_t>(actor);
		naomi_kinematics.flags[phy_t::Bottom] = false;
		naomi_kinematics.velocity.y = -4.0f;
		if (kontext.has<kinematics_t>(other)) {
			auto& other_location = kontext.get<location_t>(other);
			auto& other_kinematics = kontext.get<kinematics_t>(other);
			if (!other_kinematics.velocity.x) {
				glm::vec2 naomi_center = naomi_location.center();
				glm::vec2 other_center = other_location.center();
				if (naomi_center.x < other_center.x) {
					naomi_location.hori(direction_t::Right);
					naomi_kinematics.velocity.x = -2.0f;
				} else {
					naomi_location.hori(direction_t::Left);
					naomi_kinematics.velocity.x = 2.0f;
				}
			} else {
				naomi_kinematics.velocity.x = other_kinematics.velocity.x;
			}
		} else {
			auto& other_location = kontext.get<location_t>(other);
			glm::vec2 naomi_center = naomi_location.center();
			glm::vec2 other_center = other_location.center();
			if (naomi_center.x < other_center.x) {
				naomi_location.hori(direction_t::Right);
				naomi_kinematics.velocity.x = -2.0f;
			} else {
				naomi_location.hori(direction_t::Left);
				naomi_kinematics.velocity.x = 2.0f;
			}
		}
		auto& naomi_health = kontext.get<health_t>(actor);
		auto& other_health = kontext.get<health_t>(other);
		other_health.attack(naomi_health);
		if (naomi_health.maximum <= 0) {
			flags[naomi_flags_t::Decimated] = true;
			flags[naomi_flags_t::Killed] = true;
			naomi_location.vert(direction_t::Up);
			kontext.smoke(naomi_location.center(), 8);
			kontext.shrapnel(naomi_location.center(), 6);
		} else if (naomi_health.current <= 0) {
			if (flags[naomi_flags_t::BrokenBarrier]) {
				flags[naomi_flags_t::Killed] = true;
				naomi_location.vert(direction_t::Up);
				kontext.smoke(naomi_location.center(), 10);
			} else {
				flags[naomi_flags_t::BrokenBarrier] = true;
				chroniker[naomi_timer_t::Barrier] = 360;
				audio.play(res::sfx::BrokenBarrier, 0);
				kontext.spawn(
					ai::barrier::type,
					naomi_location.center(),
					direction_t::Right
				);
			}
		} else {
			audio.play(res::sfx::Damage, 0);
			kontext.spawn(
				ai::barrier::type,
				naomi_location.center(),
				direction_t::Right
			);
		}
	}
}

void naomi_state_t::solids(entt::entity other, kontext_t& kontext, const tilemap_t& tilemap) {
	static constexpr real_t kStandMargin = 2.0f;
	static constexpr real_t kPushyMargin = 6.0f;

	entt::entity actor = this->get_actor();

	auto& naomi_location = kontext.get<location_t>(actor);
	rect_t naomi_hitbox	= naomi_location.hitbox();
	real_t n_left = naomi_hitbox.x;
	real_t n_top = naomi_hitbox.y;
	real_t n_right = naomi_hitbox.right();
	real_t n_bottom	= naomi_hitbox.bottom();

	const auto& other_location = kontext.get<location_t>(other);
	rect_t other_hitbox	= other_location.hitbox();
	real_t o_left = other_hitbox.x;
	real_t o_top = other_hitbox.y;
	real_t o_right = other_hitbox.right();
	real_t o_bottom	= other_hitbox.bottom();
	real_t o_center_x = other_hitbox.center_x();
	real_t o_center_y = other_hitbox.center_y();

	if (n_top < o_bottom - kPushyMargin and n_bottom > o_top + kPushyMargin) {
		auto& naomi_kinematics = kontext.get<kinematics_t>(actor);
		const auto& other_kinematics = kontext.get<kinematics_t>(other);

		if (n_right > o_left and n_right < o_center_x) {
			naomi_location.position.x = other_hitbox.side(side_t::Left) - naomi_location.bounding.side(side_t::Right);
			naomi_kinematics.velocity.x = other_kinematics.velocity.x < 0.0f ?
				other_kinematics.velocity.x :
				0.0f;
		}
		if (n_left < o_right and n_left > o_center_x) {
			naomi_location.position.x = other_hitbox.side(side_t::Right) - naomi_location.bounding.side(side_t::Left);
			naomi_kinematics.velocity.x = other_kinematics.velocity.x > 0.0f ?
				other_kinematics.velocity.x :
				0.0f;
		}
	}
	if (n_left > o_right - kStandMargin or n_right < o_left + kStandMargin) {

	} else {
		auto& naomi_kinematics = kontext.get<kinematics_t>(actor);
		const auto& other_kinematics = kontext.get<kinematics_t>(other);

		if (n_bottom >= o_top and n_bottom <= o_center_y) {
			naomi_location.position.y = other_hitbox.side(side_t::Top) - naomi_location.bounding.side(side_t::Bottom);
			naomi_kinematics.velocity.y = 0.0f;
			kinematics_t::handle(naomi_location, naomi_kinematics, tilemap, other_kinematics.velocity);
			naomi_kinematics.flags[phy_t::Bottom] = true;

			if (other_kinematics.velocity.x != 0.0f and riding.x == 0.0f) {
				naomi_location.position.x -= other_kinematics.velocity.x;
				naomi_kinematics.velocity.x = 0.0f;
			}
			if (other_kinematics.velocity.y != 0.0f and riding.y == 0.0f) {
				naomi_location.position.y -= other_kinematics.velocity.y;
				naomi_kinematics.velocity.y = 0.0f;
			}

			riding = other_kinematics.velocity;
		} else if (n_top < o_bottom and n_top > o_center_y) {
			naomi_location.position.y = other_hitbox.side(side_t::Bottom) - naomi_location.bounding.side(side_t::Top);
			naomi_kinematics.velocity.y = other_kinematics.velocity.y > 0.0f ?
				other_kinematics.velocity.y :
				0.0f;
		}
	}
}

static constexpr real_t kIdealMaxHSpeed = 2.0f;
static constexpr real_t kIdealMaxVSpeed = 6.0f;
static constexpr real_t kIdealMoveAccel = 0.1f;
static constexpr real_t kIdealMoveDecel = 0.24f;
static constexpr real_t kIdealJumpPower = 3.7f;
static constexpr real_t kIdealJumpAdded = 0.16f;
static constexpr real_t kIdealGravAccel = 0.28f;
static constexpr real_t kIdealDashSpeed = 5.0f;
static constexpr sint_t kMaximumBarrier = 16;
static constexpr sint_t kMaximumLeviPwr = 999;

void naomi_state_t::set_phys_const(bool submerged) {
	if (submerged) {
		max_hspeed = kIdealMaxHSpeed / 2.0f;
		max_vspeed = kIdealMaxVSpeed / 2.0f;
		move_accel = kIdealMoveAccel / 2.0f;
		move_decel = kIdealMoveDecel / 2.0f;
		jump_power = kIdealJumpPower / 1.5f;
		jump_added = kIdealJumpAdded / 1.5f;
		grav_speed = kIdealGravAccel / 2.0f;
		dash_speed = kIdealDashSpeed / 2.0f;
	} else {
		max_hspeed = kIdealMaxHSpeed;
		max_vspeed = kIdealMaxVSpeed;
		move_accel = kIdealMoveAccel;
		move_decel = kIdealMoveDecel;
		jump_power = kIdealJumpPower;
		jump_added = kIdealJumpAdded;
		grav_speed = kIdealGravAccel;
		dash_speed = kIdealDashSpeed;
	}
}

void naomi_state_t::set_visible(bool visible) {
	entt::entity actor = this->get_actor();

	auto& sprite = backend->get<sprite_t>(actor);
	sprite.layer = visible ?
		layer_value::Automatic :
		layer_value::Invisible;
}

void naomi_state_t::set_equips(naomi_equips_t flag, bool value) {
	equips[flag] = value;
}

void naomi_state_t::set_teleport_location(real_t x, real_t y) {
	entt::entity actor = this->get_actor();

	auto& location = backend->get<location_t>(actor);
	location.position = { x, y };
	location.position *= constants::TileSize<real_t>();
}

void naomi_state_t::set_sprite_animation(arch_t state, direction_t direction) {
	entt::entity actor = this->get_actor();

	flags[naomi_flags_t::Scripted] = true;
	auto& sprite = backend->get<sprite_t>(actor);
	if (direction != direction_t::Neutral) {
		if (direction & direction_t::Left) {
			sprite.mirroring = mirroring_t::Horizontal;
		} else {
			sprite.mirroring = mirroring_t::None;
		}
		if (direction & direction_t::Down) {
			sprite.oriented = oriented_t::Down;
		} else if (direction & direction_t::Up) {
			sprite.oriented = oriented_t::Top;
		} else {
			sprite.oriented = oriented_t::None;
		}
	}
	sprite.new_state(state);
}

void naomi_state_t::bump_kinematics(direction_t direction) {
	entt::entity actor = this->get_actor();

	auto& location = backend->get<location_t>(actor);
	auto& kinematics = backend->get<kinematics_t>(actor);

	if (direction & direction_t::Left) {
		location.hori(direction_t::Right);
		kinematics.velocity = { -1.0f, -2.5f };
	} else {
		location.hori(direction_t::Left);
		kinematics.velocity = { 1.0f, -2.5f };
	}
}

void naomi_state_t::boost_current_barrier(sint_t amount) {
	entt::entity actor = this->get_actor();

	auto& health = backend->get<health_t>(actor);
	health.current = glm::max(
		health.current + amount,
		health.maximum
	);
	flags[naomi_flags_t::BrokenBarrier] = false;
	flags[naomi_flags_t::HealthIncrement] = true;
	chroniker[naomi_timer_t::Barrier] = 0;
}

void naomi_state_t::boost_maximum_barrer(sint_t amount) {
	entt::entity actor = this->get_actor();

	auto& health = backend->get<health_t>(actor);
	health.maximum = glm::clamp(
		health.maximum + amount,
		0, kMaximumBarrier
	);
}

void naomi_state_t::mut_leviathan_power(sint_t amount) {
	entt::entity actor = this->get_actor();

	auto& health = backend->get<health_t>(actor);
	health.leviathan = glm::clamp(
		health.leviathan + amount,
		0, kMaximumLeviPwr
	);
}

bool naomi_state_t::interacting() const {
	return flags[naomi_flags_t::Interacting];
}

entt::entity naomi_state_t::get_actor() const {
	// Naomi is always the first actor created,
	// and is never destroyed until the kontext dtor,
	// so I can always safely assume her ID to be zero.
	return entt::entity{0};
}

glm::vec2 naomi_state_t::get_reticule() const {
	return reticule;
}

glm::vec2 naomi_state_t::camera_placement() const {
	entt::entity actor = this->get_actor();
	return view_point + backend->get<location_t>(actor).center();
}

std::string naomi_state_t::hexadecimal_equips() const {
	return fmt::format("{:x}", equips.to_ulong());
}

naomi_death_t naomi_state_t::get_death_type(const kinematics_t& kinematics, const health_t& health) const {
	if (flags[naomi_flags_t::Decimated]) {
		return naomi_death_t::Decimated;
	} else if (flags[naomi_flags_t::Killed]) {
		return naomi_death_t::Killed;
	} else if (kinematics.flags[phy_t::Outbounds]) {
		return naomi_death_t::OutBounds;
	} else if (chroniker[naomi_timer_t::Oxygen] <= 0) {
		return naomi_death_t::Drowned;
	} else if (health.leviathan >= 999) {
		return naomi_death_t::Poisoned;
	}
	return naomi_death_t::Error;
}

sint_t naomi_state_t::get_box_data(const headsup_gui_t& headsup_gui, const std::bitset<naomi_flags_t::Total>& flags, const headsup_params_t& params) {
	if (headsup_gui.get_main_state() != kNao::BoxHeal) {
		if (flags[naomi_flags_t::HealthIncrement]) {
			return kNao::BoxHeal;
		} else if (params.current_leviathan >= 650) {
			return kNao::BoxSick;
		} else if (params.current_leviathan >= 350) {
			return kNao::BoxOkay;
		}
		return kNao::BoxGood;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void naomi_state_t::do_begin(audio_t& audio, kinematics_t& kinematics) {
	flags[naomi_flags_t::Interacting] = false;
	flags[naomi_flags_t::HealthIncrement] = false;
	flags[naomi_flags_t::TetheredTile] = kinematics.tether > 0.0f;
	if (kinematics.flags[phy_t::Bottom]) {
		if (flags[naomi_flags_t::Airbourne]) {
			flags[naomi_flags_t::Airbourne] = false;
			audio.play(res::sfx::Landing, 0);
		}
	} else {
		flags[naomi_flags_t::Airbourne] = true;
	}
}

void naomi_state_t::do_killed(location_t& location, kinematics_t& kinematics) {
	if (kinematics.flags[phy_t::Bottom]) {
		location.vert(direction_t::Neutral);
		kinematics.velocity = glm::zero<glm::vec2>();
		flags[naomi_flags_t::DeathAnimation] = true;
	}
}

void naomi_state_t::do_recovery(kinematics_t& kinematics) {
	if (kinematics.flags[phy_t::Bottom]) {
		flags[naomi_flags_t::Damaged] = false;
		flags[naomi_flags_t::Invincible] = true;
		chroniker[naomi_timer_t::Invincibility] = 60;
	}
}

void naomi_state_t::do_meter_leviathan(health_t& health) {
	if (chroniker[naomi_timer_t::Leviathan]-- <= 0) {
		chroniker[naomi_timer_t::Leviathan] = 180;
		if (health.leviathan > 0) {
			--health.leviathan;
		}
	}
}

void naomi_state_t::do_invincible(sprite_t& sprite) {
	if (flags[naomi_flags_t::Invincible]) {
		if (chroniker[naomi_timer_t::Invincibility]-- <= 0) {
			chroniker[naomi_timer_t::Invincibility] = 0;
			flags[naomi_flags_t::Invincible] = false;
			sprite.layer = layer_value::Automatic;
		} else {
			sprite.layer = sprite.layer != layer_value::Automatic ?
				layer_value::Automatic :
				layer_value::Invisible;
		}
	}
}

void naomi_state_t::do_barrier(kontext_t& kontext, const location_t& location, health_t& health) {
	if (flags[naomi_flags_t::BrokenBarrier]) {
		if (chroniker[naomi_timer_t::Barrier]-- <= 0) {
			chroniker[naomi_timer_t::Barrier] = 0;
			flags[naomi_flags_t::BrokenBarrier] = false;
			health.current = 1;
			kontext.spawn(
				ai::barrier::type,
				location.center(),
				direction_t::Left
			);
		}
	}
}

void naomi_state_t::do_strafe(const input_t& input, audio_t& audio) {
	if (input.pressed[btn_t::Strafe]) {
		flags.flip(naomi_flags_t::Strafing);
		audio.play(res::sfx::Inven, 7);
	}
}

void naomi_state_t::do_grapple(const input_t&, audio_t&, kontext_t&, const tilemap_t&, const location_t&, kinematics_t&) {
	if (equips[naomi_equips_t::Grapple]) {
		// TODO
	}
}

void naomi_state_t::do_fire(const input_t& input, audio_t& audio, kernel_t& kernel, kontext_t& kontext, const location_t& location, kinematics_t& kinematics, const sprite_t& sprite) {
	flags[naomi_flags_t::Firing] = false;
	if (chroniker[naomi_timer_t::Reloading] > 0) {
		--chroniker[naomi_timer_t::Reloading];
	}
	if (input.holding[btn_t::Item]) {
		flags[naomi_flags_t::Firing] = true;
		if (!chroniker[naomi_timer_t::Reloading]) {
			glm::ivec4* weapon = kernel.get_item_ptr();
			if (weapon and weapon->w != 0 and weapon->y > 0) {

				glm::vec2 position = location.position;
				direction_t direction = location.direction;
				glm::vec2 velocity = kinematics.velocity;
				oriented_t oriented = sprite.oriented;
				mirroring_t mirroring = sprite.mirroring;

				glm::vec2 action_point = sprite.action_point(
					naomi_anim_t::JumpingFiring,
					oriented,
					mirroring,
					position
				);

				switch (weapon->x) {
				case naomi_weapon_t::None: {
					break;
				}
				case naomi_weapon_t::Kannon: {
					--weapon->y;
					chroniker[naomi_timer_t::Reloading] = 60;
					audio.play(res::sfx::Kannon, 4);
					kontext.spawn(
						ai::kannon::type,
						action_point,
						velocity,
						direction
					);
					break;
				}
				case naomi_weapon_t::Frontier: {
					--weapon->y;
					chroniker[naomi_timer_t::Reloading] = 6;
					audio.play(res::sfx::Spark, 4);
					kontext.spawn(
						ai::frontier::type,
						action_point,
						direction
					);
					if (direction & direction_t::Down) {
						kinematics.accel_y(0.01f, 0.3f);
					}
					break;
				}
				case naomi_weapon_t::WolfVulcan: {
					--weapon->y;
					chroniker[naomi_timer_t::Reloading] = 30;
					audio.play(res::sfx::Fan, 4);
					kontext.spawn(
						ai::wolf_vulcan::type,
						action_point,
						direction
					);
					break;
				}
				case naomi_weapon_t::NailRay: {
					--weapon->y;
					chroniker[naomi_timer_t::Reloading] = 20;
					audio.play(res::sfx::Razor, 4);
					kontext.spawn(
						ai::nail_ray::type,
						action_point,
						direction
					);
					kontext.spawn(
						ai::blast_small::type,
						action_point,
						direction
					);
					break;
				}
				case naomi_weapon_t::Austere: {
					--weapon->y;
					chroniker[naomi_timer_t::Reloading] = 30;
					audio.play(res::sfx::Fan, 4);
					kontext.spawn(
						ai::austere::type,
						action_point,
						direction
					);
					break;
				}
				default: {
					break;
				}
				}
			}
		}
	}
}

void naomi_state_t::do_hammer(const input_t& input, audio_t& audio, kontext_t& kontext, const location_t& location, kinematics_t& kinematics) {
	if (equips[naomi_equips_t::WeakHammer]) {
		if (input.pressed[btn_t::Hammer]) {
			flags[naomi_flags_t::WillHammer] = true;
		}
		if (flags[naomi_flags_t::Hammering]) {
			if (chroniker[naomi_timer_t::Hammered]++ > 8) {
				chroniker[naomi_timer_t::Hammered] = 0;
				flags[naomi_flags_t::Hammering] = false;
			}
		} else if (flags[naomi_flags_t::WillHammer]) {
			if (kinematics.flags[phy_t::Bottom]) {
				kinematics.velocity.x = 0.0f;
			}
			flags[naomi_flags_t::Hammering] = true;
			flags[naomi_flags_t::WillHammer] = false;
			flags[naomi_flags_t::Firing] = false;
			if (equips[naomi_equips_t::StrongHammer]) {
				audio.play(res::sfx::Blade, 4);
				kontext.spawn(
					ai::strong_hammer::type,
					location.center(),
					kinematics.velocity,
					location.direction
				);
			} else {
				audio.play(res::sfx::Blade, 4);
				kontext.spawn(
					ai::weak_hammer::type,
					location.center(),
					kinematics.velocity,
					location.direction
				);
			}
		}
	}
}

void naomi_state_t::do_move(const input_t& input, location_t& location, kinematics_t& kinematics, bool_t locked) {
	if (!flags[naomi_flags_t::WallJumping] and !flags[naomi_flags_t::WallPrepare] and !flags[naomi_flags_t::Charging] and !flags[naomi_flags_t::Dashing]) {
		bool right = locked ? false : input.holding[btn_t::Right];
		bool lefts = locked ? false : input.holding[btn_t::Left];
		if ((right and lefts) or !(right or lefts)) {
			flags[naomi_flags_t::Moving] = false;
			if (flags[naomi_flags_t::TetheredTile] or kinematics.flags[phy_t::Bottom]) {
				kinematics.decel_x(move_decel);
			}
		} else if (!flags[naomi_flags_t::TetheredTile]) {
			flags[naomi_flags_t::Moving] = true;
			kinematics.accel_x(
				right ? move_accel : -move_accel,
				flags[naomi_flags_t::Slinging] ?
					max_hsling : max_hspeed
			);
			if (!flags[naomi_flags_t::Strafing]) {
				location.hori(right ? direction_t::Right : direction_t::Left);
			}
		} else {
			flags[naomi_flags_t::Moving] = true;
			kinematics.velocity.x += right ? 0.05f : -0.05f;
			location.hori(right ? direction_t::Right : direction_t::Left);
		}
	}
}

void naomi_state_t::do_look(const input_t& input, location_t& location, const kinematics_t& kinematics) {
	if (!flags[naomi_flags_t::Dashing]) {
		bool up = input.holding[btn_t::Up];
		bool down = input.holding[btn_t::Down];
		if ((up and down) or !(up or down)) {
			location.vert(direction_t::Neutral);
		} else if (up) {
			location.vert(direction_t::Up);
		} else if (down) {
			location.vert(
				!(kinematics.flags[phy_t::Bottom]) ?
					direction_t::Down : direction_t::Neutral
			);
		}
	}
}

void naomi_state_t::do_step(audio_t& audio, const sprite_t& sprite) {
	if (sprite.state == naomi_anim_t::Walking or sprite.state == naomi_anim_t::WalkingFiring) {
		if (sprite.frame % 2) {
			if (!flags[naomi_flags_t::Stepped]) {
				flags[naomi_flags_t::Stepped] = true;
				audio.play(res::sfx::Walk, 1);
			}
		} else {
			flags[naomi_flags_t::Stepped] = false;
		}
	} else {
		flags[naomi_flags_t::Stepped] = false;
	}
}

void naomi_state_t::do_jump(const input_t& input, audio_t& audio, kinematics_t& kinematics, bool_t locked) {
	if (!kinematics.flags[phy_t::Bottom]) {
		kinematics.velocity.x += riding.x;
		riding = glm::zero<glm::vec2>();

		if (!locked) {
			if (!flags[naomi_flags_t::WallPrepare] and !flags[naomi_flags_t::TetheredTile]) {
				if (input.holding[btn_t::Jump]) {
					if (kinematics.velocity.y < 0.0f) {
						kinematics.velocity.y -= jump_added;
					}
				}
			}
		}
	} else if (!flags[naomi_flags_t::TetheredTile]) {
		flags[naomi_flags_t::Slinging] = false;
		if (!locked) {
			if (input.pressed[btn_t::Jump]) {
				if (kinematics.flags[phy_t::FallThrough] and input.holding[btn_t::Down]) {
					kinematics.flags[phy_t::FallThrough] = false;
				} else {
					kinematics.velocity.y = -jump_power;
					kinematics.velocity += riding;
					riding = glm::zero<glm::vec2>();
					audio.play(res::sfx::Jump, 0);
				}
			}
		}
	}
}

void naomi_state_t::do_interact(const input_t& input, receiver_t& receiver, kontext_t& kontext, const location_t& location, kinematics_t& kinematics) {
	if (kinematics.flags[phy_t::Bottom]) {
		if (input.pressed[btn_t::Context]) {
			if (!flags[naomi_flags_t::Moving] and !flags[naomi_flags_t::Dashing] and !flags[naomi_flags_t::Charging] and !flags[naomi_flags_t::Hammering] and !flags[naomi_flags_t::Firing] and !flags[naomi_flags_t::TetheredTile]) {
				kinematics.velocity.x = 0.0f;
				flags[naomi_flags_t::Interacting] = true;
				rect_t hitbox = location.hitbox();
				kontext.slice<actor_trigger_t, location_t>().each([&hitbox, &receiver](entt::entity, const actor_trigger_t& trigger, const location_t& location) {
					if (trigger.bitmask[actor_trigger_t::InteractionEvent]) {
						if (location.overlap(hitbox)) {
							receiver.run_event(trigger.identity);
						}
					}
				});
			}
		}
	}
}

void naomi_state_t::do_light_dash(const input_t& input, kontext_t& kontext, location_t& location, kinematics_t& kinematics, sprite_t& sprite) {
	if (equips[naomi_equips_t::LightDash]) {
		if (flags[naomi_flags_t::Dashing]) {
			if (chroniker[naomi_timer_t::Flashing]-- <= 0) {
				chroniker[naomi_timer_t::Flashing] = 6;
				kontext.spawn(
					ai::dash_flash::type,
					location.center()
				);
			}
			if (flags[naomi_flags_t::DashingCeiling]) {
				real_t speed = location.direction & direction_t::Left ?
					-dash_speed :
					dash_speed;
				if (kinematics.flags[phy_t::Right]) {
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					location.vert(direction_t::Up);
					location.hori(
						location.direction & direction_t::Left ?
						direction_t::Right : direction_t::Left
					);
					flags[naomi_flags_t::DashingWalls] = true;
					flags[naomi_flags_t::DashingCeiling] = false;
					flags[naomi_flags_t::CannotFall] = true;
					if (location.direction & direction_t::Left) {
						sprite.mirroring = mirroring_t::Vertical;
					}
					else {
						sprite.mirroring = mirroring_t::Both;
					}
					sprite.variation = 0;
				} else if (kinematics.flags[phy_t::Left]) {
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					location.vert(direction_t::Up);
					location.hori(
						location.direction & direction_t::Left ?
						direction_t::Right : direction_t::Left
					);
					flags[naomi_flags_t::DashingWalls] = true;
					flags[naomi_flags_t::DashingCeiling] = false;
					flags[naomi_flags_t::CannotFall] = true;
					if (location.direction & direction_t::Left) {
						sprite.mirroring = mirroring_t::Vertical;
					} else {
						sprite.mirroring = mirroring_t::Both;
					}
					sprite.variation = 1;
				} else {
					kinematics.velocity.x = speed;
				}
			} else if (flags[naomi_flags_t::DashingWalls]) {
				real_t speed = location.direction & direction_t::Up ?
					dash_speed :
					-dash_speed;
				if (kinematics.flags[phy_t::Top]) {
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					location.vert(direction_t::Neutral);
					location.hori(
						location.direction & direction_t::Left ?
						direction_t::Right : direction_t::Left
					);
					flags[naomi_flags_t::DashingCeiling] = true;
					flags[naomi_flags_t::DashingWalls] = false;
					flags[naomi_flags_t::CannotFall] = false;
					if (location.direction & direction_t::Left) {
						sprite.mirroring = mirroring_t::Horizontal;
					} else {
						sprite.mirroring = mirroring_t::None;
					}
					sprite.variation = 0;
				} else if (kinematics.flags[phy_t::Bottom]) {
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					location.vert(direction_t::Neutral);
					flags[naomi_flags_t::DashingCeiling] = false;
					flags[naomi_flags_t::DashingWalls] = false;
					flags[naomi_flags_t::CannotFall] = false;
					if (location.direction & direction_t::Left) {
						sprite.mirroring = mirroring_t::Horizontal;
					} else {
						sprite.mirroring = mirroring_t::None;
					}
					sprite.variation = 0;
				} else {
					kinematics.velocity = glm::vec2(0.0f, speed);
				}
			} else {
				real_t speed = location.direction & direction_t::Left ?
					-dash_speed :
					dash_speed;
				if (kinematics.flags[phy_t::Right]) {
					flags[naomi_flags_t::DashingWalls] = true;
					flags[naomi_flags_t::CannotFall] = true;
					location.hori(direction_t::Right);
					location.vert(direction_t::Neutral);
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					sprite.mirroring = mirroring_t::None;
					sprite.variation = 0;
				} else if (kinematics.flags[phy_t::Left]) {
					flags[naomi_flags_t::DashingWalls] = true;
					flags[naomi_flags_t::CannotFall] = true;
					location.hori(direction_t::Left);
					location.vert(direction_t::Neutral);
					kinematics.flags[phy_t::Right] = false;
					kinematics.flags[phy_t::Left] = false;
					kinematics.flags[phy_t::Top] = false;
					kinematics.flags[phy_t::Bottom] = false;
					kinematics.velocity = glm::zero<glm::vec2>();
					sprite.mirroring = mirroring_t::Horizontal;
					sprite.variation = 1;
				} else {
					kinematics.velocity.x = speed;
				}
			}
			if (input.pressed[btn_t::Dash]) {
				flags[naomi_flags_t::Dashing] = false;
				flags[naomi_flags_t::DashingCeiling] = false;
				flags[naomi_flags_t::DashingWalls] = false;
				flags[naomi_flags_t::CannotFall] = false;
				chroniker[naomi_timer_t::Flashing] = 0;
				sprite.variation = 0;
			}
		} else if (!flags[naomi_flags_t::Charging] and !flags[naomi_flags_t::TetheredTile]) {
			if (kinematics.flags[phy_t::Bottom]) {
				if (input.pressed[btn_t::Dash]) {
					flags[naomi_flags_t::Charging] = true;
					chroniker[naomi_timer_t::Charging] = 15;
					kinematics.velocity.x = 0.0f;
				}
			}
		} else if (chroniker[naomi_timer_t::Charging] > 0) {
			--chroniker[naomi_timer_t::Charging];
			if (chroniker[naomi_timer_t::Charging] <= 0) {
				chroniker[naomi_timer_t::Charging] = 0;
				chroniker[naomi_timer_t::Flashing] = 6;
				flags[naomi_flags_t::Charging] = false;
				flags[naomi_flags_t::Dashing] = true;
				kinematics.flags[phy_t::Right] = false;
				kinematics.flags[phy_t::Left] = false;
				kinematics.flags[phy_t::Top] = false;
				kinematics.flags[phy_t::Bottom] = false;
				kinematics.velocity.y = 0.0f;
			}
		}
	}
}

void naomi_state_t::do_wall_kick(const input_t& input, audio_t& audio, const tilemap_t& tilemap, location_t& location, kinematics_t& kinematics) {
	if (equips[naomi_equips_t::WallKick]) {
		if (!kinematics.flags[phy_t::Bottom]) {
			if (flags[naomi_flags_t::WallJumping]) {
				kinematics.velocity.x = location.direction & direction_t::Left ?
					-max_hspeed :
					max_hspeed;
				if (chroniker[naomi_timer_t::WallJumping]-- <= 0) {
					chroniker[naomi_timer_t::WallJumping] = 0;
					flags[naomi_flags_t::WallJumping] = false;
				}
			}
			if (flags[naomi_flags_t::WallPrepare]) {
				if (chroniker[naomi_timer_t::WallJumping]++ >= 10) {
					flags[naomi_flags_t::WallPrepare] = false;
					flags[naomi_flags_t::CannotFall] = false;
					flags[naomi_flags_t::WallJumping] = true;
					kinematics.velocity.y = -jump_power;
					audio.play(res::sfx::Jump, 0);
				}
			} else if (kinematics.hori_sides()) {
				if (!flags[naomi_flags_t::TetheredTile]) {
					if (input.pressed[btn_t::Jump]) {
						if (location.direction & direction_t::Left) {
							if (collision::attempt(kinematics_t::predict(location, side_t::Left, -2.0f), kinematics.flags, tilemap, side_t::Left)) /*Check Left*/ {
								kinematics.velocity.y = 0.0f;
								location.hori(direction_t::Right);
								flags[naomi_flags_t::WallPrepare] = true;
								flags[naomi_flags_t::CannotFall] = true;
								chroniker[naomi_timer_t::WallJumping] = 0;
							}
						} else if (collision::attempt(kinematics_t::predict(location, side_t::Right, 2.0f), kinematics.flags, tilemap, side_t::Right)) /* Check Right*/ {
							kinematics.velocity.y = 0.0f;
							location.hori(direction_t::Left);
							flags[naomi_flags_t::WallPrepare] = true;
							flags[naomi_flags_t::CannotFall] = true;
							chroniker[naomi_timer_t::WallJumping] = 0;
						}
					}
				}
			}
		}
	}
}

constexpr real_t kIncrOffsetX 	= 1.0f;
constexpr real_t kFarOffsetX 	= 48.0f;
constexpr real_t kIncrOffsetY1	= 2.0f;
constexpr real_t kIncrOffsetY2	= 6.0f;
constexpr real_t kGroundOffsetY = -8.0f;
constexpr real_t kAirOffsetY 	= 80.0f;
constexpr sint64_t kFallLimit 	= 60;

void naomi_state_t::do_camera(const location_t& location, const kinematics_t& kinematics) {
	if (location.direction & direction_t::Left) {
		view_point.x = glm::max(view_point.x - kIncrOffsetX, -kFarOffsetX);
	} else {
		view_point.x = glm::min(view_point.x + kIncrOffsetX, kFarOffsetX);
	}
	if (!flags[naomi_flags_t::Airbourne]) {
		chroniker[naomi_timer_t::Freefall] = 0;
		view_point.y = glm::max(view_point.y - kIncrOffsetY1, kGroundOffsetY);
	} else if (kinematics.velocity.y >= kIdealMaxVSpeed) {
		if (chroniker[naomi_timer_t::Freefall]++ > kFallLimit) {
			view_point.y = glm::min(view_point.y + kIncrOffsetY2, kAirOffsetY);
		}
	}
}

void naomi_state_t::do_physics(kinematics_t& kinematics) {
	if (!flags[naomi_flags_t::CannotFall]) {
		kinematics.accel_y(
			flags[naomi_flags_t::DashingCeiling] ?
				-grav_speed : grav_speed,
			max_vspeed
		);
	}
}

void naomi_state_t::do_submerge(kontext_t& kontext, liquid_listener_t& listener) {
	if (listener.liquid != entt::null and kontext.valid(listener.liquid)) {
		this->set_phys_const(true);
		if (!equips[naomi_equips_t::Oxygen]) {
			--chroniker[naomi_timer_t::Oxygen];
		}
	} else {
		this->set_phys_const(false);
		chroniker[naomi_timer_t::Oxygen] = kNao::Oxygens;
	}
}

void naomi_state_t::do_animation(location_t& location, sprite_t& sprite, const health_t& health) {
	if (!flags[naomi_flags_t::Scripted]) {
		arch_t state = sprite.state;
		if (flags[naomi_flags_t::DeathAnimation]) {
			state = naomi_anim_t::Killed;
		} else if (flags[naomi_flags_t::Damaged]) {
			state = naomi_anim_t::Damaged;
		} else if (flags[naomi_flags_t::WallPrepare]) {
			state = naomi_anim_t::WallJumping;
		} else if (flags[naomi_flags_t::Charging]) {
			state = naomi_anim_t::Charging;
		} else if (flags[naomi_flags_t::DashingWalls]) {
			state = naomi_anim_t::DashingWalls;
		} else if (flags[naomi_flags_t::DashingCeiling]) {
			state = naomi_anim_t::DashingCeiling;
		} else if (flags[naomi_flags_t::Dashing]) {
			state = naomi_anim_t::DashingFloor;
			location.vert(direction_t::Neutral);
		} else if (flags[naomi_flags_t::Hammering]) {
			state = naomi_anim_t::Hammering;
		} else if (flags[naomi_flags_t::Airbourne]) {
			if (flags[naomi_flags_t::Strafing] or flags[naomi_flags_t::Firing]) {
				state = naomi_anim_t::JumpingFiring;
			} else {
				state = naomi_anim_t::Jumping;
			}
		} else if (flags[naomi_flags_t::Moving]) {
			if (flags[naomi_flags_t::Strafing] or flags[naomi_flags_t::Firing]) {
				state = naomi_anim_t::WalkingFiring;
			} else {
				state = naomi_anim_t::Walking;
			}
		} else if (flags[naomi_flags_t::Interacting] or state == naomi_anim_t::Interacting) {
			state = naomi_anim_t::Interacting;
			location.vert(direction_t::Neutral);
		} else if (flags[naomi_flags_t::Strafing] or flags[naomi_flags_t::Firing]) {
			state = naomi_anim_t::IdleFiring;
		} else if (health.leviathan >= 350) {
			state = naomi_anim_t::Tired;
		} else if (state == naomi_anim_t::Blinking) {
			state = naomi_anim_t::Blinking;
		} else {
			state = naomi_anim_t::Idle;
		}
		sprite.new_state(state);
		if (location.direction != last_direction and !flags[naomi_flags_t::Dashing]) {
			last_direction = location.direction;
			if (location.direction & direction_t::Left) {
				sprite.mirroring |= mirroring_t::Horizontal;
			} else {
				sprite.mirroring &= ~mirroring_t::Horizontal;
			}
			if (location.direction & direction_t::Down) {
				sprite.oriented = oriented_t::Down;
			} else if (location.direction & direction_t::Up) {
				sprite.oriented = oriented_t::Top;
			} else {
				sprite.oriented = oriented_t::None;
			}
		}
	}
}

void naomi_state_t::do_death(receiver_t& receiver, const kinematics_t& kinematics, const health_t& health) {
	if (!receiver.running()) {
		naomi_death_t type = this->get_death_type(kinematics, health);
		if (type != naomi_death_t::Error) {
			receiver.run_death(type);
		}
	}
}

void naomi_state_t::do_headsup(headsup_gui_t& headsup_gui, const health_t& health) {
	headsup_params_t params = {};
	params.current_barrier = health.current;
	params.maximum_barrier = health.maximum;
	params.current_leviathan = health.leviathan;
	params.main_state = naomi_state_t::get_box_data(headsup_gui, flags, params);
	params.main_strafing = flags[naomi_flags_t::Strafing];
	params.current_oxygen = equips[naomi_equips_t::Oxygen] ?
		static_cast<sint_t>(kNao::Oxygens / 15) :
		static_cast<sint_t>(chroniker[naomi_timer_t::Oxygen] / 15);
	params.maximum_oxygen = static_cast<sint_t>(kNao::Oxygens / 15);
	headsup_gui.set_parameters(params);
}
