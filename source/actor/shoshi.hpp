#pragma once

#include "../component/routine.hpp"

namespace ai {
	struct shoshi_state_t {
	public:
		shoshi_state_t() = default;
		shoshi_state_t(const shoshi_state_t&) = default;
		shoshi_state_t& operator=(const shoshi_state_t&) = default;
		shoshi_state_t(shoshi_state_t&&) noexcept = default;
		shoshi_state_t& operator=(shoshi_state_t&&) noexcept = default;
		~shoshi_state_t() = default;
	public:
		bool_t augment { false };
	};
	namespace shoshi {
		constexpr entt::hashed_string type = "shoshi";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_carry {
		constexpr entt::hashed_string type = "shoshi_carry";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_follow {
		constexpr entt::hashed_string type = "shoshi_follow";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}
