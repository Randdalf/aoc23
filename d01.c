#include "aoc.h"

const char* DefaultInputPath = "d01.txt";

#include <stdio.h>

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    int FirstDigit = -1;
    int LastDigit = 0;
    char C;
    do
    {
        C = *Input++;
        if(C >= '0' && C <= '9')
        {
            int Digit = C - '0';
            if(FirstDigit < 0) FirstDigit = Digit;
            LastDigit = Digit;
        }
        if(C == '\n' || C == '\0')
        {
            Sum += 10 * FirstDigit + LastDigit;
            FirstDigit = -1;
        }
    } while(C != '\0');
    return Sum;
}

AOC_SOLVER(Part2)
{
    return -1;
}
