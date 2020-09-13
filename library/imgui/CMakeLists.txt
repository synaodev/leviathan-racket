cmake_minimum_required (VERSION 3.15)

if (NAOMI_BUILD AND META_BUILD)
	target_compile_definitions (naomi PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")
	target_sources (naomi PRIVATE
		"imgui_draw.cpp"
		"imgui_impl_opengl3.cpp"
		"imgui_impl_sdl.cpp"
		"imgui_widgets.cpp"
		"imgui.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_compile_definitions (editor PRIVATE "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")
	target_sources (editor PRIVATE
		"imgui_draw.cpp"
		"imgui_impl_opengl3.cpp"
		"imgui_impl_sdl.cpp"
		"imgui_widgets.cpp"
		"imgui.cpp"
	)
endif ()
