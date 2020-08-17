cmake_minimum_required (VERSION 3.15)

target_compile_definitions (leviathan PRIVATE
	"-DNOMINMAX"
	"-DUNICODE"
	"-D_UNICODE"
	"-DWIN32_LEAN_AND_MEAN"
	"-D_WIN32_WINNT=0x0602"
	"-D_SCL_SECURE_NO_WARNINGS"
	"-D_CRT_SECURE_NO_WARNINGS"
	"-D_CRT_SECURE_NO_DEPRECATE"
	"-D_CRT_NONSTDC_NO_WARNINGS"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_definitions (leviathan PRIVATE
		"/sdl"	  # Enable security checks
		"/utf-8"  # UTF-8 source files
		"/WX"	  # Treat linker warnings as errors
	)
elseif (MINGW)
	find_package (Threads REQUIRED)
	target_link_libraries (leviathan PRIVATE
		Threads::Threads
		stdc++fs
	)
elseif (MSVC)
	# From http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
	foreach (flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE MAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		string (REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
	endforeach ()
	target_compile_definitions (leviathan PRIVATE
		"/sdl"	  # Enable security checks
		"/MP"	  # Multi-processor compilation
		"/utf-8"  # UTF-8 source files
		"/WX"	  # Treat linker warnings as errors
		"/wd4201" # nonstandard extension used : nameless struct/union
		"/wd4127" # conditional expression is constant
		"/wd4100" # 'identifier' : unreferenced formal parameter
		"/wd4200" # InputCommon fix temp.
		"/wd4244" # 'conversion' conversion from 'type1' to 'type2', possible loss of data
		"/wd4121" # 'symbol' : alignment of a member was sensitive to packing
		"/wd4324" # Padding was added at the end of a structure because you specified a __declspec(align) value.
		"/wd4714" # function 'function' marked as __forceinline not inlined
		"/wd4351" # new behavior: elements of array 'array' will be default initialized
		"/wd4245" # conversion from 'type1' to 'type2', signed/unsigned mismatch
		"/wd4458" # declaration of 'identifier' hides class member
		"/wd4706" # assignment within conditional expression
		"/wd26812"# Prefer 'enum class' over 'enum'
		"/w44263" # Non-virtual member function hides base class virtual function
		"/w44265" # Class has virtual functions, but destructor is not virtual
	)
else ()
	message (FATAL_ERROR "Undefined Windows toolchain!")
endif ()
