#ifndef LEVIATHAN_INCLUDED_UTILITY_ENUMS_HPP
#define LEVIATHAN_INCLUDED_UTILITY_ENUMS_HPP

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
		Invalid = (arch_t)-1
	};
}

using direction_t = __enum_direction::type;

inline direction_t operator~(direction_t lhv) {
	return static_cast<direction_t>(
		~static_cast<std::underlying_type<direction_t>::type>(lhv)
	);
}

inline direction_t operator&(direction_t lhv, direction_t rhv) {
	return static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) &
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
}

inline direction_t operator|(direction_t lhv, direction_t rhv) {
	return static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) |
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
}

inline direction_t& operator|=(direction_t& lhv, const direction_t& rhv) {
	lhv = static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) |
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
	return lhv;
}

inline direction_t& operator&=(direction_t& lhv, const direction_t& rhv) {
	lhv = static_cast<direction_t>(
		static_cast<std::underlying_type<direction_t>::type>(lhv) &
		static_cast<std::underlying_type<direction_t>::type>(rhv)
	);
	return lhv;
}

namespace __enum_orientated {
	enum type : arch_t {
		None = 0 << 0,
		Top	 = 1 << 0,
		Down = 1 << 1
	};
}

using oriented_t = __enum_orientated::type;

inline oriented_t operator~(oriented_t lhv) {
	return static_cast<oriented_t>(
		~static_cast<std::underlying_type<oriented_t>::type>(lhv)
	);
}

inline oriented_t operator&(oriented_t lhv, oriented_t rhv) {
	return static_cast<oriented_t>(
		static_cast<std::underlying_type<oriented_t>::type>(lhv) &
		static_cast<std::underlying_type<oriented_t>::type>(rhv)
	);
}

inline oriented_t operator|(oriented_t lhv, oriented_t rhv) {
	return static_cast<oriented_t>(
		static_cast<std::underlying_type<oriented_t>::type>(lhv) |
		static_cast<std::underlying_type<oriented_t>::type>(rhv)
	);
}

inline oriented_t& operator|=(oriented_t& lhv, const oriented_t& rhv) {
	lhv = static_cast<oriented_t>(
		static_cast<std::underlying_type<oriented_t>::type>(lhv) |
		static_cast<std::underlying_type<oriented_t>::type>(rhv)
	);
	return lhv;
}

inline oriented_t& operator&=(oriented_t& lhv, const oriented_t& rhv) {
	lhv = static_cast<oriented_t>(
		static_cast<std::underlying_type<oriented_t>::type>(lhv) &
		static_cast<std::underlying_type<oriented_t>::type>(rhv)
	);
	return lhv;
}

namespace __enum_mirroring {
	enum type : arch_t {
		None 		= 0 << 0,
		Horizontal 	= 1 << 0,
		Vertical 	= 1 << 1,
		Both		= (Horizontal | Vertical)
	};
}

using mirroring_t = __enum_mirroring::type;

inline mirroring_t operator~(mirroring_t lhv) {
	return static_cast<mirroring_t>(
		~static_cast<std::underlying_type<mirroring_t>::type>(lhv)
	);
}

inline mirroring_t operator&(mirroring_t lhv, mirroring_t rhv) {
	return static_cast<mirroring_t>(
		static_cast<std::underlying_type<mirroring_t>::type>(lhv) &
		static_cast<std::underlying_type<mirroring_t>::type>(rhv)
	);
}

inline mirroring_t operator|(mirroring_t lhv, mirroring_t rhv) {
	return static_cast<mirroring_t>(
		static_cast<std::underlying_type<mirroring_t>::type>(lhv) |
		static_cast<std::underlying_type<mirroring_t>::type>(rhv)
	);
}

inline mirroring_t& operator|=(mirroring_t& lhv, const mirroring_t& rhv) {
	lhv = static_cast<mirroring_t>(
		static_cast<std::underlying_type<mirroring_t>::type>(lhv) |
		static_cast<std::underlying_type<mirroring_t>::type>(rhv)
	);
	return lhv;
}

inline mirroring_t& operator&=(mirroring_t& lhv, const mirroring_t& rhv) {
	lhv = static_cast<mirroring_t>(
		static_cast<std::underlying_type<mirroring_t>::type>(lhv) &
		static_cast<std::underlying_type<mirroring_t>::type>(rhv)
	);
	return lhv;
}

#endif // LEVIATHAN_INCLUDED_UTILITY_ENUMS_HPP
