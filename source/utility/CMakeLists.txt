cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"rect.cpp"
	"rng.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"vfs.cpp"
)

target_sources (editor PRIVATE
	"rect.cpp"
	"rng.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"vfs.cpp"
)
