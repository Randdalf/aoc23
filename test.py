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


d01_p1_e1 = """1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet"""

test(1, 1, d01_p1_e1, "142")

d01_p2_e1 = """two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen"""

test(1, 2, d01_p2_e1, "281")

d02_p1_e1 = """Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green"""

test(2, 1, d02_p1_e1, "8")
