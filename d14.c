#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d14.txt";

static bool IsRound(char C)
{
    return C == 'O';
}

static bool IsCube(char C)
{
    return C == '#';
}

static bool IsEmpty(char C)
{
    return C == '.';
}

static bool IsGrid(char C)
{
    return IsRound(C) || IsCube(C) || IsEmpty(C);
}

AOC_SOLVER(Part1)
{
    size_t GridCapacity = 8;
    size_t GridCount = 0;
    char* Grid = (char*)malloc(sizeof(char) * GridCapacity);
    int Height = 0;
    while(IsGrid(*Input))
    {
        while(IsGrid(*Input))
        {
            if(GridCount == GridCapacity)
            {
                GridCapacity *= 2;
                Grid = (char*)realloc(Grid, sizeof(char) * GridCapacity);
            }
            Grid[GridCount++] = *(Input++);
        }
        Input = SkipPastNewline(Input);
        Height++;
    }
    int Width = GridCount / Height;
    int Index = 0;
    int64_t Load = 0;
    for(int Y = 0; Y < Height; Y++)
    {
        for(int X = 0; X < Width; X++, Index++)
        {
            if(!IsRound(Grid[Index])) continue;
            int MoveIndex = Index;
            int MoveY = Y;
            while(MoveIndex >= Width && IsEmpty(Grid[MoveIndex - Width]))
            {
                MoveIndex -= Width;
                MoveY--;
            }
            if(Index != MoveIndex)
            {
                Grid[MoveIndex] =  Grid[Index];
                Grid[Index] = '.';
            }
            Load += Width - MoveY;
        }
    }
    return Load;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
