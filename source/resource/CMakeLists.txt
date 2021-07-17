cmake_minimum_required (VERSION 3.13)

target_sources (lvrk PRIVATE
	"animation.cpp"
	"boot-config.cpp"
	"font.cpp"
	"image.cpp"
	"program.cpp"
	"vfs.cpp"
)

if (WIN32)
	target_sources (lvrk PRIVATE "icon.rc")
endif ()
