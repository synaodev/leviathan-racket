#ifndef SYNAO_FIELD_TILE_FLAG_HPP
#define SYNAO_FIELD_TILE_FLAG_HPP

#include "../types.hpp"

namespace __enum_tile_flag {
	enum type : sint_t {
		Empty		= (0 << 0),  // 0x0000
		Block		= (1 << 0),  // 0x0001
		Slope		= (1 << 1),  // 0x0002
		Positive	= (1 << 2),  // 0x0004
		Negative	= (1 << 3),  // 0x0008
		Floor		= (1 << 4),  // 0x0010
		Ceiling		= (1 << 5),  // 0x0020
		Short		= (1 << 6),  // 0x0040
		Tall		= (1 << 7),  // 0x0080
		Harmful		= (1 << 8),  // 0x0100
		OutBounds	= (1 << 9),  // 0x0200
		FallThrough = (1 << 10), // 0x0400
		Hooked		= (1 << 11), // 0x0800
		Slope_1 	= (Slope | Positive | Floor | Tall),
		Slope_2 	= (Slope | Positive | Floor | Short),
		Slope_3 	= (Slope | Negative | Floor | Short),
		Slope_4 	= (Slope | Negative | Floor | Tall),
		Slope_5 	= (Slope | Negative | Ceiling | Tall),
		Slope_6 	= (Slope | Negative | Ceiling | Short),
		Slope_7 	= (Slope | Positive | Ceiling | Short),
		Slope_8 	= (Slope | Positive | Ceiling | Tall)
	};
}

using tile_flag_t = __enum_tile_flag::type;

#endif // SYNAO_FIELD_TILE_FLAG_HPP