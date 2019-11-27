from PIL import Image
import os, sys, numpy

def get_nearest_power_of_two(value):
	result = 1
	while result < value:
		result <<= 1
	return result

def get_colors_from_pixels(pixels):
	colors = []
	for pix in pixels:
		if colors.count(pix) == 0:
			colors.append(pix)
	return colors

def get_palette_from_colors(colors):
	length = get_nearest_power_of_two(len(colors))
	image = Image.new("RGBA", (length, 1), 0)
	for i in range(len(colors)):
		image.putpixel(
			(i, 0), 
			colors[i]
		)
	return image

def get_indexed_from_colors(pixels, colors, size):
	length = get_nearest_power_of_two(len(colors))
	image = Image.new("RGBA", size, 0)
	for x in range(size[0]):
		for y in range(size[1]):
			index = colors.index(pixels[y * size[0] + x])
			if index != 0:
				ratio = float(index) / float(length)
				value = int(255.0 * ratio) + 1
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

def generate(path):
	image = Image.open(path)
	pixels = list(image.getdata())
	colors = get_colors_from_pixels(pixels)
	palette_image = get_palette_from_colors(colors)
	palette_image.save(
		os.path.splitext(path)[0] + '_palette.png'
	)
	indexed_image = get_indexed_from_colors(pixels, colors, image.size)
	indexed_image.save(
		os.path.splitext(path)[0] + '_indexed.png'
	)

if __name__ == '__main__':
	syslen = len(sys.argv)
	if syslen == 0:
		print('Error! No paths given!')
	elif syslen > 1:
		for i in range(1, syslen):
			generate(sys.argv[i])
