cmake_minimum_required (VERSION 3.15)

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

target_link_libraries (editor PRIVATE
	Threads::Threads
	${STLFS_LIBS}
	${CMAKE_DL_LIBS}
)

if (GOLD_BUILD)
	if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND NOT LLVM_USE_LINKER)
		message ("Warning! Compiler is clang, so pass \"-DLLVM_USE_LINKER=gold\" when configuring!")
		set (LLVM_USE_LINKER "gold")
	endif ()
	execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
	if ("${ld_version}" MATCHES "GNU gold")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
	endif()
endif ()

if (NOT RTTI_BUILD)
	target_compile_options (naomi PRIVATE "-fno-rtti")
	target_compile_options (editor PRIVATE "-fno-rtti")
endif ()
