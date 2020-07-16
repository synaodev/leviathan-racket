#ifndef __EMSCRIPTEN__

#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"
#include "./editor.hpp"

#include "../utility/vfs.hpp"
#include "../utility/logger.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/misc.hpp"

#include <cstdlib>

#ifndef __EMSCRIPTEN__
	#include <SDL2/SDL.h>
#else
	#include <SDL/SDL.h>
#endif // __EMSCRIPTEN__

static constexpr uint_t kStopDelay  = 40;
static constexpr uint_t kNormDelay  = 10;

static std::string get_boot_path() {
	const std::string init_path = vfs::resource_path(vfs_resource_path_t::Init);
	return init_path + "boot.cfg";
}

static void write_default_config(setup_file_t& config, const std::string& boot_path) {
	config.clear(boot_path);
	config.set("Setup", "Language", std::string("english"));
	// config.set("Setup", "Field", std::string("naomi"));
	// config.set("Setup", "Actor", 200);
	config.set("Video", "VerticalSync", 0);
	config.set("Video", "Fullscreen", 0);
	config.set("Video", "ScaleFactor", 3);
	config.set("Video", "FrameLimiter", 60);
	config.set("Video", "UseOpenGL4", 1);
	config.set("Audio", "Volume", 1.0f);
	config.set("Music", "Volume", 0.34f);
	config.set("Music", "Channels", 2);
	config.set("Music", "SamplingRate", 44100);
	config.set("Music", "kBufferedTime", 0.1f);
	config.set("Input", "KeyJump", (sint_t)SDL_SCANCODE_Z);
	config.set("Input", "KeyHammer", (sint_t)SDL_SCANCODE_X);
	config.set("Input", "KeyItem", (sint_t)SDL_SCANCODE_LSHIFT);
	config.set("Input", "KeyLiteDash", (sint_t)SDL_SCANCODE_A);
	config.set("Input", "KeyContext", (sint_t)SDL_SCANCODE_SPACE);
	config.set("Input", "KeyStrafe", (sint_t)SDL_SCANCODE_LCTRL);
	config.set("Input", "KeyInventory", (sint_t)SDL_SCANCODE_TAB);
	config.set("Input", "KeyOptions", (sint_t)SDL_SCANCODE_ESCAPE);
	config.set("Input", "KeyUp", (sint_t)SDL_SCANCODE_UP);
	config.set("Input", "KeyDown", (sint_t)SDL_SCANCODE_DOWN);
	config.set("Input", "KeyLeft", (sint_t)SDL_SCANCODE_LEFT);
	config.set("Input", "KeyRight", (sint_t)SDL_SCANCODE_RIGHT);
	config.set("Input", "JoyJump", 0);
	config.set("Input", "JoyHammer", 1);
	config.set("Input", "JoyItem", 2);
	config.set("Input", "JoyLiteDash", 3);
	config.set("Input", "JoyContext", 4);
	config.set("Input", "JoyStrafe", 5);
	config.set("Input", "JoyInventory",	6);
	config.set("Input", "JoyOptions", 7);
}

static void load_config_file(setup_file_t& config) {
	const std::string boot_path = get_boot_path();
	if (!config.load(boot_path)) {
		SYNAO_LOG("Couldn't find main configuration file at \"%s\"!\n", boot_path.c_str());
		SYNAO_LOG("Generating new config file...\n");
		write_default_config(config, boot_path);
	}
}

static bool run_naomi(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	runtime_t runtime;
	if (!runtime.init(input, audio, music, renderer)) {
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
						SDL_Delay(kNormDelay);
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
	glm::ivec2 version = video.get_opengl_version();
	renderer_t renderer;
	if (!renderer.init(version)) {
		return EXIT_FAILURE;
	}
	if (!run_naomi(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
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
				editor.render(video, renderer);
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

static int proc_editor(setup_file_t& config) {
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
	glm::ivec2 version = video.get_opengl_version();
	renderer_t renderer;
	if (!renderer.init(version)) {
		return EXIT_FAILURE;
	}
	if (!run_edit(input, video, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int proc_version(const setup_file_t&) {
	SYNAO_LOG("0.0.0.0\n");
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	if (!vfs::verify_structure()) {
		SYNAO_LOG("Fatal error! Directory structure is incorrect!\n");
		return EXIT_FAILURE;
	}
	if (std::atexit(SDL_Quit) != 0) {
		SYNAO_LOG("Pushing to \"std::atexit\" buffer failed!\n");
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		SYNAO_LOG("SDL Initialization failed! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	setup_file_t config;
	load_config_file(config);
	if (argc > 1) {
		const byte_t* option = argv[1];
		if (std::strcmp(option, "--editor") == 0) {
			return proc_editor(config);
		} else if (std::strcmp(option, "--version") == 0) {
			return proc_version(config);
		}
		SYNAO_LOG("Error! Unknown command option!\n");
		return EXIT_FAILURE;
	}
	return proc_naomi(config);
	return EXIT_SUCCESS;
}

#endif // __EMSCRIPTEN__
