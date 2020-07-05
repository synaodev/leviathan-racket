cmake_minimum_required (VERSION 3.15)

project (LEVIATHAN_RACKET_WEBAPP)

set (LEVIATHAN_RACKET_VERSION_MAJOR 0)
set (LEVIATHAN_RACKET_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (leviathan)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (Angelscript_LIBRARIES "${CMAKE_SOURCE_DIR}/webdev/lib/libangelscript-d.a")
    set (Tmxlite_LIBRARIES "${CMAKE_SOURCE_DIR}/webdev/lib/libtmxlite-d.a")
else ()
    set (Angelscript_LIBRARIES "${CMAKE_SOURCE_DIR}/webdev/lib/libangelscript.a")
    set (Tmxlite_LIBRARIES "${CMAKE_SOURCE_DIR}/webdev/lib/libtmxlite.a")
endif ()

target_include_directories (leviathan PRIVATE
    "${CMAKE_SOURCE_DIR}/webdev/include"
)

target_link_libraries (leviathan 
    "-s USE_SDL=2"
    "-s FULL_ES3=1"
    "--preload-file ../event/"
    "--preload-file ../field/"
    "--preload-file ../font/"
    "--preload-file ../i18n/"
    "--preload-file ../image/"
    "--preload-file ../noise/"
    "--preload-file ../palette/"
    "--preload-file ../pxtone/"
    "--preload-file ../sprite/"
    "--preload-file ../tilekey/"
    "--preload-file ../init/"
    "${Angelscript_LIBRARIES}"
    "${Tmxlite_LIBRARIES}"
    "-lopenal"
    "-o leviathan.html"
)
target_compile_options (leviathan PUBLIC "-O2")
target_link_libraries (leviathan "-O2")
