#ifndef LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
#define LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP

#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct kontext_t;
struct routine_tuple_t;

namespace ai {
	namespace shoshi_normal {
		constexpr arch_t type = synao_hash("shoshi");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_carry {
		constexpr arch_t type = synao_hash("shoshicarry");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
	}
	namespace shoshi_follow {
		constexpr arch_t type = synao_hash("shoshifollow");
		void ctor(entt::entity s, kontext_t& ktx);
		void tick(entt::entity s, routine_tuple_t& rtp);
		struct helper_t {
		public:
			helper_t() :
				augment(false) {}
			helper_t(const helper_t&) = default;
			helper_t& operator=(const helper_t&) = default;
			helper_t(helper_t&&) = default;
			helper_t& operator=(helper_t&&) = default;
			~helper_t() = default;
		public:
			bool_t augment;
		};
	}
}

#endif // LEVIATHAN_INCLUDED_ACTOR_SHOSHI_HPP
