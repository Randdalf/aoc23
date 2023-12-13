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

static uint8_t* GetIsGrid(void)
{
    // Generate lookup table for determining whether character is in grid.
    static uint8_t* IsGrid = NULL;
    if(!IsGrid)
    {
        static uint8_t IsGridBuffer[UINT8_MAX + 1];
        memset(IsGridBuffer, 0, sizeof(IsGrid));
        IsGrid = IsGridBuffer;
        IsGrid['|'] = 1;
        IsGrid['-'] = 1;
        IsGrid['L'] = 1;
        IsGrid['J'] = 1;
        IsGrid['7'] = 1;
        IsGrid['F'] = 1;
        IsGrid['.'] = 1;
        IsGrid['S'] = 1;
    }
    return IsGrid;
}

typedef void (*on_loop_start)(void* User, int InputWidth, int InputHeight);
typedef void (*on_loop_step)(void* User, int Index, uint8_t Cell);

static void TraverseLoop(const char* Input, void* User, on_loop_start OnLoopStart, on_loop_step OnLoopStep)
{
    // Generate a grid with adjacency information for every cell
    size_t CellCount = 0;
    size_t CellCapacity = 128;
    uint8_t* Cells = (uint8_t*)malloc(CellCapacity);
    int Height = 0;
    int StartIndex = 0;
    char C = *Input;
    uint8_t* IsGrid = GetIsGrid();
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
    Cells[StartIndex] = FLAG_NORTH | FLAG_EAST | FLAG_SOUTH | FLAG_WEST;

    // Traverse around the loop from the start, counting the number of steps.
    int Index = StartIndex;
    OnLoopStart(User, Width, Height);
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
        uint8_t Cell = Cells[Index];
        _BitScanForward(&Dir, Cell & Mask);
        OnLoopStep(User, Index, Cell);
    } while(Index != StartIndex);

    free(Cells);
}

static void Noop(void* User, int InputWidth, int InputHeight)
{
    AOC_UNUSED(User);
    AOC_UNUSED(InputWidth);
    AOC_UNUSED(InputHeight);
}

static void IncrementStep(void* User, int Index, uint8_t Cell)
{
    AOC_UNUSED(Index);
    AOC_UNUSED(Cell);
    (*((int*)User))++;
}

AOC_SOLVER(Part1)
{
    // The maximum distance from the start is half the number of steps.
    int Steps = 0;
    TraverseLoop(Input, &Steps, Noop, IncrementStep);
    return Steps / 2;
}

enum
{
    GRID_UNKNOWN = '?',
    GRID_LOOP = 'L',
    GRID_FRUIT = 'O',
    GRID_CONSUMED = '*'
};

typedef struct
{
    int X;
    int Y;
} ivec2;

typedef struct
{
    int InputWidth;
    int Width;
    int Height;
    uint8_t* Grid;
    ivec2* Stack;
} area;

static void InitArea(void* User, int InputWidth, int InputHeight)
{
    // Convert the input grid into 3x3 tiles. Each tile has a "fruit" tile
    // in its center. The number of fruits enclosed by the loop is the number
    // of enclosed tiles in the input grid.
    area* Area = (area*)User;
    Area->InputWidth = InputWidth;
    Area->Width = 3 * InputWidth;
    Area->Height = 3 * InputHeight;
    size_t Size = 9 * InputWidth * InputHeight;
    Area->Grid = (uint8_t*)malloc(Size);
    memset(Area->Grid, GRID_UNKNOWN, Size);
    for(int Y = 1; Y < Area->Height; Y += 3)
    {
        for(int X = 1; X < Area->Width; X += 3)
        {
            Area->Grid[X + Y * Area->Width] = GRID_FRUIT;
        }
    }
    Area->Stack = (ivec2*)malloc(sizeof(ivec2) * Size);
}

static void MarkLoop(void* User, int Index, uint8_t Cell)
{
    area* Area = (area*)User;

    // Transform the index into the center of a tile on the 3x3 grid.
    int MinX = 3 * (Index % Area->InputWidth);
    int MinY = 3 * (Index / Area->InputWidth);
    Index = MinX + 1 + (MinY + 1) * Area->Width;

    // Mark the loop on the 3x3 grid.
    Area->Grid[Index] = GRID_LOOP;
    if(Cell & FLAG_NORTH)
    {
        Area->Grid[Index - Area->Width] = GRID_LOOP;
    }
    if(Cell & FLAG_EAST)
    {
        Area->Grid[Index + 1] = GRID_LOOP;
    }
    if(Cell & FLAG_SOUTH)
    {
        Area->Grid[Index + Area->Width] = GRID_LOOP;
    }
    if(Cell & FLAG_WEST)
    {
        Area->Grid[Index - 1] = GRID_LOOP;
    }
}

