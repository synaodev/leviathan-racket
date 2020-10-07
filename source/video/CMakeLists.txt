cmake_minimum_required (VERSION 3.13)

if (NAOMI_BUILD)
	target_sources (naomi PRIVATE
		"const-buffer.cpp"
		"display-list.cpp"
		"font.cpp"
		"frame-buffer.cpp"
		"gfx.cpp"
		"gl-check.cpp"
		"image.cpp"
		"pipeline.cpp"
		"quad-buffer.cpp"
		"sampler.cpp"
		"texture.cpp"
		"vertex-pool.cpp"
		"vertex.cpp"
	)
endif ()

if (EDITOR_BUILD)
	target_sources (editor PRIVATE
		"const-buffer.cpp"
		"display-list.cpp"
		"frame-buffer.cpp"
		"gfx.cpp"
		"gl-check.cpp"
		"image.cpp"
		"pipeline.cpp"
		"quad-buffer.cpp"
		"sampler.cpp"
		"texture.cpp"
		"vertex-pool.cpp"
		"vertex.cpp"
	)
endif ()
