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

if [ -z "$(which 7z)" ]; then
	echo "Can't find 7zip!"
	exit -1
fi

pack="false"
build_type="Debug"
remove_dir="true"

for arg in "$@"
do
	if [[ $arg == "--pack" ]]; then
		pack="true"
		build_type="Release"
		remove_dir="false"
		break
	elif [[ $arg == "--release" ]]; then
		build_type="Release"
	elif [[ $arg == "--no-remove" ]]; then
		remove_dir="false"
	fi
done

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

if [[ "${remove_dir}" == "true" ]]; then
	if [ -d "mingw" ]; then
		rm -r "mingw"
	fi
fi

if [ ! -d "mingw" ]; then
	mkdir "mingw"
fi

cd "mingw"

cmake ".." -DCMAKE_BUILD_TYPE="${build_type}" -DCMAKE_C_COMPILER="$(which ${c_compiler})" -DCMAKE_CXX_COMPILER="$(which ${cxx_compiler})" -DCMAKE_TOOLCHAIN_FILE="${vcpkg_root}/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x64-mingw-dynamic" -DSTDAFX_BUILD="OFF" -DMETA_BUILD="OFF"

cp "/lib/gcc/x86_64-w64-mingw32/${compiler_version}/libgcc_s_seh-1.dll" .
cp "/lib/gcc/x86_64-w64-mingw32/${compiler_version}/libstdc++-6.dll" .
cp "/usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll" .

if [[ "${build_type}" == "Debug" ]]; then
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/libangelscript.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/OpenAL32.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/debug/bin/SDL2d.dll" .
else
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/libangelscript.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/OpenAL32.dll" .
	cp "${vcpkg_root}/installed/x64-mingw-dynamic/bin/SDL2.dll" .
fi

make -j20

if [ ! -f "naomi.exe" ]; then
	echo "Something went wrong and the game didn't build!"
	exit -1
fi

cd ".."

if [[ ${pack} != "true" ]]; then
	exit 0
fi

if [ -d "leviathan" ]; then
	rm -rf "leviathan/"
fi

mkdir "leviathan" && cd "leviathan"

cp "../mingw/naomi.exe" .
cp "../mingw/libgcc_s_seh-1.dll" .
cp "../mingw/libstdc++-6.dll" .
cp "../mingw/libwinpthread-1.dll" .
cp "../mingw/libangelscript.dll" .
cp "../mingw/libtmxlite.dll" .
cp "../mingw/OpenAL32.dll" .
cp "../mingw/SDL2.dll" .

cd "../data"

if [ -L "init" ]; then
	mv "init" "../../temp-init"
fi

if [ -L "save" ]; then
	mv "save" "../../temp-save"
fi

cd "../leviathan"

cp -r "../data" .

cd ".."

7z a "leviathan.7z" "leviathan/"

rm -rf "leviathan"

if [ -L "../temp-init" ]; then
	mv "../temp-init" "data/init"
fi

if [ -L "../temp-save" ]; then
	mv "../temp-save" "data/save"
fi

exit 0
