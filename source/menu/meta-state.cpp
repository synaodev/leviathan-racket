#include "./meta-state.hpp"

#ifdef LEVIATHAN_USES_META

#if defined(LEVIATHAN_TOOLCHAIN_MSVC) && !defined(_CRT_SECURE_NO_WARNINGS)
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../resource/program.hpp"
#include "../system/input.hpp"
#include "../system/video.hpp"
#include "../utility/logger.hpp"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>

bool_t meta_state_t::Hitboxes = false;
bool_t meta_state_t::Framerate = false;
bool_t meta_state_t::Ready = false;

meta_state_t::meta_state_t() :
	active(false),
	amend(false),
	window(nullptr),
	context(nullptr)
{

}

meta_state_t::~meta_state_t() {
	if (context) {
		ImGui_ImplOpenGL3_Shutdown();
		context = nullptr;
	}
	if (window) {
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		window = nullptr;
	}
	Ready = false;
}

bool meta_state_t::init(const video_t& video) {
	if (Ready) {
		synao_log("Error! Meta menu is already initialized!\n");
		return false;
	}
	if (video.get_meta_option()) {
		auto device = video.get_device();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		if (!ImGui_ImplSDL2_InitForOpenGL(
				std::get<SDL_Window*>(device),
				std::get<SDL_GLContext>(device)
			)) {
			synao_log("Error! Failed to initialize ImGui SDL2!\n");
			return false;
		}
		window = std::get<SDL_Window*>(device);
		const std::string directive = program::directive();
		if (!ImGui_ImplOpenGL3_Init(directive.c_str())) {
			synao_log("Error! Failed to initialize ImGui OpenGL!\nGLSL Version: \"{}\"", directive);
			return false;
		}
		context = std::get<SDL_GLContext>(device);
		Ready = true;
		synao_log("Meta menu is ready.\n");
	}
	return true;
}

void meta_state_t::handle(const input_t& input) {
	if (Ready and !amend) {
		if (input.get_meta_pressed(SDL_SCANCODE_BACKSPACE)) {
			active = !active;
		}
		if (active) {
			amend = true;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window);
			ImGui::NewFrame();
			// Begin
			ImGui::BeginMainMenuBar();
			ImGui::EndMainMenuBar();
			// End
			ImGui::Render();
		} else {
			amend = false;
		}
	}
}

void meta_state_t::update(real64_t delta) {
	if (active) {

	}
}

void meta_state_t::flush() const {
	if (active and amend) {
		amend = false;
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

meta_state_t::event_callback_t meta_state_t::get_event_callback() {
	if (Ready) {
		return ImGui_ImplSDL2_ProcessEvent;
	}
	return nullptr;
}

#endif
