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

static int CountArrangments(const char* Record, int Length, int At, int* Groups, int GroupCount, int GroupIndex, int Slack)
{
    if(GroupIndex == GroupCount)
    {
        // Check that all remaining springs could be operational before
        // affirming this arrangement counts.
        for(int Index = At; Index < Length; Index++)
        {
            if(IsBroken(Record[Index])) return 0;
        }
        return 1;
    }

    int Group = Groups[GroupIndex];
    int Arrangements = 0;
    for(int Offset = 0; Offset <= Slack; Offset++)
    {
        // Check that all springs before the group can be operational.
        int GroupStart = At + Offset;
        for(int Index = At; Index < GroupStart; Index++)
        {
            if(IsBroken(Record[Index])) goto NextOffset;
        }

        // Check that all springs inside the group can be broken.
        int GroupEnd = GroupStart + Group;
        for(int Index = GroupStart; Index < GroupEnd; Index++)
        {
            if(IsOperational(Record[Index])) goto NextOffset;
        }

        // Check that the spring following the group can be operational.
        if(GroupEnd < Length - 1)
        {
            if(IsBroken(Record[GroupEnd++])) goto NextOffset;
        }

        // We've found a good spot for our group.
        Arrangements += CountArrangments(Record, Length, GroupEnd, Groups, GroupCount, GroupIndex + 1, Slack - Offset);

    NextOffset:
        ((void)0);
    }
    return Arrangements;
}

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    size_t GroupCapacity = 16;
    int* Groups = (int*)malloc(sizeof(int) * GroupCapacity);
    while(IsCondition(*Input))
    {
        const char* Record = Input;
        Input = SkipPastConditions(Input);
        int Length = Input - Record;
        Input++;
        size_t GroupCount = 0;
        int NumBroken = 0;
        while(IsDigit(*Input))
        {
            if(GroupCount == GroupCapacity)
            {
                GroupCapacity *= 2;
                Groups = (int*)realloc(Groups, sizeof(int) * GroupCapacity);
            }
            int Group = atoi(Input);
            Groups[GroupCount++] = Group;
            Input = SkipPastDigits(Input);
            NumBroken += Group;
            if(*Input == ',') Input++;
        }
        int Slack = Length - (NumBroken + GroupCount - 1);
        Sum += CountArrangments(Record, Length, 0, Groups, GroupCount, 0, Slack);
        Input = SkipPastNewline(Input);
    }
    return Sum;
}

AOC_SOLVER(Part2)
{
    return -1;
}
