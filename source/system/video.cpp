#include "./video.hpp"

#include "../resource/config.hpp"
#include "../resource/image.hpp"
#include "../resource/vfs.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../video/frame-buffer.hpp"
#include "../video/gl-check.hpp"

#include <cstring>
#include <glm/common.hpp>
#include <SDL2/SDL.h>

video_t::~video_t() {
	if (context) {
		SDL_GL_DeleteContext(context);
		context = nullptr;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

bool video_t::init(const config_t& config, bool_t tileset_editor) {
	this->tileset_editor = tileset_editor;
	// Raw setup parameters retrieved
	meta_menu = config.get_meta_menu();
	sint_t major = 4;
	sint_t minor = 6;
	if (config.get_legacy_gl()) {
		major = 3;
		minor = 3;
	}
	// Raw screen parameters retrieved
	parameters.vsync = config.get_vertical_sync();
	parameters.full = config.get_fullscreen();
	parameters.scaling = config.get_scaling();
	parameters.framerate = config.get_framerate();
	// Setup parameters
	parameters.scaling = glm::clamp(
		parameters.scaling,
		screen_params_t::kDefaultScaling,
		screen_params_t::kHighestScaling
	);
	parameters.framerate = glm::max(
		parameters.framerate,
		screen_params_t::kDefaultFramerate
	);
	if (tileset_editor) {
		parameters.full = false;
		parameters.vsync = false;
		parameters.framerate = 60;
		parameters.scaling = 1;
	}
	if (window) {
		synao_log("Window already created!\n");
		return false;
	}
	if (context) {
		synao_log("OpenGL context already created!\n");
		return false;
	}
#if defined(LEVIATHAN_PLATFORM_MACOS)
	// MacOS build needs to set Forward Compatible Flags
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) < 0) {
		synao_log("Setting context flags failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
#endif
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) {
		synao_log("Setting OpenGL Core profile failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) < 0) {
		synao_log("Setting depth buffer size failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0) < 0) {
		synao_log("Setting stencil buffer size failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		synao_log("Setting double-buffering failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Create window
	if (tileset_editor) {
		window = SDL_CreateWindow(
			constants::EditorName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::EditorWidth<sint_t>(),
			constants::EditorHeight<sint_t>(),
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	} else {
		window = SDL_CreateWindow(
			constants::NormalName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::NormalWidth<sint_t>() * parameters.scaling,
			constants::NormalHeight<sint_t>() * parameters.scaling,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	}

	if (!window) {
		synao_log("Window creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Try to set fullscreen
	if (parameters.full and SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		synao_log("Fullscreen after window creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Try every OpenGL version from 4.6 to 3.3 and break when no errors
	while (1) {
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) < 0) {
			synao_log("Setting OpenGL major version failed! SDL Error: {}\n", SDL_GetError());
			return false;
		}
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) < 0) {
			synao_log("Setting OpenGL minor version failed! SDL Error: {}\n", SDL_GetError());
			return false;
		}
		context = SDL_GL_CreateContext(window);
		if (context) {
			break;
		} else if (major == 4 and minor > 0) {
			minor -= 1;
		} else if (major == 4 and minor == 0) {
			major = 3;
			minor = 3;
		} else {
			synao_log("Error! OpenGL version must be at least 3.3!\n");
			break;
		}
	}
	// If OpenGL 3.3 isn't available, it's worth telling the user
	if (!context) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"OpenGL Error",
			"Running Leviathan Racket requires at least OpenGL 3.3!\n",
			nullptr
		);
		synao_log("OpenGL context creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Confirm OpenGL version
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major) < 0) {
		synao_log("Getting OpenGL major version failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor) < 0) {
		synao_log("Getting OpenGL minor version failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	synao_log("OpenGL Version is {}.{}!\n", major, minor);
	// Set global version object so everything works properly
	opengl_version[0] = major;
	opengl_version[1] = minor;
	// Load OpenGL extensions with GLAD
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		synao_log("OpenGL Extension loading failed!\n");
		return false;
	}
	// Print vendor information for debugging purposes
	const byte_t* vendor = nullptr;
	glCheck(vendor = (const byte_t*)glGetString(GL_VENDOR));
	synao_log("Video card vendor is {}!\n", vendor);
	// Clear and swap so the screen isn't left blank
	if (tileset_editor) {
		frame_buffer::clear(
			constants::EditorDimensions<sint_t>(),
			glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
		);
	} else {
		frame_buffer::clear(
			constants::NormalDimensions<sint_t>() * parameters.scaling,
			glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
		);
	}
	SDL_GL_SwapWindow(window);
	// Try to set v-sync state
	if (SDL_GL_SetSwapInterval(parameters.vsync) < 0) {
		synao_log("Vertical sync after OpenGL context creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Load window icon image
	const image_t image = image_t::generate(vfs_t::resource_path(vfs_resource_path_t::Image) + "icon.png");
	if (image.empty()) {
		synao_log("Loading icon data failed!\n");
		return false;
	}
	const glm::ivec2 image_dimensions = image.get_dimensions();
	// Generate surface from loaded image
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
		(void_t)&image[0],
		image_dimensions.x,
		image_dimensions.y,
		sizeof(uint_t) * 8,
		sizeof(uint_t) * image_dimensions.x,
		SDL_PIXELFORMAT_RGBA32
	);
	if (surface) {
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
		surface = nullptr;
	} else {
		synao_log("Icon surface creation failed! SDL Error: {}\n", SDL_GetError());
	}
	synao_log("Video system initialized.\n");
	return true;
}

void video_t::flush() const {
	if (window and context) {
		SDL_GL_SwapWindow(window);
	}
}

void video_t::set_parameters(screen_params_t parameters) {
	if (this->parameters.vsync != parameters.vsync) {
		this->parameters.vsync = parameters.vsync;
		if (SDL_GL_SetSwapInterval(parameters.vsync) < 0) {
			synao_log("Vertical sync change failed! SDL Error: {}\n", SDL_GetError());
		}
	}
	if (this->parameters.full != parameters.full) {
		this->parameters.full = parameters.full;
		if (SDL_SetWindowFullscreen(window, parameters.full ? SDL_WINDOW_FULLSCREEN : 0) < 0) {
			synao_log("Window mode change failed! SDL Error: {}\n", SDL_GetError());
		} else {
			SDL_SetWindowPosition(
				window,
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED
			);
		}
	}
	if (this->parameters.scaling != parameters.scaling) {
		this->parameters.scaling = parameters.scaling;
		this->parameters.scaling = glm::clamp(
			this->parameters.scaling,
			screen_params_t::kDefaultScaling,
			screen_params_t::kHighestScaling
		);
		SDL_SetWindowSize(
			window,
			constants::NormalWidth<sint_t>() * parameters.scaling,
			constants::NormalHeight<sint_t>() * parameters.scaling
		);
		SDL_SetWindowPosition(
			window,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED
		);
	}
	if (this->parameters.framerate != parameters.framerate) {
		this->parameters.framerate = glm::max(
			parameters.framerate,
			screen_params_t::kDefaultFramerate
		);
	}
}

const screen_params_t& video_t::get_parameters() const {
	return parameters;
}

glm::vec2 video_t::get_dimensions() const {
	return glm::vec2(this->get_integral_dimensions());
}

glm::ivec2 video_t::get_integral_dimensions() const {
	if (tileset_editor) {
		return constants::EditorDimensions<sint_t>();
	}
	return constants::NormalDimensions<sint_t>() * parameters.scaling;
}

bool video_t::get_meta_menu() const {
	return meta_menu;
}

bool video_t::get_tileset_editor() const {
	return tileset_editor;
}
