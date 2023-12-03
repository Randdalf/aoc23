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
    int Size = Width * Height;
    for(int Index = 0; Index < Size; Index++)
    {
        if(!IsDigit(Schematic[Index])) continue;
        int Number = atoi(Schematic + Index);
        int LeftIndex = Index - 1;
        do { Index++; } while(IsDigit(Schematic[Index]));
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
    free((void*)Schematic);
    return Sum;
}

static int AddGearAdj(int* Numbers, int Number, int Count)
{
    if(Number == 0 || Count > 2) return Count;
    for(int Index = 0; Index < Count; Index++)
    {
        if(Numbers[Index] == Number) return Count;
    }
    Numbers[Count++] = Number;
    return Count;
}

AOC_SOLVER(Part2)
{
    int64_t Sum = 0;
    int Width, Height;
    const char* Schematic = ParseSchematic(Input, &Width, &Height);
    int Size = Width * Height;
    int* Numbers = (int*)malloc(sizeof(int) * Size);
    for(int Index = 0; Index < Size; Index++)
    {
        if(IsDigit(Schematic[Index]))
        {
            int Number = atoi(Schematic + Index);
            do
            {
                Numbers[Index++] = Number;
            } while(IsDigit(Schematic[Index]));
        }
        Numbers[Index] = 0;
    }
    int Index = 0;
    for(int Index = 0; Index < Size; Index++)
    {
        if(Schematic[Index] != '*') continue;
        int AdjNumbers[3];
        int AdjCount = 0;
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index - Width - 1], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index - Width], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index - Width + 1], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index - 1], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index + 1], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index + Width - 1], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index + Width], AdjCount);
        AdjCount = AddGearAdj(AdjNumbers, Numbers[Index + Width + 1], AdjCount);
        if(AdjCount == 2) Sum += AdjNumbers[0] * AdjNumbers[1];
    }
    free(Numbers);
    free((void*)Schematic);
    return Sum;
}
