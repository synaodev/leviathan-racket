cmake_minimum_required (VERSION 3.13)

target_sources (naomi PRIVATE
	"animation.cpp"
	"font.cpp"
	"image.cpp"
	"program.cpp"
	"vfs.cpp"
)

if (WIN32)
	target_sources (naomi PRIVATE "icon.rc")
endif ()
