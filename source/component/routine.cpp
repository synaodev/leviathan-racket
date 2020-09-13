#include "./routine.hpp"
#include "./kontext.hpp"

#include "../utility/logger.hpp"

// Ctor Table
static std::vector<void(*)(std::unordered_map<entt::id_type, routine_ctor_fn>&)>& get_ctor_callback_list() {
	static std::vector<void(*)(std::unordered_map<entt::id_type, routine_ctor_fn>&)> ctor_callback_list;
	return ctor_callback_list;
}

routine_ctor_generator_t::routine_ctor_generator_t(void(*callback)(std::unordered_map<entt::id_type, routine_ctor_fn>&)) {
	auto& ctor_callback_list = get_ctor_callback_list();
	ctor_callback_list.emplace_back(callback);
}

bool routine_ctor_generator_t::init(std::unordered_map<entt::id_type, routine_ctor_fn>& ctor_table) {
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

void routine_t::handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi, kontext_t& kontext, const tilemap_t& tilemap) {
	auto view = kontext.slice<routine_t>();
	if (!view.empty()) {
		routine_tuple_t rtp(
			input,
			audio,
			kernel,
			receiver,
			headsup_gui,
			camera,
			naomi,
			kontext,
			tilemap
		);
		view.each([&rtp](entt::entity actor, routine_t& routine) {
			routine.tick(actor, rtp);
		});
	}
}
