cmake_minimum_required (VERSION 3.13)

find_package (Threads REQUIRED)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
	set (STLFS_LIBS "")
else ()
	set (STLFS_LIBS "stdc++fs")
endif ()

target_link_libraries (naomi PRIVATE
	Threads::Threads
	${STLFS_LIBS}
	${CMAKE_DL_LIBS}
)

if (PROFILE_TIME)
	if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		target_compile_options (naomi PRIVATE "-ftime-report")
	elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
		target_compile_options (naomi PRIVATE "-ftime-trace")
	else ()
		message (FATAL_ERROR "Can't profile time if your compiler is unknown!")
	endif ()
endif ()
