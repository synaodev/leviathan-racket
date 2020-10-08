#!/usr/bin/env python3

from PIL import Image
import numpy
from typing import List
import os, sys

def get_nearest_power_of_two(value: int) -> int:
    result: int = 1
    while result < value:
        result <<= 1
    return result

def get_colors_from_pixels(pixels: list, limit: int) -> list:
    colors: list = []
    for pix in pixels:
        count: int = colors.count(pix)
        if limit > 0 and count >= limit:
            print(f'Color count is above limit of {limit}!')
            break
        if count == 0:
            colors.append(pix)
    return colors

def get_palette_from_colors(colors: list, limit: int) -> Image:
    length: int = 0
    if limit > 0:
        length = limit
    else:
        length: int = get_nearest_power_of_two(len(colors))
    image: Image = Image.new('RGBA', (length, 1), 0)
    for i in range(len(colors)):
        image.putpixel(
            (i, 0),
            colors[i]
        )
    return image

def get_indexed_from_colors(pixels: list, colors: list, size: tuple, limit: int) -> Image:
    length: int = 0
    if limit > 0:
        length = limit
    else:
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

def make_palette_image(initial_path: str, colors: list, limit: int):
    palette_image: Image = get_palette_from_colors(colors, limit)
    palette_path: str = os.path.splitext(initial_path)[0] + '_palette.png'
    palette_image.save(palette_path)

def make_indexed_image(initial_path: str, pixels: list, colors: list, size: tuple, limit: int):
    indexed_image: Image = get_indexed_from_colors(pixels, colors, size, limit)
    indexed_path: str = os.path.splitext(initial_path)[0] + '_indexed.png'
    indexed_image.save(indexed_path)

def get_limit_type(arg: str) -> int:
    if arg.endswith('64'):
        return 64
    if arg.endswith('32'):
        return 32
    if arg.endswith('16'):
        return 16
    if arg.endswith('8'):
        return 8
    if arg.endswith('4'):
        return 4
    if arg.endswith('2'):
        return 2
    return 0

def main() -> int:
    paths: List[str] = []
    limit: int = 0
    if len(sys.argv) > 1:
        for i in range(1, len(sys.argv)):
            arg: str = sys.argv[i]
            if arg.startswith('--limit='):
                if limit == 0:
                    limit = get_limit_type(arg)
                else:
                    print('Don\'t set limit more than once!')
            else:
                paths.append(arg)
    if len(paths) > 0:
        for path in paths:
            image: None = Image.open(path)
            pixel_list: list = list(image.getdata())
            color_list: list = get_colors_from_pixels(pixel_list, limit)
            make_palette_image(path, color_list, limit)
            make_indexed_image(path, pixel_list, color_list, image.size, limit)
    else:
        print('Error! No image paths given!')
        return -1
    return 0

if __name__ == '__main__':
    sys.exit(main())
