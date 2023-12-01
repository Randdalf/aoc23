#include "aoc.h"

#include <string.h>

const char* DefaultInputPath = "d01.txt";

#define DIGIT(Digit) \
    do \
    { \
        if(FirstDigit < 0) FirstDigit = Digit; \
        LastDigit = Digit; \
    } while(0);

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
            DIGIT(C - '0');
        }
        if(C == '\n' || C == '\0')
        {
            Sum += 10 * FirstDigit + LastDigit;
            FirstDigit = -1;
        }
    } while(C != '\0');
    return Sum;
}

#define STRING_DIGIT(Letters, Digit) \
    if(!strncmp(Letters, Input, sizeof(Letters) - 1)) \
    { \
        DIGIT(Digit); \
        continue; \
    }

AOC_SOLVER(Part2)
{
    int64_t Sum = 0;
    int FirstDigit = -1;
    int LastDigit = 0;
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            DIGIT(C - '0');
            break;
        case 'e':
            STRING_DIGIT("ight", 8);
            break;
        case 'f':
            STRING_DIGIT("ive", 5);
            STRING_DIGIT("our", 4);
            break;
        case 'n':
            STRING_DIGIT("ine", 9);
            break;
        case 'o':
            STRING_DIGIT("ne", 1);
            break;
        case 's':
            STRING_DIGIT("even", 7);
            STRING_DIGIT("ix", 6);
            break;
        case 't':
            STRING_DIGIT("hree", 3);
            STRING_DIGIT("wo", 2);
            break;
        case '\n': case '\0':
            Sum += 10 * FirstDigit + LastDigit;
            FirstDigit = -1;
            break;
        }
    } while(C != '\0');
    return Sum;
}
