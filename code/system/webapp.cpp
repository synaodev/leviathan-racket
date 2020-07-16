#ifdef __EMSCRIPTEN__

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

#include <emscripten.h>
#include <cstdlib>
#include <SDL/SDL.h>

static constexpr sint_t kFrameRate	= 60;

static std::string get_boot_path() {
	const std::string init_path = vfs::resource_path(vfs_resource_path_t::Init);
	return init_path + "boot.cfg";
}

struct webapp_t : public not_copyable_t {
public:
	webapp_t() :
		config(),
		input(),
		video(),
		audio(),
		fs(),
		music(),
		renderer(),
		runtime(),
		policy(policy_t::Run),
		sync_watch(),
		head_watch() {}
	webapp_t(webapp_t&&) = delete;
	webapp_t& operator=(webapp_t&&) = delete;
	~webapp_t() = default;
public:
	bool load_config() {
		const std::string boot_path = get_boot_path();
		return config.load(boot_path);
	}
public:
	setup_file_t config;
	input_t input;
	video_t video;
	audio_t audio;
	vfs_t fs;
	music_t music;
	renderer_t renderer;
	runtime_t runtime;
	policy_t policy;
	watch_t sync_watch, head_watch;
};

static void main_loop(optr_t ptr) {
	webapp_t& webapp = *reinterpret_cast<webapp_t*>(ptr);
	webapp.policy = webapp.input.poll(webapp.policy);
	if (webapp.policy != policy_t::Quit) {
		webapp.runtime.update(webapp.head_watch.restart());
		if (webapp.runtime.viable()) {
			if (webapp.runtime.handle(webapp.config, webapp.input, webapp.video, webapp.audio, webapp.music, webapp.renderer)) {
				webapp.runtime.render(webapp.video, webapp.renderer);
			} else {
				webapp.policy = policy_t::Quit;
			}
		}
	}
}

int main(int, char**) {
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
	webapp_t webapp;
	if (!webapp.load_config()) {
		SYNAO_LOG("Couldn't find main configuration file!\n");
		SYNAO_LOG("Shutting down...\n");
		return EXIT_SUCCESS;
	}
	// Global input/video/audio devices are generated here...
	if (!webapp.input.init(webapp.config)) {
		return EXIT_FAILURE;
	}
	if (!webapp.video.init(webapp.config)) {
		return EXIT_FAILURE;
	}
	if (!webapp.audio.init(webapp.config)) {
		return EXIT_FAILURE;
	}
	// Global virtual filesystem device generated here.
	// Accessible from anywhere in order to reduce headaches.
	// Must destroy this before destroying video and audio devices.
	if (!webapp.fs.mount(webapp.config)) {
		return EXIT_FAILURE;
	}
	// Global music device is dependent on existance of audio device.
	// Must destroy this before destroying audio device.
	if (!webapp.music.init(webapp.config)) {
		return EXIT_FAILURE;
	}
	// Global renderer device is dependent on existance of virtual filesystem and audio devices.
	// Must destroy this before destroying virtual filesystem and audio devices.
	glm::ivec2 version = webapp.video.get_opengl_version();
	if (!webapp.renderer.init(version)) {
		return EXIT_FAILURE;
	}
	if (!webapp.runtime.init(webapp.input, webapp.audio, webapp.music, webapp.renderer)) {
		return EXIT_FAILURE;
	}
	// Set emscripten main loop
	emscripten_set_main_loop_arg(main_loop, &webapp, kFrameRate, true);
	return EXIT_SUCCESS;
}

#endif // __EMSCRIPTEN__
