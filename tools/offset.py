#!/usr/bin/env python3

import sys, json

def make_handle(path: str) -> any:
    with open(path, 'r') as file:
        handle = json.load(file)
        return handle
    return None

def save_handle(path: str, handle: any) -> bool:
    with open(path, 'w+') as file:
        json.dump(handle, file, indent=2)
        file.close()
        return True
    return False

def main():
    path: str = sys.argv[1]
    offset_x: int = int(sys.argv[2])
    offset_y: int = int(sys.argv[3])
    handle: any = make_handle(path)
    chars = handle['font']['chars']['char']
    for char in chars:
        char['-x'] = str(int(char['-x']) + offset_x)
        char['-y'] = str(int(char['-y']) + offset_y)
    if not save_handle(path, handle):
        print(f'Couldn\'t save file {path}!')

if __name__ == '__main__':
    main()
