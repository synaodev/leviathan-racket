#!/bin/sh

if [ "$1" = "${CMAKE_C_COMPILER}" ]; then
    shift
fi

export CCACHE_CPP2=true
exec "${C_LAUNCHER}" "${CMAKE_C_COMPILER}" "$@"
