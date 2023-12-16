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

static int64_t Simulate(grid* Grid, beam_array* BeamStack, uint8_t* Visited)
{
    memset(Visited, 0, sizeof(uint8_t) * Grid->Count);
    while(BeamStack->Count > 0)
    {
        beam Beam = BeamArrayPop(BeamStack);
        if(Beam.X < 0 || Beam.Y < 0) continue;
        if(Beam.X >= Grid->Width || Beam.Y >= Grid->Height) continue;
        int Index = Beam.Y * Grid->Width + Beam.X;
        uint8_t DirMask = 1 << Beam.Dir;
        if(Visited[Index] & DirMask) continue;
        Visited[Index] |= DirMask;
        switch(Grid->Cells[Index])
        {
        case '.':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(BeamStack, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_DOWN:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_LEFT:
                BeamArrayAdd(BeamStack, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            }
            break;
        case '\\':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(BeamStack, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_DOWN:
                BeamArrayAdd(BeamStack, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            }
            break;
        case '/':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(BeamStack, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_DOWN:
                BeamArrayAdd(BeamStack, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            }
            break;
        case '|':
            switch(Beam.Dir)
            {
            case DIR_UP:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y - 1, DIR_UP);
                break;
            case DIR_RIGHT:
            case DIR_LEFT:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y - 1, DIR_UP);
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            case DIR_DOWN:
                BeamArrayAdd(BeamStack, Beam.X, Beam.Y + 1, DIR_DOWN);
                break;
            }
            break;
        case '-':
            switch(Beam.Dir)
            {
            case DIR_UP:
            case DIR_DOWN:
                BeamArrayAdd(BeamStack, Beam.X - 1, Beam.Y, DIR_LEFT);
                BeamArrayAdd(BeamStack, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_RIGHT:
                BeamArrayAdd(BeamStack, Beam.X + 1, Beam.Y, DIR_RIGHT);
                break;
            case DIR_LEFT:
                BeamArrayAdd(BeamStack, Beam.X - 1, Beam.Y, DIR_LEFT);
                break;
            }
            break;
        }
    }
    uint64_t Energized = 0;
    for(int Index = 0; Index < Grid->Count; Index++)
    {
        Energized += Visited[Index] != 0;
    }
    return Energized;
}

AOC_SOLVER(Part1)
{
    grid Grid;
    InitGrid(&Grid, Input);
    uint8_t* Visited = (uint8_t*)malloc(sizeof(uint8_t) * Grid.Count);
    beam_array BeamStack;
    InitBeamArray(&BeamStack);
    BeamArrayAdd(&BeamStack, 0, 0, DIR_RIGHT);
    int64_t Result = Simulate(&Grid, &BeamStack, Visited);
    FreeBeamArray(&BeamStack);
    free(Visited);
    FreeGrid(&Grid);
    return Result;
}

static int64_t Max(int64_t A, int64_t B)
{
    return A > B ? A : B;
}

AOC_SOLVER(Part2)
{
    grid Grid;
    InitGrid(&Grid, Input);
    uint8_t* Visited = (uint8_t*)malloc(sizeof(uint8_t) * Grid.Count);
    beam_array BeamStack;
    InitBeamArray(&BeamStack);
    int64_t Result = 0;
    for(int X = 0; X < Grid.Width; X++)
    {
        BeamArrayAdd(&BeamStack, X, 0, DIR_DOWN);
        Result = Max(Result, Simulate(&Grid, &BeamStack, Visited));
        BeamArrayAdd(&BeamStack, X, Grid.Height - 1, DIR_UP);
        Result = Max(Result, Simulate(&Grid, &BeamStack, Visited));
    }
    for(int Y = 0; Y < Grid.Height; Y++)
    {
        BeamArrayAdd(&BeamStack, 0, Y, DIR_RIGHT);
        Result = Max(Result, Simulate(&Grid, &BeamStack, Visited));
        BeamArrayAdd(&BeamStack, Grid.Width - 1, Y, DIR_LEFT);
        Result = Max(Result, Simulate(&Grid, &BeamStack, Visited));
    }
    FreeBeamArray(&BeamStack);
    free(Visited);
    FreeGrid(&Grid);
    return Result;
}
