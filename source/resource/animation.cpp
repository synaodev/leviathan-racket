#include "./animation.hpp"

#include "../system/renderer.hpp"
#include "../utility/thread-pool.hpp"
#include "../utility/setup-file.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

animation_sequence_t::animation_sequence_t() :
	frames(),
	action_points(),
	dimensions(0.0f),
	delay(0.0),
	total(0),
	repeat(true),
	reflect(false)
{

}

animation_sequence_t::animation_sequence_t(glm::vec2 dimensions, real64_t delay, arch_t total, bool_t repeat, bool_t reflect) :
	frames(),
	action_points(),
	dimensions(dimensions),
	delay(delay),
	total(total),
	repeat(repeat),
	reflect(reflect)
{

}

animation_sequence_t::animation_sequence_t(animation_sequence_t&& that) noexcept : animation_sequence_t() {
	if (this != &that) {
		std::swap(frames, that.frames);
		std::swap(action_points, that.action_points);
		std::swap(dimensions, that.dimensions);
		std::swap(delay, that.delay);
		std::swap(total, that.total);
		std::swap(repeat, that.repeat);
		std::swap(reflect, that.reflect);
	}
}

animation_sequence_t& animation_sequence_t::operator=(animation_sequence_t&& that) noexcept {
	if (this != &that) {
		std::swap(frames, that.frames);
		std::swap(action_points, that.action_points);
		std::swap(dimensions, that.dimensions);
		std::swap(delay, that.delay);
		std::swap(total, that.total);
		std::swap(repeat, that.repeat);
		std::swap(reflect, that.reflect);
	}
	return *this;
}

void animation_sequence_t::append(glm::vec2 action_point) {
	action_points.push_back(action_point);
}

void animation_sequence_t::append(glm::vec2 invert, glm::vec2 start, glm::vec4 points) {
	const glm::vec2 position = invert * (start + (glm::vec2(points[0], points[1]) * dimensions));
	const glm::vec2 origin = glm::vec2(points[2], points[3]);
	frames.emplace_back(position, origin);
}

const sequence_frame_t& animation_sequence_t::get_frame(arch_t frame, arch_t variation) const {
	static const sequence_frame_t kFrameZero = sequence_frame_t(
		glm::zero<glm::vec2>(),
		glm::one<glm::vec2>()
	);
	arch_t index = frame + (variation * total);
	if (index < frames.size()) {
		return frames[index];
	}
	return kFrameZero;
}

rect_t animation_sequence_t::get_quad(glm::vec2 invert, arch_t frame, arch_t variation) const {
	arch_t index = frame + (variation * total);
	if (index < frames.size()) {
		return rect_t(frames[index].position, dimensions * invert);
	}
	return rect_t(
		glm::zero<glm::vec2>(),
		glm::one<glm::vec2>()
	);
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
	return glm::zero<glm::vec2>();
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
	return glm::zero<glm::vec2>();
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
			if (frame != total - 1) {
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

animation_t::animation_t() :
	ready(false),
	future(),
	sequences(),
	inverts(1.0f),
	texture(nullptr),
	palette(nullptr)
{

}

animation_t::animation_t(animation_t&& that) noexcept : animation_t() {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(sequences, that.sequences);
		std::swap(inverts, that.inverts);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
	}
}

animation_t& animation_t::operator=(animation_t&& that) noexcept {
	if (this != &that) {
		std::atomic<bool> temp = ready.load();
		ready.store(that.ready.load());
		that.ready.store(temp.load());

		std::swap(future, that.future);
		std::swap(sequences, that.sequences);
		std::swap(inverts, that.inverts);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
	}
	return *this;
}

void animation_t::update(real64_t delta, bool_t& amend, arch_t state, real64_t& timer, arch_t& frame) const {
	this->assure();
	if (state < sequences.size()) {
		sequences[state].update(delta, amend, timer, frame);
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& amend, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const {
	this->assure();
	if (state < sequences.size()) {
		glm::vec2 sequsize = sequences[state].get_dimensions();
		glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - sequorig, sequsize * scale)) {
			rect_t seququad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				palette != nullptr ? program_t::Indexed : program_t::Sprites
			);
			if (amend or panic) {
				amend = false;
				sint_t texture_name = texture != nullptr ? texture->get_name() : 0;
				sint_t palette_name = palette != nullptr ? palette->get_name() + table : 0;
				list.begin(display_list_t::SingleQuad)
					.vtx_major_write(seququad, sequsize, mirroring, alpha, texture_name, palette_name)
					.vtx_transform_write(position - sequorig, scale, pivot, angle)
				.end();
			} else {
				list.skip(display_list_t::SingleQuad);
			}
		}
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& amend, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, sint_t table, glm::vec2 position, glm::vec2 scale) const {
	this->assure();
	if (state < sequences.size()) {
		glm::vec2 sequsize = sequences[state].get_dimensions();
		glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring);
		if (viewport.overlaps(position - sequorig, sequsize * scale)) {
			rect_t seququad = sequences[state].get_quad(inverts, frame, variation);
			auto& list = renderer.display_list(
				layer,
				blend_mode_t::Alpha,
				palette != nullptr ? program_t::Indexed : program_t::Sprites
			);
			if (amend or panic) {
				amend = false;
				sint_t texture_name = texture != nullptr ? texture->get_name() : 0;
				sint_t palette_name = palette != nullptr ? palette->get_name() + table : 0;
				list.begin(display_list_t::SingleQuad)
					.vtx_major_write(seququad, sequsize, mirroring, alpha, texture_name, palette_name)
					.vtx_transform_write(position - sequorig, scale)
				.end();
			} else {
				list.skip(display_list_t::SingleQuad);
			}
		}
	}
}

