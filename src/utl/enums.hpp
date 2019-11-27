#ifndef SYNAO_UTILITY_ENUMS_HPP
#define SYNAO_UTILITY_ENUMS_HPP

#include "../types.hpp"

namespace __enum_layer {
	using type = real_t;
}

namespace layer_value {
	constexpr __enum_layer::type Parallax	= -2.0f;
	constexpr __enum_layer::type TileBack	= -1.0f;
	constexpr __enum_layer::type Automatic	= 0.0f;
	constexpr __enum_layer::type TileFront	= 1.0f;
	constexpr __enum_layer::type HeadsUp	= 1.5f;
	constexpr __enum_layer::type Invisible	= 2.0f;
	inline bool equal(__enum_layer::type lhv, __enum_layer::type rhv) {
		static constexpr __enum_layer::type kEpsilon = 0.1f;
		return glm::abs(lhv - rhv) < kEpsilon;
	}
}

using layer_t = __enum_layer::type;

namespace __enum_direction {
	enum type : arch_t {
		Right 	= 0 << 0,
		Left 	= 1 << 0,
		Up 		= 1 << 1,
		Down 	= 1 << 2,
		Neutral = 1 << 3,
		Invalid = -1
	};
}

using direction_t = __enum_direction::type;

direction_t operator~(direction_t lhv) {
	return static_cast<direction_t>(
		~static_cast<std::underlying_type<direction_t>::type>(lhv)
	);
}

direction_t operator&(direction_t lhv, direction_t rhv) {
	return static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) &
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
}

direction_t operator|(direction_t lhv, direction_t rhv) {
	return static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) |
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
}

direction_t& operator|=(direction_t& lhv, const direction_t& rhv) {
	lhv = static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) |
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
	return lhv;
}

direction_t& operator&=(direction_t& lhv, const direction_t& rhv) {
	lhv = static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) &
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
	return lhv;
}

namespace __enum_policy {
	enum type : arch_t {
		Run,
		Stop,
		Quit
	};
}

using policy_t = __enum_policy::type;

#endif // SYNAO_UTILITY_ENUMS_HPP