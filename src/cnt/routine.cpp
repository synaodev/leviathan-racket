#include "./routine.hpp"
#include "./kontext.hpp"

static std::vector<void(*)(std::unordered_map<arch_t, routine_ctor_fn>&)>& get_callback_list() {
	static std::vector<void(*)(std::unordered_map<arch_t, routine_ctor_fn>&)> callback_list;
	return callback_list;
}

routine_generator_t::routine_generator_t(void(*callback)(std::unordered_map<arch_t, routine_ctor_fn>&)) {
	auto& callback_list = get_callback_list();
	callback_list.emplace_back(callback);
}

bool routine_generator_t::init(std::unordered_map<arch_t, routine_ctor_fn>& ctor_table) {
	auto& callback_list = get_callback_list();
	if (callback_list.empty()) {
		return false;
	}
	for (auto&& callback : callback_list) {
		if (callback != nullptr) {
			std::invoke(callback, ctor_table);
		}
	}
	callback_list.clear();
	callback_list.shrink_to_fit();
	return true;
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
	entt::view<routine_t> view = kontext.slice<routine_t>();
	if (!view.empty()) {
		routine_tuple_t rtp = routine_tuple_t(
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