#!/usr/bin/env python

"""
Unit tests for Advent of Code 2023
"""

from collections import Counter
import os
import subprocess

os.system("")  # Enables ANSI color codes.
test_counter = Counter()
COLORS = {
    "green": 32,
    "red": 31,
    "cyan": 36,
    "light_red": 91,
    "light_green": 92
}


def color(color, text):
    return f"\033[{COLORS[color]}m{text}\033[m"


def test(day, part, input, expected):
    exe = f'd{day:02d}.exe'
    flags = f'-i{part}q'
    key = (day, part)
    test_counter[key] += 1
    id = f'D{day}P{part} #{test_counter[key]}'
    fail = None
    try:
        result = subprocess.run([exe, flags], capture_output=True,
                                text=True, input=input)
        actual = result.stdout.strip()
    except subprocess.CalledProcessError:
        fail = f'Unexpected exit code {result.returncode}.'
    except FileNotFoundError:
        fail = 'Test executable not found.'
    else:
        if actual != expected:
            fail = f'Expected "{expected}", but got "{actual}".'
    if fail is None:
        print(f'{color("green", "PASS")} {id}')
    else:
        print(f'{color("light_red", "FAIL")} {id}: {fail}')


d01_e1 = """1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet"""

test(1, 1, d01_e1, "142")

d01_e2 = """two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen"""

test(1, 2, d01_e2, "281")

d02_e1 = """Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green"""

test(2, 1, d02_e1, "8")
test(2, 2, d02_e1, "2286")

d03_e1 = """467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598.."""

test(3, 1, d03_e1, "4361")
test(3, 2, d03_e1, "467835")

d04_e1 = """Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11"""

test(4, 1, d04_e1, "13")