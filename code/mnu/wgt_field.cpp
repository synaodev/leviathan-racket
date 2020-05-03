#include "./wgt_field.hpp"

#include "../utl/vfs.hpp"
#include "../sym/input.hpp"
#include "../sym/kernel.hpp"
#include "../oly/draw_headsup.hpp"

#include "../res/id.hpp"

static constexpr arch_t kTotalVisible = 9;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);
static const glm::vec2 kBottomPosition = glm::vec2(120.0f, 2.0f);

wgt_field_t::wgt_field_t(arch_t flags) :
	widget_i(flags),
	fields(),
	scrolling(true),
	identity(0),
	cursor(0),
	first(0),
	last(kTotalVisible),
	text_listing(),
	text_identity(),
	arrow()
{

}

void wgt_field_t::init(const input_t&, const video_t&, audio_t&, const music_t&, kernel_t& kernel) {
	ready = true;
	fields = vfs::file_list("./field/");
	kernel.freeze();
	text_listing.set_font(vfs::font(0));
	text_listing.set_position(kDefaultPosition);
	text_identity.set_font(vfs::font(0));
	text_identity.set_position(kBottomPosition);
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text_listing.get_font_size().x, 
		4.0f + kDefaultPosition.y + 
		(text_listing.get_font_size().y * 2.0f)
	);
	this->setup_listing();
	this->setup_identity();
}

void wgt_field_t::handle(setup_file_t&, input_t& input, video_t&, audio_t&, music_t&, kernel_t& kernel, stack_gui_t&, draw_headsup_t& headsup) {
	bool selection = false;
	if (input.pressed[btn_t::Right] or input.pressed[btn_t::Left]) {
		scrolling = !scrolling;
		arrow.set_visible(scrolling);
	}
	if (scrolling) {
		if (input.pressed[btn_t::Up]) {
			if (cursor > 0) {
				--cursor;
				if (cursor < first) {
					--first;
					--last;
					this->setup_listing();
				} else {
					arrow.mut_position(
						0.0f,
						-text_listing.get_font_size().y
					);
				}
			}
		} else if (input.pressed[btn_t::Down]) {
			if (cursor < fields.size() - 1) {
				++cursor;
				if (cursor >= last) {
					++first;
					++last;
					this->setup_listing();
				} else {
					arrow.mut_position(
						0.0f,
						text_listing.get_font_size().y
					);
				}
			}
		} else if (input.pressed[btn_t::Yes]) {
			active = false;
			selection = true;
		}
	} else if (input.pressed[btn_t::Up]) {
		identity -= 100;
		this->setup_identity();
	} else if (input.pressed[btn_t::Down]) {
		identity += 100;
		this->setup_identity();
	}
	if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.pressed.reset();
		if (!selection) {
			kernel.unlock();
		} else {
			headsup.fade_out();
			kernel.buffer_field(fields[cursor], identity);
		}
	}
}

void wgt_field_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_field_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text_listing.render(renderer);
		text_identity.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_field_t::force() const {
	if (ready and active) {
		text_listing.force();
		text_identity.force();
		arrow.force();
	}
}

void wgt_field_t::setup_listing() {
	text_listing.set_string("Field Select:\n\n");
	for (arch_t it = first; it < fields.size() and it != last; ++it) {
		text_listing.append_string("\t " + fields[it] + '\n');
	}
}

void wgt_field_t::setup_identity() {
	text_identity.set_string("Entry ID: " + std::to_string(identity));
}
