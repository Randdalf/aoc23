#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d12.txt";

static bool IsOperational(char C)
{
    return C == '.';
}

static bool IsBroken(char C)
{
    return C == '#';
}

static bool IsUnknown(char C)
{
    return C == '?';
}

static bool IsCondition(char C)
{
    return IsOperational(C) || IsBroken(C) || IsUnknown(C);
}

static const char* SkipPastConditions(const char* Input)
{
    while(IsCondition(*Input)) Input++;
    return Input;
}

static int MatchesGroups(const char* Record, int Length, int* Groups, int GroupCount)
{
    int RecordIndex = 0;
    for(int GroupIndex = 0; GroupIndex < GroupCount; GroupIndex++)
    {
        while(RecordIndex < Length && IsOperational(Record[RecordIndex]))
        {
            RecordIndex++;
        }
        int GroupStart = RecordIndex;
        while(RecordIndex < Length && IsBroken(Record[RecordIndex]))
        {
            RecordIndex++;
        }
        if(RecordIndex - GroupStart != Groups[GroupIndex])
        {
            return false;
        }
    }
    while(RecordIndex < Length && IsOperational(Record[RecordIndex]))
    {
        RecordIndex++;
    }
    return RecordIndex == Length;
}

static int CountArrangments(const char* Record, int Length, char* Scratch, int Index, int* Groups, int GroupCount)
{
    while(Index < Length && !IsUnknown(Record[Index]))
    {
        Scratch[Index] = Record[Index];
        Index++;
    }
    if(Index == Length)
    {
        return MatchesGroups(Scratch, Length, Groups, GroupCount);
    }
    int Arrangments = 0;
    Scratch[Index] = '.';
    Arrangments += CountArrangments(Record, Length, Scratch, Index + 1, Groups, GroupCount);
    Scratch[Index] = '#';
    Arrangments += CountArrangments(Record, Length, Scratch, Index + 1, Groups, GroupCount);
    return Arrangments;
}

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    size_t GroupCapacity = 16;
    int* Groups = (int*)malloc(sizeof(int) * GroupCapacity);
    size_t ScratchCapacity = 32;
    char* Scratch = (char*)malloc(sizeof(char) * ScratchCapacity);
    while(IsCondition(*Input))
    {
        const char* Record = Input;
        Input = SkipPastConditions(Input);
        int Length = Input - Record;
        Input++;
        size_t GroupCount = 0;
        while(IsDigit(*Input))
        {
            if(GroupCount == GroupCapacity)
            {
                GroupCapacity *= 2;
                Groups = (int*)realloc(Groups, sizeof(int) * GroupCapacity);
            }
            Groups[GroupCount++] = atoi(Input);
            Input = SkipPastDigits(Input);
            if(*Input == ',') Input++;
        }
        while(Length > ScratchCapacity)
        {
            ScratchCapacity *= 2;
            Scratch = (char*)realloc(Scratch, sizeof(char) * ScratchCapacity);
        }
        Sum += CountArrangments(Record, Length, Scratch, 0, Groups, GroupCount);
        Input = SkipPastNewline(Input);
    }
    return Sum;
}

AOC_SOLVER(Part2)
{
    return -1;
}
