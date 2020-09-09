#ifndef LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
#define LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct kontext_t;
struct routine_tuple_t;

namespace ai {
	struct shoshi_helper_t {
	public:
		shoshi_helper_t() : augment(false) {}
		shoshi_helper_t(const shoshi_helper_t&) = default;
		shoshi_helper_t& operator=(const shoshi_helper_t&) = default;
		shoshi_helper_t(shoshi_helper_t&&) = default;
		shoshi_helper_t& operator=(shoshi_helper_t&&) = default;
		~shoshi_helper_t() = default;
	public:
		bool_t augment;
	};
	namespace shoshi_normal {
		constexpr arch_t type = synao_hash("shoshi");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_carry {
		constexpr arch_t type = synao_hash("shoshi_carry");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_follow {
		constexpr arch_t type = synao_hash("shoshi_follow");
		void ctor(entt::entity s, kontext_t& kontext);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
