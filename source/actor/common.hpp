#ifndef LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP
#define LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace null {
		constexpr arch_t type = synao_hash("null");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace hv_trigger {
		constexpr arch_t type = synao_hash("hvtrig");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace chest_full {
		constexpr arch_t type = synao_hash("chest");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace chest_empty {
		constexpr arch_t type = synao_hash("empty");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace door {
		constexpr arch_t type = synao_hash("door");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace spikes {
		constexpr arch_t type = synao_hash("spikes");
		void ctor(entt::entity s, kontext_t& ktc);
	}
	namespace death_spikes {
		constexpr arch_t type = synao_hash("death");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace bed {
		constexpr arch_t type = synao_hash("bed");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace ammo_station {
		constexpr arch_t type = synao_hash("ammostn");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace computer {
		constexpr arch_t type = synao_hash("cmptr");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace fireplace {
		constexpr arch_t type = synao_hash("fire");
		void ctor(entt::entity s, kontext_t& kontext);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_COMMON_HPP
