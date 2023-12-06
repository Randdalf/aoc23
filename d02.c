#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d02.txt";

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    int GameID, Cubes;
    bool Possible;
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case 'G':
            Input += 4;
            GameID = atoi(Input);
            Possible = true;
            Input = SkipPastDigits(Input) + 2;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Cubes = atoi(Input - 1);
            Input = SkipPastDigits(Input) + 1;
            break;
        case 'r':
            Possible &= Cubes <= 12;
            Input += 2;
            break;
        case 'g':
            Possible &= Cubes <= 13;
            Input += 4;
            break;
        case 'b':
            Possible &= Cubes <= 14;
            Input += 3;
            break;
        case ',':
            Input++;
            break;
        case ';':
            Input++;
            break;
        case '\n':
        case '\0':
            if(Possible) Sum += GameID;
            break;
        }
    } while(C != '\0');
    return Sum;
}

AOC_SOLVER(Part2)
{
    int64_t Sum = 0;
    int Cubes, MaxRed, MaxBlue, MaxGreen;
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case 'G':
            MaxRed = MaxBlue = MaxGreen = 0;
            Input += 4;
            Input = SkipPastDigits(Input) + 2;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Cubes = atoi(Input - 1);
            Input = SkipPastDigits(Input) + 1;
            break;
        case 'r':
            if(Cubes > MaxRed) MaxRed = Cubes;
            Input += 2;
            break;
        case 'g':
            if(Cubes > MaxGreen) MaxGreen = Cubes;
            Input += 4;
            break;
        case 'b':
            if(Cubes > MaxBlue) MaxBlue = Cubes;
            Input += 3;
            break;
        case ';':
        case ',':
            Input++;
            break;
        case '\n':
        case '\0':
            Sum += MaxRed * MaxGreen * MaxBlue;
            break;
        }
    } while(C != '\0');
    return Sum;
}
