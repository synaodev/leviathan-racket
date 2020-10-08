#!/usr/bin/env python3

import os, sys, ctypes
import OpenGL.GL as gl
import imgui

from sdl2 import *
from imgui.integrations.sdl2 import SDL2Renderer

def get_tileset_name(attribute_path: str) -> str:
    sep_pos = attribute_path.rfind(os.pathsep) + 1
    dot_pos = attribute_path.rfind('.')
    return attribute_path[sep_pos : dot_pos - sep_pos] + '.png'

def init():
    dimensions = 800
    name = 'Attribute Editor'
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)
    window = SDL_CreateWindow(
        name.encode('utf-8'),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        dimensions, dimensions,
        SDL_WINDOW_OPENGL
    )
    if window is None:
        print('Error! Couldn\'t create window!')
        return None, None
    context = SDL_GL_CreateContext(window)
    if context is None:
        print('Error! Couldn\'t create context!')
        SDL_DestroyWindow(window)
        return None, None
    SDL_GL_MakeCurrent(window, context)
    if SDL_GL_SetSwapInterval(1) < 0:
        print('Error! Couldn\'t activate vsync!')
        SDL_DestroyWindow(window)
        SDL_GL_DeleteContext(context)
        return None, None
    return window, context

def loop(working_directory: str) -> int:
    window, context = init()
    if window is None or context is None:
        SDL_Quit()
        return -1
    imgui.create_context()
    impl = SDL2Renderer(window)
    running = True
    event = SDL_Event()
    while running:
        while SDL_PollEvent(ctypes.byref(event)) != 0:
            if event.type == SDL_QUIT:
                running = False
                break
            impl.process_event(event)
        impl.process_inputs()
        imgui.new_frame()
        if imgui.begin_main_menu_bar():
            if imgui.begin_menu("File", True):
                clicked_quit, selected_quit = imgui.menu_item(
                    "Quit", 'Cmd+Q', False, True
                )
                if clicked_quit:
                    running = False
                imgui.end_menu()
            imgui.end_main_menu_bar()
        gl.glClearColor(1.0, 1.0, 1.0, 1.0)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        imgui.render()
        impl.render(imgui.get_draw_data())
        SDL_GL_SwapWindow(window)
    impl.shutdown()
    SDL_GL_DeleteContext(context)
    SDL_DestroyWindow(window)
    SDL_Quit()
    return 0

def main() -> int:
    working_directory: str = os.getcwd()
    if len(sys.argv) > 1:
        working_directory = os.path.abspath(sys.argv[1])
        os.chdir(working_directory)
    if not os.path.exists(os.path.join(working_directory, 'data', 'tilekey')):
        print('Error! Couldn\'t find "data/tilekey"!')
        return -1
    if not os.path.exists(os.path.join(working_directory, 'data', 'image')):
        print('Error! Couldn\'t find "data/image"!')
        return -1
    if SDL_Init(SDL_INIT_EVERYTHING) < 0:
        print('Error! SDL failed to initialize!')
        return -1
    return loop(working_directory)

if __name__ == '__main__':
    sys.exit(main())
