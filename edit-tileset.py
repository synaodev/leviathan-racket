#!/usr/bin/env python3

import os, sys, ctypes
import OpenGL.GL as gl
import imgui

from typing import List
from sdl2 import *
from imgui.integrations.sdl2 import SDL2Renderer

def get_tileset_name(attribute_path: str) -> str:
    sep_pos = attribute_path.rfind(os.pathsep) + 1
    dot_pos = attribute_path.rfind('.')
    return attribute_path[sep_pos : dot_pos - sep_pos] + '.png'

def get_tilekey_list() -> List[str]:
    pass

class AttributeCtrl():
    def __init__(self):
        self.enable: bool = True
        self.save: bool = False
        self.load: bool = False
        self.index: int = 0
        self.files: List[str] = []
    def handle(self):
        imgui.begin_main_menu_bar()
        if imgui.begin_menu('File'):
            if imgui.menu_item('Load', 'Ctrl+L', False, self.enable):
                self.enable = False
                self.load = True
                self.save = False
            elif imgui.menu_item('Save', 'Ctrl+S', False, self.enable):
                self.enable = False
                self.load = False
                self.save = True
            elif imgui.menu_item('Clear', 'Ctrl+Q', False, self.enable):
                self.enable = False
                self.load = True
                self.save = False
                self.index = 0
                # tileset_viewer.reset()
            imgui.end_menu()
        imgui.end_main_menu_bar()
        if self.load:
            imgui.begin('File Dialog')
            if len(self.files) == 0:
                pass

def loop() -> int:
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)
    window = SDL_CreateWindow(
        'Tileset Editor'.encode('utf-8'),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 800,
        SDL_WINDOW_OPENGL
    )
    if window is None:
        print('Error! Couldn\'t create window!')
        SDL_Quit()
        return -1
    context = SDL_GL_CreateContext(window)
    if context is None:
        print('Error! Couldn\'t create context!')
        SDL_DestroyWindow(window)
        SDL_Quit()
        return -1
    SDL_GL_MakeCurrent(window, context)
    if SDL_GL_SetSwapInterval(1) < 0:
        print('Error! Couldn\'t activate vsync!')
        SDL_DestroyWindow(window)
        SDL_GL_DeleteContext(context)
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
        # Run Everything
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
    return loop()

if __name__ == '__main__':
    sys.exit(main())
