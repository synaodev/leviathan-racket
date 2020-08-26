cmake_minimum_required (VERSION 3.15)

find_package (Threads REQUIRED)

target_link_libraries (naomi PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
target_link_libraries (editor PRIVATE Threads::Threads ${CMAKE_DL_LIBS})

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
	target_link_libraries (naomi PRIVATE stdc++fs)
	target_link_libraries (editor PRIVATE stdc++fs)
endif ()
