#include "aoc.h"
#include "parse.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

const char* DefaultInputPath = "d23.txt";

typedef struct
{
    size_t Capacity;
    size_t Count;
    char* Cells;
    int Width;
    int Height;
} grid;

static bool IsSlope(char C)
{
    return C == '^' || C == '>' || C == 'v' || C == '<';
}

static bool IsGrid(char C)
{
    return C == '#' || C == '.' || IsSlope(C);
}

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
    uint64_t Visited;
    int16_t X;
    int16_t Y;
    int16_t Dir;
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

static node MakeNode(uint64_t Visited, int16_t X, int16_t Y, int16_t Dir)
{
    node Node;
    memset(&Node, 0, sizeof(node));
    Node.Visited = Visited;
    Node.X = X;
    Node.Y = Y;
    Node.Dir = Dir;
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
    node Source = MakeNode(0, 1, 0, DIR_SOUTH);
    TableSet(&Dist, Source, 0);
    PriorityQueuePush(&Queue, Source, 0);
    while(Queue.Count > 0)
    {
        node Curr = PriorityQueuePop(&Queue);
        int64_t CurrDist;
        TableGet(&Dist, Curr, &CurrDist);
        int Dirs[4];
        int DirCount = 0;
        switch(Grid.Cells[Curr.Y * Grid.Width + Curr.X])
        {
        case '.':
            Dirs[0] = DIR_NORTH;
            Dirs[1] = DIR_EAST;
            Dirs[2] = DIR_SOUTH;
            Dirs[3] = DIR_WEST;
            DirCount = 4;
            break;
        case '^':
            Dirs[DirCount++] = DIR_NORTH;
            break;
        case '>':
            Dirs[DirCount++] = DIR_EAST;
            break;
        case 'v':
            Dirs[DirCount++] = DIR_SOUTH;
            break;
        case '<':
            Dirs[DirCount++] = DIR_WEST;
            break;
        }
        int OppositeDir = (Curr.Dir + 2) % NUM_DIRS;
        for(int DirIndex = 0; DirIndex < DirCount; DirIndex++)
        {
            int Dir = Dirs[DirIndex];
            if(Dir == OppositeDir) continue;
            node Neighbor = MakeNode(0, Curr.X + MoveX[Dir], Curr.Y + MoveY[Dir], Dir);
            if(Neighbor.X < 0 || Neighbor.X >= Grid.Width) continue;
            if(Neighbor.Y < 0 || Neighbor.Y >= Grid.Height) continue;
            char NeighborC = Grid.Cells[Neighbor.Y * Grid.Width + Neighbor.X];
            if(NeighborC == '#') continue;
            int64_t NeighborDist;
            if(!TableGet(&Dist, Neighbor, &NeighborDist))
            {
                NeighborDist = INT64_MAX;
            }
            int64_t AltDist = CurrDist - 1;
            if(AltDist < NeighborDist)
            {
                TableSet(&Dist, Neighbor, AltDist);
                PriorityQueuePush(&Queue, Neighbor, AltDist);
            }
        }
    }
    node Target = MakeNode(0, Grid.Width - 2, Grid.Height - 1, DIR_SOUTH);
    int64_t Result;
    TableGet(&Dist, Target, &Result);
    FreePriorityQueue(&Queue);
    FreeTable(&Dist);
    FreeGrid(&Grid);
    return -Result;
}

static bool FindBranchingPoint(grid* Grid, int16_t* Lookup, int X, int Y, int Dist, int InDir, int16_t* OutIndex, int16_t* OutDist)
{
    if(X < 0 || X >= Grid->Width) return false;
    if(Y < 0 || Y >= Grid->Height) return false;
    int Index =  Y * Grid->Width + X;
    char C = Grid->Cells[Index];
    if(C == '+')
    {
        *OutIndex = Lookup[Index];
        *OutDist = Dist;
        return true;
    }
    if(C == '#') return false;
    int OppositeDir = (InDir + 2) % NUM_DIRS;
    for(int Dir = 0; Dir < NUM_DIRS; Dir++)
    {
        if(Dir == OppositeDir) continue;
        if(FindBranchingPoint(Grid, Lookup, X + MoveX[Dir], Y + MoveY[Dir], Dist + 1, Dir, OutIndex, OutDist)) return true;
    }
    return false;
}

typedef struct
{
    int X;
    int Y;
    int NeighborCount;
    int16_t NeighborIndices[NUM_DIRS];
    int16_t NeighborDists[NUM_DIRS];
} branch;

