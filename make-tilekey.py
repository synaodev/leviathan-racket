# #!/usr/bin/env python3

# import os, sys, struct, ctypes
# import OpenGL.GL as gl
# import imgui
# import numpy

# from PIL import Image
# from typing import List, Dict
# from sdl2 import *
# from imgui.integrations.sdl2 import SDL2Renderer

# def get_tilekey_name(tileset_path: str) -> str:
#     sep_pos = tileset_path.rfind(os.pathsep) + 1
#     dot_pos = tileset_path.rfind('.')
#     return tileset_path[sep_pos : dot_pos - sep_pos] + '.attr'

# def get_tilekey_bitmasks(path: str) -> List[int]:
#     try:
#         fp = open(path, 'rb')
#     except IOError:
#         print(f'IO Error! Couldn\'t read from {path}!')
#     else:
#         result: List[int] = []
#         with open(path, 'rb') as fp:
#             while len(result) < 256:
#                 data: int = struct.unpack('i', fp.read(4))[0]
#                 result.append(data)
#         return result
#     return [0] * 256

# def set_tilekey_bitmasks(path: str, bitmasks: List[int]) -> bool:
#     try:
#         fp = open(path, 'wb')
#     except IOError:
#         print(f'IO Error! Couldn\'t write to {path}!')
#     else:
#         with open(path, 'wb') as fp:
#             for b in bitmasks:
#                 fp.write(struct.pack('i', b))
#             fp.close()
#         return True
#     return False

# class Texture():
#     def __init__(self):
#         self.valid: bool = False
#         self.dimensions: tuple = (0, 0)
#         self.pointer: int = 0
#     def __init__(self, path: str):
#         self.__init__()
#         self.load(path)
#     def __del__(self):
#         if self.valid:
#             gl.glDeleteTextures(1, [self.pointer])
#             self.valid = False
#             self.dimensions = (0, 0)
#             self.pointer = 0
#     def load(self, path: str) -> bool:
#         if self.valid:
#             gl.glDeleteTextures(1, [self.pointer])
#             self.valid = False
#             self.dimensions = (0, 0)
#             self.pointer = 0
#         image: Image.Image = Image.open(path)
#         if image != None:
#             pointer = gl.glGenTextures(1)
#             gl.glBindTexture(gl.GL_TEXTURE_2D, pointer)
#             gl.glTexImage2D(
#                 gl.GL_TEXTURE_2D, 0,
#                 gl.GL_RGBA8,
#                 image.size[0],
#                 image.size[1],
#                 0, gl.GL_RGBA,
#                 gl.GL_UNSIGNED_BYTE,
#                 image.tostring('raw', 'RBGA', 0, -1)
#             )
#             gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_NEAREST)
#             gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_NEAREST)
#             gl.glBindTexture(gl.GL_TEXTURE_2D, 0)
#             self.valid = True
#             self.dimensions = image.size
#             self.pointer = pointer
#         return self.valid

# class InputCtl():
#     def __init__(self):
#         self.quitting: bool = False
#         self.clicked: bool = False
#         self.pressed: Dict[SDL_Event, bool] = {}

# class TilesetCtl():
#     def __init__(self):
#         self.amend: bool = False
#         self.selected: bool = False
#         self.cursor: tuple = (0.0, 0.0, 16.0, 16.0)
#         self.index: int = 0
#         self.texture: Texture = Texture()
#         self.bitmasks: List[int] = [0] * 256
#         self.tileset_path: str = ''
#     def reset(self):
#         self.amend = True
#         self.selected = False
#         self.index = 0
#         for mask in self.bitmasks:
#             mask = 0
#     def load(self, tileset_path: str) -> bool:
#         self.amend = True
#         self.selected = False
#         if self.tileset_path != tileset_path:
#             if not self.texture.load(tileset_path):
#                 return False
#             self.bitmasks = get_tilekey_bitmasks(os.path.join('data', 'tilekey', get_tilekey_name(tileset_path)))
#             if len(self.bitmasks) < 256:
#                 return False
#             self.tileset_path = tileset_path
#             print('Loading successful!')
#             return True
#         return False
#     def save(self):
#         self.selected = False
#         if len(self.tileset_path) > 0:
#             return set_tilekey_bitmasks(os.path.join('data', 'tilekey', get_tilekey_name(self.tileset_path)))
#         print('Error! Nothing to save currently!')
#         return False
#     def handle(self):
#         if len(self.tileset_path) == 0 or not self.texture.valid:
#             self.amend = True
#             self.selected = False
#             self.index = 0
#             return
#         pass

