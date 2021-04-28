#pragma once

#include <string>
#include <iterator>

#include "../types.hpp"

std::back_insert_iterator<std::u32string> utf8_to_utf32(
	std::string::const_iterator begin,
	std::string::const_iterator end,
	std::back_insert_iterator<std::u32string> output
);
