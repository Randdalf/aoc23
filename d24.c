#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d24.txt";

typedef struct
{
    double X;
    double Y;
    double Z;
} vec3;

typedef struct
{
    vec3 Position;
    vec3 Velocity;
} hailstone;

const char* ParseNumber(const char* Input, double* OutNumber)
{
    *OutNumber = strtod(Input, (char**)&Input);
    return Input;
}

const char* ParseVector(const char* Input, vec3* OutVector)
{
    Input = ParseNumber(Input, &OutVector->X);
    Input = ParseNumber(Input + 2, &OutVector->Y);
    return ParseNumber(Input + 2, &OutVector->Z);
}

AOC_SOLVER(Part1)
{
    double RangeMin = 200000000000000;
    double RangeMax = 400000000000000;
    if(*Input == 'r')
    {
        Input = ParseNumber(Input + 6, &RangeMin);
        Input = ParseNumber(Input + 1, &RangeMax);
        Input = SkipPastNewline(Input);
    }

    size_t HailstoneCapacity = 8;
    size_t HailstoneCount = 0;
    hailstone* Hailstones = (hailstone*)malloc(sizeof(hailstone) * HailstoneCapacity);
    while(IsDigit(*Input))
    {
        while(IsDigit(*Input))
        {
            if(HailstoneCount == HailstoneCapacity)
            {
                HailstoneCapacity *= 2;
                Hailstones = (hailstone*)realloc(Hailstones, sizeof(hailstone) * HailstoneCapacity);
            }
            hailstone* Hailstone = &Hailstones[HailstoneCount++];
            Input = ParseVector(Input, &Hailstone->Position);
            Input = ParseVector(Input + 3, &Hailstone->Velocity);
            Input = SkipPastNewline(Input);
        }
    }

    int64_t Result = 0;
    for(int IndexA = 0; IndexA < HailstoneCount; IndexA++)
    {
        hailstone* A = &Hailstones[IndexA];
        for(int IndexB = IndexA + 1; IndexB < HailstoneCount; IndexB++)
        {
            hailstone* B = &Hailstones[IndexB];

            // The intersection point can be found by determining t_a or t_b:
            //
            //     pos_a + t_a * vel_a = pos_b + t_b * vel_b
            //
            // This breaks down into two simultaneous equations in x and y:
            //
            //     pos_a.x + t_a * vel_a.x = pos_b.x + t_b * vel_b.x
            //     pos_a.y + t_a * vel_a.y = pos_b.y + t_b * vel_b.y
            //
            // The code solves for t_b to calculate the intersection point.
            double TimeB = 0.0;
            TimeB += A->Position.X * A->Velocity.Y;
            TimeB -= B->Position.X * A->Velocity.Y;
            TimeB += B->Position.Y * A->Velocity.X;
            TimeB -= A->Position.Y * A->Velocity.X;
            TimeB /= B->Velocity.X * A->Velocity.Y - B->Velocity.Y * A->Velocity.X;

            // If t_b < 0 then the intersection happens in the past for B.
            if(TimeB < 0) continue;

            // Calculate pos_b + t_b * vel_b to determine if the intersection
            // point occurs within the range.
            double CrossX = B->Position.X + TimeB * B->Velocity.X;
            if(CrossX < RangeMin || CrossX > RangeMax) continue;
            double CrossY = B->Position.Y + TimeB * B->Velocity.Y;
            if(CrossY < RangeMin || CrossY > RangeMax) continue;

            // If t_a < 0 then the intersection happens in the past for A.
            double TimeA = (CrossX - A->Position.X) / A->Velocity.X;
            if(TimeA < 0) continue;

            // Found a future intersection in the range.
            Result++;
        }
    }

    free(Hailstones);
    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
