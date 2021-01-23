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

def parse(line: str) -> Tuple[str, str]:
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

def load(filename: str) -> List[Chunk]:
    result: List[Chunk] = []
    with open(filename, 'r') as file:
        title: str = 'NONE'
        for line in file.readlines():
            kvp: Tuple[str, str] = parse(line)
            if len(kvp[0]) > 0:
                if kvp[0][0] != '!':
                    for chunk in result:
                        if chunk.title == title:
                            chunk.data[kvp[0]] = kvp[1]
                else:
                    title = kvp[1]
                    result.append(Chunk(kvp[1]))
    return result

def main() -> int:
    chunks = load('./data/sprite/naomi.cfg')
    for chunk in chunks:
        print(f'[{chunk.title}]')
        for k in chunk.data.keys():
            print(f'{k} = {chunk.data[k]}')
    return 0

if __name__ == '__main__':
    sys.exit(main())
