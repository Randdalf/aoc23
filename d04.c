#include "aoc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char* DefaultInputPath = "d04.txt";

static bool IsDigit(char C)
{
    return C >= '0' && C <= '9';
}

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    uint64_t Lo, Hi;
    int Number, Matches;
    bool Winning;
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case 'C':
            Lo = Hi = Matches = 0;
            Winning = true;
            Input += 4;
            while(*Input == ' ') Input++;
            while(IsDigit(*Input)) Input++;
            Input += 2;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Number = atoi(Input - 1);
            while(IsDigit(*Input)) Input++;
            if(Winning)
            {
                if(Number >= 64)
                {
                    Hi |= 1llu << (Number - 64);
                }
                else
                {
                    Lo |= 1llu << Number;
                }
            }
            else
            {
                uint64_t Set;
                if(Number >= 64)
                {
                    Set = Hi;
                    Number -= 64;
                }
                else
                {
                    Set = Lo;
                }
                if(Set & (1llu << Number)) Matches++;
            }
            break;
        case '|':
            Winning = false;
            Input++;
            break;
        case '\n':
        case '\0':
            if(Matches) Sum += 1 << (Matches - 1);
            break;
        }
    } while(C != '\0');
    return Sum;
}

AOC_SOLVER(Part2)
{
    int64_t Sum = 0;
    uint64_t Lo, Hi;
    int Number, Matches;
    int CardIndex = 0;
    int Capacity = 256;
    int* Copies = (int*)calloc(Capacity, sizeof(int));
    bool Winning;
    char C;
    do
    {
        C = *Input++;
        switch(C)
        {
        case 'C':
            Lo = Hi = Matches = 0;
            Winning = true;
            Input += 4;
            while(*Input == ' ') Input++;
            while(IsDigit(*Input)) Input++;
            Input += 2;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Number = atoi(Input - 1);
            while(IsDigit(*Input)) Input++;
            if(Winning)
            {
                if(Number >= 64)
                {
                    Hi |= 1llu << (Number - 64);
                }
                else
                {
                    Lo |= 1llu << Number;
                }
            }
            else
            {
                uint64_t Set;
                if(Number >= 64)
                {
                    Set = Hi;
                    Number -= 64;
                }
                else
                {
                    Set = Lo;
                }
                if(Set & (1llu << Number)) Matches++;
            }
            break;
        case '|':
            Winning = false;
            Input++;
            break;
        case '\n':
        case '\0':
            int NumCards = 1;
            if(CardIndex < Capacity)
            {
                NumCards += Copies[CardIndex];
            }
            Sum += NumCards;
            for(int Match = 1; Match <= Matches; Match++)
            {
                int CopyIndex = CardIndex + Match;
                while(CopyIndex >= Capacity)
                {
                    size_t HalfSize = sizeof(int) * Capacity;
                    Capacity *= 2;
                    Copies = (int*)realloc(Copies, sizeof(int) * Capacity);
                    memset(Copies + CopyIndex, 0, HalfSize);
                }
                Copies[CopyIndex] += NumCards;
            }
            CardIndex++;
            break;
        }
    } while(C != '\0');
    free(Copies);
    return Sum;
}
