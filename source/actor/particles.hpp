#ifndef LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
#define LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP

#include "../component/routine.hpp"

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
		constexpr arch_t type = synao_hash("blast_small");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_medium {
		constexpr arch_t type = synao_hash("blast_medium");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_large {
		constexpr arch_t type = synao_hash("blast_large");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace energy_trail {
		constexpr arch_t type = synao_hash("energy_trail");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace dash_flash {
		constexpr arch_t type = synao_hash("dash_flash");
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace barrier {
		constexpr arch_t type = synao_hash("barrier");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
