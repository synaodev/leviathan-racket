cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"rng.cpp"
	"setup-file.cpp"
	"thread-pool.cpp"
	"utf32.cpp"
	"vfs.cpp"
)

target_sources (editor PRIVATE
	"rng.cpp"
	"setup-file.cpp"
	"thread-pool.cpp"
	"utf32.cpp"
	"vfs.cpp"
)
