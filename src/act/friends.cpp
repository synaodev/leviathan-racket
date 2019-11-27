#include "./friends.hpp"

#include "../cnt/kontext.hpp"
#include "../cnt/location.hpp"
#include "../cnt/kinematics.hpp"
#include "../cnt/sprite.hpp"
#include "../cnt/blinker.hpp"

#include "../res_id.hpp"

SYNAO_CTOR_TABLE_CREATE(routine_generator_t) {
	SYNAO_CTOR_TABLE_PUSH(ai::kyoko::type, 		ai::kyoko::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::nauzika::type, 	ai::nauzika::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::senma::type, 		ai::senma::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::fox::type, 		ai::fox::ctor);
	SYNAO_CTOR_TABLE_PUSH(ai::gyo::type, 		ai::gyo::ctor);
}

void ai::friends::tick(entt::entity s, routine_tuple_t& rtp) {
	auto& kinematics = rtp.ktx.get<kinematics_t>(s);
	kinematics.accel_y(0.1f, 6.0f);
}

void ai::kyoko::ctor(entt::entity s, kontext_t& ktx) {
	auto& location = ktx.get<location_t>(s);
	location.bounding = rect_t(4.0f, -2.0f, 8.0f, 18.0f);

	auto& sprite = ktx.assign_if<sprite_t>(s, res::anim::Kyoko);
	sprite.table = 1.0f;
	sprite.layer = 0.2f;
	sprite.position = location.position;

	ktx.assign_if<kinematics_t>(s);
	ktx.assign_if<blinker_t>(s, 0, 2);
	ktx.assign_if<routine_t>(s, friends::tick);
	ktx.sort<sprite_t>(sprite_t::compare);
}