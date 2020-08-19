#ifndef LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP
#define LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace friends {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace kyoko {
		constexpr arch_t type = synao_hash("kyoko");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace nauzika {
		constexpr arch_t type = synao_hash("nauzika");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace senma {
		constexpr arch_t type = synao_hash("senma");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace fox {
		constexpr arch_t type = synao_hash("fox");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace gyo {
		constexpr arch_t type = synao_hash("gyo");
		void ctor(entt::entity s, kontext_t& ktx);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP
