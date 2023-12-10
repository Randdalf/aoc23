#include "aoc.h"
#include "parse.h"

#include <intrin.h>

const char* DefaultInputPath = "d10.txt";

enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    NUM_DIRS
};

enum
{
    FLAG_NORTH = 1 << DIR_NORTH,
    FLAG_EAST  = 1 << DIR_EAST,
    FLAG_SOUTH = 1 << DIR_SOUTH,
    FLAG_WEST  = 1 << DIR_WEST
};

static uint8_t ToCell(char C)
{
    switch(C)
    {
    case '|': return FLAG_NORTH | FLAG_SOUTH;
    case '-': return FLAG_EAST | FLAG_WEST;
    case 'L': return FLAG_NORTH | FLAG_EAST;
    case 'J': return FLAG_NORTH | FLAG_WEST;
    case '7': return FLAG_SOUTH | FLAG_WEST;
    case 'F': return FLAG_EAST | FLAG_SOUTH;
    case 'S':
    default: return 0;
    }
}

AOC_SOLVER(Part1)
{
    // Create lookup table for determining whether character is in grid.
    uint8_t IsGrid[UINT8_MAX + 1];
    memset(IsGrid, 0, sizeof(IsGrid));
    IsGrid['|'] = 1;
    IsGrid['-'] = 1;
    IsGrid['L'] = 1;
    IsGrid['J'] = 1;
    IsGrid['7'] = 1;
    IsGrid['F'] = 1;
    IsGrid['.'] = 1;
    IsGrid['S'] = 1;

    // Generate a grid with adjacency information for every cell
    size_t CellCount = 0;
    size_t CellCapacity = 128;
    uint8_t* Cells = (uint8_t*)malloc(CellCapacity);
    int Height = 0;
    int StartIndex = 0;
    char C = *Input;
    while(IsGrid[C])
    {
        Height++;
        while(IsGrid[C])
        {
            if(CellCount == CellCapacity)
            {
                CellCapacity *= 2;
                Cells = (uint8_t*)realloc(Cells, CellCapacity);
            }
            if(C == 'S') StartIndex = CellCount;
            Cells[CellCount++] = ToCell(C);
            C = *(++Input);
        }
        Input = SkipPastNewline(Input);
        C = *Input;
    }
    int Width = CellCount / Height;

    // Choose a start direction.
    unsigned long Dir;
    int StartX = StartIndex % Height;
    int StartY = StartIndex / Height;
    if(StartY > 0 && (Cells[StartIndex - Width] & FLAG_SOUTH))
    {
        Dir = DIR_NORTH;
    }
    else if(StartX < Width - 1 && (Cells[StartIndex + 1] & FLAG_WEST))
    {
        Dir = DIR_EAST;
    }
    else if(StartY < Height - 1 && (Cells[StartIndex + Width] & FLAG_NORTH))
    {
        Dir = DIR_SOUTH;
    }
    else
    {
        Dir = DIR_WEST;
    }

    // Traverse around the loop from the start, counting the number of steps.
    int Index = StartIndex;
    int Steps = 0;
    do
    {
        uint8_t Mask;
        switch(Dir)
        {
        case DIR_NORTH:
            Index = Index - Width;
            Mask = ~FLAG_SOUTH;
            break;
        case DIR_EAST:
            Index = Index + 1;
            Mask = ~FLAG_WEST;
            break;
        case DIR_SOUTH:
            Index = Index + Width;
            Mask = ~FLAG_NORTH;
            break;
        case DIR_WEST:
            Index = Index - 1;
            Mask = ~FLAG_EAST;
            break;
        }
        _BitScanForward(&Dir, Cells[Index] & Mask);
        Steps++;
    } while(Index != StartIndex);

    // The maximum distance from the start is half the number of steps.
    free(Cells);
    return Steps / 2;
}

AOC_SOLVER(Part2)
{
    return -1;
}
