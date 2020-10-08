#!/bin/bash

if [[ "${OSTYPE}" != "linux-gnu" ]]; then
	echo "Only cross-compiling on linux to windows is currently supported!"
	exit -1
fi

if [ ! -d "data" ] || [ ! -d "cmake" ] || [ ! -f "CMakeLists.txt" ]; then
	echo "This script needs to be run from the root of the repository!"
	exit -1
fi

if [ -z "$(which 7z)" ]; then
	echo "Install 7zip command-line tool to run this script!"
	exit -1
fi

bash "build-win32.sh --no-remove"

cd "mingw"

if [ ! -f "naomi.exe" ]; then
	echo "Something went wrong and the game didn't build!"
	exit -1
fi

cd ".."

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
