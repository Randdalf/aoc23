#include "aoc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char* DefaultInputPath = "d03.txt";

static bool IsDigit(char C)
{
    return C >= '0' && C <= '9';
}

static int GetInputWidth(const char* Input)
{
    int Width = 0;
    for(;;)
    {
        switch(*Input++)
        {
        case '\0': case '\r': case '\n':
            return Width;
        default:
            Width++;
            break;
        }
    }
}

static const char* ParseSchematic(const char* Input, int* OutWidth, int* OutHeight)
{
    char* Schematic = (char*)malloc(strlen(Input) * 2); // Estimate.
    int Height = 2;
    int Width = GetInputWidth(Input) + 2;
    int Index = 0;
    for(int X = 0; X < Width; X++)
    {
        Schematic[Index++] = '.'; // Top padding.
    }
    Schematic[Index++] = '.'; // Left padding.
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case '\r':
            break;
        case '\0':
            Schematic[Index++] = '.'; // Right padding.
            Height++;
            break;
        case '\n':
            Schematic[Index++] = '.'; // Right padding.
            Height++;
            Schematic[Index++] = '.'; // Left padding.
            break;
        default:
            Schematic[Index++] = C;
            break;
        }
    } while(C != '\0');
    for(int X = 0; X < Width; X++)
    {
        Schematic[Index++] = '.'; // Bottom padding.
    }
    *OutWidth = Width;
    *OutHeight = Height;
    return Schematic;
}

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    int Width, Height;
    const char* Schematic = ParseSchematic(Input, &Width, &Height);
    int Index = 0;
    for(int Y = 0; Y < Height; Y++)
    {
        for(int X = 0; X < Width; X++, Index++)
        {
            if(!IsDigit(Schematic[Index])) continue;
            int Number = atoi(Schematic + Index);
            int LeftIndex = Index - 1;
            do { Index++; X++; } while(IsDigit(Schematic[Index]));
            if(Schematic[LeftIndex] != '.') goto PartNumber;
            if(Schematic[Index] != '.') goto PartNumber;
            for(int TestIndex = LeftIndex - Width; TestIndex <= Index - Width; TestIndex++)
            {
                if(Schematic[TestIndex] != '.') goto PartNumber;
            }
            for(int TestIndex = LeftIndex + Width; TestIndex <= Index + Width; TestIndex++)
            {
                if(Schematic[TestIndex] != '.') goto PartNumber;
            }
            continue;
        PartNumber:
            Sum += Number;
        }
    }
    free((void*)Schematic);
    return Sum;
}

AOC_SOLVER(Part2)
{
    return -1;
}
