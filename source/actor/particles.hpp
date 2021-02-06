#ifndef LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
#define LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP

#include "../component/routine.hpp"

namespace ai {
	namespace particles {
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace smoke {
		constexpr entt::hashed_string type = "smoke";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shrapnel {
		constexpr entt::hashed_string type = "shrapnel";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace dust {
		constexpr entt::hashed_string type = "dust";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace splash {
		constexpr entt::hashed_string type = "splash";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_small {
		constexpr entt::hashed_string type = "blast_small";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_medium {
		constexpr entt::hashed_string type = "blast_medium";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace blast_large {
		constexpr entt::hashed_string type = "blast_large";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace energy_trail {
		constexpr entt::hashed_string type = "energy_trail";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace dash_flash {
		constexpr entt::hashed_string type = "dash_flash";
		void ctor(entt::entity s, kontext_t& kontext);
	}
	namespace barrier {
		constexpr entt::hashed_string type = "barrier";
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_PARTICLES_HPP
