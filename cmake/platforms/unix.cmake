cmake_minimum_required (VERSION 3.13)

find_package (Threads REQUIRED)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
	set (STLFS_LIBS "")
else ()
	set (STLFS_LIBS "stdc++fs")
endif ()

target_link_libraries (lvrk PRIVATE
	Threads::Threads
	${STLFS_LIBS}
	${CMAKE_DL_LIBS}
)
