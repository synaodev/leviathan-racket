#ifndef LEVIATHAN_INCLUDED_COMPONENT_KONTEXT_HPP
#define LEVIATHAN_INCLUDED_COMPONENT_KONTEXT_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <entt/entity/registry.hpp>
#include <tmxlite/Layer.hpp>

#include "./common.hpp"
#include "./routine.hpp"
#include "./sprite.hpp"
#include "../utility/rect.hpp"

class asIScriptFunction;

struct input_t;
struct audio_t;
struct renderer_t;
struct kernel_t;
struct receiver_t;
struct headsup_gui_t;
struct camera_t;
struct naomi_state_t;
struct tilemap_t;

struct kontext_t : public not_copyable_t {
public:
	kontext_t() = default;
	kontext_t(kontext_t&&) noexcept = default;
	kontext_t& operator=(kontext_t&&) noexcept = default;
	~kontext_t() = default;
public:
	bool init(receiver_t& receiver, headsup_gui_t& headsup_gui);
	void reset();
	void handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi_state, const tilemap_t& tilemap);
	void update(real64_t delta);
	void render(renderer_t& renderer, const rect_t& viewport) const;
	entt::entity search_type(const entt::hashed_string& type) const;
	entt::entity search_id(sint_t identity) const;
	void destroy_id(sint_t identity);
	void kill_id(sint_t identity);
	bool create(const actor_spawn_t& spawn);
	bool create(const std::string& name, const glm::vec2& position, direction_t direction, sint_t identity, arch_t flags);
	bool create_minimally(const std::string& name, real_t x, real_t y, sint_t identity);
	void setup_layer(const std::unique_ptr<tmx::Layer>& layer, const kernel_t& kernel, receiver_t& receiver);
	void smoke(const glm::vec2& position, arch_t count);
	void smoke(real_t x, real_t y, arch_t count);
	void shrapnel(const glm::vec2& position, arch_t count);
	void shrapnel(real_t x, real_t y, arch_t count);
	void bump(sint_t identity, real_t velocity_x, real_t velocity_y);
	void animate(sint_t identity, arch_t state, arch_t variation);
	void set_state(sint_t identity, arch_t state);
	void set_mask(sint_t identity, arch_t index, bool value);
	void set_event(sint_t identity, asIScriptFunction* function);
	void set_fight(sint_t identity, asIScriptFunction* function);
	bool still(sint_t identity) const;
	void run(const actor_trigger_t& trigger) const;
	void meter(sint_t current, sint_t maximum) const;
	template<typename... Args>
	bool spawn(const entt::hashed_string& type, Args&& ...args);
	bool spawn(const actor_spawn_t& spawn);
	void dispose(entt::entity actor);
	bool valid(entt::entity actor) const;
	arch_t size() const;
	arch_t active() const;
	entt::registry* backend();
	entt::basic_view<entt::entity, entt::exclude_t<>, actor_header_t> actors();
	template<typename... Component>
	entt::basic_view<entt::entity, entt::exclude_t<>, Component...> slice();
	template<typename... Component>
	entt::basic_view<entt::entity, entt::exclude_t<>, Component...> slice() const;
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
	mutable bool_t panic_draw { false };
	entt::registry registry {};
	std::vector<actor_spawn_t> spawn_commands {};
	std::unordered_map<entt::id_type, routine_ctor_fn> ctor_table {};
	std::function<void(sint_t)> run_event {};
	std::function<void(sint_t, asIScriptFunction*)> push_event {};
	std::function<void(sint_t, sint_t)> push_meter {};
};

inline void kontext_t::run(const actor_trigger_t& trigger) const {
	std::invoke(run_event, trigger.identity);
}

inline void kontext_t::meter(sint_t current, sint_t maximum) const {
	std::invoke(push_meter, current, maximum);
}

template<typename... Args>
inline bool kontext_t::spawn(const entt::hashed_string& type, Args&& ...args) {
	spawn_commands.emplace_back(type, std::forward<Args>(args)...);
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

inline entt::basic_view<entt::entity, entt::exclude_t<>, actor_header_t> kontext_t::actors() {
	return this->slice<actor_header_t>();
}

template<typename... Component>
inline entt::basic_view<entt::entity, entt::exclude_t<>, Component...> kontext_t::slice() {
	return registry.view<Component...>();
}

template<typename... Component>
inline entt::basic_view<entt::entity, entt::exclude_t<>, Component...> kontext_t::slice() const {
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
	return registry.get_or_emplace<Component>(actor, std::forward<Args>(args)...);
}

template<typename Component, typename Compare, typename... Args>
inline void kontext_t::sort(Compare compare, Args&& ...args) {
	registry.sort<Component>(compare, entt::std_sort{}, std::forward<Args>(args)...);
}

#endif // LEVIATHAN_INCLUDED_COMPONENT_KONTEXT_HPP
