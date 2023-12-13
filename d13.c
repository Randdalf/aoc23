#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d13.txt";

static bool IsAsh(char C)
{
    return C == '.';
}

static bool IsRock(char C)
{
    return C == '#';
}

static bool IsPattern(char C)
{
    return IsAsh(C) || IsRock(C);
}

typedef struct
{
    uint32_t* Elements;
    size_t Count;
    size_t Capacity;
} slice_array;

static void InitSliceArray(slice_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

static void FreeSliceArray(slice_array* Array)
{
    free(Array->Elements);
}

static void SliceArrayAdd(slice_array* Array, uint32_t Element)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (uint32_t*)realloc(Array->Elements, sizeof(uint32_t) * Capacity);
        Array->Capacity = Capacity;
    }
    Array->Elements[Array->Count++] = Element;
}

static void SliceArrayReset(slice_array* Array)
{
    Array->Count = 0;
}

static int CheckReflection(slice_array* Slices)
{
    for(int SliceIndex = 0; SliceIndex < Slices->Count - 1; SliceIndex++)
    {
        int LeftIndex = SliceIndex;
        int RightIndex = SliceIndex + 1;
        for(; LeftIndex >= 0 && RightIndex < Slices->Count; RightIndex++, LeftIndex--)
        {
            if(Slices->Elements[LeftIndex] != Slices->Elements[RightIndex]) goto NextSlice;
        }
        return SliceIndex + 1;
    NextSlice:
        continue;
    }
    return 0;
}

AOC_SOLVER(Part1)
{
    int64_t SumCols = 0;
    int64_t SumRows = 0;
    slice_array Rows, Cols;
    InitSliceArray(&Rows);
    InitSliceArray(&Cols);
    while(IsPattern(*Input))
    {
        int Row = 0;
        while(IsPattern(*Input))
        {
            uint32_t RowSlice = 0;
            int Col = 0;
            while(IsPattern(*Input))
            {
                uint32_t IsRockBit = IsRock(*Input);
                if(Row == 0)
                {
                    SliceArrayAdd(&Cols, IsRockBit << Row);
                }
                else if(IsRockBit)
                {
                    Cols.Elements[Col] |= IsRockBit << Row;
                }
                RowSlice |= IsRockBit << Col;
                Input++;
                Col++;
            }
            SliceArrayAdd(&Rows, RowSlice);
            Input = SkipPastNewline(Input);
            Row++;
        }
        SumCols += CheckReflection(&Cols);
        SumRows += CheckReflection(&Rows);
        SliceArrayReset(&Rows);
        SliceArrayReset(&Cols);
        Input = SkipPastNewline(Input);
    }
    FreeSliceArray(&Rows);
    FreeSliceArray(&Cols);
    return SumCols + 100 * SumRows;
}

AOC_SOLVER(Part2)
{
    return -1;
}
