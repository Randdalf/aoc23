#include "aoc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char* DefaultInputPath = "d05.txt";

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
    // Parse the seeds.
    size_t SeedCount = 0;
    size_t SeedCapacity = 8;
    int64_t* SeedsIn = (int64_t*)malloc(sizeof(int64_t) * SeedCapacity);
    Input += 7; // Skip "seeds: "
    do
    {
        if(SeedCount == SeedCapacity)
        {
            SeedCapacity *= 2;
            SeedsIn = (int64_t*)realloc(SeedsIn, sizeof(int64_t) * SeedCapacity);
        }
        SeedsIn[SeedCount++] = atoll(Input);
        Input = SkipPastDigits(Input);
        Input = SkipPastWhitespace(Input);
    } while(*Input != '\n');
    size_t SeedSize = sizeof(int64_t) * SeedCount;
    int64_t* SeedsOut = (int64_t*)malloc(SeedSize);
    memcpy(SeedsOut, SeedsIn, SeedSize);

    // Run the seeds through each map.
    while(*Input != '\0')
    {
        Input = SkipToDigits(Input);

        // Parse and apply each range mapping.
        while(IsDigit(*Input))
        {
            int64_t Dest = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeStart = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeEnd = RangeStart + atoll(Input);
            for(int SeedIndex = 0; SeedIndex < SeedCount; SeedIndex++)
            {
                int64_t Seed = SeedsIn[SeedIndex];
                if(Seed >= RangeStart && Seed < RangeEnd)
                {
                    SeedsOut[SeedIndex] = Dest + Seed - RangeStart;
                }
            }
            Input = SkipPastDigits(Input);
            Input = SkipPastNewline(Input);
        }
        memcpy(SeedsIn, SeedsOut, SeedSize);
    }

    // Find the lowest seed.
    int64_t LowestSeed = INT64_MAX;
    for(int Index = 0; Index < SeedCount; Index++)
    {
        int64_t Seed = SeedsOut[Index];
        if(Seed < LowestSeed) LowestSeed = Seed;
    }

    free(SeedsIn);
    free(SeedsOut);
    return LowestSeed;
}

AOC_SOLVER(Part2)
{
    return -1;
}
