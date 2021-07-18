#include "./runtime.hpp"

#include <fstream>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>
#include <tmxlite/Map.hpp>

#include "../component/location.hpp"
#include "../component/health.hpp"
#include "../field/properties.hpp"
#include "../resource/config.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/video.hpp"
#include "../system/renderer.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"

namespace {
	constexpr byte_t kStatProgess[] 	= "progress-";
	constexpr byte_t kStatCheckpoint[] 	= "checkpoint-";
	constexpr byte_t kBarrierEntry[] 	= "Barrier";
	constexpr byte_t kLeviathanEntry[] 	= "Leviathan";
	constexpr byte_t kFieldEntry[] 		= "Field";
	constexpr byte_t kPositionEntry[] 	= "Position";
	constexpr byte_t kDirectionEntry[] 	= "Direction";
	constexpr byte_t kEquipmentEntry[] 	= "Equipment";
}

bool runtime_t::init(input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	if (!kernel.init(receiver)) {
		return false;
	}
	if (!receiver.init(input, audio, music, kernel, stack_gui, dialogue_gui, headsup_gui, camera, naomi, kontext)) {
		return false;
	}
	if (!dialogue_gui.init(receiver)) {
		return false;
	}
	if (!inventory_gui.init()) {
		return false;
	}
	if (!headsup_gui.init(receiver)) {
		return false;
	}
	if (!kontext.init(receiver, headsup_gui)) {
		return false;
	}
	if (!naomi.init(kontext)) {
		return false;
	}
#ifdef LEVIATHAN_USES_META
	if (!meta_state.init(video)) {
		return false;
	}
#endif
	this->setup_boot(video, renderer);
	synao_log("Runtime subsystems initialized.\n");
	return true;
}

bool runtime_t::handle(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	while (this->viable()) {
		accum = glm::max(accum - constants::MinInterval(), 0.0);
		input.advance();
		if (headsup_gui.is_fade_done()) {
			if (kernel.has(kernel_t::Language)) {
				if (!this->setup_language(config, renderer)) {
					return false;
				}
			}
			if (kernel.has(kernel_t::Boot)) {
				this->setup_boot(video, renderer);
			}
			if (kernel.has(kernel_t::Load)) {
				this->setup_load(video, renderer);
			}
			if (kernel.has(kernel_t::Field)) {
				if (!this->setup_field(audio, renderer)) {
					return false;
				}
			}
		}
		if (kernel.has(kernel_t::Save)) {
			this->setup_save();
		}
		if (kernel.has(kernel_t::Quit)) {
			return false;
		}
		receiver.handle(input, kernel, stack_gui, dialogue_gui, inventory_gui, headsup_gui);
		stack_gui.handle(config, input, video, audio, music, kernel, headsup_gui);
		dialogue_gui.handle(input, audio);
		inventory_gui.handle(input, audio, kernel, receiver, stack_gui, dialogue_gui, headsup_gui);
		headsup_gui.handle(kernel, dialogue_gui);
		if (!kernel.has(kernel_t::Freeze)) {
			camera.handle(kontext, naomi);
			naomi.handle(input, audio, kernel, receiver, headsup_gui, kontext, tilemap);
			kontext.handle(input, audio, kernel, receiver, headsup_gui, camera, naomi, tilemap);
			tilemap.handle(camera);
		}
#ifdef LEVIATHAN_USES_META
		meta_state.handle(input);
#endif
		input.flush();
		audio.flush();
	}
	return true;
}

void runtime_t::update(real64_t delta) {
	accum += delta;
	kernel.update(delta);
	receiver.update(delta);
	stack_gui.update(delta);
	dialogue_gui.update(delta);
	inventory_gui.update(delta);
	headsup_gui.update(delta);
	if (!kernel.has(kernel_t::Freeze)) {
		camera.update(delta);
		kontext.update(delta);
	}
#ifdef LEVIATHAN_USES_META
	meta_state.update(delta);
#endif
}

