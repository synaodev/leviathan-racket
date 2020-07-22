#ifndef SYNAO_ACTOR_FRIENDS_HPP
#define SYNAO_ACTOR_FRIENDS_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace friends {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace kyoko {
		constexpr arch_t type = SYNAO_HASH("kyoko");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace nauzika {
		constexpr arch_t type = SYNAO_HASH("nauzika");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace senma {
		constexpr arch_t type = SYNAO_HASH("senma");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace fox {
		constexpr arch_t type = SYNAO_HASH("fox");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace gyo {
		constexpr arch_t type = SYNAO_HASH("gyo");
		void ctor(entt::entity s, kontext_t& ktx);
	}
}

#endif // SYNAO_ACTOR_FRIENDS_HPP