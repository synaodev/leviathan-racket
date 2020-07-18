#include "./debug.hpp"

namespace debug {
#ifdef SYNAO_DEBUG_BUILD
	bool_t Framerate = false;
	bool_t Hitboxes	 = false;
#endif // SYNAO_DEBUG_BUILD
}