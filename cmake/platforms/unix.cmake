cmake_minimum_required (VERSION 3.13)

find_package (Threads REQUIRED)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
	set (STLFS_LIBS "")
else ()
	set (STLFS_LIBS "stdc++fs")
endif ()

if (NAOMI_BUILD)
	target_link_libraries (naomi PRIVATE
		Threads::Threads
		${STLFS_LIBS}
		${CMAKE_DL_LIBS}
	)
endif ()
if (EDITOR_BUILD)
	target_link_libraries (editor PRIVATE
		Threads::Threads
		${STLFS_LIBS}
		${CMAKE_DL_LIBS}
	)
endif ()
