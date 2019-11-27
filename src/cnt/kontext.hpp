#ifndef SYNAO_COMPONENT_KONTEXT_HPP
#define SYNAO_COMPONENT_KONTEXT_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <entt/entity/registry.hpp>
#include <entt/signal/delegate.hpp>
#include <tmxlite/Layer.hpp>

#include "./common.hpp"
#include "./routine.hpp"

#include "../utl/rect.hpp"

class asIScriptFunction;

struct audio_t;
struct renderer_t;
struct kernel_t;
struct receiver_t;
struct camera_t;
struct naomi_state_t;
struct tilemap_t;

struct sprite_t;

struct kontext_t : public not_copyable_t, public not_moveable_t {
public:
	kontext_t();
	~kontext_t() = default;
public:
	bool init(receiver_t& receiver);
	void reset();
	void handle(audio_t& audio, receiver_t& receiver, camera_t& camera, naomi_state_t& naomi_state, tilemap_t& tilemap);
	void update(real64_t delta);
	void render(renderer_t& renderer, rect_t viewport) const;
	entt::entity search_type(arch_t type) const;
	entt::entity search_id(sint_t identity) const;
	void destroy_id(sint_t identity);
	void kill_id(sint_t identity);
	bool create(const std::string& name, glm::vec2 position, direction_t direction, sint_t identity, arch_t flags);
	bool create(const std::string& name, real_t x, real_t y, direction_t direction, sint_t identity, arch_t flags, asIScriptFunction* function);
	bool create(const actor_spawn_t& spawn);
	void setup_layer(const std::unique_ptr<tmx::Layer>& layer, const kernel_t& kernel, receiver_t& receiver);
	void smoke(glm::vec2 position, arch_t count);
	void smoke(real_t x, real_t y, arch_t count);
	void shrapnel(glm::vec2 position, arch_t count);
	void shrapnel(real_t x, real_t y, arch_t count);
	void bump(sint_t identity, direction_t direction, real_t velocity_x, real_t velocity_y);
	void animate(sint_t identity, arch_t state, direction_t direction);
	void set_state(sint_t identity, arch_t state);
	void set_mask(sint_t identity, arch_t index, bool value);
	void set_event(sint_t identity, asIScriptFunction* function);
	bool still(sint_t identity) const;
	void run(const actor_trigger_t& trigger) const;
	template<typename... Args>
	bool spawn(arch_t type, Args&& ...args);
	bool spawn(const actor_spawn_t& spawn);
	void dispose(entt::entity actor);
	bool valid(entt::entity actor) const;
	arch_t size() const;
	arch_t active() const;
	entt::registry* backend();
	entt::view<actor_header_t> actors();
	template<typename... Component>
	entt::view<Component...> slice();
	template<typename... Component>
	entt::view<Component...> slice() const;
	template<typename... Component>
	bool has(entt::entity actor) const;
	template<typename... Component>
	decltype(auto) get(entt::entity actor);
	template<typename... Component> 
	decltype(auto) get(entt::entity actor) const;
	template<typename Component, typename ...Args>
	void assign(entt::entity actor, Args&& ...args);
	template<typename Component, typename ...Args>
	decltype(auto) assign_if(entt::entity actor, Args&& ...args);
	template<typename Component, typename Compare, typename... Args>
	void sort(Compare compare, Args&& ...args);
private:
	bool_t panic_draw, liquid_flag;
	entt::registry registry;
	std::vector<actor_spawn_t> spawn_commands;
	std::unordered_map<arch_t, routine_ctor_fn> ctor_table;
	entt::delegate<void(sint_t)> run_event;
	entt::delegate<void(sint_t, asIScriptFunction*)> push_event;
};

inline void kontext_t::run(const actor_trigger_t& trigger) const {
	std::invoke(run_event, trigger.identity);
}

template<typename... Args>
inline bool kontext_t::spawn(arch_t type, Args&& ...args) {
	spawns.emplace_back(type, std::forward<Args>(args)...);
	return true;
}

inline bool kontext_t::spawn(const actor_spawn_t& spawn) {
	spawn_commands.push_back(spawn);
	return true;
}

inline void kontext_t::dispose(entt::entity actor) {
	if (!panic_draw) {
		panic_draw = registry.has<sprite_t>(actor);
	}
	registry.destroy(actor);
}

inline bool kontext_t::valid(entt::entity actor) const {
	return registry.valid(actor);
}

inline arch_t kontext_t::size() const {
	return registry.size();
}

inline arch_t kontext_t::active() const {
	return registry.alive();
}

inline entt::registry* kontext_t::backend() {
	return &registry;
}

inline entt::view<actor_header_t> kontext_t::actors() {
	return this->slice<actor_header_t>();
}

template<typename... Component>
inline entt::view<Component...> kontext_t::slice() {
	return registry.view<Component...>();
}

template<typename... Component>
inline entt::view<Component...> kontext_t::slice() const {
	return const_cast<entt::registry&>(registry).view<Component...>();
}

template<typename... Component>
inline bool kontext_t::has(entt::entity actor) const {
	return registry.has<Component...>(actor);
}

template<typename... Component>
inline decltype(auto) kontext_t::get(entt::entity actor) {
	return registry.get<Component...>(actor);
}

template<typename... Component>
inline decltype(auto) kontext_t::get(entt::entity actor) const {
	return registry.get<Component...>(actor);
}

template<typename Component, typename ...Args>
inline void kontext_t::assign(entt::entity actor, Args&& ...args) {
	registry.assign(actor, std::forward<Args>(args)...);
}

template<typename Component, typename ...Args>
inline decltype(auto) kontext_t::assign_if(entt::entity actor, Args&& ...args) {
	return registry.get_or_assign<Component>(actor, std::forward<Args>(args)...);
}

template<typename Component, typename Compare, typename... Args>
inline void kontext_t::sort(Compare compare, Args&& ...args) {
	registry.sort<Component>(compare, entt::std_sort{}, std::forward<Args>(args)...);
}

#endif // SYNAO_COMPONENT_KONTEXT_HPP