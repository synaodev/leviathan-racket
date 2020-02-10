#include "./animation.hpp"

#include "../utl/thread_pool.hpp"
#include "../utl/setup_file.hpp"
#include "../utl/vfs.hpp"
#include "../sys/renderer.hpp"

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

void animation_t::update(real64_t delta, bool_t& write, arch_t state, real64_t& timer, arch_t& frame) const {
	this->assure();
	sequences[state].update(delta, write, timer, frame);
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& write, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale, real_t angle, glm::vec2 pivot) const {
	this->assure();
	glm::vec2 sequsize = sequences[state].get_dimensions();
	glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring);
	if (viewport.overlaps(position - sequorig, sequsize * scale)) {
		rect_t seququad = sequences[state].get_quad(inverts, frame, variation);
		pipeline_t pipeline = pipeline_t::VtxMajorSprites;
		if (palette != nullptr) {
			pipeline = pipeline_t::VtxMajorIndexed;
			index = palette->convert(index);
		}
		auto& batch = renderer.get_normal_quads(
			layer, 
			blend_mode_t::Alpha,
			pipeline,
			texture,
			palette
		);
		if (write or panic) {
			write = false;
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_major_write(seququad, sequsize, index, alpha, mirroring)
				.vtx_transform_write(position - sequorig, scale, pivot, angle)
			.end();
		} else {
			batch.skip(quad_batch_t::SingleQuad);
		}
	}
}

void animation_t::render(renderer_t& renderer, const rect_t& viewport, bool_t panic, bool_t& write, arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring, layer_t layer, real_t alpha, real_t index, glm::vec2 position, glm::vec2 scale) const {
	this->assure();
	const glm::vec2& sequsize = sequences[state].get_dimensions();
	const glm::vec2& sequorig = sequences[state].get_origin(frame, variation, mirroring);
	if (viewport.overlaps(position - sequorig, sequsize * scale)) {
		rect_t seququad = sequences[state].get_quad(inverts, frame, variation);
		pipeline_t pipeline = pipeline_t::VtxMajorSprites;
		if (palette != nullptr) {
			pipeline = pipeline_t::VtxMajorIndexed;
			index = palette->convert(index);
		}
		auto& batch = renderer.get_normal_quads(
			layer,
			blend_mode_t::Alpha,
			pipeline,
			texture,
			palette
		);
		if (write or panic) {
			write = false;
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_major_write(seququad, sequsize, index, alpha, mirroring)
				.vtx_transform_write(position - sequorig, scale)
			.end();
		} else {
			batch.skip(quad_batch_t::SingleQuad);
		}
	}
}

void animation_t::render(renderer_t& renderer, bool_t& write, arch_t state, arch_t frame, arch_t variation, glm::vec2 position) const {
	this->assure();
	auto& batch = renderer.get_overlay_quads(
		layer_value::HeadsUp, 
		blend_mode_t::Alpha,
		pipeline_t::VtxMajorSprites,
		texture,
		palette
	);
	if (write) {
		write = false;
		glm::vec2 sequsize	= sequences[state].get_dimensions();
		glm::vec2 sequorig	= sequences[state].get_origin(frame, variation, mirroring_t::None);
		rect_t seququads	= sequences[state].get_quad(inverts, frame, variation);
		batch.begin(quad_batch_t::SingleQuad)
			.vtx_major_write(seququads, sequsize, 0.0f, 1.0f, mirroring_t::None)
			.vtx_transform_write(position - sequorig)
		.end();
	} else {
		batch.skip(quad_batch_t::SingleQuad);
	}
}

bool animation_t::load(const std::string& full_path) {
	setup_file_t setup;
	if (setup.load(full_path)) {
		std::vector<std::string> matfile;
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
			bool_t origin 	 = false;
			setup.get(chunk, "starts", starts);
			setup.get(chunk, "vksize", vksize);
			setup.get(chunk, "tdelay", tdelay);
			setup.get(chunk, "hvtype", hvtype);
			setup.get(chunk, "frames", frames);
			setup.get(chunk, "repeat", repeat);
			setup.get(chunk, "origin", origin);

			auto& sequence = sequences.emplace_back(vksize, tdelay, frames, repeat, origin);
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
	}
	return ready;
}

bool animation_t::load(const std::string& full_path, thread_pool_t& thread_pool) {
	if (!ready) {
		future = thread_pool.push([this](const std::string& full_path) -> void {
			this->load(full_path);
		}, full_path);
		return true;
	}
	return false;
}

void animation_t::assure() const {
	if (!ready) {
		future.wait();
	}
}

bool animation_t::visible(const rect_t& viewport, arch_t state, arch_t frame, arch_t variation, layer_t layer, glm::vec2 position, glm::vec2 scale) const {
	if (layer == layer_value::Invisible) {
		return false;
	}
	this->assure();
	glm::vec2 sequsize = sequences[state].get_dimensions();
	glm::vec2 sequorig = sequences[state].get_origin(frame, variation, mirroring_t::None);
	return viewport.overlaps(position - sequorig, sequsize * scale);
}

bool animation_t::is_finished(arch_t state, arch_t frame, real64_t timer) const {
	this->assure();
	return sequences[state].is_finished(frame, timer);
}

glm::vec2 animation_t::get_origin(arch_t state, arch_t frame, arch_t variation, mirroring_t mirroring) const {
	this->assure();
	return sequences[state].get_origin(frame, variation, mirroring);
}

glm::vec2 animation_t::get_action_point(arch_t state, arch_t variation, mirroring_t mirroring) const {
	this->assure();
	return sequences[state].get_action_point(variation, mirroring);
}