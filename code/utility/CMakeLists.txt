cmake_minimum_required (VERSION 3.15)

target_sources (leviathan PRIVATE
	"enums.hpp"
	"hash.hpp"
	"json.hpp"
	"logger.hpp"
	"misc.hpp"
	"rect.cpp" "rect.hpp"
	"setup_file.cpp" "setup_file.hpp"
	"stb_image.h"
	"thread_pool.cpp" "thread_pool.hpp"
	"tmx_convert.cpp" "tmx_convert.hpp"
	"vfs.cpp" "vfs.hpp"
	"watch.cpp" "watch.hpp"
)
