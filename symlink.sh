#!/bin/bash

if [[ "${OSTYPE}" != "linux-gnu"* ]]; then
	echo "This script is exclusive to Linux! Sorry!"
	exit -1
fi

if [ ! -d "data" ] || [ ! -d "cmake" ] || [ ! -f "CMakeLists.txt" ]; then
	echo "This script needs to be run from the root of the repository!"
	exit -1
fi

if [ -z "${XDG_DATA_HOME}" ]; then
	echo "XDG_DATA_HOME variable is not defined!"
	exit -1
fi

cd "data"

if [ ! -L "init" ]; then
	ln -s "${XDG_DATA_HOME}/studio-synao/leviathan/init" "init"
fi

if [ ! -L "save" ]; then
	ln -s "${XDG_DATA_HOME}/studio-synao/leviathan/save" "save"
fi

cd ".."

exit 0
