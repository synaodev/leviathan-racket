#ifndef LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP
#define LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP

#include "../component/routine.hpp"

namespace ai {
	namespace friends {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace kyoko {
		constexpr entt::hashed_string type = "kyoko"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace nauzika {
		constexpr entt::hashed_string type = "nauzika"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace senma {
		constexpr entt::hashed_string type = "senma"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace fox {
		constexpr entt::hashed_string type = "fox"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace gyo {
		constexpr entt::hashed_string type = "gyo"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_FRIENDS_HPP
