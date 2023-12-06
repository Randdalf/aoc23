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

static int64_t MarginForError(double Time, double Dist)
{
    double Disc = sqrt(Time * Time - 4 * Dist);
    int64_t Lo = (int64_t)floor((Time - Disc)/2 + 1);
    int64_t Hi = (int64_t)ceil((Time + Disc)/2 - 1);
    return Hi - Lo + 1;
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
        Input = SkipPastNewline(Input);
    }

    // Calculate product of margins of error in each record.
    int64_t Product = 1;
    for(int Index = 0; Index < Count; Index++)
    {
        Product *= MarginForError(Times[Index], Dists[Index]);
    }
    return Product;
}

AOC_SOLVER(Part2)
{
    double Data[2];
    for(int Index = 0; Index < 2; Index++)
    {
        int64_t Number = 0;
        Input = SkipToDigits(Input);
        for(;;)
        {
            char C = *Input++;
            if(IsDigit(C))
            {
                Number = (Number * 10) + (C - '0');
            }
            else if(C != ' ')
            {
                break;
            }
        }
        Data[Index] = (double)Number;
        Input = SkipPastNewline(Input);
    }
    return MarginForError(Data[0], Data[1]);
}
