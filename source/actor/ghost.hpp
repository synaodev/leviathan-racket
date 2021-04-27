#pragma once

#include "../component/routine.hpp"

namespace ai {
	namespace ghost {
		constexpr entt::hashed_string type = "ghost";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}
