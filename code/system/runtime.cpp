#include "./runtime.hpp"

#include <tmxlite/Map.hpp>

#include "../component/location.hpp"
#include "../component/health.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/video.hpp"
#include "../system/renderer.hpp"
#include "../utility/debug.hpp"
#include "../utility/logger.hpp"
#include "../utility/tmx_convert.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/vfs.hpp"

static const byte_t kStatProgPath[] = "_prog.cfg";
static const byte_t kStatCpntPath[] = "_check.cfg";

runtime_t::runtime_t() : 
	accum(0.0),
	kernel(),
	receiver(),
	stack_gui(),
	dialogue_gui(),
	inventory_gui(),
	title_view(),
	headsup(),
	camera(),
	naomi_state(),
	kontext(),
	tilemap()
{
	
}

bool runtime_t::init(input_t& input, audio_t& audio, music_t& music, renderer_t& renderer) {
	if (!receiver.init(input, audio, music, kernel, stack_gui, dialogue_gui, title_view, headsup, camera, naomi_state, kontext)) {
		return false;
	}
	if (!dialogue_gui.init(receiver)) {
		return false;
	}
	if (!inventory_gui.init()) {
		return false;
	}
	if (!title_view.init()) {
		return false;
	}
	if (!headsup.init(receiver)) {
		return false;
	}
	if (!kontext.init(receiver, headsup)) {
		return false;
	}
	if (!naomi_state.init(kontext)) {
		return false;
	}
	this->setup_boot(renderer);
	SYNAO_LOG("Runtime subsystems initialized.\n");
	return true;
}

bool runtime_t::handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	while (this->viable()) {
		accum = glm::max(accum - interval::kMin, 0.0);
		if (headsup.is_fade_done()) {
			if (kernel.has(kernel_state_t::Boot)) {
				this->setup_boot(renderer);
			}
			if (kernel.has(kernel_state_t::Load)) {
				this->setup_load(renderer);
			}
			if (kernel.has(kernel_state_t::Field)) {
				if (!this->setup_field(audio, renderer)) {
					return false;
				}
			}
		}
		if (kernel.has(kernel_state_t::Save)) {
			this->setup_save();
		}
		if (kernel.has(kernel_state_t::Quit)) {
			return false;
		}
#ifdef SYNAO_DEBUG_BUILD
		this->setup_debug(input, renderer);
#endif // SYNAO_DEBUG_BUILD
		receiver.handle(input, kernel, stack_gui, dialogue_gui, inventory_gui, headsup);
		stack_gui.handle(config, input, video, audio, music, kernel, title_view, headsup);
		dialogue_gui.handle(input, audio);
		inventory_gui.handle(input, audio, kernel, receiver, stack_gui, dialogue_gui, title_view);
		title_view.handle();
		headsup.handle(kernel);
		if (!kernel.has(kernel_state_t::Freeze)) {
			camera.handle(naomi_state);
			naomi_state.handle(input, audio, kernel, receiver, headsup, kontext, tilemap);
			kontext.handle(audio, receiver, camera, naomi_state, tilemap);
			tilemap.handle(camera);
		}
		input.flush();
		audio.flush();
	}
	return true;
}

void runtime_t::update(real64_t delta) {
	accum += delta;
	kernel.update(delta);
	stack_gui.update(delta);
	dialogue_gui.update(delta);
	inventory_gui.update(delta);
	headsup.update(delta);
	if (!kernel.has(kernel_state_t::Freeze)) {
		camera.update(delta);
		kontext.update(delta);
	}
}

void runtime_t::render(const video_t& video, renderer_t& renderer) const {
	stack_gui.render(renderer, inventory_gui);
	dialogue_gui.render(renderer);
	inventory_gui.render(renderer, kernel);
	title_view.render(renderer);
	headsup.render(renderer, kernel);
	if (!headsup.is_fade_done()) {
		const rect_t viewport = camera.get_viewport();
		kontext.render(renderer, viewport);
		tilemap.render(renderer, viewport);
	}
	renderer.flush(video, camera.get_matrix());
	video.flush();
}

bool runtime_t::viable() const {
	return accum >= interval::kMax;
}

bool runtime_t::setup_field(audio_t& audio, renderer_t& renderer) {
	renderer.clear();
	kernel.lock();
	receiver.reset();
	stack_gui.reset();
	camera.reset();
	kontext.reset();
	tilemap.reset();
	if (!receiver.load(kernel)) {
		kernel.finish_field();
		return false;
	}
	receiver.run_field(kernel);
	const std::string full_path = vfs::resource_path(vfs_resource_path_t::Field) + kernel.get_field() + ".tmx";
	tmx::Map tmxmap;
	if (!tmxmap.load(full_path)) {
		SYNAO_LOG("Map file loading failed! Map Path: %s\n", full_path.c_str());
		kernel.finish_field();
		return false;
	}
	camera.set_view_limits(
		tmx_convert::rect_to_rect(tmxmap.getBounds())
	);
	tilemap.push_properties(tmxmap);
	for (auto&& layer : tmxmap.getLayers()) {
		switch (layer->getType()) {
		case tmx::Layer::Type::Tile:
			tilemap.push_tile_layer(layer);
			break;
		case tmx::Layer::Type::Object:
			kontext.setup_layer(layer, kernel, receiver);
			break;
		case tmx::Layer::Type::Image:
			tilemap.push_parallax_background(layer);
			break;
		default:
			break;
		}
	}
	naomi_state.setup(audio, kernel, camera, kontext);
	kernel.finish_field();
	SYNAO_LOG("Field loading successful.\n");
	return true;
}

