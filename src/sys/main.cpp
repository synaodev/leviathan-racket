#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"
#include "./editor.hpp"

#include "../utl/vfs.hpp"
#include "../utl/logger.hpp"
#include "../utl/setup_file.hpp"
#include "../utl/misc.hpp"

#include <cstdlib>
#include <SDL2/SDL.h>

static constexpr byte_t kBootPath[]	= "./data/boot.cfg";
static constexpr uint_t kStopDelay  = 40;
static constexpr uint_t kGL42Delay  = 10;
static constexpr uint_t kGL33Delay  = 16;

static void generate_default_config(setup_file_t& config) {
	config.clear(kBootPath);
	config.set("Setup", "Language", std::string("english"));
	config.set("Setup", "Field", std::string("naomi"));
	config.set("Setup", "Actor", 200);
	config.set("Video", "VerticalSync", 0);
	config.set("Video", "Fullscreen", 0);
	config.set("Video", "ScaleFactor", 3);
	config.set("Video", "FrameLimiter", 60);
	config.set("Video", "UseOpenGL4", 1);
	config.set("Audio", "Volume", 1.0f);
	config.set("Music", "Volume", 0.3f);
	config.set("Music", "Channels", 2);
	config.set("Music", "SamplingRate", 44100);
	config.set("Music", "kBufferedTime", 0.1f);
	config.set("Input", "KeyJump", 29);
	config.set("Input", "KeyHammer", 27);
	config.set("Input", "KeyItem", 225);
	config.set("Input", "KeyLiteDash", 22);
	config.set("Input", "KeyContext", 44);
	config.set("Input", "KeyStrafe", 224);
	config.set("Input", "KeyInventory", 43);
	config.set("Input", "KeyOptions", 41);
	config.set("Input", "KeyUp", 82);
	config.set("Input", "KeyDown", 81);
	config.set("Input", "KeyLeft", 80);
	config.set("Input", "KeyRight", 79);
}

static bool run_naomi(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
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

static bool run_edit(input_t& input, video_t& video, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	editor_t editor;
	if (!editor.init(video, renderer)) {
		return false;
	}
	watch_t sync_watch, head_watch;
	while (policy != policy_t::Quit) {
		policy = input.poll(policy, editor_t::get_event_callback());
		if (policy != policy_t::Stop) {
			editor.update(head_watch.restart());
			if (editor.viable()) {
				editor.handle(input, renderer);
				editor.render(renderer, video);
				real64_t waiting = misc::kIntervalMin - sync_watch.elapsed();
				if (waiting > 0.0) {
					uint_t ticks = static_cast<uint_t>(waiting * 1000.0);
					SDL_Delay(ticks);
				}
				sync_watch.restart();
			}
		} else {
			SDL_Delay(kStopDelay);
		}
	}
	return true;
}

static int proc_naomi(setup_file_t& config) {
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
	if (!run_naomi(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int proc_edit(setup_file_t& config) {
	config.set("Video", "VerticalSync", 0);
	input_t input;
	if (!input.init(config)) {
		return EXIT_FAILURE;
	}
	video_t video;
	if (!video.init(config, true)) {
		return EXIT_FAILURE;
	}
	vfs_t fs;
	if (!fs.mount(config)) {
		return EXIT_FAILURE;
	}
	renderer_t renderer;
	if (!renderer.init(config)) {
		return EXIT_FAILURE;
	}
	if (!run_edit(input, video, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
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
		SYNAO_LOG("Couldn't find main configuration file! Generating new config file...\n");
		generate_default_config(config);
	}
	if (argc > 1) {
		const std::string arg = argv[1];
		if (arg == "edit") {
			return proc_edit(config);
		}
		SYNAO_LOG("Error! Unknown command option!\n");
		return EXIT_FAILURE;
	}
	return proc_naomi(config);
}
