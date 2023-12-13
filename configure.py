#!/usr/bin/env python

"""
Configures ninja files for building Advent of Code solutions.
"""

from ninja.ninja_syntax import Writer
from pathlib import Path


def main():
    with open('build.ninja', 'wt') as f:
        n = Writer(f)
        n.variable('ninja_required_version', '1.11.0')
        n.newline()

        # Set compiler flags.
        cflags = [
            '-fno-rtti',
            '-fno-exceptions',
            '-std=c11',
            '-D_CRT_SECURE_NO_WARNINGS',
            '-O3',
            '-Wall',
            '-Wextra',
            '-Werror',
            '-Wno-sign-compare',
            '-Wno-char-subscripts',
            '-pedantic'
        ]
        n.variable('cflags', ' '.join(cflags))
        n.newline()

        # Set compile rule.
        n.rule('cc', 'clang -MD -MF $out.d $cflags -c $in -o $out',
               depfile='$out.d')
        n.newline()

        # Set link rule.
        n.rule('link', f'clang -o $out $in')
        n.newline()

        # Build aoc object.
        n.build('aoc.o', 'cc', 'aoc.c')

        # Build executable for each day.
        for day in Path('.').glob('d*.c'):
            obj = str(day.with_suffix('.o'))
            exe = str(day.with_suffix('.exe'))
            n.build(obj, 'cc', str(day))
            n.build(exe, 'link', ['aoc.o', obj])


if __name__ == '__main__':
    main()