void animation_t::render(renderer_t& renderer, bool_t& amend, arch_t state, arch_t frame, arch_t variation, sint_t table, glm::vec2 position) const {
	this->assure();
	if (state < sequences.size()) {
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			palette != nullptr ? program_t::Indexed : program_t::Sprites
		);
		if (amend) {
			amend = false;
			glm::vec2 sequsize = sequences[state].get_dimensions();
			glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring_t::None);
			rect_t seququads = sequences[state].get_quad(inverts, frame, variation);
			sint_t texture_name = texture != nullptr ? texture->get_name() : 0;
			sint_t palette_name = palette != nullptr ? palette->get_name() + table : 0;
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
	if (sequences.size() > 0) {
		synao_log("Warning! Tried to overwrite animation!\n");
		return;
	}
	setup_file_t setup;
	if (setup.load(full_path)) {
		std::string matfile;
		std::string palfile;
		setup.get("Main", "Material", matfile);
		setup.get("Main", "Palettes", palfile);
		setup.get("Main", "Inverter", inverts);

		if (inverts.x == 0.0f or inverts.y == 0.0f) {
			inverts = glm::one<glm::vec2>();
		} else {
			inverts = 1.0f / inverts;
		}

		if (!matfile.empty()) {
			texture = vfs::texture(matfile);
		}
		if (!palfile.empty()) {
			palette = vfs::palette(palfile);
		}

		glm::vec4 points = glm::zero<glm::vec4>();
		glm::vec2 axnpnt = glm::zero<glm::vec2>();

		for (arch_t chunk = 1; chunk < setup.size(); ++chunk) {
			glm::vec2 starts = glm::zero<glm::vec2>();
			glm::vec2 vksize = glm::zero<glm::vec2>();
			real64_t tdelay  = 0.0;
			arch_t hvtype 	 = 0;
			arch_t frames 	 = 0;
			bool_t repeat 	 = true;
			bool_t reflect 	 = false;
			setup.get(chunk, "starts", starts);
			setup.get(chunk, "vksize", vksize);
			setup.get(chunk, "tdelay", tdelay);
			setup.get(chunk, "hvtype", hvtype);
			setup.get(chunk, "frames", frames);
			setup.get(chunk, "repeat", repeat);
			setup.get(chunk, "reflect", reflect);

			auto& sequence = sequences.emplace_back(vksize, tdelay, frames, repeat, reflect);
			for (arch_t d = 0; d < hvtype; ++d) {
				axnpnt = glm::zero<glm::vec2>();
				setup.get(chunk, std::to_string(d) + "-X", axnpnt);
				sequence.append(axnpnt);
				for (arch_t f = 0; f < frames; ++f) {
					points = glm::zero<glm::vec4>();
					setup.get(
						chunk,
						std::to_string(d) + '-' + std::to_string(f),
						points
					);
					sequence.append(inverts, starts, points);
				}
			}
		}
		ready = true;
	} else {
		synao_log("Failed to load animation from {}!\n", full_path);
	}
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

bool animation_t::visible(const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, layer_t layer, glm::vec2 position, glm::vec2 scale) const {
	if (layer == layer_value::Invisible) {
		return false;
	}
	this->assure();
	if (state < sequences.size()) {
		glm::vec2 sequsize = sequences[state].get_dimensions();
		glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring_t::None);
		return viewport.overlaps(position - sequorig, sequsize * scale);
	}
	return false;
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
	return glm::zero<glm::vec2>();
}

glm::vec2 animation_t::get_action_point(arch_t state, arch_t variation, mirroring_t mirroring) const {
	this->assure();
	if (state < sequences.size()) {
		return sequences[state].get_action_point(variation, mirroring);
	}
	return glm::zero<glm::vec2>();
}
