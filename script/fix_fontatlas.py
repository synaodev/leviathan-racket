#!/usr/bin/env python3

from PIL import Image, ImageColor
import os, sys, numpy

def regenerate_transparency(font_image_path: str):
	font_image: None = Image.open(font_image_path)
	pixel_list: list = list(font_image.getdata())
	result_image: Image = Image.new('RGBA', font_image.size, 0)
	for x in range(font_image.size[0]):
		for y in range(font_image.size[1]):
			pixel = pixel_list[x + y * font_image.size[0]]
			if pixel[3] != 0:
				result_image.putpixel((x, y), pixel)
	result_image.save(font_image_path)

def main():
	argc: int = len(sys.argv)
	if argc >= 1:
		for i in range(1, argc):
			font_image_path: str = sys.argv[i]
			regenerate_transparency(font_image_path)
	else:
		print('Error! No paths given!')

if __name__ == '__main__':
	main()
