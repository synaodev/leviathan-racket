#include "./video.hpp"

#include "../utl/setup_file.hpp"
#include "../utl/logger.hpp"
#include "../gfx/glad.hpp"
#include "../gfx/frame_buffer.hpp"

#include <SDL2/SDL_timer.h>

static constexpr sint_t kVideoWidths = 320;
static constexpr sint_t kVideoHeight = 180;

static const byte_t kWindowName[] = "Leviathan Racket";

video_t::video_t() :
	window(nullptr),
	context(nullptr),
	params()
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

bool video_t::init(const setup_file_t& config) {
	config.get("Video", "VerticalSync", params.vsync);
	config.get("Video", "Fullscreen", 	params.full);
	config.get("Video", "ScaleFactor", params.scaling);
	config.get("Video", "FrameLimiter", params.framerate);
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
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0) < 0) {
		SYNAO_LOG("Setting context flags failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) {
		SYNAO_LOG("Setting context profile mask failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
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
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) < 0) {
		SYNAO_LOG("Setting OpenGL major version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) < 0) {
		SYNAO_LOG("Setting OpenGL minor version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow(
		kWindowName,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		kVideoWidths * params.scaling,
		kVideoHeight * params.scaling,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
	);

	if (window == nullptr) {
		SYNAO_LOG("Window creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if (params.full and SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		SYNAO_LOG("Fullscreen after window creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	
	context = SDL_GL_CreateContext(window);

	if (context == nullptr) {
		SYNAO_LOG("OpenGL context creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	sint_t opengl_major = 0;
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major) < 0) {
		SYNAO_LOG("Getting OpenGL major version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	sint_t opengl_minor = 0;
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor) < 0) {
		SYNAO_LOG("Getting OpenGL minor version failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	if ((opengl_major < 3) or (opengl_major == 3 and opengl_minor < 3)) {
		SYNAO_LOG("OpenGL Version %d.%d is unsupported! At least 3.3 is required!", opengl_major, opengl_minor);
		return false;
	} else {
		SYNAO_LOG("OpenGL Version is %d.%d!\n", opengl_major, opengl_minor);
	}
	
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		SYNAO_LOG("OpenGL Extension loading failed!\n");
		return false;
	}

	frame_buffer_t::clear(
		glm::ivec2(kVideoWidths, kVideoHeight) * params.scaling,
		glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
	);

	SDL_GL_SwapWindow(window);

	if (SDL_GL_SetSwapInterval(params.vsync) < 0) {
		SYNAO_LOG("Vertical sync after OpenGL context creation failed! SDL Error: %s\n", SDL_GetError());
		return false;
	}

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

void video_t::flip() const {
	assert(window != nullptr and context != nullptr);
	SDL_GL_SwapWindow(window);
}

void video_t::set_parameters(screen_params_t params) {
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
