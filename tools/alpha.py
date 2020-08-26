#!/usr/bin/env python3

from PIL import Image, ImageColor
import os, sys, numpy

def fix_alpha(image_path: str):
	loaded_image: None = Image.open(image_path)
	pixel_list: list = list(loaded_image.getdata())
	result_image: Image = Image.new('RGBA', loaded_image.size, 0)
	for x in range(loaded_image.size[0]):
		for y in range(loaded_image.size[1]):
			pixel = pixel_list[x + y * loaded_image.size[0]]
			if pixel[3] != 0:
				result_image.putpixel((x, y), pixel)
	result_image.save(image_path)

def main():
	argc: int = len(sys.argv)
	if argc > 1:
		for i in range(1, argc):
			image_path: str = sys.argv[i]
			fix_alpha(image_path)
	else:
		print('Error! No paths given!')

if __name__ == '__main__':
	main()
