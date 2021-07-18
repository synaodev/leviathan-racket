#include "./animation.hpp"
#include "./vfs.hpp"

#include "../system/renderer.hpp"
#include "../utility/thread-pool.hpp"
#include "../utility/logger.hpp"

#include <fstream>

#include <glm/common.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>

namespace {
	constexpr byte_t kMaterialEntry[] 	= "Material";
	constexpr byte_t kPaletteEntry[] 	= "Palette";
	constexpr byte_t kDimensionsEntry[] = "Dimensions";
	constexpr byte_t kAnimationsEntry[] = "Animations";

	constexpr byte_t kStartsEntry[] 	= "starts";
	constexpr byte_t kVksizeEntry[] 	= "vksize";
	constexpr byte_t kTdelayEntry[] 	= "tdelay";
	constexpr byte_t kRepeatEntry[] 	= "repeat";
	constexpr byte_t kReflectEntry[] 	= "reflect";
	constexpr byte_t kActionEntry[] 	= "action";
	constexpr byte_t kFramesEntry[] 	= "frames";
}

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

void animation_t::render(renderer_t& renderer, const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, const glm::vec2& position, const glm::vec2& scale, real_t angle, const glm::vec2& pivot) const {
	this->assure();
	if (state < sequences.size()) {
		const glm::vec2 dimensions = sequences[state].get_dimensions();
		const glm::vec2 origin = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - origin, dimensions * scale)) {
			const rect_t quad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				program_t::Sprites
			);
			const sint_t texID = texture ? texture->get_name() : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(quad, dimensions, mirroring, alpha, texID)
				.vtx_transform_write(position - origin, scale, pivot, angle)
			.end();
		}
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, const glm::vec2& position, const glm::vec2& scale) const {
	this->assure();
	if (state < sequences.size()) {
		const glm::vec2 dimensions = sequences[state].get_dimensions();
		const glm::vec2 origin = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - origin, dimensions * scale)) {
			const rect_t quad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				program_t::Sprites
			);
			sint_t texID = texture ? texture->get_name() : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(quad, dimensions, mirroring, alpha, texID)
				.vtx_transform_write(position - origin, scale)
			.end();
		}
	}
}

void animation_t::render(renderer_t& renderer, bool_t& amend, arch_t state, arch_t frame, arch_t variation, const glm::vec2& position) const {
	this->assure();
	if (state < sequences.size()) {
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Sprites
		);
		if (amend) {
			amend = false;
			const glm::vec2 dimensions = sequences[state].get_dimensions();
			const glm::vec2 origin = sequences[state].get_origin(frame, variation, mirroring_t::None);
			const rect_t quad = sequences[state].get_quad(inverts, frame, variation);
			sint_t texID = texture ? texture->get_name() : 0;
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(quad, dimensions, mirroring_t::None, 1.0f, texID)
				.vtx_transform_write(position - origin)
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
	if (file.contains(kMaterialEntry) and file[kMaterialEntry].is_string()) {
		texture = vfs_t::texture(file[kMaterialEntry].get<std::string>());
	}

	if (file.contains(kDimensionsEntry) and file[kDimensionsEntry].is_array()) {
		auto dimensions = file[kDimensionsEntry];
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

	for (auto anim : file[kAnimationsEntry]) {
		glm::vec2 starts {};
		if (
			anim.contains(kStartsEntry) and
			anim[kStartsEntry].is_array() and
			anim[kStartsEntry].size() >= 2 and
			anim[kStartsEntry][0].is_number() and
			anim[kStartsEntry][1].is_number()
		) {
			starts = {
				anim[kStartsEntry][0].get<real_t>(),
				anim[kStartsEntry][1].get<real_t>()
			};
		}

		glm::vec2 vksize {};
		if (
			anim.contains(kVksizeEntry) and
			anim[kVksizeEntry].is_array() and
			anim[kVksizeEntry].size() >= 2 and
			anim[kVksizeEntry][0].is_number() and
			anim[kVksizeEntry][1].is_number()
		) {
			vksize = {
				anim[kVksizeEntry][0].get<real_t>(),
				anim[kVksizeEntry][1].get<real_t>()
			};
		}

		real64_t tdelay = 0.0;
		if (
			anim.contains(kTdelayEntry) and
			anim[kTdelayEntry].is_number_float()
		) {
			tdelay = anim[kTdelayEntry].get<real_t>();
		}

		bool repeat = true;
		if (
			anim.contains(kRepeatEntry) and
			anim[kRepeatEntry].is_boolean()
		) {
			repeat = anim[kRepeatEntry].get<bool>();
		}

		bool reflect = false;
		if (
			anim.contains(kReflectEntry) and
			anim[kReflectEntry].is_boolean()
		) {
			reflect = anim[kReflectEntry].get<bool>();
		}

		arch_t predict = 0;
		if (
			anim.contains(kFramesEntry) and
			anim[kFramesEntry].is_array() and
			anim[kFramesEntry].size() > 0 and
			anim[kFramesEntry][0].is_array()
		) {
			predict = anim[kFramesEntry][0].size();
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
			anim.contains(kActionEntry) and
			anim[kActionEntry].is_array() and
			anim[kActionEntry].size() > 0
		) {
			for (auto action : anim[kActionEntry]) {
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

		for (auto framerule : anim[kFramesEntry]) {
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
