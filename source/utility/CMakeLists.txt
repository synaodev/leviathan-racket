cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"rng.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"utf32.cpp"
	"vfs.cpp"
)

target_sources (editor PRIVATE
	"rng.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"utf32.cpp"
	"vfs.cpp"
)
