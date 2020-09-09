#ifndef LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
#define LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct routine_tuple_t;
struct kontext_t;

namespace ai {
	namespace particles {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace smoke {
		constexpr arch_t type = synao_hash("smoke");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shrapnel {
		constexpr arch_t type = synao_hash("shrapnel");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace dust {
		constexpr arch_t type = synao_hash("dust");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace splash {
		constexpr arch_t type = synao_hash("splash");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_small {
		constexpr arch_t type = synao_hash("sblast");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_medium {
		constexpr arch_t type = synao_hash("mblast");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_large {
		constexpr arch_t type = synao_hash("lblast");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace energy_trail {
		constexpr arch_t type = synao_hash("engtrail");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace dash_flash {
		constexpr arch_t type = synao_hash("dashflash");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace barrier {
		constexpr arch_t type = synao_hash("barrier");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
