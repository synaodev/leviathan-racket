cmake_minimum_required (VERSION 3.13)

if (META_BUILD)
	target_compile_definitions (lvrk PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")
	target_sources (lvrk PRIVATE
		"imgui_draw.cpp"
		"imgui_impl_opengl3.cpp"
		"imgui_impl_sdl.cpp"
		"imgui_widgets.cpp"
		"imgui.cpp"
	)
endif ()
