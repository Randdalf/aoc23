#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d16.txt";

typedef struct
{
    size_t Capacity;
    size_t Count;
    char* Cells;
    int Width;
    int Height;
} grid;

static bool IsGrid(char C)
{
    switch(C)
    {
    case '.': case '\\': case '/': case '|': case '-':
        return true;
    default:
        return false;
    }
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
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
};

typedef struct
{
    int X;
    int Y;
    int Dir;
} beam;

typedef struct
{
    beam* Elements;
    size_t Count;
    size_t Capacity;
} beam_array;

static void InitBeamArray(beam_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

static void FreeBeamArray(beam_array* Array)
{
    free(Array->Elements);
}

static void BeamArrayAdd(beam_array* Array, int X, int Y, int Dir)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (beam*)realloc(Array->Elements, sizeof(beam) * Capacity);
        Array->Capacity = Capacity;
    }
    Array->Elements[Array->Count++] = (beam){.X = X, .Y = Y, .Dir = Dir};
}

static beam BeamArrayPop(beam_array* Array)
{
    return Array->Elements[--Array->Count];
}

AOC_SOLVER(Part1)
{
    grid Grid;
    InitGrid(&Grid, Input);
    uint8_t* Marked = (uint8_t*)calloc(Grid.Count, sizeof(uint8_t));
    beam_array Pending;
    InitBeamArray(&Pending);
    BeamArrayAdd(&Pending, 0, 0, DIR_RIGHT);
    while(Pending.Count > 0)
    {
        beam Beam = BeamArrayPop(&Pending);
        if(Beam.X < 0 || Beam.Y < 0) continue;
        if(Beam.X >= Grid.Width || Beam.Y >= Grid.Height) continue;
        int Index = Beam.Y * Grid.Width + Beam.X;
        uint8_t DirMask = 1 << Beam.Dir;
        if(Marked[Index] & DirMask) continue;
        Marked[Index] |= DirMask;
        switch(Grid.Cells[Index])
        {
        case '.':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(&Pending, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_DOWN:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_LEFT:
                BeamArrayAdd(&Pending, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            }
            break;
        case '\\':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(&Pending, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_DOWN:
                BeamArrayAdd(&Pending, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            }
            break;
        case '/':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(&Pending, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_DOWN:
                BeamArrayAdd(&Pending, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            }
            break;
        case '|':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_RIGHT:
            case DIR_LEFT:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y - 1, DIR_UP);
                BeamArrayAdd(&Pending, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_DOWN:
                BeamArrayAdd(&Pending, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            }
            break;
        case '-':
            switch(Beam.Dir)
            {
            case DIR_UP:
            case DIR_DOWN:
                BeamArrayAdd(&Pending, Beam.X - 1, Beam.Y, DIR_LEFT);
                BeamArrayAdd(&Pending, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(&Pending, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(&Pending, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            }
            break;
        }
    }
    uint64_t Energized = 0;
    for(int Index = 0; Index < Grid.Count; Index++)
    {
        Energized += Marked[Index] != 0;
    }
    FreeBeamArray(&Pending);
    free(Marked);
    FreeGrid(&Grid);
    return Energized;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