void runtime_t::setup_boot(renderer_t& renderer) {
	renderer.clear();
	kernel.reset();
	stack_gui.reset();
	dialogue_gui.reset();
	headsup.reset();
	naomi_state.reset(kontext);
	SYNAO_LOG("Boot successful.\n");
}

// If loading fails, run setup_boot() to make sure the game doesn't get stuck.
void runtime_t::setup_load(renderer_t& renderer) {
	bool failure = false;
	const std::string save_path = vfs::resource_path(vfs_resource_path_t::Save);
	if (vfs::create_directory(save_path)) {
		setup_file_t file;
		const std::string path_type = kernel.has(kernel_state_t::Check) ? kStatCpntPath : kStatProgPath;
		if (file.load(save_path + std::to_string(kernel.get_file_index()) + path_type)) {
			arch_t maximum = 2;
			arch_t current = 2;
			arch_t leviathan = 1;
			glm::vec2 position = glm::zero<glm::vec2>();
			arch_t direction = 0;
			std::string equips;
			std::string field_name;
			file.get("Status", "MaxHp", maximum);
			file.get("Status", "CurHp", current);
			file.get("Status", "CurAp", leviathan);
			file.get("Status", "Field", field_name);
			file.get("Status", "Position", position);
			file.get("Status", "Direction", direction);
			file.get("Status", "Equips", equips);
			kernel.reset(field_name);
			stack_gui.reset();
			dialogue_gui.reset();
			headsup.reset();
			naomi_state.reset(
				kontext, position * 16.0f,
				static_cast<direction_t>(direction),
				static_cast<sint_t>(current),
				static_cast<sint_t>(maximum),
				static_cast<sint_t>(leviathan), 
				static_cast<arch_t>(std::stoi(equips, nullptr, 0))
			);
			kernel.read_data(file);
			if (!kernel.read_stream(save_path)) {
				failure = true;
				SYNAO_LOG("Couldn't load current flags!\n");
			} else {
				SYNAO_LOG("Load successful.\n");
			}
		} else {
			failure = true;
			SYNAO_LOG("Couldn't load current state!\n");
		}
	} else {
		failure = true;
		SYNAO_LOG("Couldn't create save directory!\n");
	}
	kernel.finish_file_operation();
	if (failure) {
		this->setup_boot(renderer);
	}
}

void runtime_t::setup_save() {
	const std::string save_path = vfs::resource_path(vfs_resource_path_t::Save);
	if (vfs::create_directory(save_path)) {
		setup_file_t file;
		const std::string path_type = kernel.has(kernel_state_t::Check) ? kStatCpntPath : kStatProgPath;
		auto& location = kontext.get<location_t>(naomi_state.actor);
		auto& health = kontext.get<health_t>(naomi_state.actor);
		file.set("Status", "MaxHp", health.maximum);
		file.set("Status", "CurHp", health.current);
		file.set("Status", "CurAp", health.leviathan);
		file.set("Status", "Field", kernel.get_field());
		file.set("Status", "Position", location.position / 16.0f);
		file.set("Status", "Direction", static_cast<std::underlying_type<direction_t>::type>(location.direction));
		file.set("Status", "Equips", naomi_state.hexadecimal_equips());
		kernel.write_data(file);
		if (!kernel.write_stream(save_path)) {
			SYNAO_LOG("Couldn't save current flags!\n");
		} else if (!file.save(save_path + std::to_string(kernel.get_file_index()) + path_type)) {
			SYNAO_LOG("Couldn't save current state!\n");
		} else {
			SYNAO_LOG("Save successful.\n");
		}
	} else {
		SYNAO_LOG("Couldn't create save directory!\n");
	}
	kernel.finish_file_operation();
}

#ifdef SYNAO_DEBUG_BUILD
void runtime_t::setup_debug(input_t& input, const renderer_t& renderer) {
	// if (input.debug_pressed[SDL_SCANCODE_BACKSPACE]) {
	// 	if (!input.debug_holding[SDL_SCANCODE_LSHIFT]) {
	// 		sint_t draw_calls = static_cast<sint_t>(renderer.get_draw_calls());
	// 		sint_t actor_count = static_cast<sint_t>(kontext.active());
	// 		std::printf("Draw Calls: %d\n", draw_calls);
	// 		std::printf("Actor Count: %d\n", actor_count);
	// 	} else if (!kernel.has(kernel_state_t::Lock) and stack_gui.empty()) {
	// 		stack_gui.push(menu_t::Field, 0);
	// 	}
	// }
	if (input.debug_pressed[SDL_SCANCODE_KP_0]) {
		debug::Framerate = !debug::Framerate;
	} else if (input.debug_pressed[SDL_SCANCODE_KP_1]) {
		debug::Hitboxes = !debug::Hitboxes;
	}
}
#endif // SYNAO_DEBUG_BUILD
