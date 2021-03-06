#!/usr/bin/env python3

import os

def main():
    if os.path.exists('boot.cfg'):
        print('Overwriting existing boot config file!')
        os.remove('boot.cfg')
    with open('boot.cfg', 'w') as file:
        file.write('\n[Setup]\n\n')
        file.write('MetaMenu = 0\n')
        file.write('LegacyGL = 0\n')
        file.write('Language = english\n')
        file.write('\n[Video]\n\n')
        file.write('VerticalSync = 0\n')
        file.write('Fullscreen = 0\n')
        file.write('ScaleFactor = 3\n')
        file.write('FrameLimiter = 60\n')
        file.write('\n[Audio]\n\n')
        file.write('Volume = 1\n')
        file.write('\n[Music]\n\n')
        file.write('Volume = 0.34\n')
        file.write('Channels = 2\n')
        file.write('SamplingRate = 44100\n')
        file.write('BufferedTime = 0.1\n')
        file.write('\n[Input]\n\n')
        file.write('KeyJump = 29\n')
        file.write('KeyHammer = 27\n')
        file.write('KeyItem = 225\n')
        file.write('KeyLiteDash = 4\n')
        file.write('KeyContext = 44\n')
        file.write('KeyStrafe = 224\n')
        file.write('KeyInventory = 43\n')
        file.write('KeyOptions = 41\n')
        file.write('KeyUp = 82\n')
        file.write('KeyDown = 81\n')
        file.write('KeyLeft = 80\n')
        file.write('KeyRight = 79\n')
        file.write('JoyJump = 0\n')
        file.write('JoyHammer = 1\n')
        file.write('JoyItem = 2\n')
        file.write('JoyLiteDash = 3\n')
        file.write('JoyContext = 4\n')
        file.write('JoyStrafe = 5\n')
        file.write('JoyInventory = 6\n')
        file.write('JoyOptions = 7\n')
        file.close()

if __name__ == '__main__':
    main()
