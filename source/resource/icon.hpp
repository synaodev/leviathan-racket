#ifndef SYNAO_RESOURCE_ICON_HPP
#define SYNAO_RESOURCE_ICON_HPP

#include "../types.hpp"

namespace icon {
	sint_t width();
	sint_t height();
	sint_t depth();
	sint_t pitch();
	optr_t pixels(); 
}

#endif // SYNAO_RESOURCE_ICON_HPP