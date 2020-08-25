#include "./button.hpp"

#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

static constexpr arch_t kNotReady = (arch_t)-1;

demo_player_t::demo_player_t() :
	record(false),
	index(kNotReady),
	buttons()
{
	
}

demo_player_t::demo_player_t(bool_t record) :
	record(record),
	index(kNotReady),
	buttons()
{

}

bool demo_player_t::load(const std::string& name) {
	record = false;
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
	synao_log("Loaded demo file: %s\n", name.c_str());
	return true;
}

bool demo_player_t::write(const std::string& name) {
	if (record and !buttons.empty()) {
		const std::string path = vfs::resource_path(vfs_resource_path_t::Init) + name + ".dmo";
		if (vfs::create_recording(path, buttons)) {
			buttons.clear();
			record = false;
			return true;
		}		
	}
	return false;
}

void demo_player_t::push(const std::bitset<btn_t::Total>& pressed, const std::bitset<btn_t::Total>& holding) {
	if (record) {
		buttons.push_back(pressed.to_ullong());
		buttons.push_back(holding.to_ullong());
	}
}

bool demo_player_t::recording() const {
	return record;
}

bool demo_player_t::finished() const {
	if (record) {
		return true;
	}
	if (index == kNotReady) {
		return true;
	}
	return (index + 1) >= buttons.size();
}

std::pair<std::bitset<btn_t::Total>, std::bitset<btn_t::Total> > demo_player_t::next() {
	if (!this->finished()) {
		std::bitset<btn_t::Total> pressed(index++);
		std::bitset<btn_t::Total> holding(index++);
		return std::make_pair(pressed, holding);
	}
	return {};
}
