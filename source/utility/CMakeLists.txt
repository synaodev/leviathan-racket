cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"meta.cpp"
	"rect.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"vfs.cpp"
	"watch.cpp"
)

target_sources (editor PRIVATE
	"rect.cpp"
	"setup_file.cpp"
	"thread_pool.cpp"
	"vfs.cpp"
	"watch.cpp"
)
