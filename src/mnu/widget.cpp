#include "./widget.hpp"

widget_i::widget_i() :
	ready(false),
	active(true),
	bitmask(0)
{

}

widget_i::widget_i(arch_t flags) :
	ready(false),
	active(true),
	bitmask(flags)
{

}

bool widget_i::is_ready() const {
	return ready;
}

bool widget_i::is_active() const {
	return active;
}

bool widget_i::get_flag(arch_t index) const {
	if (index >= bitmask.size()) {
		return false;
	}
	return bitmask[index];
}