#include "aoc.h"
#include "parse.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

const char* DefaultInputPath = "d21.txt";

static bool IsStart(char C)
{
    return C == 'S';
}

static bool IsGrid(char C)
{
    return C == '.' || C == '#' || IsStart(C);
}

typedef struct
{
    int X;
    int Y;
} ivec2;

typedef struct
{
    size_t Capacity;
    size_t Count;
    char* Cells;
    int Width;
    int Height;
    ivec2 Start;
} grid;

static void InitGrid(grid* Grid, const char* Input)
{
    Grid->Capacity = 8;
    Grid->Count = 0;
    Grid->Cells = (char*)malloc(sizeof(char) * Grid->Capacity);
    Grid->Height = 0;
    while(IsGrid(*Input))
    {
        int X = 0;
        while(IsGrid(*Input))
        {
            if(IsStart(*Input))
            {
                Grid->Start.X = X;
                Grid->Start.Y = Grid->Height;
            }
            if(Grid->Count == Grid->Capacity)
            {
                Grid->Capacity *= 2;
                Grid->Cells = (char*)realloc(Grid->Cells, sizeof(char) * Grid->Capacity);
            }
            Grid->Cells[Grid->Count++] = *Input++;
            X++;
        }
        Input = SkipPastNewline(Input);
        Grid->Height++;
    }
    Grid->Width = Grid->Count / Grid->Height;
}

typedef struct
{
    int X;
    int Y;
    int StepsRemaining;
} key;

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

static void TableSet(table* Table, key Key, value Value);

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
            TableSet(&NewTable, Table->Keys[Index], Table->Values[Index]);
        }
    }
    FreeTable(Table);
    *Table = NewTable;
}

static void TableSet(table* Table, key Key, value Value)
{
    if(Table->Count >= 0.666 * Table->Capacity)
    {
        TableGrow(Table);
    }
    int Hash = (int)XXH3_64bits(&Key, sizeof(key));
    int CapacityMask = Table->Capacity - 1;
    int Index = Hash & CapacityMask;
    for(;;)
    {
        if(Table->Slots[Index])
        {
            if(!memcmp(&Table->Keys[Index], &Key, sizeof(key)))
            {
                Table->Values[Index] = Value;
                return;
            }
        }
        else
        {
            Table->Slots[Index] = 1;
            Table->Keys[Index] = Key;
            Table->Values[Index] = Value;
            Table->Count++;
            return;
        }
        Index = (Index + 1) & CapacityMask;
    }
}

static bool TableGet(table* Table, key Key, value* Value)
{
    if(Table->Count == 0) return false;
    uint64_t Hash = (int)XXH3_64bits(&Key, sizeof(key));
    int CapacityMask = Table->Capacity - 1;
    int Index = Hash & CapacityMask;
    for(;;)
    {
        if(Table->Slots[Index])
        {
            if(!memcmp(&Table->Keys[Index], &Key, sizeof(key)))
            {
                *Value = Table->Values[Index];
                return true;
            }
        }
        else
        {
            return false;
        }
        Index = (Index + 1) & CapacityMask;
    }
}

static int64_t CountPlots(table* Cache, grid* Grid, int X, int Y, int StepsRemaining)
{
    key Key = (key){.X = X, .Y = Y, .StepsRemaining = StepsRemaining};
    int64_t Value = 0;
    if(X < 0 || X >= Grid->Width) return 0;
    if(Y < 0 || Y >= Grid->Height) return 0;
    int Index = Y * Grid->Width + X;
    if(Grid->Cells[Index] == '#') return 0;
    if(TableGet(Cache, Key, &Value)) return 0;
    if(!StepsRemaining)
    {
        TableSet(Cache, Key, 1);
        return 1;
    }
    StepsRemaining--;
    Value += CountPlots(Cache, Grid, X, Y - 1, StepsRemaining);
    Value += CountPlots(Cache, Grid, X + 1, Y, StepsRemaining);
    Value += CountPlots(Cache, Grid, X, Y + 1, StepsRemaining);
    Value += CountPlots(Cache, Grid, X - 1, Y, StepsRemaining);
    TableSet(Cache, Key, 1);
    return Value;
}

AOC_SOLVER(Part1)
{
    int NumSteps = 64;
    if(IsDigit(*Input))
    {
        NumSteps = 0;
        while(IsDigit(*Input)) NumSteps = NumSteps * 10 + *Input++ - '0';
        Input = SkipPastNewline(Input);
    }
    grid Grid;
    InitGrid(&Grid, Input);
    table Cache;
    InitTable(&Cache);
    int64_t Result = CountPlots(&Cache, &Grid, Grid.Start.X, Grid.Start.Y, NumSteps);
    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
