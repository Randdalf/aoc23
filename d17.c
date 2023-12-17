#include "aoc.h"
#include "parse.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

const char* DefaultInputPath = "d17.txt";

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
    while(IsDigit(*Input))
    {
        while(IsDigit(*Input))
        {
            if(Grid->Count == Grid->Capacity)
            {
                Grid->Capacity *= 2;
                Grid->Cells = (char*)realloc(Grid->Cells, sizeof(char) * Grid->Capacity);
            }
            Grid->Cells[Grid->Count++] = *(Input++) - '0';
        }
        Input = SkipPastNewline(Input);
        Grid->Height++;
    }
    Grid->Width = Grid->Count / Grid->Height;
}

static void FreeGrid(grid* Grid)
{
    free(Grid->Cells);
}

enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    NUM_DIRS
};

static int MoveX[NUM_DIRS] = {0, 1, 0, -1};
static int MoveY[NUM_DIRS] = {-1, 0, 1, 0};

typedef struct
{
    int X;
    int Y;
    int Dir;
    int Cons;
} node;

typedef node key;
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

typedef struct
{
    node Node;
    int64_t Dist;
} entry;

typedef struct
{
    entry* Entries;
    size_t Count;
    size_t Capacity;
} priority_queue;

static void InitPriorityQueue(priority_queue* Queue)
{
    Queue->Entries = NULL;
    Queue->Count = 0;
    Queue->Capacity = 0;
}

static void FreePriorityQueue(priority_queue* Queue)
{
    free(Queue->Entries);
}

static void PriorityQueuePush(priority_queue* Queue, node Node, int64_t Dist)
{
    size_t Capacity = Queue->Capacity;
    if(Queue->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Queue->Entries = (entry*)realloc(Queue->Entries, sizeof(entry) * Capacity);
        Queue->Capacity = Capacity;
    }
    int Index = Queue->Count++;
    int ParentIndex = (Index - 1) / 2;
    while(Index > 0 && Queue->Entries[ParentIndex].Dist > Dist)
    {
        Queue->Entries[Index] = Queue->Entries[ParentIndex];
        Index = ParentIndex;
        ParentIndex = (Index - 1) / 2;
    }
    Queue->Entries[Index] = (entry){.Node = Node, .Dist = Dist};
}

static node PriorityQueuePop(priority_queue* Queue)
{
    node Node = Queue->Entries[0].Node;
    Queue->Entries[0] = Queue->Entries[--Queue->Count];
    int Index = 0;
    for(;;)
    {
        int MinIndex = Index;
        int LeftIndex = 2 * Index + 1;
        if(LeftIndex < Queue->Count && Queue->Entries[LeftIndex].Dist < Queue->Entries[MinIndex].Dist)
        {
            MinIndex = LeftIndex;
        }
        int RightIndex = LeftIndex + 1;
        if(RightIndex < Queue->Count && Queue->Entries[RightIndex].Dist < Queue->Entries[MinIndex].Dist)
        {
            MinIndex = RightIndex;
        }
        if(MinIndex == Index) break;
        entry Temp = Queue->Entries[MinIndex];
        Queue->Entries[MinIndex] = Queue->Entries[Index];
        Queue->Entries[Index] = Temp;
        Index = MinIndex;
    }
    return Node;
}

AOC_SOLVER(Part1)
{
    grid Grid;
    InitGrid(&Grid, Input);
    table Dist;
    InitTable(&Dist);
    priority_queue Queue;
    InitPriorityQueue(&Queue);
    node Source = (node){.X = 0, .Y = 0, .Dir = DIR_EAST, .Cons = 0};
    TableSet(&Dist, Source, 0);
    PriorityQueuePush(&Queue, Source, 0);
    int TargetX = Grid.Width - 1;
    int TargetY = Grid.Height - 1;
    int64_t Result = 0;
    while(Queue.Count > 0)
    {
        node Curr = PriorityQueuePop(&Queue);
        int64_t CurrDist;
        TableGet(&Dist, Curr, &CurrDist);
        if(Curr.X == TargetX && Curr.Y == TargetY)
        {
            Result = CurrDist;
            break;
        }
        static int Dirs[3];
        Dirs[0] = (Curr.Dir + 3) % 4;
        Dirs[1] = Curr.Dir;
        Dirs[2] = (Curr.Dir + 1) % 4;
        for(int DirIndex = 0; DirIndex < 3; DirIndex++)
        {
            int Dir = Dirs[DirIndex];
            node Neighbor;
            Neighbor.X = Curr.X + MoveX[Dir];
            Neighbor.Y = Curr.Y + MoveY[Dir];
            Neighbor.Dir = Dir;
            Neighbor.Cons = Dir == Curr.Dir ? Curr.Cons + 1 : 1;
            if(Neighbor.Cons > 3) continue;
            if(Neighbor.X < 0 || Neighbor.X > TargetX) continue;
            if(Neighbor.Y < 0 || Neighbor.Y > TargetX) continue;
            int64_t NeighborDist;
            if(!TableGet(&Dist, Neighbor, &NeighborDist))
            {
                NeighborDist = INT64_MAX;
            }
            int64_t AltDist = CurrDist + Grid.Cells[Neighbor.Y * Grid.Width + Neighbor.X];
            if(AltDist < NeighborDist)
            {
                TableSet(&Dist, Neighbor, AltDist);
                PriorityQueuePush(&Queue, Neighbor, AltDist);
            }
        }
    }
    FreePriorityQueue(&Queue);
    FreeTable(&Dist);
    FreeGrid(&Grid);
    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