static bool IsConsumable(char C)
{
    return C == GRID_UNKNOWN || C == GRID_FRUIT;
}

static int CountEnclosedFruit(area* Area, int FromX, int FromY)
{
    bool Enclosed = true;
    int EnclosedCount = 0;
    ivec2* Stack = Area->Stack;
    int StackCount = 0;
    Stack[StackCount++] = (ivec2){.X = FromX, .Y = FromY};
    Area->Grid[FromX + FromY * Area->Width] = GRID_CONSUMED;
    while(StackCount > 0)
    {
        ivec2 At = Stack[--StackCount];
        if(At.Y > 0)
        {
            int NorthIndex = At.X + (At.Y - 1) * Area->Width;
            char C = Area->Grid[NorthIndex];
            if(IsConsumable(C))
            {
                EnclosedCount += C == GRID_FRUIT;
                Area->Grid[NorthIndex] = GRID_CONSUMED;
                Stack[StackCount++] = (ivec2){.X = At.X, .Y = At.Y - 1};
            }
        }
        else
        {
            Enclosed = false;
        }
        if(At.X < Area->Width - 1)
        {
            int EastIndex = (At.X + 1) + At.Y * Area->Width;
            char C = Area->Grid[EastIndex];
            if(IsConsumable(C))
            {
                EnclosedCount += C == GRID_FRUIT;
                Area->Grid[EastIndex] = GRID_CONSUMED;
                Stack[StackCount++] = (ivec2){.X = At.X + 1, .Y = At.Y};
            }
        }
        else
        {
            Enclosed = false;
        }
        if(At.Y < Area->Height - 1)
        {
            int SouthIndex = At.X + (At.Y + 1) * Area->Width;
            char C = Area->Grid[SouthIndex];
            if(IsConsumable(C))
            {
                EnclosedCount += C == GRID_FRUIT;
                Area->Grid[SouthIndex] = GRID_CONSUMED;
                Stack[StackCount++] = (ivec2){.X = At.X, .Y = At.Y + 1};
            }
        }
        else
        {
            Enclosed = false;
        }
        if(At.X > 0)
        {
            int WestIndex = (At.X - 1) + At.Y * Area->Width;
            char C = Area->Grid[WestIndex];
            if(IsConsumable(C))
            {
                EnclosedCount += C == GRID_FRUIT;
                Area->Grid[WestIndex] = GRID_CONSUMED;
                Stack[StackCount++] = (ivec2){.X = At.X - 1, .Y = At.Y};
            }
        }
        else
        {
            Enclosed = false;
        }
    }
    return Enclosed ? EnclosedCount : 0;
}

AOC_SOLVER(Part2)
{
    area Area;
    memset(&Area, 0, sizeof(area));
    TraverseLoop(Input, &Area, InitArea, MarkLoop);

    // Flood fill from each loop tile and count the fruit encountered. If the
    // flood fill does not hit the edge of the grid, then the fruit are
    // enclosed by the loop.
    int Index = 0;
    int Enclosed = 0;
    for(int Y = 0; Y < Area.Height; Y++)
    {
        for(int X = 0; X < Area.Width; X++, Index++)
        {
            if(Area.Grid[Index] != GRID_LOOP) continue;
            if(Y > 0)
            {
                int NorthIndex = Index - Area.Width;
                if(Area.Grid[NorthIndex] == GRID_UNKNOWN)
                {
                    Enclosed += CountEnclosedFruit(&Area, X, Y - 1);
                }
            }
            if(X < Area.Width - 1)
            {
                int EastIndex = Index + 1;
                if(Area.Grid[EastIndex] == GRID_UNKNOWN)
                {
                    Enclosed += CountEnclosedFruit(&Area, X + 1, Y);
                }
            }
            if(Y < Area.Height - 1)
            {
                int SouthIndex = Index + Area.Width;
                if(Area.Grid[SouthIndex] == GRID_UNKNOWN)
                {
                    Enclosed += CountEnclosedFruit(&Area, X, Y + 1);
                }
            }
            if(X > 0)
            {
                int WestIndex = Index - 1;
                if(Area.Grid[WestIndex] == GRID_UNKNOWN)
                {
                    Enclosed += CountEnclosedFruit(&Area, X - 1, Y);
                }
            }
        }
    }

    free(Area.Grid);
    free(Area.Stack);
    return Enclosed;
}
