#!/bin/bash

if [[ "${OSTYPE}" != "linux-gnu"* ]]; then
	echo "Only cross-compiling on linux to windows is currently supported!"
	exit -1
fi

if [ ! -d "data" ] || [ ! -d "cmake" ] || [ ! -f "CMakeLists.txt" ]; then
	echo "This script needs to be run from the root of the repository!"
	exit -1
fi

if [ -z "$(which cmake)" ]; then
	echo "Can't find cmake!"
	exit -1
fi

build_type="Release"

if [[ $1 == "--debug" ]]; then
	build_type="Debug"
fi

c_compiler="x86_64-w64-mingw32-gcc-posix"
cxx_compiler="x86_64-w64-mingw32-g++-posix"

if [ -z "$(which ${c_compiler})" ] || [ -z "$(which ${cxx_compiler})" ]; then
	echo "Can't find mingw compiler on the PATH!"
	exit -1
fi

vcpkg_root="$(find ${HOME} -name ".vcpkg-root" | cut -d'.' -f 1 | cut -d'/' -f 1,2,3,4)"

if [ -z "${vcpkg_root}" ]; then
	echo "Can't find vcpkg root directory!"
	exit -1
fi

if [[ "${build_type}" == "Debug" ]]; then
	if [ ! -d "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin" ]; then
		echo "Can't find mingw game dependencies!"
		exit -1
	fi
else
	if [ ! -d "${vcpkg_root}/installed/x64-mingw-dynamic/bin" ]; then
		echo "Can't find mingw game dependencies!"
		exit -1
	fi
fi

dummy_output="${c_compiler} (GCC) "
dummy_date=" 00000000"
compiler_version="$(${c_compiler} --version | grep posix | cut -c $(($(echo ${#dummy_output}) + 1))- | cut -c -${#dummy_date})"

if [ -d "mingw" ]; then
	rm -rf "mingw"
fi

mkdir "mingw"
cd "mingw"

cmake ".." -DCMAKE_BUILD_TYPE="${build_type}" -DCMAKE_C_COMPILER="$(which ${c_compiler})" -DCMAKE_CXX_COMPILER="$(which ${cxx_compiler})" -DCMAKE_TOOLCHAIN_FILE="${vcpkg_root}/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x64-mingw-dynamic" -DSTDAFX_BUILD:BOOL="OFF" -DCCACHE_BUILD:BOOL="OFF" -DGOLD_BUILD:BOOL="OFF" -DRTTI_BUILD:BOOL="ON" -DMETA_BUILD:BOOL="ON" -DNAOMI_BUILD:BOOL="ON" -DEDITOR_BUILD:BOOL="ON"

cp "/lib/gcc/x86_64-w64-mingw32/${compiler_version}/libgcc_s_seh-1.dll" .
cp "/lib/gcc/x86_64-w64-mingw32/${compiler_version}/libstdc++-6.dll" .
cp "/usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll" .

if [[ "${build_type}" == "Debug" ]]; then
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/libangelscript.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/libtmxlite-d.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/OpenAL32.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/SDL2d.dll" .
else
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/libangelscript.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/libtmxlite.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/OpenAL32.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/SDL2.dll" .
fi

cd ".."

exit 0
