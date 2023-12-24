#pragma once

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AOC_SOLVER(Name) int64_t Name(const char* Input)
typedef AOC_SOLVER(aoc_solver);

#define AOC_UNUSED(X) ((void)X)
