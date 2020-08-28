cmake_minimum_required (VERSION 3.15)

if (EDITOR)
	target_compile_definitions (editor PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")
	target_sources (editor PRIVATE
		"imgui_draw.cpp"
		"imgui_impl_opengl3.cpp"
		"imgui_impl_sdl.cpp"
		"imgui_widgets.cpp"
		"imgui.cpp"
	)
endif ()
