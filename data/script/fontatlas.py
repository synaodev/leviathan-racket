from PIL import Image, ImageColor
import os, sys, numpy

def generate(path):
	image = Image.open(path)
	pixels = list(image.getdata())
	result = Image.new("RGBA", image.size, 0)
	for x in range(image.size[0]):
		for y in range(image.size[1]):
			pixel = pixels[x + y * image.size[0]]
			if pixel[3] != 0:
				result.putpixel((x, y), pixel)
	result.save(path)

if __name__ == '__main__':
	syslen = len(sys.args)
	if syslen == 0:
		print('Error! No paths given!')
	elif syslen > 1:
		for i in range(1, syslen):
			generate(sys.argv[i])
