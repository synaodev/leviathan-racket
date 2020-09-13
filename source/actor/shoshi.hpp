#ifndef LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
#define LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP

#include "../component/routine.hpp"

namespace ai {
	struct shoshi_state_t {
	public:
		shoshi_state_t() : augment(false) {}
		shoshi_state_t(const shoshi_state_t&) = default;
		shoshi_state_t& operator=(const shoshi_state_t&) = default;
		shoshi_state_t(shoshi_state_t&&) = default;
		shoshi_state_t& operator=(shoshi_state_t&&) = default;
		~shoshi_state_t() = default;
	public:
		bool_t augment;
	};
	namespace shoshi {
		constexpr entt::hashed_string type = "shoshi"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_carry {
		constexpr entt::hashed_string type = "shoshi_carry"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_follow {
		constexpr entt::hashed_string type = "shoshi_follow"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
