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
    int64_t X;
    int64_t Y;
} key;

typedef uint8_t value;

typedef struct
{
    size_t Count;
    size_t Capacity;
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

static int64_t Solve(const char* Input, int64_t NumSteps)
{
    if(IsDigit(*Input))
    {
        NumSteps = 0;
        while(IsDigit(*Input)) NumSteps = NumSteps * 10 + *Input++ - '0';
        Input = SkipPastNewline(Input);
    }
    grid Grid;
    InitGrid(&Grid, Input);
    table Tables[2];
    InitTable(&Tables[0]);
    InitTable(&Tables[1]);
    int TableIndex = 0;
    TableSet(&Tables[TableIndex], (key){.X = Grid.Start.X, .Y = Grid.Start.Y}, 1);
    for(int64_t Step = 0; Step < NumSteps; Step++)
    {
        table* From = &Tables[TableIndex];
        table* To = &Tables[1 - TableIndex];
        TableReset(To);
        TableIndex = 1 - TableIndex;
        for(int Index = 0; Index < From->Capacity; Index++)
        {
            if(!From->Slots[Index]) continue;
            key Key = From->Keys[Index];
            if(Key.Y > 0)
            {
                key North = (key){.X = Key.X, .Y = Key.Y - 1};
                if(Grid.Cells[North.Y * Grid.Width + North.X] != '#')
                {
                    TableSet(To, North, 1);
                }
            }
            if(Key.X < Grid.Width - 1)
            {
                key East = (key){.X = Key.X + 1, .Y = Key.Y};
                if(Grid.Cells[East.Y * Grid.Width + East.X] != '#')
                {
                    TableSet(To, East, 1);
                }
            }
            if(Key.Y < Grid.Height - 1)
            {
                key South = (key){.X = Key.X, .Y = Key.Y + 1};
                if(Grid.Cells[South.Y * Grid.Width + South.X] != '#')
                {
                    TableSet(To, South, 1);
                }
            }
            if(Key.X > 0)
            {
                key West = (key){.X = Key.X - 1, .Y = Key.Y};
                if(Grid.Cells[West.Y * Grid.Width + West.X] != '#')
                {
                    TableSet(To, West, 1);
                }
            }
        }
    }
    return Tables[TableIndex].Count;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, 64);
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
