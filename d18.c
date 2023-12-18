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

AOC_SOLVER(Part1)
{
    // Parse the input into a list of all outer trench locations.
    size_t Count = 0;
    size_t Capacity = 16;
    ivec2* Stack = (ivec2*)malloc(sizeof(ivec2) * Capacity);
    ivec2 At = (ivec2){.X = 0, .Y = 0};
    Stack[Count++] = At;
    ivec2 Min = At;
    ivec2 Max = At;
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
        int StepX, StepY;
        switch(Dir)
        {
        case 'U': StepX = 0; StepY = -1; break;
        case 'R': StepX = 1; StepY = 0; break;
        case 'D': StepX = 0; StepY = 1; break;
        case 'L': StepX = -1; StepY = 0; break;
        }
        for(int Step = 1; Step <= Dist; Step++)
        {
            At.X += StepX;
            At.Y += StepY;
            if(Count == Capacity)
            {
                Capacity *= 2;
                Stack = (ivec2*)realloc(Stack, sizeof(ivec2) * Capacity);
            }
            Stack[Count++] = At;
        }
        switch(Dir)
        {
        case 'U': if(At.Y < Min.Y) Min.Y = At.Y; break;
        case 'R': if(At.X > Max.X) Max.X = At.X; break;
        case 'D': if(At.Y > Max.Y) Max.Y = At.Y; break;
        case 'L': if(At.X < Min.X) Min.X = At.X; break;
        }
        Input = SkipPastNewline(Input + 10);
    }

    // Normalize the trench locations and mark them on a grid.
    int Width = 1 + Max.X - Min.X;
    int Height = 1 + Max.Y - Min.Y;
    size_t GridCount = Width * Height;
    size_t GridSize = sizeof(uint8_t) * GridCount;
    uint8_t* Grid = (uint8_t*)malloc(GridSize);
    memset(Grid, '.', GridSize);
    for(int TrenchIndex = 0; TrenchIndex < Count; TrenchIndex++)
    {
        ivec2 Trench = Stack[TrenchIndex];
        Grid[(Trench.Y - Min.Y) * Width + (Trench.X - Min.X)] = '#';
    }

    // Flood fill from the edges of the grid, leaving any unmarked tiles as
    // those which are contained within the outer trench. (Recycle the already
    // allocated stack to store pending locations.)
    Count = 0;
    for(int X = 0; X < Width; X++)
    {
        Stack[Count++] = (ivec2){.X = X, .Y = 0};
        Stack[Count++] = (ivec2){.X = X, .Y = Height - 1};
    }
    for(int Y = 1; Y < Height - 1; Y++)
    {
        Stack[Count++] = (ivec2){.X = 0, .Y = Y};
        Stack[Count++] = (ivec2){.X = Width - 1, .Y = Y};
    }
    while(Count > 0)
    {
        ivec2 At = Stack[--Count];
        int Index = At.Y * Width + At.X;
        if(Grid[Index] != '.') continue;
        Grid[Index] = '*';
        if(Count + 4 > Capacity)
        {
            Capacity *= 2;
            Stack = (ivec2*)realloc(Stack, sizeof(ivec2) * Capacity);
        }
        if(At.Y > 0)
        {
            Stack[Count++] = (ivec2){.X = At.X, .Y = At.Y - 1};
        }
        if(At.X < Width - 1)
        {
            Stack[Count++] = (ivec2){.X = At.X + 1, .Y = At.Y};
        }
        if(At.Y < Height - 1)
        {
            Stack[Count++] = (ivec2){.X = At.X, .Y = At.Y + 1};
        }
        if(At.X > 0)
        {
            Stack[Count++] = (ivec2){.X = At.X - 1, .Y = At.Y};
        }
    }

    // Count all the trenches.
    int64_t Result = Count;
    for(int GridIndex = 0; GridIndex < GridCount; GridIndex++)
    {
        Result += (Grid[GridIndex] == '#') + (Grid[GridIndex] == '.');
    }

    free(Grid);
    free(Stack);
    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
