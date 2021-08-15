#!/usr/bin/env python3

import os, sys

def remove_link(directory: str) -> bool:
    if os.path.exists(directory):
        if os.path.islink(directory):
            os.remove(directory)
            return True
        else:
            print(f'Error! \"{directory}\" already exists and is not a link!')
            return False
    return True

def ensure_dir(directory: str) -> bool:
    if os.path.exists(directory):
        if not os.path.isdir(directory):
            print(f'Error! \"{directory}\" isn\'t a directory! Remove it and create the directory manually, or generate it using the game!')
            return False
        return True
    print(f'Error! \"{directory}\" doesn\'t exist! Create the directory manually, or generate it using the game!')
    return False

def get_base_directory() -> str:
    if os.name == 'nt':
        return os.environ.get('AppData', '')
    return os.environ.get('XDG_DATA_HOME', '')

def main() -> int:
    curr_dir: str = os.curdir
    if not os.path.isdir(os.path.join(curr_dir, 'data')) or not os.path.isdir(os.path.join(curr_dir, 'cmake')) or not os.path.isfile(os.path.join(curr_dir, 'CMakeLists.txt')):
        print('Error! This script needs to be run from the root of the repository!')
        return -1
    init_link: str = os.path.join(curr_dir, 'data', 'init')
    if not remove_link(init_link):
        return -1
    save_link: str = os.path.join(curr_dir, 'data', 'save')
    if not remove_link(save_link):
        return -1
    base_dir: str = get_base_directory()
    if len(base_dir) == 0:
        print('Error! XDG_DATA_HOME environment variable is not defined!')
        return -1
    init_dir: str = os.path.join(base_dir, 'studio-synao', 'leviathan', 'init')
    if not ensure_dir(init_dir):
        return -1
    save_dir: str = os.path.join(base_dir, 'studio-synao', 'leviathan', 'save')
    if not ensure_dir(save_dir):
        return -1
    os.symlink(init_dir, init_link, True)
    os.symlink(save_dir, save_link, True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
