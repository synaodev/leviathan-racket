#include "./liquid.hpp"
#include "./kontext.hpp"
#include "./location.hpp"

#include "../system/audio.hpp"
#include "../system/renderer.hpp"

void liquid::handle(audio_t& audio, kontext_t& kontext, const location_t& location, liquid_listener_t& listener) {
	auto checker = [&location, &listener](entt::entity liquid, const liquid_body_t& body) {
		if (listener.liquid == entt::null and location.overlap(body.hitbox)) {
			listener.liquid = liquid;
		}
	};
	auto spawner = [&audio, &kontext](const location_t& location, const liquid_listener_t& listener, rect_t bounds) {
		if (listener.sound.value() != 0) {
			audio.play(listener.sound, 11);
		}
		if (listener.particle.value() != 0) {
			kontext.spawn(
				listener.particle,
				glm::vec2(location.center().x, bounds.y)
			);
		}

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
	const auto view = kontext.slice<liquid_body_t>();
	if (!view.empty()) {
		auto& list = renderer.display_list(
			layer_value::Foreground,
			blend_mode_t::Add,
			program_t::Colors
		);
		view.each([&list, &viewport](entt::entity, const liquid_body_t& instance) {
			if (!instance.hitbox.overlaps(viewport)) {
				instance.amend = true;
			} else if (instance.amend) {
				instance.amend = false;
				list.begin(display_list_t::SingleQuad)
					.vtx_blank_write(instance.hitbox, glm::vec4(0.0f, 0.25f, 0.5f, 0.5f))
					.vtx_transform_write(instance.hitbox.left_top())
				.end();
			} else {
				list.skip(display_list_t::SingleQuad);
			}
		});
	}
}
