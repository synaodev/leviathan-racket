#include "./animation.hpp"
#include "./vfs.hpp"

#include "../system/renderer.hpp"
#include "../utility/thread-pool.hpp"
#include "../utility/setup-file.hpp"
#include "../utility/logger.hpp"

#include <fstream>

#include <glm/common.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>

void animation_sequence_t::append(const glm::vec2& action_point) {
	action_points.push_back(action_point);
}

void animation_sequence_t::append(const glm::vec2& invert, const glm::vec2& start, const glm::vec4& points) {
	const glm::vec2 position = invert * (start + (glm::vec2(points[0], points[1]) * dimensions));
	const glm::vec2 origin = glm::vec2(points[2], points[3]);
	frames.emplace_back(position, origin);
}

const sequence_frame_t& animation_sequence_t::get_frame(arch_t frame, arch_t variation) const {
	static const sequence_frame_t kNullFrame = sequence_frame_t {
		glm::zero<glm::vec2>(),
		glm::one<glm::vec2>()
	};
	arch_t index = frame + (variation * total);
	if (index < frames.size()) {
		return frames[index];
	}
	return kNullFrame;
}

rect_t animation_sequence_t::get_quad(const glm::vec2& invert, arch_t frame, arch_t variation) const {
	arch_t index = frame + (variation * total);
	if (index < frames.size()) {
		return rect_t(frames[index].position, dimensions * invert);
	}
	return rect_t {
		glm::zero<glm::vec2>(),
		glm::one<glm::vec2>()
	};
}

glm::vec2 animation_sequence_t::get_dimensions() const {
	return dimensions;
}

glm::vec2 animation_sequence_t::get_origin(arch_t frame, arch_t variation, mirroring_t mirroring) const {
	arch_t index = frame + (variation * total);
	if (index < frames.size()) {
		glm::vec2 origin = frames[index].origin;
		if (reflect) {
			if (mirroring & mirroring_t::Horizontal) {
				origin.x = -origin.x;
			}
			if (mirroring & mirroring_t::Vertical) {
				origin.y = -origin.y;
			}
		}
		return origin;
	}
	return {};
}

glm::vec2 animation_sequence_t::get_action_point(arch_t variation, mirroring_t mirroring) const {
	if (variation < action_points.size()) {
		glm::vec2 action_point = action_points[variation];
		if (mirroring & mirroring_t::Horizontal) {
			real_t center_x = dimensions.x / 2.0f;
			real_t distance = glm::abs(action_point.x - center_x);
			action_point.x = action_point.x > center_x ?
				glm::round(action_point.x - distance * 2.0f) :
				glm::round(action_point.x + distance * 2.0f);
		}
		if (mirroring & mirroring_t::Vertical) {
			real_t center_y = dimensions.y / 2.0f;
			real_t distance = glm::abs(action_point.y - center_y);
			action_point.y = action_point.y > center_y ?
				glm::round(center_y - distance * 2.0f) :
				glm::round(center_y + distance * 2.0f);
		}
		return action_point;
	}
	return {};
}

void animation_sequence_t::update(real64_t delta, real64_t& timer, arch_t& frame) const {
	if (total > 1) {
		if (repeat) {
			timer += delta;
			if (timer >= delay) {
				timer = glm::mod(timer, delay);
				frame++;
				frame %= total;
			}
		} else {
			if (frame != (total - 1)) {
				timer += delta;
				if (timer >= delay) {
					timer = glm::mod(timer, delay);
					frame++;
				}
			} else if (timer <= delay) {
				timer += delta;
			}
		}
	} else {
		timer = 0.0f;
		frame = 0;
	}
}

void animation_sequence_t::update(real64_t delta, bool_t& amend, real64_t& timer, arch_t& frame) const {
	if (total > 1) {
		if (repeat) {
			timer += delta;
			if (timer >= delay) {
				amend = true;
				timer = glm::mod(timer, delay);
				frame++;
				frame %= total;
			}
		} else {
			if (frame != (total - 1)) {
				timer += delta;
				if (timer >= delay) {
					amend = true;
					timer = glm::mod(timer, delay);
					frame++;
				}
			} else if (timer <= delay) {
				timer += delta;
			}
		}
	} else {
		timer = 0.0f;
		frame = 0;
	}
}

