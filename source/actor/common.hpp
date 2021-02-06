#ifndef LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP
#define LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP

#include "../component/routine.hpp"

namespace ai {
	namespace null {
		constexpr entt::hashed_string type = "null";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace hv_trigger {
		constexpr entt::hashed_string type = "hv_trigger";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace full_chest {
		constexpr entt::hashed_string type = "full_chest";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace empty_chest {
		constexpr entt::hashed_string type = "empty_chest";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace door {
		constexpr entt::hashed_string type = "door";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace spikes {
		constexpr entt::hashed_string type = "spikes";
		void ctor(entt::entity s, kontext_t& ktc);
	}
	namespace death_spikes {
		constexpr entt::hashed_string type = "death";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace bed {
		constexpr entt::hashed_string type = "bed";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace ammo_station {
		constexpr entt::hashed_string type = "ammo_station";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace computer {
		constexpr entt::hashed_string type = "computer";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace fire {
		constexpr entt::hashed_string type = "fire";
		void ctor(entt::entity s, kontext_t& kontext);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP
