#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d24.txt";

typedef union
{
    struct
    {
        double X;
        double Y;
        double Z;
    };
    double Axes[3];
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

typedef struct
{
    hailstone* Elements;
    size_t Count;
    size_t Capacity;
} hailstone_array;

static hailstone* HailstoneArrayAdd(hailstone_array* Array)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (hailstone*)realloc(Array->Elements, sizeof(hailstone) * Capacity);
        Array->Capacity = Capacity;
    }
    return &Array->Elements[Array->Count++];
}

static void InitHailstoneArray(hailstone_array* Array, const char* Input)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
    while(IsDigit(*Input))
    {
        while(IsDigit(*Input))
        {
            hailstone* Hailstone = HailstoneArrayAdd(Array);
            Input = ParseVector(Input, &Hailstone->Position);
            Input = ParseVector(Input + 3, &Hailstone->Velocity);
            Input = SkipPastNewline(Input);
        }
    }
}

static void FreeHailstoneArray(hailstone_array* Array)
{
    free(Array->Elements);
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

    hailstone_array Hailstones;
    InitHailstoneArray(&Hailstones, Input);

    int64_t Result = 0;
    for(int IndexA = 0; IndexA < Hailstones.Count; IndexA++)
    {
        hailstone* A = &Hailstones.Elements[IndexA];
        for(int IndexB = IndexA + 1; IndexB < Hailstones.Count; IndexB++)
        {
            hailstone* B = &Hailstones.Elements[IndexB];

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

    FreeHailstoneArray(&Hailstones);
    return Result;
}

typedef struct
{
    vec3 Position;
    vec3 Velocity;
    double Times[3];
} solution;

AOC_SOLVER(Part2)
{
    hailstone_array Hailstones;
    InitHailstoneArray(&Hailstones, Input);

    // Every hailstone adds 3 equations and 1 unknown (time). There are 6 base
    // unknowns (3 position + 3 velocity). To solve the system of equations,
    // we need at least as many equations as there are unknowns.
    //
    //     equations = unknowns
    //     3n = 6 + n
    //     n = 3
    //
    // Therefore, we only need to consider 3 hailstones to form a solvable
    // system of equations.
    //
    // These equations are as follows, where pos_r, vel_r and t_a/b/c are the
    // unknowns:
    //
    //    pos_a.x - pos_r.x + (vel_a.x - vel_r.x) * t_a = 0
    //    pos_a.y - pos_r.y + (vel_a.y - vel_r.y) * t_a = 0
    //    pos_a.z - pos_r.z + (vel_a.z - vel_r.z) * t_a = 0
    //
    //    pos_b.x - pos_r.x + (vel_b.x - vel_r.x) * t_b = 0
    //    pos_b.y - pos_r.y + (vel_b.y - vel_r.y) * t_b = 0
    //    pos_b.z - pos_r.z + (vel_b.z - vel_r.z) * t_b = 0
    //
    //    pos_c.x - pos_r.x + (vel_c.x - vel_r.x) * t_c = 0
    //    pos_c.y - pos_r.y + (vel_c.y - vel_r.y) * t_c = 0
    //    pos_c.z - pos_r.z + (vel_c.z - vel_r.z) * t_c = 0
    //
    // Because these equations are non-linear we solve them numerically using
    // Newton's Method.
    //
    // Or we would, but it's an awful lot of work [1] and it's nearly Christmas.
    // So we just output the equations so they can be fed into a solver instead,
    // knowing that we _could_ have implemented our own one.
    //
    // [1] https://www.lakeheadu.ca/sites/default/files/uploads/77/docs/RemaniFinal.pdf
    for(int HailstoneIndex = 0; HailstoneIndex < 3; HailstoneIndex++)
    {
        hailstone* Hailstone = &Hailstones.Elements[HailstoneIndex];
        char TimeSymbol = "ghi"[HailstoneIndex];
        for(int Axis = 0; Axis < 3; Axis++)
        {
            printf("%.0f - %c + (%.0f - %c) * %c = 0\n",
                Hailstone->Position.Axes[Axis],
                "abc"[Axis],
                Hailstone->Velocity.Axes[Axis],
                "def"[Axis],
                TimeSymbol);
        }
    }

    return -1;
}
