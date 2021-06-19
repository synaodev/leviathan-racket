#include "./editor.hpp"

#ifdef LEVIATHAN_USES_META

#include "../system/input.hpp"
#include "../system/video.hpp"
#include "../system/renderer.hpp"

#include "../field/tileflag.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"

#if defined(LEVIATHAN_TOOLCHAIN_MSVC) && !defined(_CRT_SECURE_NO_WARNINGS)
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>

static constexpr arch_t kFirstFlagSizes = 12;
static const byte_t* kFirstFlagNames[kFirstFlagSizes] = {
	"Block", "Slope", "Positive", "Negative",
	"Floor", "Ceiling", "Short", "Tall",
	"Harmful", "Outbounds", "Fallthrough", "Hooked"
};
static const uint_t kFirstFlagInts[kFirstFlagSizes] = {
	tileflag_t::Block, tileflag_t::Slope, tileflag_t::Positive, tileflag_t::Negative,
	tileflag_t::Floor, tileflag_t::Ceiling, tileflag_t::Short, tileflag_t::Tall,
	tileflag_t::Harmful, tileflag_t::OutBounds, tileflag_t::FallThrough, tileflag_t::Hooked
};

static constexpr arch_t kSecondFlagSizes = 8;
static const byte_t* kSecondFlagNames[kSecondFlagSizes] = {
	"Slope-1",	"Slope-2",	"Slope-3",	"Slope-4",
	"Slope-5",	"Slope-6",	"Slope-7",	"Slope-8"
};
static const uint_t kSecondFlagInts[kSecondFlagSizes] = {
	tileflag_t::Slope_1, tileflag_t::Slope_2,
	tileflag_t::Slope_3, tileflag_t::Slope_4,
	tileflag_t::Slope_5, tileflag_t::Slope_6,
	tileflag_t::Slope_7, tileflag_t::Slope_8
};

editor_t::~editor_t() {
	if (context) {
		ImGui_ImplOpenGL3_Shutdown();
		context = nullptr;
	}
	if (window) {
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		window = nullptr;
	}
}

bool editor_t::init(const video_t& video, renderer_t& renderer) {
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

	renderer.ortho(video.get_integral_dimensions());

	synao_log("Tileset attribute editor subsystems initialized.\n");
	return true;
}

bool editor_t::handle(input_t& input, renderer_t& renderer) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
	accum = 0.0;
	attribute_viewer.handle(tileset_viewer, renderer);
	tileset_viewer.handle(input);
	if (!attribute_viewer.active()) {
		if (tileset_viewer.selected()) {
			ImGui::SetNextWindowPos(ImVec2(8, 32));
			ImGui::Begin(
				"Attributes",
				nullptr,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize
			);
			uint_t bitmask = tileset_viewer.get_bitmask();
			if (ImGui::Button("Reset")) {
				bitmask = 0;
			}
			for (arch_t it = 0; it < kFirstFlagSizes; ++it) {
				ImGui::CheckboxFlags(
					kFirstFlagNames[it],
					&bitmask,
					kFirstFlagInts[it]
				);
			}
			ImGui::Separator();
			for (arch_t it = 0; it < kSecondFlagSizes; ++it) {
				ImGui::CheckboxFlags(
					kSecondFlagNames[it],
					&bitmask,
					kSecondFlagInts[it]
				);
			}
			tileset_viewer.set_bitmask(bitmask);
			ImGui::End();
		}
	}

	ImGui::Render();
	input.flush();
	return true;
}

void editor_t::update(real64_t delta) {
	accum += delta;
	tileset_viewer.update(delta);
}

void editor_t::render(const video_t& video, renderer_t& renderer) const {
	tileset_viewer.render(renderer);
	renderer.flush(video.get_integral_dimensions());
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	video.flush();
}

bool editor_t::viable() const {
	return accum >= constants::MaxInterval();
}

editor_t::event_callback_t editor_t::get_event_callback() {
	return ImGui_ImplSDL2_ProcessEvent;
}

#endif
