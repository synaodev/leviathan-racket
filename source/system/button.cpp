#include "./button.hpp"

#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

static constexpr arch_t kNotReady = (arch_t)-1;

demo_buffer_t::demo_buffer_t() :
	index(kNotReady),
	buttons()
{
	
}

bool demo_buffer_t::load(const std::string& name) {
	index = kNotReady;
	buttons.clear();
	buttons = vfs::uint64_buffer(vfs::resource_path(vfs_resource_path_t::Init) + name + ".dmo");
	if (buttons.empty()) {
		synao_log("Error! Failed to load demo file!\n");
		buttons.clear();
		return false;
	}
	if ((buttons.size() % 2) != 0) {
		synao_log("Error! Demo file is improperly aligned!\n");
		buttons.clear();
		return false;
	}
	index = 0;
	return true;
}

bool demo_buffer_t::valid() const {
	return index != kNotReady;
}

bool demo_buffer_t::done() const {
	return ( 
		this->valid() and
		(index + 1) < buttons.size()
	);
}

std::pair<std::bitset<btn_t::Total>, std::bitset<btn_t::Total> > demo_buffer_t::next() {
	if (!this->done()) {
		return std::make_pair(
			std::bitset<btn_t::Total>(buttons[index++]),
			std::bitset<btn_t::Total>(buttons[index++])
		);
	}
	return {};
}
