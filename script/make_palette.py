#!/usr/bin/env python3

from PIL import Image
import os, sys, numpy

def get_nearest_power_of_two(value: int) -> int:
	result: int = 1
	while result < value:
		result <<= 1
	return result

def get_colors_from_pixels(pixels: list) -> list:
	colors: list = []
	for pix in pixels:
		if colors.count(pix) == 0:
			colors.append(pix)
	return colors

def get_palette_from_colors(colors: list) -> Image:
	length: int = get_nearest_power_of_two(len(colors))
	image: Image = Image.new('RGBA', (length, 1), 0)
	for i in range(len(colors)):
		image.putpixel(
			(i, 0), 
			colors[i]
		)
	return image

def get_indexed_from_colors(pixels: list, colors: list, size: tuple) -> Image:
	length: int = get_nearest_power_of_two(len(colors))
	image: Image = Image.new('RGBA', size, 0)
	for x in range(size[0]):
		for y in range(size[1]):
			index: int = colors.index(pixels[y * size[0] + x])
			if index != 0:
				ratio: float = float(index) / float(length)
				value: int = int(255.0 * ratio) + 1
				image.putpixel(
					(x, y),
					(value, value, value, 255)
				)
			else:
				image.putpixel(
					(x, y), 
					(0, 0, 0, 0)
				)
	return image

def make_palette_image(initial_path: str, colors: list):
	palette_image: Image = get_palette_from_colors(colors)
	palette_path: str = os.path.splitext(initial_path)[0] + '_palette.png'
	palette_image.save(palette_path)

def make_indexed_image(initial_path: str, pixels: list, colors: list, size: tuple):
	indexed_image: Image = get_indexed_from_colors(pixels, colors, size)
	indexed_path: str = os.path.splitext(initial_path)[0] + '_indexed.png'
	indexed_image.save(indexed_path)

def main():
	argc: int = len(sys.argv)
	if argc >= 1:
		for i in range(1, argc):
			initial_path: str = sys.argv[i]
			initial_image: None = Image.open(initial_path)
			pixel_list: list = list(initial_image.getdata())
			color_list: list = get_colors_from_pixels(pixel_list)
			make_palette_image(initial_path, color_list)
			make_indexed_image(initial_path, pixel_list, color_list, initial_image.size)
	else:
		print('Error! No paths given!')

if __name__ == '__main__':
	main()
