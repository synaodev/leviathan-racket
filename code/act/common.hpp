#ifndef SYNAO_ACTOR_COMMON_HPP
#define SYNAO_ACTOR_COMMON_HPP

#include <entt/entity/registry.hpp>

#include "../utl/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace null {
		constexpr arch_t type = SYNAO_HASH("null");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace hv_trigger {
		constexpr arch_t type = SYNAO_HASH("hvtrig");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace chest_full {
		constexpr arch_t type = SYNAO_HASH("chest");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace chest_empty {
		constexpr arch_t type = SYNAO_HASH("empty");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace door {
		constexpr arch_t type = SYNAO_HASH("door");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace death_spikes {
		constexpr arch_t type = SYNAO_HASH("death");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace bed {
		constexpr arch_t type = SYNAO_HASH("bed");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace ammo_station {
		constexpr arch_t type = SYNAO_HASH("ammostn");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace computer {
		constexpr arch_t type = SYNAO_HASH("cmptr");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace fireplace {
		constexpr arch_t type = SYNAO_HASH("fire");
		void ctor(entt::entity s, kontext_t& ktx);
	}
}

#endif // SYNAO_ACTOR_COMMON_HPP