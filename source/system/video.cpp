#include "./video.hpp"

#include "../video/frame_buffer.hpp"
#include "../video/glad.hpp"
#include "../resource/icon.hpp"
#include "../utility/constants.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/logger.hpp"

#include <SDL2/SDL.h>

video_t::video_t() :
	window(nullptr),
	context(nullptr),
	params(),
	major(4),
	minor(6)
{

}

video_t::~video_t() {
	if (context != nullptr) {
		SDL_GL_DeleteContext(context);
		context = nullptr;
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

bool video_t::init(const setup_file_t& config, bool start_imgui) {
	config.get("Video", "VerticalSync", params.vsync);
	config.get("Video", "Fullscreen", 	params.full);
	config.get("Video", "ScaleFactor", params.scaling);
	config.get("Video", "FrameLimiter", params.framerate);
	bool_t use_opengl_4 = true;
	config.get("Video", "UseOpenGL4", use_opengl_4);
	if (!use_opengl_4) {
		this->major = 3;
		this->minor = 3;
	}
	params.scaling = glm::clamp(
		params.scaling,
		screen_params_t::kDefaultScaling,
		screen_params_t::kHighestScaling
	);
	params.framerate = glm::max(
		params.framerate,
		screen_params_t::kDefaultFramerate
	);
	if (window != nullptr) {
		synao_log("Window already created!\n");
		return false;
	}
	if (context != nullptr) {
		synao_log("OpenGL context already created!\n");
		return false;
	}
#ifdef __APPLE__
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) < 0) {
		synao_log("Setting context flags failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
#endif // __APPLE__
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) {
		synao_log("Setting OpenGL Core profile failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) < 0) {
		synao_log("Setting depth buffer size failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0) < 0) {
		synao_log("Setting stencil buffer size failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		synao_log("Setting double-buffering failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (start_imgui) {
		window = SDL_CreateWindow(
			constants::WindowName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::ImguiWidth<sint_t>(),
			constants::ImguiWidth<sint_t>(),
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	} else {
		window = SDL_CreateWindow(
			constants::WindowName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::NormalWidth<sint_t>() * params.scaling,
			constants::NormalHeight<sint_t>() * params.scaling,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	}
	if (window == nullptr) {
		synao_log("Window creation failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (params.full and SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		synao_log("Fullscreen after window creation failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	while (1) {
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, this->major) < 0) {
			synao_log("Setting OpenGL major version failed!\nSDL Error: %s\n", SDL_GetError());
			return false;
		}
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, this->minor) < 0) {
			synao_log("Setting OpenGL minor version failed!\nSDL Error: %s\n", SDL_GetError());
			return false;
		}
		context = SDL_GL_CreateContext(window);
		if (context != nullptr) {
			break;
		} else if (this->major == 4 and this->minor > 0) {
			this->minor -= 1;
		} else if (this->major == 4 and this->minor == 0) {
			this->major = 3;
			this->minor = 3;
		} else {
			synao_log("Error! OpenGL version must be at least 3.3!\n");
			break;
		}
	}
	// If OpenGL 3.3 isn't available, it's worth telling the user.
	if (context == nullptr) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"OpenGL Error",
			"Running Leviathan Racket requires at least OpenGL 3.3.\n",
			nullptr
		);
		synao_log("OpenGL context creation failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &this->major) < 0) {
		synao_log("Getting OpenGL major version failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &this->minor) < 0) {
		synao_log("Getting OpenGL minor version failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	synao_log("OpenGL Version is %d.%d!\n", this->major, this->minor);
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		synao_log("OpenGL Extension loading failed!\n");
		return false;
	}
	if (start_imgui) {
		frame_buffer_t::clear(
			constants::ImguiDimensions<sint_t>(),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	} else {
		frame_buffer_t::clear(
			constants::NormalDimensions<sint_t>() * params.scaling,
			glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
		);
	}
	SDL_GL_SwapWindow(window);
	if (!start_imgui and SDL_GL_SetSwapInterval(params.vsync) < 0) {
		synao_log("Vertical sync after OpenGL context creation failed!\nSDL Error: %s\n", SDL_GetError());
		return false;
	}
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
		icon::pixels(),
		icon::width(),
		icon::height(),
		icon::depth(),
		icon::pitch(),
		SDL_PIXELFORMAT_RGBA32
	);
	if (surface != nullptr) {
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
		surface = nullptr;
	} else {
		synao_log("Icon surface creation failed!\nSDL Error: %s\n", SDL_GetError());
	}
	synao_log("Video system initialized.\n");
	return true;
}

void video_t::submit(const frame_buffer_t* frame_buffer, arch_t index) const {
	if (frame_buffer != nullptr) {
		const glm::ivec2 source_dimensions = frame_buffer->get_integral_dimensions();
		const glm::ivec2 destination_dimensions = this->get_integral_dimensions();
		frame_buffer_t::bind(nullptr, frame_buffer_binding_t::Write, 0);
		frame_buffer_t::bind(frame_buffer, frame_buffer_binding_t::Read, index);
		frame_buffer_t::blit(source_dimensions, destination_dimensions);
	}
}

void video_t::flush() const {
	if (window != nullptr and context != nullptr) {
		SDL_GL_SwapWindow(window);
	}
}

void video_t::set_parameters(screen_params_t params) {
	if (this->params.vsync != params.vsync) {
		this->params.vsync = params.vsync;
		if (SDL_GL_SetSwapInterval(params.vsync) < 0) {
			synao_log("Vertical sync change failed! SDL Error: %s\n", SDL_GetError());
		}
	}
	if (this->params.full != params.full) {
		this->params.full = params.full;
		if (SDL_SetWindowFullscreen(window, params.full ? SDL_WINDOW_FULLSCREEN : 0) < 0) {
			synao_log("Window mode change failed! SDL Error: %s\n", SDL_GetError());
		} else {
			SDL_SetWindowPosition(
				window,
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED
			);
		}
	}
	if (this->params.scaling != params.scaling) {
		this->params.scaling = params.scaling;
		this->params.scaling = glm::clamp(
			this->params.scaling,
			screen_params_t::kDefaultScaling,
			screen_params_t::kHighestScaling
		);
		SDL_SetWindowSize(
			window,
			constants::NormalWidth<sint_t>() * params.scaling,
			constants::NormalHeight<sint_t>() * params.scaling
		);
		SDL_SetWindowPosition(
			window,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED
		);
	}
	if (this->params.framerate != params.framerate) {
		this->params.framerate = glm::max(
			params.framerate,
			screen_params_t::kDefaultFramerate
		);
	}
}

screen_params_t video_t::get_parameters() const {
	return params;
}

glm::vec2 video_t::get_dimensions() const {
	return glm::vec2(this->get_integral_dimensions());
}

glm::ivec2 video_t::get_integral_dimensions() const {
	return constants::NormalDimensions<sint_t>() * params.scaling;
}

glm::ivec2 video_t::get_imgui_dimensions() const {
	return constants::ImguiDimensions<sint_t>();
}

glm::ivec2 video_t::get_opengl_version() const {
	return glm::ivec2(major, minor);
}
