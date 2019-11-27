#include "./naomi.hpp"
#include "./particles.hpp"
#include "./shoshi.hpp"

#include <entt/entity/registry.hpp>

#include "../cnt/kontext.hpp"
#include "../cnt/location.hpp"
#include "../cnt/kinematics.hpp"
#include "../cnt/sprite.hpp"
#include "../cnt/health.hpp"
#include "../cnt/blinker.hpp"
#include "../cnt/liquid.hpp"
#include "../sys/kernel.hpp"
#include "../sys/camera.hpp"
#include "../utl/logger.hpp" 

#include "../res_id.hpp"

namespace kNao {
	static constexpr arch_t BoxGood = 0;
	static constexpr arch_t BoxOkay = 1;
	static constexpr arch_t BoxSick = 2;
	static constexpr arch_t BoxHeal = 3;
	static constexpr sint_t Oxygens = 1500;
}

naomi_state_t::naomi_state_t() :
	actor(entt::null),
	flags(0),
	equips(0),
	chroniker(),
	riding(0.0f),
	viewpoint(0.0f),
	reticule(0.0f),
	backend(nullptr),
	max_hspeed(0.0f),
	max_hsling(0.0f),
	max_vspeed(0.0f),
	move_accel(0.0f),
	move_decel(0.0f),
	jump_power(0.0f),
	jump_added(0.0f),
	grav_speed(0.0f),
	dash_speed(0.0f)
{

}

bool naomi_state_t::init(kontext_t& kontext) {
	backend = kontext.backend();
	actor = backend->create();
	if (actor == entt::null) {
		SYNAO_LOG("Couldn't generate naomi actor!\n");
		return false;
	}
	backend->assign<location_t>(actor);
	backend->assign<kinematics_t>(actor);
	backend->assign<sprite_t>(actor, res::anim::Naomi);
	backend->assign<health_t>(actor);
	backend->assign<blinker_t>(actor);
	backend->assign<liquid_listener_t>(actor, ai::splash::type, res::sfx::Splash);

	auto& location = kontext.get<location_t>(actor);
	location.bounding = rect_t(4.0f, 0.0f, 8.0f, 16.0f);
	auto& sprite = kontext.get<sprite_t>(actor);
	sprite.table = 1.0f;
	sprite.layer = 0.35f;
	auto& blinker = kontext.get<blinker_t>(actor);
	blinker.blink_state = naomi_anim_t::Blinking;
	blinker.first_state = naomi_anim_t::Idle;
	return true;
}

void naomi_state_t::reset(kontext_t& kontext) {
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
		90				// Blinked
	};
	flags.reset();
	equips.reset();
	riding = glm::zero<glm::vec2>();
	viewpoint = glm::zero<glm::vec2>();
	reticule = glm::zero<glm::vec2>();
	this->set_phys_const(false);
}

void naomi_state_t::reset(kontext_t& kontext, glm::vec2 position, direction_t direction, sint_t current_barrier, sint_t maximum_barrier, sint_t leviathan, arch_t hexadecimal_equips) {
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
	sprite.direction = direction;
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
		90				// Blinked
	};
	flags.reset();
	equips = std::bitset<naomi_equips_t::Total>(hexadecimal_equips);
	riding = glm::zero<glm::vec2>();
	viewpoint = glm::zero<glm::vec2>();
	reticule = glm::zero<glm::vec2>();
	this->set_phys_const(false);
}

void naomi_state_t::setup(audio_t& audio, const kernel_t& kernel, camera_t& camera, kontext_t& kontext) {
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
	auto& sprite = backend->get<sprite_t>(actor);
	sprite.layer = visible ? 
		layer_value::Automatic : 
		layer_value::Invisible;
}

void naomi_state_t::set_equips(naomi_equips_t flag, bool value) {
	equips[flag] = value;
}

void naomi_state_t::set_teleport_location(real_t x, real_t y) {
	auto& location = backend->get<location_t>(actor);
	location.position = glm::vec2(x, y) * 16.0f;
}

void naomi_state_t::set_sprite_animation(arch_t state, direction_t direction) {
	flags[naomi_flags_t::Scripted] = true;
	auto& location = backend->get<location_t>(actor);
	auto& sprite = backend->get<sprite_t>(actor);
	if (direction != direction_t::Neutral) {
		location.direction = direction;
		sprite.direction = direction;
		sprite.write = true;
	}
	sprite.new_state(state);
}

void naomi_state_t::bump_kinematics(direction_t direction) {
	auto& location = backend->get<location_t>(actor);
	auto& kinematics = backend->get<kinematics_t>(actor);
	if (direction & direction_t::Left) {
		location.hori(direction_t::Right);
		kinematics.velocity = glm::vec2(-1.0f, -2.5f);
	} else {
		location.hori(direction_t::Left);
		kinematics.velocity = glm::vec2(1.0f, -2.5f);
	}
}

void naomi_state_t::boost_current_barrier(sint_t amount) {
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
	auto& health = backend->get<health_t>(actor);
	health.maximum = glm::clamp(
		health.maximum + amount, 
		0, kMaximumBarrier
	);
}

void naomi_state_t::mut_leviathan_power(sint_t amount) {
	auto& health = backend->get<health_t>(actor);
	health.leviathan = glm::clamp(
		health.leviathan + amount,
		0, kMaximumLeviPwr
	);
}