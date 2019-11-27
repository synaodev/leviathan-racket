#include "./liquid.hpp"
#include "./kontext.hpp"
#include "./location.hpp"

#include "../sys/audio.hpp"
#include "../sys/renderer.hpp"

void liquid::handle(audio_t& audio, kontext_t& kontext, const location_t& location, liquid_listener_t& listener) {
	auto checker = [&location, &listener](entt::entity liquid, const liquid_body_t& body) {
		if (listener.liquid == entt::null and location.overlap(body.hitbox)) {
			listener.liquid = liquid;
		}
	};
	auto spawner = [&audio, &kontext](const location_t& location, const liquid_listener_t& listener, rect_t bounds) {
		if (listener.sound_name != nullptr) {
			audio.play(listener.sound_name, 11);
		}
		kontext.spawn(
			listener.particle_type,
			glm::vec2(location.center().x, bounds.y)
		);
	};
	if (listener.liquid == entt::null or !kontext.valid(listener.liquid)) {
		listener.liquid = entt::null;
		kontext.slice<liquid_body_t>().each(checker);
		if (listener.liquid != entt::null) {
			std::invoke(
				spawner,
				location,
				listener,
				kontext.get<liquid_body_t>(listener.liquid).hitbox
			);
		}
	} else if (!location.overlap(kontext.get<liquid_body_t>(listener.liquid).hitbox)) {
		rect_t copy_box = kontext.get<liquid_body_t>(listener.liquid).hitbox;
		listener.liquid = entt::null;
		kontext.slice<liquid_body_t>().each(checker);
		if (listener.liquid == entt::null) {
			std::invoke(spawner, location, listener, copy_box);
		}
	}
}

void liquid::handle(audio_t& audio, kontext_t& kontext) {
	kontext.slice<location_t, liquid_listener_t>().each([&audio, &kontext](entt::entity, const location_t& location, liquid_listener_t& listener) {
		liquid::handle(audio, kontext, location, listener);
	});
}

void liquid::render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport) {
	const entt::view<liquid_body_t> view = kontext.slice<liquid_body_t>();
	if (!view.empty()) {
		auto& batch = renderer.get_normal_quads(
			layer_value::TileFront,
			blend_mode_t::Add,
			render_pass_t::VtxBlankColors
		);
		view.each([&batch, &viewport](entt::entity, const liquid_body_t& instance) {
			if (!instance.hitbox.overlaps(viewport)) {
				instance.write = true;
			} else if (instance.write) {
				instance.write = false;
				batch.begin(quad_batch_t::SingleQuad)
					.vtx_blank_write(instance.hitbox, glm::vec4(0.0f, 0.25f, 0.5f, 0.5f))
					.vtx_transform_write(instance.hitbox.left_top())
				.end();
			} else {
				batch.skip(quad_batch_t::SingleQuad);
			}
		});
	}
}