bool animation_sequence_t::is_finished(arch_t frame, real64_t timer) const {
	if (frame == total - 1) {
		return timer > delay;
	}
	return false;
}

animation_t::~animation_t() {
	if (future.valid()) {
		future.wait();
	}
}

void animation_t::update(real64_t delta, arch_t state, real64_t& timer, arch_t& frame) const {
	this->assure();
	if (state < sequences.size()) {
		sequences[state].update(delta, timer, frame);
	}
}

void animation_t::update(real64_t delta, bool_t& amend, arch_t state, real64_t& timer, arch_t& frame) const {
	this->assure();
	if (state < sequences.size()) {
		sequences[state].update(delta, amend, timer, frame);
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const {
	this->assure();
	if (state < sequences.size()) {
		const glm::vec2 dimensions = sequences[state].get_dimensions();
		const glm::vec2 origin = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - origin, dimensions * scale)) {
			const rect_t quad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				palette ? program_t::Indexed : program_t::Sprites
			);
			const sint_t texture_name = texture ? texture->get_name() : 0;
			const sint_t palette_name = palette ? palette->get_name() + table : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(quad, dimensions, mirroring, alpha, texture_name, palette_name)
				.vtx_transform_write(position - origin, scale, pivot, angle)
			.end();
		}
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale) const {
	this->assure();
	if (state < sequences.size()) {
		const glm::vec2 dimensions = sequences[state].get_dimensions();
		const glm::vec2 origin = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - origin, dimensions * scale)) {
			const rect_t quad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				palette ? program_t::Indexed : program_t::Sprites
			);
			sint_t texture_name = texture ? texture->get_name() : 0;
			sint_t palette_name = palette ? palette->get_name() + table : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(quad, dimensions, mirroring, alpha, texture_name, palette_name)
				.vtx_transform_write(position - origin, scale)
			.end();
		}
	}
}

void animation_t::render(renderer_t& renderer, bool_t& amend, arch_t state, arch_t frame, arch_t variation, sint_t table, glm::vec2 position) const {
	this->assure();
	if (state < sequences.size()) {
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			palette ? program_t::Indexed : program_t::Sprites
		);
		if (amend) {
			amend = false;
			glm::vec2 sequsize = sequences[state].get_dimensions();
			glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring_t::None);
			rect_t seququads = sequences[state].get_quad(inverts, frame, variation);
			sint_t texture_name = texture ? texture->get_name() : 0;
			sint_t palette_name = palette ? palette->get_name() + table : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(seququads, sequsize, mirroring_t::None, 1.0f, texture_name, palette_name)
				.vtx_transform_write(position - sequorig)
			.end();
		} else {
			list.skip(display_list_t::SingleQuad);
		}
	}
}

