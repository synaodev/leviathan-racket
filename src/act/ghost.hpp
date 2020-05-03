#ifndef SYNAO_ACTOR_GHOST_HPP
#define SYNAO_ACTOR_GHOST_HPP

#include <entt/entity/registry.hpp>

#include "../utl/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace ghost {
		constexpr arch_t type = SYNAO_HASH("ghost");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // SYNAO_ACTOR_GHOST_HPP