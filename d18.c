#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d18.txt";

static bool IsDir(char C)
{
    return C == 'U' || C == 'R' || C == 'D' || C == 'L';
}

typedef struct
{
    int X;
    int Y;
} ivec2;

typedef struct
{
    int* Elements;
    size_t Count;
    size_t Capacity;
} edge_buffer;

static void InitEdgeBuffer(edge_buffer* Buffer)
{
    Buffer->Elements = NULL;
    Buffer->Count = 0;
    Buffer->Capacity = 0;
}

static void FreeEdgeBuffer(edge_buffer* Buffer)
{
    free(Buffer->Elements);
}

static void EdgeBufferAdd(edge_buffer* Buffer, int Element)
{
    size_t Capacity = Buffer->Capacity;
    if(Buffer->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Buffer->Elements = (int*)realloc(Buffer->Elements, sizeof(int) * Capacity);
        Buffer->Capacity = Capacity;
    }
    Buffer->Elements[Buffer->Count++] = Element;
}

static void EdgeBufferReset(edge_buffer* Buffer)
{
    Buffer->Count = 0;
}

static bool CompareCorners(ivec2 A, ivec2 B)
{
    return A.Y > B.Y || (A.Y == B.Y && A.X > B.X);
}

static void CornersSwap(ivec2* Corners, int FromIndex, int ToIndex)
{
    ivec2 Temp = Corners[ToIndex];
    Corners[ToIndex] = Corners[FromIndex];
    Corners[FromIndex] = Temp;
}

static int CornersPartition(ivec2* Corners, int LoIndex, int HiIndex)
{
    ivec2 Pivot = Corners[HiIndex];
    int PivotIndex = LoIndex - 1;
    for(int TestIndex = LoIndex; TestIndex < HiIndex; TestIndex++)
    {
        if(!CompareCorners(Corners[TestIndex], Pivot))
        {
            CornersSwap(Corners, ++PivotIndex, TestIndex);
        }
    }
    CornersSwap(Corners, ++PivotIndex, HiIndex);
    return PivotIndex;
}

static void CornersQuickSort(ivec2* Corners, int LoIndex, int HiIndex)
{
    if(LoIndex >= HiIndex || LoIndex < 0) return;
    int Pivot = CornersPartition(Corners, LoIndex, HiIndex);
    CornersQuickSort(Corners, LoIndex, Pivot - 1);
    CornersQuickSort(Corners, Pivot + 1, HiIndex);
}

