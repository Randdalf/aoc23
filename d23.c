#include "aoc.h"
#include "parse.h"

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
    int16_t X;
    int16_t Y;
    int16_t NeighborCount;
    int16_t NeighborIndices[NUM_DIRS];
    int16_t NeighborDists[NUM_DIRS];
} node;

static bool FindBranchingPoint(grid* Grid, int16_t* Lookup, int X, int Y, int Dist, int InDir, int16_t* OutIndex, int16_t* OutDist)
{
    if(X < 0 || X >= Grid->Width) return false;
    if(Y < 0 || Y >= Grid->Height) return false;
    int Index =  Y * Grid->Width + X;
    char C = Grid->Cells[Index];
    switch(C)
    {
    case '#':
        return false;
    case '+':
        *OutIndex = Lookup[Index];
        *OutDist = Dist;
        return true;
    case '^':
        if(InDir != DIR_NORTH) return false;
        break;
    case '>':
        if(InDir != DIR_EAST) return false;
        break;
    case 'v':
        if(InDir != DIR_SOUTH) return false;
        break;
    case '<':
        if(InDir != DIR_WEST) return false;
        break;
    }
    int OppositeDir = (InDir + 2) % NUM_DIRS;
    for(int Dir = 0; Dir < NUM_DIRS; Dir++)
    {
        if(Dir == OppositeDir) continue;
        if(FindBranchingPoint(Grid, Lookup, X + MoveX[Dir], Y + MoveY[Dir], Dist + 1, Dir, OutIndex, OutDist)) return true;
    }
    return false;
}

static int64_t FindLongestPath(node* Nodes, int NodeIndex, int TargetNodeIndex, uint64_t Visited, int Dist)
{
    if(NodeIndex == TargetNodeIndex) return Dist;
    int64_t MaxDist = 0;
    node Node = Nodes[NodeIndex];
    for(int Index = 0; Index < Node.NeighborCount; Index++)
    {
        int NeighborIndex = Node.NeighborIndices[Index];
        uint64_t NeighborMask = 1ull << NeighborIndex;
        if(Visited & NeighborMask) continue;
        int NeighborDist = FindLongestPath(Nodes, NeighborIndex, TargetNodeIndex, Visited | NeighborMask, Dist + Node.NeighborDists[Index]);
        MaxDist = NeighborDist > MaxDist ? NeighborDist : MaxDist;
    }
    return MaxDist;
}

static int64_t Solve(const char* Input, bool RemoveSlopes)
{
    grid Grid;
    InitGrid(&Grid, Input);

    // Remove all slopes if required.
    if(RemoveSlopes)
    {
        for(int Index = 0; Index < Grid.Count; Index++)
        {
            if(IsSlope(Grid.Cells[Index]))
            {
                Grid.Cells[Index] = '.';
            }
        }
    }

    // Find branching points in the graph.
    size_t NodeCount = 0;
    size_t NodeCapacity = 64;
    node* Nodes = (node*)malloc(sizeof(node) * NodeCapacity);
    int16_t* NodeLookup = (int16_t*)calloc(Grid.Count, sizeof(int16_t));
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
                NumNeighbors += Grid.Cells[GridIndex + 1] != '#';
                if(Y < Grid.Height - 1)
                {
                    NumNeighbors += Grid.Cells[GridIndex + Grid.Width] != '#';
                }
                NumNeighbors += Grid.Cells[GridIndex - 1] != '#';
            }
            if(NumNeighbors > 2 || (X == 1 && Y == 0) || (X == Grid.Width - 2 && Y == Grid.Height - 1))
            {
                if(NodeCount == NodeCapacity)
                {
                    NodeCapacity *= 2;
                    Nodes = (node*)realloc(Nodes, sizeof(node) * NodeCapacity);
                }
                Nodes[NodeCount] = (node){.X = X, .Y = Y, .NeighborCount = 0};
                Grid.Cells[GridIndex] = '+';
                NodeLookup[GridIndex] = NodeCount++;
            }
        }
    }

    // Form a smaller graph consisting only of the branching points.
    for(int NodeIndex = 0; NodeIndex < NodeCount; NodeIndex++)
    {
        node* Node = &Nodes[NodeIndex];
        for(int Dir = 0; Dir < NUM_DIRS; Dir++)
        {
            int16_t NeighborIndex, NeighborDist;
            if(FindBranchingPoint(&Grid, NodeLookup, Node->X + MoveX[Dir], Node->Y + MoveY[Dir], 1, Dir, &NeighborIndex, &NeighborDist))
            {
                Node->NeighborIndices[Node->NeighborCount] = NeighborIndex;
                Node->NeighborDists[Node->NeighborCount] = NeighborDist;
                Node->NeighborCount++;
            }
        }
    }

    // Find the longest path using depth-first search.
    int64_t Result = FindLongestPath(Nodes, 0, NodeCount - 1, 1ull, 0);

    free(NodeLookup);
    free(Nodes);
    FreeGrid(&Grid);
    return Result;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, false);
}

AOC_SOLVER(Part2)
{
    return Solve(Input, true);
}
