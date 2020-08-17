cmake_minimum_required (VERSION 3.15)

target_compile_definitions (leviathan PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")

target_sources (leviathan PRIVATE
	"imconfig.h"
	"imgui_draw.cpp"
	"imgui_impl_opengl3.cpp" "imgui_impl_opengl3.h"
	"imgui_impl_sdl.cpp" "imgui_impl_sdl.h"
	"imgui_internal.h"
	"imgui_widgets.cpp"
	"imgui.cpp" "imgui.h"
	"imstb_rectpack.h"
	"imstb_textedit.h"
	"imstb_truetype.h"
)
