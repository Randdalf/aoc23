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
    id = f'D{day:02d}P{part} #{test_counter[key]}'
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
test(4, 2, d04_e1, "30")

d05_e1 = """seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4"""

test(5, 1, d05_e1, "35")
test(5, 2, d05_e1, "46")

d06_e1 = """Time:      7  15   30
Distance:  9  40  200"""

test(6, 1, d06_e1, "288")
test(6, 2, d06_e1, "71503")

d07_e1 = """32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483"""

test(7, 1, d07_e1, "6440")
test(7, 2, d07_e1, "5905")

d08_e1 = """RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ)"""

d08_e2 = """LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ)"""

test(8, 1, d08_e1, "2")
test(8, 1, d08_e2, "6")

d08_e3 = """LR

YYA = (YYB, XXX)
YYB = (XXX, YYZ)
YYZ = (YYB, XXX)
ZZA = (ZZB, XXX)
ZZB = (ZZC, ZZC)
ZZC = (ZZZ, ZZZ)
ZZZ = (ZZB, ZZB)
XXX = (XXX, XXX)"""

test(8, 2, d08_e3, "6")

d09_e1 = """0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45"""

test(9, 1, d09_e1, "114")
test(9, 2, d09_e1, "2")

d10_e1 = """.....
.S-7.
.|.|.
.L-J.
....."""

d10_e2 = """..F7.
.FJ|.
SJ.L7
|F--J
LJ..."""

test(10, 1, d10_e1, "4")
test(10, 1, d10_e2, "8")
