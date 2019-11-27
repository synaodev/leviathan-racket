#ifndef SYNAO_COMPONENT_ROUTINE_HPP
#define SYNAO_COMPONENT_ROUTINE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entity/registry.hpp>

#include "../utl/hash.hpp"

struct audio_t;
struct camera_t;
struct naomi_state_t;
struct kontext_t;
struct tilemap_t;

struct routine_tuple_t;

using routine_ctor_fn = void(*)(entt::entity, kontext_t&);
using routine_tick_fn = void(*)(entt::entity, routine_tuple_t&);

struct routine_tuple_t : public not_copyable_t, public not_moveable_t {
public:
	routine_tuple_t(audio_t& aud, camera_t& cam, naomi_state_t& nao, kontext_t& ktx, tilemap_t& map) :
		aud(aud),
		cam(cam),
		nao(nao),
		ktx(ktx),
		map(map) {}
	~routine_tuple_t() = default;
public:
	audio_t& aud;
	camera_t& cam;
	naomi_state_t& nao;
	kontext_t& ktx;
	tilemap_t& map;
};

struct routine_generator_t : public not_copyable_t, public not_moveable_t {
public:
	routine_generator_t(void(*callback)(std::unordered_map<arch_t, routine_ctor_fn>&));
	~routine_generator_t() = default;
public:
	static bool init(std::unordered_map<arch_t, routine_ctor_fn>& ctor_table);
};

struct routine_t {
public:
	routine_t(routine_tick_fn tick);
	routine_t();
	routine_t(const routine_t&) = default;
	routine_t& operator=(const routine_t&) = default;
	routine_t(routine_t&&) = default;
	routine_t& operator=(routine_t&&) = default;
	~routine_t() = default;
public:
	static void handle(audio_t& audio, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext, tilemap_t& tilemap);
public:
	arch_t state;
	routine_tick_fn tick;
};

#define SYNAO_CTOR_TABLE_CREATE(GENERATOR_TYPE) 											\
	static void ___routine_ctor_func(std::unordered_map<arch_t, routine_ctor_fn>& table); 	\
	static const GENERATOR_TYPE ___routine_generator(___routine_ctor_func);					\
	static void ___routine_ctor_func(std::unordered_map<arch_t, routine_ctor_fn>& table) 	\

#define SYNAO_CTOR_TABLE_PUSH(ACTOR, CTOR) table[ACTOR] = CTOR

#endif // SYNAO_COMPONENT_ROUTINE_HPP