static int64_t CalculateArea(ivec2* Corners, size_t CornerCount)
{
    // Sort the corners in row major order.
    CornersQuickSort(Corners, 0, CornerCount - 1);

    // Progressively calculate the filled area by determining the area of rows
    // of boxes formed between the unique Y values of the corners.
    int64_t Result = 0;
    edge_buffer Edges, NextEdges;
    InitEdgeBuffer(&Edges);
    InitEdgeBuffer(&NextEdges);
    int CornerIndex = 0;
    int Y = Corners[0].Y;
    while(Corners[CornerIndex].Y == Y)
    {
        EdgeBufferAdd(&Edges, Corners[CornerIndex++].X);
    }
    while(CornerIndex < CornerCount)
    {
        // Add the area of the boxes formed by the current edges to the result.
        int NextY = Corners[CornerIndex].Y;
        int64_t Height = NextY - Y;
        for(int EdgeIndex = 1; EdgeIndex < Edges.Count; EdgeIndex += 2)
        {
            int64_t Width = 1 + Edges.Elements[EdgeIndex] - Edges.Elements[EdgeIndex - 1];
            Result += Width * Height;
        }

        // Determine the next edges by merging the current edges with the edges
        // on the next unique Y. Duplicates cancel out.
        int EdgeIndex = 0;
        while(CornerIndex < CornerCount && Corners[CornerIndex].Y == NextY && EdgeIndex < Edges.Count)
        {
            int CornerX = Corners[CornerIndex].X;
            int EdgeX = Edges.Elements[EdgeIndex];
            if(CornerX > EdgeX)
            {
                EdgeBufferAdd(&NextEdges, EdgeX);
                EdgeIndex++;
            }
            else if(CornerX < EdgeX)
            {
                EdgeBufferAdd(&NextEdges, CornerX);
                CornerIndex++;
            }
            else
            {
                CornerIndex++;
                EdgeIndex++;
            }
        }
        while(CornerIndex < CornerCount && Corners[CornerIndex].Y == NextY)
        {
            EdgeBufferAdd(&NextEdges, Corners[CornerIndex++].X);
        }
        while(EdgeIndex < Edges.Count)
        {
            EdgeBufferAdd(&NextEdges, Edges.Elements[EdgeIndex++]);
        }

        // The boxes overlap on their bottom edge. Perform range intersections
        // to count any cells which are not overlapped by the next boxes. This
        // also accounts for the final row of cells, which are overlapped by
        // nothing.
        for(int EdgeIndex = 1; EdgeIndex < Edges.Count; EdgeIndex += 2)
        {
            int LeftEdge = Edges.Elements[EdgeIndex - 1];
            int RightEdge = Edges.Elements[EdgeIndex];
            Result += 1 + RightEdge - LeftEdge;
            for(int NextIndex = 1; NextIndex < NextEdges.Count; NextIndex += 2)
            {
                int NextLeftEdge = NextEdges.Elements[NextIndex - 1];
                if(NextLeftEdge > RightEdge) continue;
                int NextRightEdge = NextEdges.Elements[NextIndex];
                if(NextRightEdge < LeftEdge) continue;
                int LeftIntersect = NextLeftEdge > LeftEdge ? NextLeftEdge : LeftEdge;
                int RightIntersect = NextRightEdge < RightEdge ? NextRightEdge : RightEdge;
                Result -= 1 + RightIntersect - LeftIntersect;
            }
        }

        // Flip edge buffers for next row of boxes.
        Y = NextY;
        edge_buffer TempEdges = Edges;
        Edges = NextEdges;
        NextEdges = TempEdges;
        EdgeBufferReset(&NextEdges);
    }

    FreeEdgeBuffer(&Edges);
    FreeEdgeBuffer(&NextEdges);
    return Result;
}


AOC_SOLVER(Part1)
{
    size_t CornerCount = 0;
    size_t CornerCapacity = 16;
    ivec2* Corners = (ivec2*)malloc(sizeof(ivec2) * CornerCapacity);
    ivec2 At = (ivec2){.X = 0, .Y = 0};
    while(IsDir(*Input))
    {
        char Dir = *Input;
        Input += 2;
        int Dist = 0;
        while(IsDigit(*Input))
        {
            Dist = Dist * 10 + *Input - '0';
            Input++;
        }
        switch(Dir)
        {
        case 'R': At.X += Dist; break;
        case 'D': At.Y += Dist; break;
        case 'L': At.X -= Dist; break;
        case 'U': At.Y -= Dist; break;
        }
        if(CornerCount == CornerCapacity)
        {
            CornerCapacity *= 2;
            Corners = (ivec2*)realloc(Corners, sizeof(ivec2) * CornerCapacity);
        }
        Corners[CornerCount++] = At;
        Input = SkipPastNewline(Input + 10);
    }
    int64_t Result = CalculateArea(Corners, CornerCount);
    free(Corners);
    return Result;
}

AOC_SOLVER(Part2)
{
    size_t CornerCount = 0;
    size_t CornerCapacity = 16;
    ivec2* Corners = (ivec2*)malloc(sizeof(ivec2) * CornerCapacity);
    ivec2 At = (ivec2){.X = 0, .Y = 0};
    while(IsDir(*Input))
    {
        Input = SkipPastDigits(Input + 2) + 3;
        int Dist = 0;
        for(int Index = 0; Index < 5; Index++)
        {
            char C = Input[Index];
            Dist *= 16;
            if(C >= 'a')
            {
                Dist += 10 + C - 'a';
            }
            else
            {
                Dist += C - '0';
            }
        }
        switch(Input[5])
        {
        case '0': At.X += Dist; break;
        case '1': At.Y += Dist; break;
        case '2': At.X -= Dist; break;
        case '3': At.Y -= Dist; break;
        }
        if(CornerCount == CornerCapacity)
        {
            CornerCapacity *= 2;
            Corners = (ivec2*)realloc(Corners, sizeof(ivec2) * CornerCapacity);
        }
        Corners[CornerCount++] = At;
        Input = SkipPastNewline(Input + 7);
    }
    int64_t Result = CalculateArea(Corners, CornerCount);
    free(Corners);
    return Result;
}
