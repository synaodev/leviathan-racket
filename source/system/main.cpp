#include "./main.hpp"

#include <cstdlib>
#include <cstring>
#include <csignal>
#include <atomic>
#include <iostream>
#include <SDL2/SDL.h>

#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"

#include "../editor/editor.hpp"
#include "../resource/config.hpp"
#include "../resource/vfs.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"

static std::atomic<bool> interrupt = false;
static void sigint_handler(int) {
	interrupt = true;
}

static constexpr uint_t kStopDelay = 40;
static constexpr uint_t kNormDelay = 10;

#ifdef LEVIATHAN_USES_META

static bool editor_loop(config_t& config, input_t& input, video_t& video, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	editor_t editor {};
	if (!editor.init(video, renderer)) {
		synao_log("Editor initialization failed!\n");
		return false;
	}
	// Start watches
	watch_t sync_watch {};
	watch_t head_watch {};

	synao_log("Entering editor loop...\n");
	while (policy != policy_t::Quit) {
		if (interrupt) {
			synao_log("[SIGINT]\n");
			break;
		}
		policy = input.poll(policy, editor.get_event_callback());
		if (policy != policy_t::Stop) {
			editor.update(head_watch.restart());
			if (editor.viable()) {
				if (editor.handle(input, renderer)) {
					editor.render(video, renderer);
					auto& params = video.get_parameters();
					real64_t waiting = (1.0 / params.framerate) - sync_watch.elapsed();
					if (waiting > 0.0) {
						uint_t ticks = static_cast<uint_t>(waiting * 1000.0);
						SDL_Delay(ticks);
					}
					sync_watch.restart();
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

static int editor_process(config_t& config) {
	// Global input/video devices are generated here...
	input_t input {};
	if (!input.init(config)) {
		return EXIT_FAILURE;
	}
	video_t video {};
	if (!video.init(config, true)) {
		return EXIT_FAILURE;
	}
	// Global virtual filesystem device generated here.
	// Accessible from anywhere in order to reduce headaches.
	// Must destroy this before destroying video and audio devices.
	vfs_t vfs {};
	if (!vfs.init(config)) {
		return EXIT_FAILURE;
	}
	// Global renderer device is dependent on existance of virtual filesystem and audio devices.
	// Must destroy this before destroying virtual filesystem and audio devices.
	renderer_t renderer {};
	if (!renderer.init(vfs)) {
		return EXIT_FAILURE;
	}
	if (!editor_loop(config, input, video, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

#endif

static bool normal_loop(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	runtime_t runtime {};
	if (!runtime.init(input, video, audio, music, renderer)) {
		synao_log("Runtime initialization failed!\n");
		return false;
	}
	// Start watches
	watch_t sync_watch {};
	watch_t head_watch {};

	synao_log("Entering main loop...\n");
	while (policy != policy_t::Quit) {
		if (interrupt) {
			synao_log("[SIGINT]\n");
			break;
		}
		policy = input.poll(policy, meta_state_t::get_event_callback());
		if (policy != policy_t::Stop) {
			runtime.update(head_watch.restart());
			if (runtime.viable()) {
				if (runtime.handle(config, input, video, audio, music, renderer)) {
					runtime.render(video, renderer);
					auto& params = video.get_parameters();
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
	if (!input.save(config)) {
		synao_log("Warning! Something went wrong when trying to save inputs to macro file!\n");
	}
	return config.save();
}

static int normal_process(config_t& config) {
	// Global input/video/audio devices are generated here...
	input_t input {};
	if (!input.init(config)) {
		return EXIT_FAILURE;
	}
	video_t video {};
	if (!video.init(config)) {
		return EXIT_FAILURE;
	}
	audio_t audio {};
	if (!audio.init(config)) {
		return EXIT_FAILURE;
	}
	// Global virtual filesystem device generated here.
	// Accessible from anywhere in order to reduce headaches.
	// Must destroy this before destroying video and audio devices.
	vfs_t vfs {};
	if (!vfs.init(config)) {
		return EXIT_FAILURE;
	}
	// Global music device is dependent on existance of audio device.
	// Must destroy this before destroying audio device.
	music_t music {};
	if (!music.init(config)) {
		return EXIT_FAILURE;
	}
	// Global renderer device is dependent on existance of virtual filesystem and audio devices.
	// Must destroy this before destroying virtual filesystem and audio devices.
	renderer_t renderer {};
	if (!renderer.init(vfs)) {
		return EXIT_FAILURE;
	}
	if (!normal_loop(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static config_t load_config() {
	config_t result {};
	const std::string init_path = vfs_t::resource_path(vfs_resource_path_t::Init);
	if (vfs_t::create_directory(init_path)) {
		const std::string boot_path = init_path + "boot.json";
		if (!result.load(boot_path)) {
			synao_log("Couldn't find config file named \"{}\"! Generating new config file.\n", boot_path);
			result.generate(boot_path);
		}
	} else {
		synao_log("Warning! Will not be able to save config file!\n");
	}
	return result;
}

static bool mount(const byte_t* provided_directory) {
	// Save initial working directory before attempting to change it
	const std::string working_directory = vfs_t::working_directory();
	if (provided_directory) {
		if (vfs_t::mount(provided_directory, false)) {
			return true;
		} else {
			synao_log("Warning! Couldn\'t mount filesystem at directory: \"{}\"!\n", provided_directory);
		}
	}
	// Try to mount (initial) working directory
	if (vfs_t::mount(working_directory, false)) {
		return true;
	}
	// If that doesn't work, look inside executable directory
	const std::string executable_directory = vfs_t::executable_directory();
	if (vfs_t::mount(executable_directory)) {
		return true;
	}
	// If mounting is impossible, notify user of the initial working directory
	synao_log("Warning! Working directory before mounting process was \"{}\"!\n", working_directory);
	return false;
}

static constexpr byte_t kArgTileset[] = "--tileset-editor";

int main(int argc, char** argv) {
	// If writing to stdout (debug build), speed up I/O
#ifdef LEVIATHAN_BUILD_DEBUG
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);
#endif
	// Print version information
	synao_log(
		"====== Leviathan Racket ======\n"
		"   Version: {}.{}.{}.{}\n"
		"   Author: Tyler Cadena\n"
		"   Platform: {} {}\n"
		"   Compiler: {}\n"
		"   Toolchain: {}\n"
		"   Build Type: {}\n"
		"==============================\n",
		LEVIATHAN_VERSION_INFORMATION_MAJOR,
		LEVIATHAN_VERSION_INFORMATION_MINOR,
		LEVIATHAN_VERSION_INFORMATION_PATCH,
		LEVIATHAN_VERSION_INFORMATION_TWEAK,
		version_information::platform,
		version_information::architecture,
		version_information::compiler,
		version_information::toolchain,
		version_information::build_type
	);
	// Handle arguments
	bool tileset_editor = false;
	{
		const byte_t* directory = nullptr;
		for (sint_t it = 1; it < argc; ++it) {
			const byte_t* option = argv[it];
			if (!option) {
				synao_log("Fatal error! Option #{} should not be null!\n", it);
			} else if (!tileset_editor and std::strcmp(option, kArgTileset) == 0) {
				tileset_editor = true;
#ifndef LEVIATHAN_USES_META
				synao_log("Error! Tileset editor is not available!\n");
#endif
			} else if (!directory) {
				directory = option;
			} else {
				break;
			}
		}
		// Set "mounting" directory for virtual filesystem
		if (!mount(directory)) {
			synao_log("Fatal error! Could not mount filesystem!\n");
			return EXIT_FAILURE;
		}
	}
	// Register SIGINT handler
	std::signal(SIGINT, sigint_handler);
	// Initialize SDL2
	if (std::atexit(SDL_Quit) != 0) {
		synao_log("Pushing to \"std::atexit\" buffer failed!\n");
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		synao_log("SDL Initialization failed! SDL Error: {}\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Load config
	config_t config = load_config();
	// Run desired process
#ifdef LEVIATHAN_USES_META
	return tileset_editor ?
		editor_process(config) :
		normal_process(config);
#else
	return normal_process(config);
#endif
}
