#ifndef LEVIATHAN_INCLUDED_ACTOR_GHOST_HPP
#define LEVIATHAN_INCLUDED_ACTOR_GHOST_HPP

#include "../component/routine.hpp"

namespace ai {
	namespace ghost {
		constexpr arch_t type = synao_hash("ghost");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_GHOST_HPP
