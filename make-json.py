#!/usr/bin/env python3

import os, sys, json
from typing import List, Tuple, Dict

class Chunk:
    def __init__(self):
        self.title: str = ''
        self.data: Dict[str, str] = {}
    def __init__(self, newtitle: str):
        self.title: str = newtitle
        self.data: Dict[str, str] = {}

def parse_line(line: str) -> Tuple[str, str]:
    if len(line) > 0 and line[0] != '#' and line[0] != ';' and line[0] != '[' and not line[0].isspace():
        index: int = 0
        while line[index].isspace():
            index += 1
        begin: int = index
        while not line[index].isspace():
            index += 1
        key: str = line[begin:index]
        while line[index].isspace() or line[index] == '=':
            index += 1
        val: str = line[index:len(line)].strip()
        return (key, val)
    elif line[0] == '[':
        index: int = 1
        begin: int = index
        while not line[index].isspace() and line[index] != ']':
            index += 1
        title: str = line[begin:index]
        return ('!', title)
    return ('', '')

def load_file(file_name: str) -> List[Chunk]:
    result: List[Chunk] = []
    with open(file_name, 'r') as file:
        title: str = 'NONE'
        for line in file.readlines():
            kvp: Tuple[str, str] = parse_line(line)
            if len(kvp[0]) > 0:
                if kvp[0][0] != '!':
                    for chunk in result:
                        if chunk.title == title:
                            chunk.data[kvp[0]] = kvp[1]
                else:
                    title = kvp[1]
                    result.append(Chunk(kvp[1]))
    return result

def extract_values(line: str) -> List[int]:
    return [int(i) for i in line.split(',')]

def extract_action_points(data: Dict[str, str]):
    direction_count: int = int(data['hvtype'])
    action_points = []
    for d in range(0, direction_count):
        if f'{d}-X' in data:
            action_points.append(extract_values(data[f'{d}-X']))
    return action_points

def extract_frames(data: Dict[str, str]):
    frame_count: int = int(data['frames'])
    direction_count: int = int(data['hvtype'])
    frames = []
    for d in range(0, direction_count):
        frame = []
        for f in range(0, frame_count):
            frame.append(extract_values(data[f'{d}-{f}']))
        frames.append(frame)
    return frames

def write_file(chunks: List[Chunk], file_name: str) -> bool:
    output = {}
    output['Material'] = chunks[0].data['Material']
    if 'Palettes' in chunks[0].data:
        output['Palette'] = chunks[0].data['Palettes']
    output['Dimensions'] = extract_values(chunks[0].data['Inverter'])
    output['Animations'] = []
    for i in range(1, len(chunks)):
        animation = {}
        animation['name'] = chunks[i].title
        animation['starts'] = extract_values(chunks[i].data['starts'])
        animation['vksize'] = extract_values(chunks[i].data['vksize'])
        animation['tdelay'] = float(chunks[i].data['tdelay'])
        action_points = extract_action_points(chunks[i].data)
        if len(action_points) > 0:
            animation['action'] = action_points
        if 'reflect' in chunks[i].data:
            if chunks[i].data['reflect'] == '1':
                animation['reflect'] = True
            else:
                animation['reflect'] = False
        if 'repeat' in chunks[i].data:
            if chunks[i].data['repeat'] == '1':
                animation['repeat'] = True
            else:
                animation['repeat'] = False
        animation['frames'] = extract_frames(chunks[i].data)
        output['Animations'].append(animation)
    with open(file_name, 'w') as file:
        json.dump(output, file, indent='\t')
        return True
    return False

def main() -> int:
    if len(sys.argv) > 1:
        for i in range(1, len(sys.argv)):
            input_name: str = sys.argv[i]
            chunks = load_file(input_name)
            if len(chunks) > 0:
                output_name = os.path.splitext(input_name)[0] + '.json'
                if write_file(chunks, output_name):
                    print(f'Successfully converted {input_name}!')
                else:
                    print(f'Conversion failed for {input_name}!')
                    return -1
            else:
                print(f'There is no file named {input_name}!')
                return -1
    else:
        print(f'No arguments were passed!')
        return -1
    return 0

if __name__ == '__main__':
    sys.exit(main())
