#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d11.txt";

static bool IsEmpty(char C)
{
    return C == '.';
}

static bool IsGalaxy(char C)
{
    return C == '#';
}

static bool IsImage(char C)
{
    return IsEmpty(C) || IsGalaxy(C);
}

static const char* SkipPastEmpty(const char* Input)
{
    while(IsEmpty(*Input)) Input++;
    return Input;
}

typedef struct
{
    uint64_t Bits[3];
} bit_set;

static void InitBitSet(bit_set* Set, uint8_t Byte)
{
    memset(Set, Byte, sizeof(bit_set));
}

static void BitSetClear(bit_set* Set, uint64_t Index)
{
    Set->Bits[Index / 64llu] &= ~(1llu << (Index % 64llu));
}

static bool BitSetContains(bit_set* Set, uint64_t Index)
{
    return (Set->Bits[Index / 64llu] & (1llu << (Index % 64llu))) != 0;
}

static int BitSetCount(bit_set* Set)
{
    return (int)(__popcnt64(Set->Bits[0]) + __popcnt64(Set->Bits[1]) + __popcnt64(Set->Bits[2]));
}

typedef struct
{
    int64_t X;
    int64_t Y;
} ivec2;

static int64_t Solve(const char* Input, int64_t Expansion)
{
    // Scan the input, to determine which rows are empty and which contain
    // galaxies.
    bit_set EmptyRows, EmptyCols;
    InitBitSet(&EmptyRows, 0xFF);
    InitBitSet(&EmptyCols, 0xFF);
    int GalaxyCount = 0;
    const char* At = Input;
    int Row = 0;
    while(IsImage(*At))
    {
        int Col = 0;
        while(IsImage(*At))
        {
            if(IsGalaxy(*At))
            {
                BitSetClear(&EmptyCols, Col);
                BitSetClear(&EmptyRows, Row);
                GalaxyCount++;
            }
            Col++;
            At++;
        }
        At = SkipPastNewline(At);
        Row++;
    }

    // Scan the input again, calculating the positions of the galaxies with the
    // rows expanded.
    ivec2* Galaxies = (ivec2*)malloc(sizeof(ivec2) * GalaxyCount);
    GalaxyCount = 0;
    Row = 0;
    At = Input;
    int64_t ExpandedRow = 0;
    while(IsImage(*At))
    {
        if(BitSetContains(&EmptyRows, Row))
        {
            At = SkipPastEmpty(At);
            ExpandedRow += Expansion;
        }
        else
        {
            int Col = 0;
            int64_t ExpandedCol = 0;
            while(IsImage(*At))
            {
                if(BitSetContains(&EmptyCols, Col))
                {
                    ExpandedCol += Expansion;
                }
                else
                {
                    if(IsGalaxy(*At))
                    {
                        Galaxies[GalaxyCount++] = (ivec2){.X = ExpandedCol, .Y = ExpandedRow};
                    }
                    ExpandedCol++;
                }
                Col++;
                At++;
            }
            ExpandedRow++;
        }
        At = SkipPastNewline(At);
        Row++;
    }

    // Calculate the manhattan distance between galaxies.
    uint64_t Sum = 0;
    for(int FromIndex = 0; FromIndex < GalaxyCount; FromIndex++)
    {
        ivec2 From = Galaxies[FromIndex];
        for(int ToIndex = FromIndex + 1; ToIndex < GalaxyCount; ToIndex++)
        {
            ivec2 To = Galaxies[ToIndex];
            Sum += llabs(To.X - From.X) + llabs(To.Y - From.Y);
        }
    }

    free(Galaxies);
    return Sum;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, 2);
}

AOC_SOLVER(Part2)
{
    return Solve(Input, 1000000);
}
