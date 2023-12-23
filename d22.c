#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d22.txt";

typedef struct
{
    int X;
    int Y;
    int Z;
} coord;

typedef struct
{
    coord Start;
    coord End;
    int Unsafe;
    int LastSupportedIndex;
} brick;

typedef struct
{
    brick* Elements;
    size_t Count;
    size_t Capacity;
} brick_array;

typedef struct
{
    int BrickIndex;
    int Height;
} depth_cell;

static void InitBrickArray(brick_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

static void FreeBrickArray(brick_array* Array)
{
    free(Array->Elements);
}

static void BrickArrayAdd(brick_array* Array, coord Start, coord End)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (brick*)realloc(Array->Elements, sizeof(brick) * Capacity);
        Array->Capacity = Capacity;
    }
    Array->Elements[Array->Count++] = (brick){.Start = Start, .End = End};
}

static void BricksSwap(brick* Bricks, int FromIndex, int ToIndex)
{
    brick Temp = Bricks[ToIndex];
    Bricks[ToIndex] = Bricks[FromIndex];
    Bricks[FromIndex] = Temp;
}

static int BricksPartition(brick* Bricks, int LoIndex, int HiIndex)
{
    brick Pivot = Bricks[HiIndex];
    int PivotIndex = LoIndex - 1;
    for(int TestIndex = LoIndex; TestIndex < HiIndex; TestIndex++)
    {
        if(Bricks[TestIndex].Start.Z < Pivot.Start.Z)
        {
            BricksSwap(Bricks, ++PivotIndex, TestIndex);
        }
    }
    BricksSwap(Bricks, ++PivotIndex, HiIndex);
    return PivotIndex;
}

static void BricksQuickSort(brick* Bricks, int LoIndex, int HiIndex)
{
    if(LoIndex >= HiIndex || LoIndex < 0) return;
    int Pivot = BricksPartition(Bricks, LoIndex, HiIndex);
    BricksQuickSort(Bricks, LoIndex, Pivot - 1);
    BricksQuickSort(Bricks, Pivot + 1, HiIndex);
}

static void BrickArraySort(brick_array* Array)
{
    BricksQuickSort(Array->Elements, 0, Array->Count - 1);
}

static const char* ParseNumber(const char* Input, int* OutNumber)
{
    int Number = 0;
    while(IsDigit(*Input)) Number = Number * 10 + *Input++ - '0';
    *OutNumber = Number;
    return Input;
}

static const char* ParseCoord(const char* Input, coord* OutCoord)
{
    Input = ParseNumber(Input, &OutCoord->X) + 1;
    Input = ParseNumber(Input, &OutCoord->Y) + 1;
    return ParseNumber(Input, &OutCoord->Z);
}

static inline int Max(int A, int B)
{
    return A > B ? A : B;
}

AOC_SOLVER(Part1)
{
    // Parse the bricks into an array and sort them by their height off the
    // ground.
    brick_array Bricks;
    InitBrickArray(&Bricks);
    int MaxX = INT32_MIN;
    int MaxY = INT32_MIN;
    while(IsDigit(*Input))
    {
        coord Start, End;
        Input = ParseCoord(Input, &Start) + 1;
        Input = ParseCoord(Input, &End);
        Input = SkipPastNewline(Input);
        MaxX = Max(Start.X, Max(End.X, MaxX));
        MaxY = Max(Start.Y, Max(End.Y, MaxY));
        BrickArrayAdd(&Bricks, Start, End);
    }
    BrickArraySort(&Bricks);

    // Simulate the bricks falling downwards, lowest first, using a depth
    // buffer style map to record the highest brick at each point.
    int Width = MaxX + 1;
    int Height = MaxY + 1;
    depth_cell* DepthMap = (depth_cell*)calloc(Width * Height, sizeof(depth_cell));
    for(int BrickIndex = 0; BrickIndex < Bricks.Count; BrickIndex++)
    {
        brick* Brick = &Bricks.Elements[BrickIndex];

        // Find the highest point underneath the brick.
        int MaxHeight = 0;
        for(int Y = Brick->Start.Y; Y <= Brick->End.Y; Y++)
        {
            for(int X = Brick->Start.X; X <= Brick->End.X; X++)
            {
                MaxHeight = Max(MaxHeight, DepthMap[Y * Width + X].Height);
            }
        }

        // Move the brick downwards so it is touching the highest brick(s)
        // below it.
        int ZOffset = Brick->Start.Z - MaxHeight - 1;
        Brick->Start.Z -= ZOffset;
        Brick->End.Z -= ZOffset;

        // Blit the brick into the depth map, and simultaneously count all the
        // bricks supporting it.
        int NumSupporting = 0;
        int LastSupportingIndex = -1;
        for(int Y = Brick->Start.Y; Y <= Brick->End.Y; Y++)
        {
            for(int X = Brick->Start.X; X <= Brick->End.X; X++)
            {
                depth_cell* Cell = &DepthMap[Y * Width + X];
                if(MaxHeight > 0
                    && Cell->Height == MaxHeight
                    && Bricks.Elements[Cell->BrickIndex].LastSupportedIndex != BrickIndex)
                {
                    NumSupporting++;
                    LastSupportingIndex = Cell->BrickIndex;
                    Bricks.Elements[Cell->BrickIndex].LastSupportedIndex = BrickIndex;
                }
                Cell->Height = Brick->End.Z;
                Cell->BrickIndex = BrickIndex;
            }
        }

        // If this brick has only one support, mark that support as unsafe.
        if(NumSupporting == 1)
        {
            Bricks.Elements[LastSupportingIndex].Unsafe = 1;
        }

        // Take this opportunity to reset the last supported index (all bricks
        // this one can support are sorted after it in the bricks array).
        Brick->LastSupportedIndex = -1;
    }

    // Subtract unsafe bricks to count bricks that can be safely disintegrated.
    int64_t Result = Bricks.Count;
    for(int BrickIndex = 0; BrickIndex < Bricks.Count; BrickIndex++)
    {
        Result -= Bricks.Elements[BrickIndex].Unsafe;
    }

    FreeBrickArray(&Bricks);
    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
