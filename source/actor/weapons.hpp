#ifndef LEVIATHAN_INCLUDED_ACTOR_WEAPONS_HPP
#define LEVIATHAN_INCLUDED_ACTOR_WEAPONS_HPP

#include "../component/routine.hpp"

namespace ai {
	namespace weapons {
		entt::entity find_closest(entt::entity s, kontext_t& kontext);
		entt::entity find_hooked(entt::entity s, kontext_t& kontext);
		bool damage_check(entt::entity s, kontext_t& kontext);
		bool damage_range(entt::entity s, kontext_t& kontext, glm::vec2 center, glm::vec2 dimensions);
		bool reverse_range(entt::entity s, kontext_t& kontext);
	}
	namespace frontier {
		constexpr entt::hashed_string type = "frontier"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace toxitier {
		constexpr entt::hashed_string type = "toxitier"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace weak_hammer {
		constexpr entt::hashed_string type = "weak_hammer"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace strong_hammer {
		constexpr entt::hashed_string type = "strong_hammer"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace holy_lance {
		constexpr entt::hashed_string type = "holy_lance"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace holy_tether {
		constexpr entt::hashed_string type = "holy_tether"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace kannon {
		constexpr entt::hashed_string type = "kannon"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace nail_ray {
		constexpr entt::hashed_string type = "nail_ray"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace wolf_vulcan {
		constexpr entt::hashed_string type = "wolf_vulcan"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace austere {
		constexpr entt::hashed_string type = "austere"_hs;
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_WEAPONS_HPP
