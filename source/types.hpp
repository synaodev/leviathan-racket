#ifndef LEVIATHAN_INCLUDED_TYPES_HPP
#define LEVIATHAN_INCLUDED_TYPES_HPP

#if defined(_WIN32) || defined(__MINGW32__)
	#define LEVIATHAN_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#include <TargetConditionals.h>
	#if defined(TARGET_OS_MAC)
		#define LEVIATHAN_PLATFORM_MACOS
		#define LEVIATHAN_POSIX_COMPLIANT
	#else
		#error "Apple platform is not supported!"
	#endif
#elif defined(__unix__)
	#define LEVIATHAN_POSIX_COMPLIANT
	#if defined(__linux__)
		#define LEVIATHAN_PLATFORM_LINUX
	#elif defined(__bsdi__)
		#if defined(__FreeBSD__)
			#define LEVIATHAN_PLATFORM_FREEBSD
		#elif defined(__OpenBSD__)
			#define LEVIATHAN_PLATFORM_OPENBSD
		#elif defined(__NetBSD__)
			#define LEVIATHAN_PLATFORM_NETBSD
		#elif defined(__DragonFly__)
			#define LEVIATHAN_PLATFORM_DRAGONFLY
		#else
			#error "Unknown BSD platform is not supported!"
		#endif
	#elif defined(__CYGWIN__)
		#error "Cygwin is not supported!"
	#else
		#error "Unknown unix-like platform is not supported!"
	#endif
#else
	#error "Unknown platform is not supported!"
#endif

#if defined(__clang__)
	#define LEVIATHAN_COMPILER_LLVM
	#if defined(_MSC_VER)
		#define LEVIATHAN_TOOLCHAIN_MSVC
	#elif defined(__apple_build_version__)
		#define LEVIATHAN_TOOLCHAIN_APPLECLANG
	#else
		#define LEVIATHAN_TOOLCHAIN_LLVM
	#endif
#elif defined(__GNUC__)
	#define LEVIATHAN_COMPILER_GNUC
	#if defined(__MINGW32__)
		#define LEVIATHAN_TOOLCHAIN_MINGW
	#else
		#define LEVIATHAN_TOOLCHAIN_GNUC
	#endif
#elif defined(_MSC_VER)
	#define LEVIATHAN_COMPILER_MSVC
	#define LEVIATHAN_TOOLCHAIN_MSVC
#else
	#error "Target compiler and toolchain are not supported!"
#endif

#if defined(LEVIATHAN_PLATFORM_WINDOWS)
	#if defined(_WIN64)
		#define LEVIATHAN_MACHINE_64BIT
	#else
		#define LEVIATHAN_MACHINE_32BIT
	#endif
#elif defined(LEVIATHAN_PLATFORM_MACOS)
	#define LEVIATHAN_MACHINE_64BIT
#elif defined(LEVIATHAN_COMPILER_GNUC) || defined(LEVIATHAN_COMPILER_LLVM)
	#if __x86_64__ || __ppc64__
		#define LEVIATHAN_MACHINE_64BIT
	#else
		#define LEVIATHAN_MACHINE_32BIT
	#endif
#else
	#error "Can't determine if architecture is 32-bit or 64-bit!"
#endif

#if !defined(NDEBUG)
	#define LEVIATHAN_BUILD_DEBUG
#endif

#include <glm/fwd.hpp>

using sint8_t  = signed char;
using bool16_t = short;
using sint16_t = signed short;
using bool32_t = int;
using sint32_t = signed int;
using real32_t = float;
using bool64_t = long long;
using sint64_t = signed long long;
using real64_t = double;

using byte_t = char;
using bool_t = int;
using sint_t = signed int;
using uint_t = unsigned int;
using real_t = float;
using void_t = void*;

#ifdef LEVIATHAN_MACHINE_64BIT
	using arch_t = uint64_t;
#else
	using arch_t = uint32_t;
#endif

struct not_copyable_t {
public:
	not_copyable_t() = default;
	not_copyable_t(not_copyable_t&&) = default;
	not_copyable_t& operator=(not_copyable_t&&) = default;
	not_copyable_t(const not_copyable_t&) = delete;
	not_copyable_t& operator=(const not_copyable_t&) = delete;
};

#endif // LEVIATHAN_INCLUDED_TYPES_HPP
