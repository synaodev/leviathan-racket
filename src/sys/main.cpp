#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"

#include "../utl/setup_file.hpp"
#include "../utl/vfs.hpp"

#include <cstdlib>
#include <SDL2/SDL.h>

static constexpr uint_t kStopDelay = 40;
static constexpr uint_t kGL42Delay = 10;
static constexpr uint_t kGL33Delay = 16;

static bool run(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	runtime_t runtime;
	if (!runtime.init(config, input, audio, music, renderer)) {
		return false;
	}
	watch_t sync_watch, head_watch;
	while (policy != policy_t::Quit) {
		input.poll(policy);
		if (policy != policy_t::Stop) {
			runtime.update(
				head_watch.restart(),
				config, policy,
				input, audio, renderer
			);
			if (runtime.viable()) {
				runtime.handle(config, policy, input, video, audio, music);
				runtime.render(video, renderer);
				const screen_params_t params = video.get_parameters();
				if (params.vsync != 0) {
					SDL_Delay(program_t::has_pipelines() ?
						kGL42Delay : kGL33Delay
					);
				} else {
					real64_t waiting = (1.0 / params.framerate) - sync_watch.elapsed();
					if (waiting > 0.0) {
						uint_t ticks = static_cast<uint_t>(waiting * 1000.0);
						SDL_Delay(ticks);
					}
					sync_watch.restart();
				}
			}
		} else {
			SDL_Delay(kStopDelay);
		}
	}
	return config.save();
}

static const byte_t kBootPath[]	= "data/boot.cfg";

int main(int, char**) {
	if (std::atexit(SDL_Quit) != 0) {
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		return EXIT_FAILURE;
	}
	setup_file_t config;
	if (!config.load(kBootPath)) {
		return EXIT_FAILURE;
	}
	input_t input;
	if (!input.init(config)) {
		return EXIT_FAILURE;
	}
	video_t video;
	if (!video.init(config)) {
		return EXIT_FAILURE;
	}
	audio_t audio;
	if (!audio.init(config)) {
		return EXIT_FAILURE;
	}
	auto fs = vfs::mount(config);
	if (fs == nullptr) {
		return EXIT_FAILURE;
	}
	music_t music;
	if (!music.init(config)) {
		return EXIT_FAILURE;
	}
	renderer_t renderer;
	if (!renderer.init(config)) {
		return EXIT_FAILURE;
	}
	if (!run(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}