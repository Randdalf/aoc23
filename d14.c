#include "aoc.h"
#include "parse.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

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

typedef struct
{
    size_t Capacity;
    size_t Count;
    char* Cells;
    int Width;
    int Height;
} grid;

static void InitGrid(grid* Grid, const char* Input)
{
    Grid->Capacity = 8;
    Grid->Count = 0;
    Grid->Cells = (char*)malloc(sizeof(char) * Grid->Capacity);
    Grid->Height = 0;
    while(IsGrid(*Input))
    {
        while(IsGrid(*Input))
        {
            if(Grid->Count == Grid->Capacity)
            {
                Grid->Capacity *= 2;
                Grid->Cells = (char*)realloc(Grid->Cells, sizeof(char) * Grid->Capacity);
            }
            Grid->Cells[Grid->Count++] = *(Input++);
        }
        Input = SkipPastNewline(Input);
        Grid->Height++;
    }
    Grid->Width = Grid->Count / Grid->Height;
}

static inline int GridIndex(grid* Grid, int X, int Y)
{
    return X + Grid->Width * Y;
}

static void GridRollNorth(grid* Grid)
{
    int Index = 0;
    for(int Y = 0; Y < Grid->Height; Y++)
    {
        for(int X = 0; X < Grid->Width; X++, Index++)
        {
            if(!IsRound(Grid->Cells[Index])) continue;
            int MoveIndex = Index;
            while(MoveIndex >= Grid->Width && IsEmpty(Grid->Cells[MoveIndex - Grid->Width]))
            {
                MoveIndex -= Grid->Width;
            }
            if(Index != MoveIndex)
            {
                Grid->Cells[MoveIndex] =  Grid->Cells[Index];
                Grid->Cells[Index] = '.';
            }
        }
    }
}

static void GridRollEast(grid* Grid)
{
    for(int Y = 0; Y < Grid->Height; Y++)
    {
        for(int X = Grid->Width - 1; X >= 0; X--)
        {
            int Index = GridIndex(Grid, X, Y);
            if(!IsRound(Grid->Cells[Index])) continue;
            int MoveX = X;
            int MoveIndex = Index;
            while(MoveX < Grid->Width - 1 && IsEmpty(Grid->Cells[MoveIndex + 1]))
            {
                MoveX++;
                MoveIndex++;
            }
            if(X != MoveX)
            {
                Grid->Cells[MoveIndex] =  Grid->Cells[Index];
                Grid->Cells[Index] = '.';
            }
        }
    }
}

static void GridRollSouth(grid* Grid)
{
    for(int Y = Grid->Height - 1; Y >= 0; Y--)
    {
        for(int X = 0; X < Grid->Width; X++)
        {
            int Index = GridIndex(Grid, X, Y);
            if(!IsRound(Grid->Cells[Index])) continue;
            int MoveY = Y;
            int MoveIndex = Index;
            while(MoveY < Grid->Height - 1 && IsEmpty(Grid->Cells[MoveIndex + Grid->Width]))
            {
                MoveY++;
                MoveIndex += Grid->Width;
            }
            if(Index != MoveIndex)
            {
                Grid->Cells[MoveIndex] =  Grid->Cells[Index];
                Grid->Cells[Index] = '.';
            }
        }
    }
}

static void GridRollWest(grid* Grid)
{
    int Index = 0;
    for(int Y = 0; Y < Grid->Height; Y++)
    {
        for(int X = 0; X < Grid->Width; X++, Index++)
        {
            if(!IsRound(Grid->Cells[Index])) continue;
            int MoveX = X;
            int MoveIndex = Index;
            while(MoveX > 0 && IsEmpty(Grid->Cells[MoveIndex - 1]))
            {
                MoveX--;
                MoveIndex--;
            }
            if(Index != MoveIndex)
            {
                Grid->Cells[MoveIndex] =  Grid->Cells[Index];
                Grid->Cells[Index] = '.';
            }
        }
    }
}

static void GridCycle(grid* Grid)
{
    GridRollNorth(Grid);
    GridRollWest(Grid);
    GridRollSouth(Grid);
    GridRollEast(Grid);
}