void animation_t::load(const std::string& full_path) {
	if (!sequences.empty()) {
		synao_log("Warning! Tried to overwrite animation!\n");
		return;
	}

	std::ifstream ifs { full_path, std::ios::binary };
	if (!ifs.is_open()) {
		synao_log("Failed to load animation from {}!\n", full_path);
		return;
	}

	nlohmann::json file = nlohmann::json::parse(ifs);
	if (file.contains("Material") and file["Material"].is_string()) {
		texture = vfs_t::texture(file["Material"].get<std::string>());
	}
	if (file.contains("Palette") and file["Palette"].is_string()) {
		palette = vfs_t::palette(file["Palette"].get<std::string>());
	}

	if (file.contains("Dimensions") and file["Dimensions"].is_array()) {
		auto dimensions = file["Dimensions"];
		if (
			dimensions.size() >= 2 and
			dimensions[0].is_number() and
			dimensions[1].is_number()
		) {
			inverts = {
				dimensions[0].get<real_t>(),
				dimensions[1].get<real_t>()
			};
		}
		if (inverts.x == 0.0f or inverts.y == 0.0f) {
			inverts = glm::one<glm::vec2>();
		} else {
			inverts = 1.0f / inverts;
		}
	}

	for (auto anim : file["Animations"]) {
		glm::vec2 starts {};
		if (
			anim.contains("starts") and
			anim["starts"].is_array() and
			anim["starts"].size() >= 2 and
			anim["starts"][0].is_number() and
			anim["starts"][1].is_number()
		) {
			starts = {
				anim["starts"][0].get<real_t>(),
				anim["starts"][1].get<real_t>()
			};
		}

		glm::vec2 vksize {};
		if (
			anim.contains("vksize") and
			anim["vksize"].is_array() and
			anim["vksize"].size() >= 2 and
			anim["vksize"][0].is_number() and
			anim["vksize"][1].is_number()
		) {
			vksize = {
				anim["vksize"][0].get<real_t>(),
				anim["vksize"][1].get<real_t>()
			};
		}

		real64_t tdelay = 0.0;
		if (
			anim.contains("tdelay") and
			anim["tdelay"].is_number_float()
		) {
			tdelay = anim["tdelay"].get<real_t>();
		}

		bool repeat = true;
		if (
			anim.contains("repeat") and
			anim["repeat"].is_boolean()
		) {
			repeat = anim["repeat"].get<bool>();
		}

		bool reflect = false;
		if (
			anim.contains("reflect") and
			anim["reflect"].is_boolean()
		) {
			reflect = anim["reflect"].get<bool>();
		}

		arch_t predict = 0;
		if (
			anim.contains("frames") and
			anim["frames"].is_array() and
			anim["frames"].size() > 0 and
			anim["frames"][0].is_array()
		) {
			predict = anim["frames"][0].size();
		}

		if (predict == 0) {
			synao_log("Failed to load animation frames from {}!\n", full_path);
			return;
		}

		auto& sequence = sequences.emplace_back(
			vksize,
			tdelay,
			predict,
			(bool_t)repeat,
			(bool_t)reflect
		);

		if (
			anim.contains("action") and
			anim["action"].is_array() and
			anim["action"].size() > 0
		) {
			for (auto action : anim["action"]) {
				glm::vec2 point {};
				if (
					action.is_array() and
					action.size() >= 2 and
					action[0].is_number() and
					action[1].is_number()
				) {
					point = {
						action[0].get<real_t>(),
						action[1].get<real_t>()
					};
				}
				sequence.append(point);
			}
		}

		for (auto framerule : anim["frames"]) {
			for (auto frame : framerule) {
				glm::vec4 points {};
				if (frame.is_array()) {
					glm::length_t total = glm::min(
						points.length(),
						(glm::length_t)frame.size()
					);
					for (glm::length_t it = 0; it < total; ++it) {
						if (frame[it].is_number()) {
							points[it] = frame[it].get<real_t>();
						}
					}
				}
				sequence.append(inverts, starts, points);
			}
		}
	}
	ready = true;
}

void animation_t::load(const std::string& full_path, thread_pool_t& thread_pool) {
	assert(!ready);
	this->future = thread_pool.push([this](const std::string& full_path) -> void {
		this->load(full_path);
	}, full_path);
}

void animation_t::assure() const {
	if (!ready and future.valid()) {
		future.wait();
	}
}

bool animation_t::is_finished(arch_t state, arch_t frame, real64_t timer) const {
	this->assure();
	if (state < sequences.size()) {
		return sequences[state].is_finished(frame, timer);
	}
	return false;
}

glm::vec2 animation_t::get_origin(arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring) const {
	this->assure();
	if (state < sequences.size()) {
		return sequences[state].get_origin(frame, variation, mirroring);
	}
	return {};
}

glm::vec2 animation_t::get_action_point(arch_t state, arch_t variation, mirroring_t mirroring) const {
	this->assure();
	if (state < sequences.size()) {
		return sequences[state].get_action_point(variation, mirroring);
	}
	return {};
}
