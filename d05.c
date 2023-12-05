#include "aoc.h"

#include <stdbool.h>
#include <stdio.h>
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
    size_t SeedCapacity = 32;
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

    // Parse and apply each seed mapping.
    while(*Input != '\0')
    {
        Input = SkipToDigits(Input);
        while(IsDigit(*Input))
        {
            int64_t Dest = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeStart = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeEnd = RangeStart + atoll(Input);
            for(int Index = 0; Index < SeedCount; Index++)
            {
                int64_t Seed = SeedsIn[Index];
                if(Seed >= RangeStart && Seed < RangeEnd)
                {
                    SeedsOut[Index] = Dest + Seed - RangeStart;
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

typedef struct
{
    int64_t Start;
    int64_t End;
} range;

typedef struct
{
    range* Elements;
    size_t Count;
    size_t Capacity;
} range_array;

void InitRangeArray(range_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

void FreeRangeArray(range_array* Array)
{
    free(Array->Elements);
}

void RangeArrayAdd(range_array* Array, int64_t Start, int64_t End)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (range*)realloc(Array->Elements, sizeof(range) * Capacity);
        Array->Capacity = Capacity;
    }
    Array->Elements[Array->Count++] = (range){.Start = Start, .End = End};
}

void RangeArrayReset(range_array* Array)
{
    Array->Count = 0;
}

AOC_SOLVER(Part2)
{
    range_array SeedRangesIn, SeedRangesOut;
    InitRangeArray(&SeedRangesIn);
    InitRangeArray(&SeedRangesOut);

    // Parse the ranges.
    Input += 7; // Skip "seeds: "
    do
    {
        int64_t Start = atoll(Input);
        Input = SkipPastDigits(Input);
        Input = SkipPastWhitespace(Input);
        int64_t End = Start + atoll(Input);
        Input = SkipPastDigits(Input);
        Input = SkipPastWhitespace(Input);
        RangeArrayAdd(&SeedRangesIn, Start, End);
    } while(*Input != '\n');

    // Parse and apply each seed mapping.
    while(*Input != '\0')
    {
        Input = SkipToDigits(Input);
        while(IsDigit(*Input))
        {
            int64_t Dest = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeStart = atoll(Input);
            Input = SkipPastDigits(Input);
            Input = SkipPastWhitespace(Input);
            int64_t RangeEnd = RangeStart + atoll(Input);
            int64_t DestLength = -1;
            for(int Index = 0; Index < SeedRangesIn.Count; Index++)
            {
                range SeedRange = SeedRangesIn.Elements[Index];
                if(RangeEnd <= SeedRange.Start) continue;
                if(RangeStart >= SeedRange.End) continue;
                if(RangeStart <= SeedRange.Start && RangeEnd >= SeedRange.End)
                {
                    int64_t DestStart = Dest + SeedRange.Start - RangeStart;
                    int64_t DestEnd = DestStart + SeedRange.End - SeedRange.Start;
                    RangeArrayAdd(&SeedRangesOut, DestStart, DestEnd);
                    SeedRangesIn.Elements[Index] = (range){.Start = -1, .End = -1};
                }
                else if(RangeStart <= SeedRange.Start)
                {
                    int64_t DestStart = Dest + SeedRange.Start - RangeStart;
                    int64_t DestEnd = DestStart + RangeEnd - SeedRange.Start;
                    RangeArrayAdd(&SeedRangesOut, DestStart, DestEnd);
                    SeedRangesIn.Elements[Index].Start = RangeEnd;
                }
                else if(RangeEnd >= SeedRange.End)
                {
                    int64_t DestStart = Dest;
                    int64_t DestEnd = DestStart + SeedRange.End - RangeStart;
                    SeedRangesIn.Elements[Index].End = RangeStart;
                    RangeArrayAdd(&SeedRangesOut, DestStart, DestEnd);
                }
                else
                {
                    int64_t DestStart = Dest;
                    int64_t DestEnd = DestStart + RangeEnd - RangeStart;
                    SeedRangesIn.Elements[Index].End = RangeStart;
                    RangeArrayAdd(&SeedRangesOut, DestStart, DestEnd);
                    RangeArrayAdd(&SeedRangesIn, RangeEnd, SeedRange.End);
                }
            }
            Input = SkipPastDigits(Input);
            Input = SkipPastNewline(Input);
        }
        for(int Index = 0; Index < SeedRangesIn.Count; Index++)
        {
            range SeedRange = SeedRangesIn.Elements[Index];
            if(SeedRange.Start >= SeedRange.End) continue;
            RangeArrayAdd(&SeedRangesOut, SeedRange.Start, SeedRange.End);
        }
        range_array SeedRangesTemp = SeedRangesOut;
        SeedRangesOut = SeedRangesIn;
        SeedRangesIn = SeedRangesTemp;
        RangeArrayReset(&SeedRangesOut);
    }

    // Find the lowest seed.
    int64_t LowestSeed = INT64_MAX;
    for(int Index = 0; Index < SeedRangesIn.Count; Index++)
    {
        int64_t Seed = SeedRangesIn.Elements[Index].Start;
        if(Seed < LowestSeed) LowestSeed = Seed;
    }

    FreeRangeArray(&SeedRangesIn);
    FreeRangeArray(&SeedRangesOut);
    return LowestSeed;
}