void runtime_t::render(const video_t& video, renderer_t& renderer) const {
	stack_gui.render(renderer, inventory_gui);
	dialogue_gui.render(renderer);
	inventory_gui.render(renderer, kernel);
	headsup_gui.render(renderer, kernel);
	if (!headsup_gui.is_fade_done()) {
		const rect_t viewport = camera.get_viewport();
		kontext.render(renderer, viewport);
		tilemap.render(renderer, viewport);
	}
	renderer.flush(video, camera.get_matrix());
#ifdef LEVIATHAN_USES_META
	meta_state.flush();
#endif
	video.flush();
}

bool runtime_t::viable() const {
	return accum >= constants::MaxInterval();
}

bool runtime_t::setup_language(config_t& config, renderer_t& renderer) {
	renderer.clear();
	const std::string& language = kernel.get_language();
	if (vfs_t::try_language(language)) {
		if (!dialogue_gui.refresh() or !headsup_gui.refresh()) {
			synao_log("Error! Font loading errors have made switching language to \"{}\" unsuccessful!\n");
			return false;
		}
	}
	config.set_language(language);
	kernel.finish_language();
	return true;
}

bool runtime_t::setup_field(audio_t& audio, renderer_t& renderer) {
	renderer.clear();
	kernel.lock();
	receiver.reset();
	stack_gui.reset();
	headsup_gui.invalidate();
	camera.reset();
	kontext.reset();
	tilemap.reset();
	if (!receiver.load(kernel)) {
		kernel.finish_field();
		return false;
	}
	receiver.run_function(kernel);
	const std::string full_path = vfs_t::resource_path(vfs_resource_path_t::Field) + kernel.get_field() + ".tmx";
	tmx::Map tmxmap;
	if (!tmxmap.load(full_path)) {
		synao_log("Map file loading failed! Map Path: {}\n", full_path);
		kernel.finish_field();
		return false;
	}
	camera.set_view_limits(
		ftcv::rect_to_rect(tmxmap.getBounds())
	);
	tilemap.push_properties(tmxmap);
	for (auto&& layer : tmxmap.getLayers()) {
		switch (layer->getType()) {
		case tmx::Layer::Type::Tile:
			tilemap.push_layer(layer);
			break;
		case tmx::Layer::Type::Object:
			kontext.setup_layer(layer, kernel, receiver);
			break;
		case tmx::Layer::Type::Image:
			tilemap.push_parallax(layer);
			break;
		default:
			break;
		}
	}
	naomi.setup(audio, kernel, camera, kontext);
	kernel.finish_field();
	synao_log("Field loading successful.\n");
	return true;
}

void runtime_t::setup_boot(const video_t&, renderer_t& renderer) {
	renderer.clear();
	kernel.reset();
	stack_gui.reset();
	dialogue_gui.reset();
	headsup_gui.reset();
	naomi.reset(kontext);
	synao_log("Boot successful.\n");
	receiver.run_function(kernel);
}

