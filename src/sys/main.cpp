#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"

#include "../utl/setup_file.hpp"
#include "../utl/vfs.hpp"
#include "../utl/logger.hpp"

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
		policy = input.poll(policy);
		if (policy != policy_t::Stop) {
			runtime.update(head_watch.restart());
			if (runtime.viable()) {
				if (runtime.handle(config, input, video, audio, music, renderer)) {
					runtime.render(video, renderer);
					const screen_params_t params = video.get_parameters();
					if (params.vsync != 0) {
						SDL_Delay(program_t::has_pipelines() ? kGL42Delay : kGL33Delay);
					} else {
						real64_t waiting = (1.0 / params.framerate) - sync_watch.elapsed();
						if (waiting > 0.0) {
							uint_t ticks = static_cast<uint_t>(waiting * 1000.0);
							SDL_Delay(ticks);
						}
						sync_watch.restart();
					}
				} else {
					policy = policy_t::Quit;
				}
			}
		} else {
			SDL_Delay(kStopDelay);
		}
	}
	return config.save();
}

static const byte_t kBootPath[]	= "./data/boot.cfg";

int main(int, char**) {
	// Basics
	if (std::atexit(SDL_Quit) != 0) {
		SYNAO_LOG("Pushing to \"std::atexit\" buffer failed!\n");
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		SYNAO_LOG("SDL Initialization failed! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	setup_file_t config;
	if (!config.load(kBootPath)) {
		SYNAO_LOG("Couldn't find main configuration file!\n");
		SYNAO_LOG("Main configuration file must exist at \"./data/boot.cfg\" relative to the executable!\n");
		return EXIT_FAILURE;
	}
	// Global input/video/audio devices are generated here...
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
	// Global virtual filesystem device generated here.
	// Accessible from anywhere in order to reduce headaches.
	// Must destroy this before destroying video and audio devices.
	vfs_t fs;
	if (!fs.mount(config)) {
		return EXIT_FAILURE;
	}
	// Global music device is dependent on existance of audio device.
	// Must destroy this before destroying audio device.
	music_t music;
	if (!music.init(config)) {
		return EXIT_FAILURE;
	}
	// Global renderer device is dependent on existance of virtual filesystem and audio devices.
	// Must destroy this before destroying virtual filesystem and audio devices.
	renderer_t renderer;
	if (!renderer.init(config)) {
		return EXIT_FAILURE;
	}
	if (!run(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}