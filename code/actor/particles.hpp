#ifndef SYNAO_ACTOR_PARTICLES_HPP
#define SYNAO_ACTOR_PARTICLES_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace particles {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace smoke {
		constexpr arch_t type = SYNAO_HASH("smoke");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shrapnel {
		constexpr arch_t type = SYNAO_HASH("shrapnel");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace dust {
		constexpr arch_t type = SYNAO_HASH("dust");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace splash {
		constexpr arch_t type = SYNAO_HASH("splash");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace blast_small {
		constexpr arch_t type = SYNAO_HASH("sblast");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace blast_medium {
		constexpr arch_t type = SYNAO_HASH("mblast");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace blast_large {
		constexpr arch_t type = SYNAO_HASH("lblast");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace energy_trail {
		constexpr arch_t type = SYNAO_HASH("engtrail");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace dash_flash {
		constexpr arch_t type = SYNAO_HASH("dashflash");
		void ctor(entt::entity s, kontext_t& ktx);
	}
	namespace barrier {
		constexpr arch_t type = SYNAO_HASH("barrier");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // SYNAO_ACTOR_PARTICLES_HPP