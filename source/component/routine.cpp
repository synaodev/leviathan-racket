#include "./routine.hpp"
#include "./kontext.hpp"

#include "../utility/logger.hpp"

// Ctor Table
static std::vector<void(*)(std::unordered_map<arch_t, routine_ctor_fn>&)>& get_ctor_callback_list() {
	static std::vector<void(*)(std::unordered_map<arch_t, routine_ctor_fn>&)> ctor_callback_list;
	return ctor_callback_list;
}

routine_ctor_generator_t::routine_ctor_generator_t(void(*callback)(std::unordered_map<arch_t, routine_ctor_fn>&)) {
	auto& ctor_callback_list = get_ctor_callback_list();
	ctor_callback_list.emplace_back(callback);
}

bool routine_ctor_generator_t::init(std::unordered_map<arch_t, routine_ctor_fn>& ctor_table) {
	bool result = true;
	auto& callback_list = get_ctor_callback_list();
	for (auto&& callback : callback_list) {
		if (callback != nullptr) {
			std::invoke(callback, ctor_table);
		} else {
			synao_log("Constructor table should not have null entries!\n");
			result = false;
			break;
		}
	}
	callback_list.clear();
	callback_list.shrink_to_fit();
	return result;
}

// Name Table
static std::vector<void(*)(std::unordered_map<arch_t, std::string>&)>& get_name_callback_list() {
	static std::vector<void(*)(std::unordered_map<arch_t, std::string>&)> name_callback_list;
	return name_callback_list;
}

routine_name_generator_t::routine_name_generator_t(void(*callback)(std::unordered_map<arch_t, std::string>&)) {
	auto& name_callback_list = get_name_callback_list();
	name_callback_list.emplace_back(callback);
}

std::string routine_name_generator_t::search(arch_t hash) {
	static bool first_time = true;
	static std::unordered_map<arch_t, std::string> name_table;
	if (first_time) {
		first_time = false;
		auto& callback_list = get_name_callback_list();
		for (auto&& callback : callback_list) {
			if (callback != nullptr) {
				std::invoke(callback, name_table);
			} else {
				synao_log("Name table should not have null entries!\n");
				break;
			}
		}
		callback_list.clear();
		callback_list.shrink_to_fit();
	}
	auto it = name_table.find(hash);
	if (it == name_table.end()) {
		synao_log("Couldn't retrieve name with hash value: {}!\n", hash);
		return std::string();
	}
	return it->second;
}

routine_t::routine_t(routine_tick_fn tick) :
	state(0),
	tick(tick)
{

}

routine_t::routine_t() :
	state(0),
	tick(nullptr)
{

}

void routine_t::handle(audio_t& audio, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext, tilemap_t& tilemap) {
	auto view = kontext.slice<routine_t>();
	if (!view.empty()) {
		routine_tuple_t rtp(
			audio,
			camera,
			naomi_state,
			kontext,
			tilemap
		);
		view.each([&rtp](entt::entity actor, routine_t& routine) {
			routine.tick(actor, rtp);
		});
	}
}
