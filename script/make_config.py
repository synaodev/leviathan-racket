#!/usr/bin/env python3

import os, sys

def main():
	if len(sys.argv) > 3:
		print('Error! Too many arguments!')
		return
	force_generate: bool = False
	into_directory: str = ''
	for i in range(0, len(sys.argv)):
		if force_generate and len(into_directory) > 0:
			break
		elif sys.argv[i] == '--force':
			force_generate = True
		elif i > 1 and len(sys.argv[i]) != 0:
			into_directory = sys.argv[i]
	config_path: str = os.path.abspath(os.path.join(os.path.curdir, os.path.pardir))
	if len(into_directory) != 0:
		config_path = os.path.join(config_path, into_directory)
	config_file: str = os.path.join(config_path, 'boot.cfg')
	if os.path.exists(config_file) and not force_generate:
		if os.path.isfile(config_file):
			print('Configuration file already exists! Run this script again with \'--force\' to regenerate the configuration file.')
		else:
			print(f'Error! There is already a filesystem object at {config_file} which is not a file! Please remove/rename it try again...')
		return
	elif os.path.exists(config_file) and force_generate:
		os.remove(config_file)
	if not os.path.exists(config_path):
		os.mkdir(config_path, 493) # 0775
	with open(config_file, 'x') as file:
		file.write('\n[Setup]\n\n')
		file.write('Language = english\n')
		file.write('\n[Video]\n\n')
		file.write('VerticalSync = 1\n')
		file.write('Fullscreen = 0\n')
		file.write('ScaleFactor = 3\n')
		file.write('FrameLimiter = 60\n')
		file.write('UseOpenGL4 = 1\n')
		file.write('\n[Audio]\n\n')
		file.write('Volume = 1\n')
		file.write('\n[Music]\n\n')
		file.write('Volume = 0.34\n')
		file.write('Channels = 2\n')
		file.write('SamplingRate = 44100\n')
		file.write('kBufferedTime = 0.1\n')
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
	print(f'Successfully created file {config_file}!')

if __name__ == '__main__':
	main()