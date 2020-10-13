#ifndef LEVIATHAN_INCLUDED_ACTOR_NAOMI_HPP
#define LEVIATHAN_INCLUDED_ACTOR_NAOMI_HPP

#include <vector>
#include <bitset>
#include <glm/vec2.hpp>
#include <entt/entity/fwd.hpp>

#include "../utility/enums.hpp"

struct setup_file_t;
struct input_t;
struct audio_t;
struct kernel_t;
struct receiver_t;
struct headsup_gui_t;
struct camera_t;
struct kontext_t;
struct tilemap_t;

struct location_t;
struct kinematics_t;
struct sprite_t;
struct blinker_t;
struct health_t;
struct headsup_params_t;

struct liquid_body_t;
struct liquid_listener_t;

namespace __enum_naomi_flags {
	enum type : arch_t {
		Reticule,
		Scripted,
		Moving,
		Airbourne,
		Stepped,
		Firing,
		Hammering,
		WillHammer,
		Interacting,
		Strafing,
		CannotFall,
		Slinging,
		WallPrepare,
		WallJumping,
		BrokenBarrier,
		Damaged,
		Invincible,
		Killed,
		Decimated,
		DeathAnimation,
		Charging,
		Dashing,
		DashingWalls,
		DashingCeiling,
		HealthIncrement,
		AimingGrapple,
		CannotGrapple,
		TetheredTile,
		Total
	};
}

using naomi_flags_t = __enum_naomi_flags::type;

namespace __enum_naomi_equips {
	enum type : arch_t {
		WeakHammer,
		WallKick,
		Grapple,
		LightDash,
		Comet,
		Oxygen,
		OnceMore,
		StrongHammer,
		ShoshiCarry,
		ShoshiFollow,
		Total
	};
}

using naomi_equips_t = __enum_naomi_equips::type;

namespace __enum_naomi_timer {
	enum type : arch_t {
		Hammered,
		Leviathan,
		WallJumping,
		Oxygen,
		Reloading,
		Invincibility,
		Barrier,
		Charging,
		Flashing,
		Blinked
	};
}

using naomi_timer_t = __enum_naomi_timer::type;

namespace __enum_naomi_anim {
	enum type : arch_t {
		Idle,
		Walking,
		Jumping,
		IdleFiring,
		WalkingFiring,
		JumpingFiring,
		Tired,
		Interacting,
		Damaged,
		WallJumping,
		Hammering,
		Charging,
		DashingFloor,
		DashingWalls,
		DashingCeiling,
		Killed,
		Sleeping,
		Awakening,
		StandingUp,
		Blinking
	};
}

using naomi_anim_t = __enum_naomi_anim::type;

namespace __enum_naomi_weapon {
	enum type : arch_t {
		None,
		Kannon,
		Frontier,
		WolfVulcan,
		NailRay,
		Austere
	};
}

using naomi_weapon_t = __enum_naomi_weapon::type;

namespace __enum_naomi_death {
	enum type : arch_t {
		Error,
		Decimated,
		Killed,
		OutBounds,
		Drowned,
		Poisoned
	};
}

using naomi_death_t = __enum_naomi_death::type;

struct naomi_state_t : public not_copyable_t {
public:
	naomi_state_t();
	naomi_state_t(naomi_state_t&&) = default;
	naomi_state_t& operator=(naomi_state_t&&) = default;
	~naomi_state_t() = default;
public:
	bool init(kontext_t& kontext);
	void reset(kontext_t& kontext);
	void reset(kontext_t& kontext, glm::vec2 position, direction_t direction, sint_t current_barrier, sint_t maximum_barrier, sint_t leviathan, arch_t hexadecimal_equips);
	void setup(audio_t& audio, const kernel_t& kernel, camera_t& camera, kontext_t& kontext);
	void handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, headsup_gui_t& headsup_gui, kontext_t& kontext, const tilemap_t& tilemap);
	void damage(entt::entity other, audio_t& audio, kontext_t& kontext);
	void solids(entt::entity other, kontext_t& kontext, const tilemap_t& tilemap);
	void set_phys_const(bool submerged);
	void set_visible(bool visible);
	void set_equips(naomi_equips_t flag, bool value);
	void set_teleport_location(real_t x, real_t y);
	void set_sprite_animation(arch_t state, direction_t direction);
	void bump_kinematics(direction_t direction);
	void boost_current_barrier(sint_t amount);
	void boost_maximum_barrer(sint_t amount);
	void mut_leviathan_power(sint_t amount);
	bool interacting() const;
	entt::entity get_actor() const;
	glm::vec2 get_reticule() const;
	glm::vec2 camera_placement() const;
	std::string hexadecimal_equips() const;
	naomi_death_t get_death_type(const kinematics_t& kinematics, const health_t& health) const;
	static sint_t get_box_data(const headsup_gui_t& headsup_gui, const std::bitset<naomi_flags_t::Total>& flags, const headsup_params_t& params);
private:
	void do_begin(audio_t& audio, kinematics_t& kinematics);
	void do_killed(location_t& location, kinematics_t& kinematics);
	void do_recovery(kinematics_t& kinematics);
	void do_meter_leviathan(health_t& health);
	void do_invincible(sprite_t& sprite);
	void do_barrier(kontext_t& kontext, const location_t& location, health_t& health);
	void do_strafe(const input_t& input, audio_t& audio);
	void do_grapple(const input_t& input, audio_t& audio, kontext_t& kontext, const tilemap_t& tilemap, const location_t& location, kinematics_t& kinematics);
	void do_fire(const input_t& input, audio_t& audio, kernel_t& kernel, kontext_t& kontext, const location_t& location, kinematics_t& kinematics, const sprite_t& sprite);
	void do_hammer(const input_t& input, audio_t& audio, kontext_t& kontext, const location_t& location, kinematics_t& kinematics);
	void do_move(const input_t& input, location_t& location, kinematics_t& kinematics, bool_t locked);
	void do_look(const input_t& input, location_t& location, const kinematics_t& kinematics);
	void do_step(audio_t& audio, const sprite_t& sprite);
	void do_jump(const input_t& input, audio_t& audio, kinematics_t& kinematics, bool_t locked);
	void do_interact(const input_t& input, receiver_t& receiver, kontext_t& kontext, const location_t& location, kinematics_t& kinematics);
	void do_light_dash(const input_t& input, kontext_t& kontext, location_t& location, kinematics_t& kinematics, sprite_t& sprite);
	void do_wall_kick(const input_t& input, audio_t& audio, const tilemap_t& tilemap, location_t& location, kinematics_t& kinematics);
	void do_cam_move(const location_t& location);
	void do_physics(kinematics_t& kinematics);
	void do_submerge(kontext_t& kontext, liquid_listener_t& listener);
	void do_animation(location_t& location, sprite_t& sprite, const health_t& health);
	void do_death(receiver_t& receiver, const kinematics_t& kinematics, const health_t& health);
	void do_headsup(headsup_gui_t& headsup_gui, const health_t& health);
private:
	std::bitset<naomi_flags_t::Total> flags;
	std::bitset<naomi_equips_t::Total> equips;
	std::vector<sint64_t> chroniker;
	glm::vec2 riding, view_point, reticule;
	entt::registry* backend;
	direction_t last_direction;
	real_t max_hspeed, max_hsling, max_vspeed;
	real_t move_accel, move_decel;
	real_t jump_power, jump_added;
	real_t grav_speed, dash_speed;
};

#endif // LEVIATHAN_INCLUDED_ACTOR_NAOMI_HPP
