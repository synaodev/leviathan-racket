cmake_minimum_required (VERSION 3.15)

if (NAOMI)
	target_sources (naomi PRIVATE
		"constants.hpp"
		"enums.hpp"
		"hash.hpp"
		"logger.hpp"
		"meta.hpp" "meta.cpp"
		"rect.cpp" "rect.hpp"
		"setup_file.cpp" "setup_file.hpp"
		"thread_pool.cpp" "thread_pool.hpp"
		"tmx_convert.cpp" "tmx_convert.hpp"
		"vfs.cpp" "vfs.hpp"
		"watch.cpp" "watch.hpp"
	)
endif ()

if (EDITOR)
	target_sources (editor PRIVATE
		"constants.hpp"
		"enums.hpp"
		"logger.hpp"
		"rect.cpp" "rect.hpp"
		"setup_file.cpp" "setup_file.hpp"
		"thread_pool.cpp" "thread_pool.hpp"
		"vfs.cpp" "vfs.hpp"
		"watch.cpp" "watch.hpp"
	)
endif ()