// If loading fails, run setup_boot() to make sure the game doesn't get stuck.
void runtime_t::setup_load(const video_t& video, renderer_t& renderer) {
	bool failure = false;
	const std::string directory = vfs_t::resource_path(vfs_resource_path_t::Save);
	if (vfs_t::create_directory(directory)) {
		const std::string path_type = kernel.has(kernel_t::Check) ?
			kStatCheckpoint :
			kStatProgess;
		std::ifstream ifs {
			directory + path_type + std::to_string(kernel.get_file_index()) + ".json",
			std::ios::binary
		};
		if (ifs.is_open()) {
			auto file = nlohmann::json::parse(ifs);

			sint_t maximum = 2;
			sint_t current = 2;
			if (
				file.contains(kBarrierEntry) and
				file[kBarrierEntry].is_array() and
				file[kBarrierEntry].size() == 2 and
				file[kBarrierEntry][0].is_number_unsigned() and
				file[kBarrierEntry][1].is_number_unsigned()
			) {
				maximum = file[kBarrierEntry][0].get<sint_t>();
				current = file[kBarrierEntry][1].get<sint_t>();
			}

			sint_t leviathan = 1;
			if (
				file.contains(kLeviathanEntry) and
				file[kLeviathanEntry].is_number_unsigned()
			) {
				leviathan = file[kLeviathanEntry].get<sint_t>();
			}

			glm::vec2 position {};
			if (
				file.contains(kPositionEntry) and
				file[kPositionEntry].is_array() and
				file[kPositionEntry].size() == 2 and
				file[kPositionEntry][0].is_number() and
				file[kPositionEntry][1].is_number()
			) {
				position = {
					file[kPositionEntry][0].get<real_t>(),
					file[kPositionEntry][1].get<real_t>()
				};
				position *= constants::TileSize<real_t>();
			}

			direction_t direction = direction_t::Right;
			if (
				file.contains(kDirectionEntry) and
				file[kDirectionEntry].is_number_unsigned()
			) {
				arch_t literal = file[kDirectionEntry].get<arch_t>();
				if (literal < (direction_t::Left | direction_t::Down)) {
					direction = static_cast<direction_t>(literal);
				}
			}

			arch_t equipment {};
			if (
				file.contains(kEquipmentEntry) and
				file[kEquipmentEntry].is_string()
			) {
				const std::string hex = file[kEquipmentEntry].get<std::string>();
				equipment = static_cast<arch_t>(std::strtoul(hex.c_str(), nullptr, 0));
			}

			std::string field {};
			if (
				file.contains(kFieldEntry) and
				file[kFieldEntry].is_string()
			) {
				field = file[kFieldEntry].get<std::string>();
			}

			kernel.reset(field);
			stack_gui.reset();
			dialogue_gui.reset();
			headsup_gui.reset();
			naomi.reset(
				kontext,
				position,
				direction,
				current,
				maximum,
				leviathan,
				equipment
			);
			kernel.read(file);
			synao_log("Load successful.\n");
		} else {
			failure = true;
			synao_log("Couldn't load current state!\n");
		}
	} else {
		failure = true;
		synao_log("Couldn't create save directory!\n");
 	}
	kernel.finish_file_operation();
	if (failure) {
		this->setup_boot(video, renderer);
	}
}

void runtime_t::setup_save() {
	const std::string directory = vfs_t::resource_path(vfs_resource_path_t::Save);
	if (vfs_t::create_directory(directory)) {
		const std::string path_type = kernel.has(kernel_t::Check) ?
			kStatCheckpoint :
			kStatProgess;
		std::ofstream ofs {
			directory + path_type + std::to_string(kernel.get_file_index()) + ".json",
			std::ios::binary
		};
		if (ofs.is_open()) {
			auto& health = kontext.get<health_t>(naomi.get_actor());
			auto& location = kontext.get<location_t>(naomi.get_actor());

			auto file = nlohmann::json::object();
			file[kBarrierEntry] = nlohmann::json::array({
				health.maximum,
				health.current
			});
			file[kLeviathanEntry] = health.leviathan;
			file[kFieldEntry] = kernel.get_field();
			file[kPositionEntry] = nlohmann::json::array({
				location.position.x / constants::TileSize<real_t>(),
				location.position.y / constants::TileSize<real_t>()
			});
			file[kDirectionEntry] = static_cast<std::underlying_type<direction_t>::type>(location.direction);
			file[kEquipmentEntry] = naomi.hexadecimal_equips();
			kernel.write(file);

			const std::string output = file.dump(
				1, '\t', true,
				nlohmann::detail::error_handler_t::ignore
			);
			ofs.write(output.data(), output.size());

			synao_log("Save successful.\n");
		} else {
			synao_log("Couldn't save current state!\n");
		}
	} else {
		synao_log("Couldn't create save directory!\n");
	}
	kernel.finish_file_operation();
}
