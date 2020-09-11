#ifndef LEVIATHAN_INCLUDED_SYSTEM_MAIN_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_MAIN_HPP

#include "../types.hpp"

#ifdef LEVIATHAN_DEFINES_VERSION
	#include <leviathan-racket.hpp>
#else
	#include "../version.inc"
#endif

namespace version_information {
	const byte_t platform[] =
#if defined(LEVIATHAN_PLATFORM_WINDOWS)
		"Windows"
#elif defined(LEVIATHAN_PLATFORM_MACOS)
		"MacOS"
#elif defined(LEVIATHAN_PLATFORM_LINUX)
		"Linux"
#elif defined(LEVIATHAN_PLATFORM_FREEBSD)
		"FreeBSD"
#elif defined(LEVIATHAN_PLATFORM_OPENBSD)
		"OpenBSD"
#elif defined(LEVIATHAN_PLATFORM_NETBSD)
		"NetBSD"
#elif defined(LEVIATHAN_PLATFORM_DRAGONFLY)
		"Dragonfly"
#endif
	;
	const byte_t architecture[] =
#if defined(LEVIATHAN_MACHINE_64BIT)
		"64-bit"
#elif defined(LEVIATHAN_MACHINE_32BIT)
		"32-bit"
#endif
	;
	const byte_t compiler[] =
#if defined(LEVIATHAN_COMPILER_MSVC)
		"MSVC"
#elif defined(LEVIATHAN_COMPILER_LLVM)
		"Clang"
#elif defined(LEVIATHAN_COMPILER_GNUC)
		"GCC"
#endif
	;
	const byte_t toolchain[] =
#if defined(LEVIATHAN_TOOLCHAIN_LLVM)
		"LLVM"
#elif defined(LEVIATHAN_TOOLCHAIN_APPLECLANG)
		"Apple's"
#elif defined(LEVIATHAN_TOOLCHAIN_GNUC)
		"GNU"
#elif defined(LEVIATHAN_TOOLCHAIN_MINGW)
		"MinGW"
#elif defined(LEVIATHAN_TOOLCHAIN_MSVC)
		"MSBuild"
#endif
	;
	const byte_t build_type[] =
#if defined(LEVIATHAN_BUILD_DEBUG)
		"Debug"
#else
		"Release"
#endif
	;
	const byte_t executable[] =
#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
		"Naomi"
#elif defined(LEVIATHAN_EXECUTABLE_EDITOR)
		"Editor"
#else
		"Unknown"
#endif
	;
}

#endif // LEVIATHAN_INCLUDED_SYSTEM_MAIN_HPP
