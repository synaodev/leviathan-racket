#include "./video.hpp"

#include "../video/frame_buffer.hpp"
#include "../resource/icon.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/logger.hpp"

#ifndef __EMSCRIPTEN__
	#include "../video/glad.hpp"
#else // __EMSCRIPTEN__
	#include <SDL/SDL.h>
	#include <SDL/SDL_opengles2.h>
	#include <GLES3/gl3.h>
	#include <emscripten.h>
#endif // __EMSCRIPTEN__

static constexpr byte_t kWindowName[] = "Leviathan Racket";
static constexpr sint_t kVideoWidths  = 320;
static constexpr sint_t kVideoHeight  = 180;
static constexpr sint_t kImguiWidths  = 800;

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
#ifndef __EMSCRIPTEN__
	bool_t use_opengl_4 = true;
	config.get("Video", "UseOpenGL4", use_opengl_4);
	if (use_opengl_4) {
		this->major = 4;
		this->minor = 6;
	} else {
		this->major = 3;
		this->minor = 3;
	}
#else // __EMSCRIPTEN__
	this->major = 3;
	this->minor = 2;
#endif // __EMSCRIPTEN__
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
		SYNAO_LOG("Window already created!\n");
		return false;
	}
	if (context != nullptr) {
		SYNAO_LOG("OpenGL context already created!\n");
		return false;
	}
#ifdef __APPLE__
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0) < 0) {
		SYNAO_LOG("Setting context flags failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
#endif // __APPLE__
#ifdef __EMSCRIPTEN__
	// if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES) < 0) {
	// 	SYNAO_LOG("Setting OpenGLES/WebGL profile failed! SDL Error: %s\n", SDL_GetError());
	// 	return false;
	// }
#else // __EMSCRIPTEN__
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) {
		SYNAO_LOG("Setting OpenGL Core profile failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
#endif // __EMSCRIPTEN__
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) < 0) {
		SYNAO_LOG("Setting depth buffer size failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0) < 0) {
		SYNAO_LOG("Setting stencil buffer size failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		SYNAO_LOG("Setting double-buffering failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (start_imgui) {
		window = SDL_CreateWindow(
			kWindowName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			kImguiWidths,
			kImguiWidths,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	} else {
		window = SDL_CreateWindow(
			kWindowName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			kVideoWidths * params.scaling,
			kVideoHeight * params.scaling,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	}
	if (window == nullptr) {
		SYNAO_LOG("Window creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
#ifndef __EMSCRIPTEN__
	if (params.full and SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		SYNAO_LOG("Fullscreen after window creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
#endif // __EMSCRIPTEN__
	while (1) {
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, this->major) < 0) {
			SYNAO_LOG("Setting OpenGL major version failed! SDL Error: %s\n", SDL_GetError());
			return false;
		}
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, this->minor) < 0) {
			SYNAO_LOG("Setting OpenGL minor version failed! SDL Error: %s\n", SDL_GetError());
			return false;
		}
		context = SDL_GL_CreateContext(window);
		if (context != nullptr) {
			break;
		}
#ifdef __EMSCRIPTEN__
		else if (this->major == 3 and this->minor > 0) {
			this->minor -= 1;
		} else {
			SYNAO_LOG("Error! OpenGL ES version must be at least 3.0!\n");
			break;
		}
#else // __EMSCRIPTEN__
		else if (this->major == 4 and this->minor > 0) {
			this->minor -= 1;
		} else if (this->major == 4 and this->minor == 0) {
			this->major = 3;
			this->minor = 3;
		} else {
			SYNAO_LOG("Error! OpenGL version must be at least 3.3!\n");
			break;
		}
#endif // __EMSCRIPTEN__
	}
	if (context == nullptr) {
		SYNAO_LOG("OpenGL context creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &this->major) < 0) {
		SYNAO_LOG("Getting OpenGL major version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &this->minor) < 0) {
		SYNAO_LOG("Getting OpenGL minor version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	SYNAO_LOG("OpenGL Version is %d.%d!\n", this->major, this->minor);
#ifndef __EMSCRIPTEN__
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		SYNAO_LOG("OpenGL Extension loading failed!\n");
		return false;
	}
#endif // __EMSCRIPTEN__
	if (start_imgui) {
		frame_buffer_t::clear(
			glm::ivec2(kImguiWidths),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	} else {
		frame_buffer_t::clear(
			glm::ivec2(kVideoWidths, kVideoHeight) * params.scaling,
			glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
		);
	}
	SDL_GL_SwapWindow(window);
#ifndef __EMSCRIPTEN__
	if (!start_imgui and SDL_GL_SetSwapInterval(params.vsync) < 0) {
		SYNAO_LOG("Vertical sync after OpenGL context creation failed! SDL Error: %s\n", SDL_GetError());
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
		SYNAO_LOG("Icon surface creation failed!\nSDL Error: %s\n", SDL_GetError());
	}
#endif // __EMSCRIPTEN__
	SYNAO_LOG("Video system initialized.\n");
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
#ifndef __EMSCRIPTEN__
	if (this->params.vsync != params.vsync) {
		this->params.vsync = params.vsync;
		if (SDL_GL_SetSwapInterval(params.vsync) < 0) {
			SYNAO_LOG("Vertical sync change failed! SDL Error: %s\n", SDL_GetError());
		}
	}
	if (this->params.full != params.full) {
		this->params.full = params.full;
		if (SDL_SetWindowFullscreen(window, params.full ? SDL_WINDOW_FULLSCREEN : 0) < 0) {
			SYNAO_LOG("Window mode change failed! SDL Error: %s\n", SDL_GetError());
		} else 
		{
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
			kVideoWidths * params.scaling, 
			kVideoHeight * params.scaling
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
#endif // __EMSCRIPTEN__
}

screen_params_t video_t::get_parameters() const {
	return params;
}

glm::vec2 video_t::get_dimensions() const {
	return glm::vec2(
		kVideoWidths * params.scaling,
		kVideoHeight * params.scaling
	);
}

glm::ivec2 video_t::get_integral_dimensions() const {
	return params.scaling * glm::ivec2(
		kVideoWidths,
		kVideoHeight
	);
}

glm::ivec2 video_t::get_imgui_dimensions() const {
	return glm::ivec2(kImguiWidths);
}

glm::ivec2 video_t::get_opengl_version() const {
	return glm::ivec2(major, minor);
}
