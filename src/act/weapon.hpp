#ifndef SYNAO_ACTOR_WEAPON_HPP
#define SYNAO_ACTOR_WEAPON_HPP

#include <entt/entity/registry.hpp>

#include "../utl/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace weapon {
		entt::entity find_closest(entt::entity s, kontext_t& ktx);
		entt::entity find_hooked(entt::entity s, kontext_t& ktx);
		bool damage_check(entt::entity s, kontext_t& ktx);
		bool damage_range(entt::entity s, kontext_t& ktx, glm::vec2 center, glm::vec2 dimensions);
		bool reverse_range(entt::entity s, kontext_t& ktx);
	}
	namespace frontier {
		constexpr arch_t type = SYNAO_HASH("frontier");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace toxitier {
		constexpr arch_t type = SYNAO_HASH("toxitier");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace weak_hammer {
		constexpr arch_t type = SYNAO_HASH("wkhammer");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace strong_hammer {
		constexpr arch_t type = SYNAO_HASH("sthammer");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace holy_lance {
		constexpr arch_t type = SYNAO_HASH("hylance");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace holy_tether {
		constexpr arch_t type = SYNAO_HASH("hytethr");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace kannon {
		constexpr arch_t type = SYNAO_HASH("kannon");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace nail_ray {
		constexpr arch_t type = SYNAO_HASH("nailray");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace wolf_vulcan {
		constexpr arch_t type = SYNAO_HASH("wlfvulc");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace austere {
		constexpr arch_t type = SYNAO_HASH("austere");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // SYNAO_ACTOR_WEAPON_HPP