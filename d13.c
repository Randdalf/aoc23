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

typedef int (*check_reflection_fn)(slice_array* Slices, int Width);

static int CheckReflectionWithSkip(slice_array* Slices, int SkipSliceIndex)
{
    for(int SliceIndex = 0; SliceIndex < Slices->Count - 1; SliceIndex++)
    {
        if(SliceIndex == SkipSliceIndex) continue;
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

static int CheckReflection(slice_array* Slices, int Width)
{
    AOC_UNUSED(Width);
    return CheckReflectionWithSkip(Slices, -1);
}

static int CheckReflectionWithSmudge(slice_array* Slices, int Width)
{
    int SkipSliceIndex = CheckReflection(Slices, Width) - 1;
    for(int SliceIndex = 0; SliceIndex < Slices->Count; SliceIndex++)
    {
        for(int BitIndex = 0; BitIndex < Width; BitIndex++)
        {
            uint32_t Flipper = 1 << BitIndex;
            Slices->Elements[SliceIndex] ^= Flipper;
            int Result = CheckReflectionWithSkip(Slices, SkipSliceIndex);
            Slices->Elements[SliceIndex] ^= Flipper;
            if(Result) return Result;
        }
    }
    return 0;
}

static int64_t Solve(const char* Input, check_reflection_fn CheckReflectionFn)
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
        SumCols += CheckReflectionFn(&Cols, Rows.Count);
        SumRows += CheckReflectionFn(&Rows, Cols.Count);
        SliceArrayReset(&Rows);
        SliceArrayReset(&Cols);
        Input = SkipPastNewline(Input);
    }
    FreeSliceArray(&Rows);
    FreeSliceArray(&Cols);
    return SumCols + 100 * SumRows;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, CheckReflection);
}

AOC_SOLVER(Part2)
{
    return Solve(Input, CheckReflectionWithSmudge);
}
