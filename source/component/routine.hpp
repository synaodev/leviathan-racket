#ifndef LEVIATHAN_INCLUDED_COMPONENT_ROUTINE_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_ROUTINE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entity/registry.hpp>

#include "../utility/hash.hpp"

struct audio_t;
struct camera_t;
struct naomi_state_t;
struct kontext_t;
struct tilemap_t;

struct routine_tuple_t : public not_copyable_t {
public:
	routine_tuple_t(audio_t& audio, camera_t& camera, naomi_state_t& naomi, kontext_t& kontext, tilemap_t& tilemap) :
		audio(audio),
		camera(camera),
		naomi(naomi),
		kontext(kontext),
		tilemap(tilemap) {}
	routine_tuple_t(routine_tuple_t&&) = delete;
	routine_tuple_t& operator=(routine_tuple_t&&) = delete;
	~routine_tuple_t() = default;
public:
	audio_t& audio;
	camera_t& camera;
	naomi_state_t& naomi;
	kontext_t& kontext;
	tilemap_t& tilemap;
};

using routine_ctor_fn = void(*)(entt::entity, kontext_t&);
using routine_tick_fn = void(*)(entt::entity, routine_tuple_t&);

struct routine_ctor_generator_t {
public:
	routine_ctor_generator_t(void(*callback)(std::unordered_map<arch_t, routine_ctor_fn>&));
	routine_ctor_generator_t(const routine_ctor_generator_t&) = delete;
	routine_ctor_generator_t& operator=(const routine_ctor_generator_t&) = delete;
	routine_ctor_generator_t(routine_ctor_generator_t&&) = delete;
	routine_ctor_generator_t& operator=(routine_ctor_generator_t&&) = delete;
	~routine_ctor_generator_t() = default;
public:
	static bool init(std::unordered_map<arch_t, routine_ctor_fn>& ctor_table);
};

struct routine_name_generator_t {
public:
	routine_name_generator_t(void(*callback)(std::unordered_map<arch_t, std::string>&));
	routine_name_generator_t(const routine_name_generator_t&) = delete;
	routine_name_generator_t& operator=(const routine_name_generator_t&) = delete;
	routine_name_generator_t(routine_name_generator_t&&) = delete;
	routine_name_generator_t& operator=(routine_name_generator_t&&) = delete;
	~routine_name_generator_t() = default;
public:
	static std::string search(arch_t hash);
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

#define LEVIATHAN_CTOR_TABLE_CREATE(GENERATOR_TYPE) 										\
	static void ___routine_ctor_func(std::unordered_map<arch_t, routine_ctor_fn>& table); 	\
	static const GENERATOR_TYPE ___routine_ctor_generator(___routine_ctor_func);			\
	static void ___routine_ctor_func(std::unordered_map<arch_t, routine_ctor_fn>& table) 	\

#define LEVIATHAN_NAME_TABLE_CREATE(GENERATOR_TYPE)											\
	static void ___routine_name_func(std::unordered_map<arch_t, std::string>& table);		\
	static const GENERATOR_TYPE ___routine_name_generator(___routine_name_func);			\
	static void ___routine_name_func(std::unordered_map<arch_t, std::string>& table)		\

#define LEVIATHAN_TABLE_PUSH(ACTOR, DATA) table[ACTOR] = DATA

#endif // LEVIATHAN_INCLUDED_COMPONENT_ROUTINE_HPP