# class AttributeCtrl():
#     def __init__(self):
#         self.enable: bool = False
#         self.save: bool = False
#         self.load: bool = False
#         self.index: int = 0
#         self.files: List[str] = []
#     def reset(self):
#         self.enable = True
#         self.save = False
#         self.load = False
#         self.index = 0
#         self.files.clear()
#     def active(self) -> bool:
#         return self.load or self.save
#     def handle(self, tileset_ctl: TilesetCtl):
#         imgui.begin_main_menu_bar()
#         if imgui.begin_menu('File'):
#             if imgui.menu_item('Load', 'Ctrl+L', False, self.enable):
#                 self.enable = False
#                 self.load = True
#                 self.save = False
#             elif imgui.menu_item('Save', 'Ctrl+S', False, self.enable):
#                 self.enable = False
#                 self.load = False
#                 self.save = True
#             elif imgui.menu_item('Clear', 'Ctrl+Q', False, self.enable):
#                 self.enable = False
#                 self.load = True
#                 self.save = False
#                 self.index = 0
#                 tileset_ctl.reset()
#             imgui.end_menu()
#         imgui.end_main_menu_bar()
#         if self.load:
#             imgui.begin('File Dialog')
#             if len(self.files) == 0:
#                 self.files = os.listdir(os.path.join('data', 'tilekey'))
#             status: tuple = imgui.listbox('Files', self.index, self.files)
#             self.index = status.current
#             if imgui.button('Load'):
#                 tileset_ctl.load(self.files[self.index])
#                 self.reset()
#             elif imgui.button('Clear'):
#                 self.reset()
#             imgui.end()
#         elif self.save:
#             tileset_ctl.save()
#             self.reset()

# def loop() -> int:
#     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
#     SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)
#     SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
#     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
#     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
#     SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)
#     window = SDL_CreateWindow(
#         'Tileset Editor'.encode('utf-8'),
#         SDL_WINDOWPOS_CENTERED,
#         SDL_WINDOWPOS_CENTERED,
#         800, 800,
#         SDL_WINDOW_OPENGL
#     )
#     if window is None:
#         print('Error! Couldn\'t create window!')
#         SDL_Quit()
#         return -1
#     context = SDL_GL_CreateContext(window)
#     if context is None:
#         print('Error! Couldn\'t create context!')
#         SDL_DestroyWindow(window)
#         SDL_Quit()
#         return -1
#     SDL_GL_MakeCurrent(window, context)
#     if SDL_GL_SetSwapInterval(1) < 0:
#         print('Error! Couldn\'t activate vsync!')
#         SDL_DestroyWindow(window)
#         SDL_GL_DeleteContext(context)
#         SDL_Quit()
#         return -1
#     imgui.create_context()
#     impl = SDL2Renderer(window)
#     running = True
#     event = SDL_Event()
#     while running:
#         while SDL_PollEvent(ctypes.byref(event)) != 0:
#             if event.type == SDL_QUIT:
#                 running = False
#                 break
#             impl.process_event(event)
#         impl.process_inputs()
#         imgui.new_frame()
#         # Run Everything
#         gl.glClearColor(1.0, 1.0, 1.0, 1.0)
#         gl.glClear(gl.GL_COLOR_BUFFER_BIT)
#         imgui.render()
#         impl.render(imgui.get_draw_data())
#         SDL_GL_SwapWindow(window)
#     impl.shutdown()
#     SDL_GL_DeleteContext(context)
#     SDL_DestroyWindow(window)
#     SDL_Quit()
#     return 0

# def main() -> int:
#     working_directory: str = os.getcwd()
#     if len(sys.argv) > 1:
#         working_directory = os.path.abspath(sys.argv[1])
#         os.chdir(working_directory)
#     if not os.path.exists(os.path.join(working_directory, 'data', 'tilekey')):
#         print('Error! Couldn\'t find "data/tilekey"!')
#         return -1
#     if not os.path.exists(os.path.join(working_directory, 'data', 'image')):
#         print('Error! Couldn\'t find "data/image"!')
#         return -1
#     if SDL_Init(SDL_INIT_EVERYTHING) < 0:
#         print('Error! SDL failed to initialize!')
#         return -1
#     return loop()

# if __name__ == '__main__':
#     sys.exit(main())
