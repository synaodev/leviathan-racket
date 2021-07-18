#pragma once

#include "./kernel.hpp"
#include "./receiver.hpp"

#include "../actor/naomi.hpp"
#include "../component/kontext.hpp"
#include "../field/camera.hpp"
#include "../field/tilemap.hpp"
#include "../menu/stack-gui.hpp"
#include "../menu/dialogue-gui.hpp"
#include "../menu/inventory-gui.hpp"
#include "../menu/headsup-gui.hpp"
#include "../menu/meta-state.hpp"

struct config_t;
struct input_t;
struct video_t;
struct audio_t;
struct music_t;
struct renderer_t;

struct runtime_t : public not_copyable_t, public not_moveable_t {
public:
	runtime_t() = default;
	~runtime_t() = default;
public:
	bool init(input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer);
	bool handle(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer);
	void update(real64_t delta);
	void render(const video_t& video, renderer_t& renderer) const;
	bool viable() const;
private:
	bool setup_language(config_t& config, renderer_t& renderer);
	bool setup_field(audio_t& audio, renderer_t& renderer);
	void setup_boot(const video_t& video, renderer_t& renderer);
	void setup_load(const video_t& video, renderer_t& renderer);
	void setup_save();
private:
	real64_t accum { 0.0 };
	kernel_t kernel {};
	receiver_t receiver {};
	stack_gui_t stack_gui {};
	dialogue_gui_t dialogue_gui {};
	inventory_gui_t inventory_gui {};
	headsup_gui_t headsup_gui {};
	camera_t camera {};
	naomi_state_t naomi {};
	kontext_t kontext {};
	tilemap_t tilemap {};
	meta_state_t meta_state {};
};