AOC_SOLVER(Part2)
{
    grid Grid;
    InitGrid(&Grid, Input);

    // Remove all slopes.
    for(int Index = 0; Index < Grid.Count; Index++)
    {
        if(IsSlope(Grid.Cells[Index]))
        {
            Grid.Cells[Index] = '.';
        }
    }

    // Find branching points in the graph.
    size_t BranchCount = 0;
    size_t BranchCapacity = 64;
    branch* Branches = (branch*)malloc(sizeof(branch) * BranchCapacity);
    int16_t* BranchLookup = (int16_t*)calloc(Grid.Count, sizeof(int16_t));
    int GridIndex = 0;
    for(int Y = 0; Y < Grid.Height; Y++)
    {
        for(int X = 0; X < Grid.Width; X++, GridIndex++)
        {
            char C = Grid.Cells[GridIndex];
            int NumNeighbors = 0;
            if(C == '.')
            {
                if(Y > 0)
                {
                    NumNeighbors += Grid.Cells[GridIndex - Grid.Width] != '#';
                }
                if(X < Grid.Width - 1)
                {
                    NumNeighbors += Grid.Cells[GridIndex + 1] != '#';
                }
                if(Y < Grid.Height - 1)
                {
                    NumNeighbors += Grid.Cells[GridIndex + Grid.Width] != '#';
                }
                if(X > 0)
                {
                    NumNeighbors += Grid.Cells[GridIndex - 1] != '#';
                }
            }
            if(NumNeighbors > 2 || (X == 1 && Y == 0) || (X == Grid.Width - 2 && Y == Grid.Height - 1))
            {
                if(BranchCount == BranchCapacity)
                {
                    BranchCapacity *= 2;
                    Branches = (branch*)realloc(Branches, sizeof(branch) * BranchCapacity);
                }
                Branches[BranchCount] = (branch){.X = X, .Y = Y, .NeighborCount = 0};
                Grid.Cells[GridIndex] = '+';
                BranchLookup[GridIndex] = BranchCount++;
            }
        }
    }

    // Form a smaller graph consisting only of the branching points.
    for(int BranchIndex = 0; BranchIndex < BranchCount; BranchIndex++)
    {
        branch* Branch = &Branches[BranchIndex];
        for(int Dir = 0; Dir < NUM_DIRS; Dir++)
        {
            int16_t NeighborIndex, NeighborDist;
            if(FindBranchingPoint(&Grid, BranchLookup, Branch->X + MoveX[Dir], Branch->Y + MoveY[Dir], 1, Dir, &NeighborIndex, &NeighborDist))
            {
                Branch->NeighborIndices[Branch->NeighborCount] = NeighborIndex;
                Branch->NeighborDists[Branch->NeighborCount] = NeighborDist;
                Branch->NeighborCount++;
            }
        }
    }

    table Dist;
    InitTable(&Dist);
    priority_queue Queue;
    InitPriorityQueue(&Queue);
    node Source = MakeNode(1llu << 0, 0, 0, 0);
    TableSet(&Dist, Source, 0);
    PriorityQueuePush(&Queue, Source, 0);
    int64_t Result = INT64_MAX;
    int TargetIndex = BranchCount - 1;
    while(Queue.Count > 0)
    {
        node Curr = PriorityQueuePop(&Queue);
        int64_t CurrDist;
        TableGet(&Dist, Curr, &CurrDist);
        if(Curr.X == TargetIndex && CurrDist < Result)
        {
            Result = CurrDist;
        }
        branch Branch = Branches[Curr.X];
        for(int Index = 0; Index < Branch.NeighborCount; Index++)
        {
            int NeighborIndex = Branch.NeighborIndices[Index];
            uint64_t NeighborMask = 1llu << NeighborIndex;
            if(Curr.Visited & NeighborMask) continue;
            node Neighbor = MakeNode(Curr.Visited | NeighborMask, NeighborIndex, 0, 0);
            int64_t NeighborDist;
            if(!TableGet(&Dist, Neighbor, &NeighborDist))
            {
                NeighborDist = INT64_MAX;
            }
            int64_t AltDist = CurrDist - Branch.NeighborDists[Index];
            if(AltDist < NeighborDist)
            {
                TableSet(&Dist, Neighbor, AltDist);
                PriorityQueuePush(&Queue, Neighbor, AltDist);
            }
        }
    }
    FreePriorityQueue(&Queue);
    FreeTable(&Dist);
    free(BranchLookup);
    free(Branches);
    FreeGrid(&Grid);
    return -Result;
}