static int64_t GridTotalLoad(grid* Grid)
{
    int64_t Sum = 0;
    int Index = 0;
    for(int Y = 0; Y < Grid->Height; Y++)
    {
        for(int X = 0; X < Grid->Width; X++, Index++)
        {
            if(IsRound(Grid->Cells[Index])) Sum += Grid->Height - Y;
        }
    }
    return Sum;
}

static uint64_t GridHash(grid* Grid)
{
    return XXH3_64bits(Grid->Cells, Grid->Count);
}

#if 0
static void GridPrint(grid* Grid)
{
    int Index = 0;
    for(int Y = 0; Y < Grid->Height; Y++)
    {
        for(int X = 0; X < Grid->Width; X++, Index++)
        {
            putchar(Grid->Cells[Index]);
        }
        putchar('\n');
    }
}
#endif

static void FreeGrid(grid* Grid)
{
    free(Grid->Cells);
}

typedef uint64_t key;
typedef int64_t value;

typedef struct
{
    int Count;
    int Capacity;
    uint8_t* Slots;
    key* Keys;
    value* Values;
} table;

static void InitTable(table* Table)
{
    Table->Count = 0;
    Table->Capacity = 0;
    Table->Slots = NULL;
    Table->Keys = NULL;
    Table->Values = NULL;
}

static void FreeTable(table* Table)
{
    free(Table->Slots);
    free(Table->Keys);
    free(Table->Values);
}

static void TableReset(table* Table)
{
    memset(Table->Slots, 0, sizeof(uint8_t) * Table->Capacity);
    Table->Count = 0;
}

static bool TableSet(table* Table, key Key, value Value, value* ExistingValue);

static void TableGrow(table* Table)
{
    table NewTable;
    NewTable.Capacity = Table->Capacity ? 2 * Table->Capacity : 2048;
    NewTable.Slots = (uint8_t*)malloc(sizeof(uint8_t) * NewTable.Capacity);
    NewTable.Keys = (key*)malloc(sizeof(key) * NewTable.Capacity);
    NewTable.Values = (value*)malloc(sizeof(value) * NewTable.Capacity);
    TableReset(&NewTable);
    for(int Index = 0; Index < Table->Capacity; Index++)
    {
        if(Table->Slots[Index])
        {
            TableSet(&NewTable, Table->Keys[Index], Table->Values[Index], NULL);
        }
    }
    FreeTable(Table);
    *Table = NewTable;
}

static bool TableSet(table* Table, key Key, value Value, value* ExistingValue)
{
    if(Table->Count >= 0.666 * Table->Capacity)
    {
        TableGrow(Table);
    }
    uint64_t CapacityMask = Table->Capacity - 1;
    int Index = (int)(Key & CapacityMask);
    for(;;)
    {
        if(Table->Slots[Index])
        {
            if(Table->Keys[Index] == Key)
            {
                *ExistingValue = Table->Values[Index];
                return true;
            }
        }
        else
        {
            Table->Slots[Index] = 1;
            Table->Keys[Index] = Key;
            Table->Values[Index] = Value;
            Table->Count++;
            return false;
        }
        Index = (Index + 1) & CapacityMask;
    }
}

AOC_SOLVER(Part1)
{
    grid Grid;
    InitGrid(&Grid, Input);
    GridRollNorth(&Grid);
    int64_t Result = GridTotalLoad(&Grid);
    FreeGrid(&Grid);
    return Result;
}

AOC_SOLVER(Part2)
{
    int64_t Result;
    grid Grid;
    InitGrid(&Grid, Input);
    table Cache;
    InitTable(&Cache);
    int64_t MaxCycles = 1000000000;
    for(int64_t Cycle = 0; Cycle < MaxCycles; Cycle++)
    {
        int64_t PrevCycle;
        if(TableSet(&Cache, GridHash(&Grid), Cycle, &PrevCycle))
        {
            MaxCycles = (MaxCycles - PrevCycle) % (Cycle - PrevCycle);
            break;
        }
        GridCycle(&Grid);
    }
    for(int64_t Cycle = 0; Cycle < MaxCycles; Cycle++)
    {
        GridCycle(&Grid);
    }
    Result = GridTotalLoad(&Grid);
    FreeTable(&Cache);
    FreeGrid(&Grid);
    return Result;
}
