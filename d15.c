#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d15.txt";

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    int Value = 0;
    for(;;)
    {
        char C = *Input++;
        switch(C)
        {
        case ',':
            Sum += Value;
            Value = 0;
            break;
        case '\0':
        case '\r':
        case '\n':
            return Sum + Value;
        default:
            Value = ((Value + C) * 17) & 0xFF;
            break;
        }
    }
    return Sum;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}
