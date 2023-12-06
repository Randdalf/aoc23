#include "aoc.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char* DefaultInputPath = "d06.txt";

static bool IsDigit(char C)
{
    return C >= '0' && C <= '9';
}

static bool IsWhitespace(char C)
{
    return C == ' ' || C == '\r';
}

static const char* SkipPastDigits(const char* Input)
{
    while(IsDigit(*Input)) Input++;
    return Input;
}

static const char* SkipPastNewline(const char* Input)
{
    if(*Input == '\r') Input++;
    if(*Input == '\n') Input++;
    return Input;
}

static const char* SkipPastWhitespace(const char* Input)
{
    while(IsWhitespace(*Input)) Input++;
    return Input;
}

static const char* SkipToDigits(const char* Input)
{
    while(!IsDigit(*Input)) Input++;
    return Input;
}

AOC_SOLVER(Part1)
{
    double Times[4];
    double Dists[4];
    int Count = 0;

    // Parse input;
    double* Data[2] = { Times, Dists };
    for(int Index = 0; Index < 2; Index++)
    {
        Count = 0;
        Input = SkipToDigits(Input);
        while(IsDigit(*Input))
        {
            Data[Index][Count++] = (double)atoi(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
        }
        Input = SkipPastWhitespace(Input);
    }

    // Calculate margin for error in each record.
    int64_t Product = 1;
    for(int Index = 0; Index < Count; Index++)
    {
        double Time = Times[Index];
        double Dist = Dists[Index];
        double Disc = sqrt(Time * Time - 4 * Dist);
        int Lo = (int)floor((Time - Disc)/2 + 1);
        int Hi = (int)ceil((Time + Disc)/2 - 1);
        Product *= Hi - Lo + 1;
    }

    return Product;
}

AOC_SOLVER(Part2)
{
    return -1;
}
