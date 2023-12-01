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
    id = f'D1P1 #{test_counter[key]}'
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
