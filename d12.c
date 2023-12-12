#include "aoc.h"
#include "parse.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

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

typedef struct
{
    int At;
    int GroupIndex;
    int Slack;
} key;

typedef int64_t value;

typedef struct
{
    int Count;
    int Capacity;
    uint8_t* Slots;
    key* Keys;
    value* Values;
} table;

static void InitTable(table* Table)
{
    Table->Count = 0;
    Table->Capacity = 0;
    Table->Slots = NULL;
    Table->Keys = NULL;
    Table->Values = NULL;
}

static void FreeTable(table* Table)
{
    free(Table->Slots);
    free(Table->Keys);
    free(Table->Values);
}

static void TableReset(table* Table)
{
    memset(Table->Slots, 0, sizeof(uint8_t) * Table->Capacity);
    Table->Count = 0;
}

static void TableSet(table* Table, key Key, value Value);

static void TableGrow(table* Table)
{
    table NewTable;
    NewTable.Capacity = Table->Capacity ? 2 * Table->Capacity : 2048;
    NewTable.Slots = (uint8_t*)malloc(sizeof(uint8_t) * NewTable.Capacity);
    NewTable.Keys = (key*)malloc(sizeof(key) * NewTable.Capacity);
    NewTable.Values = (value*)malloc(sizeof(value) * NewTable.Capacity);
    TableReset(&NewTable);
    for(int Index = 0; Index < Table->Capacity; Index++)
    {
        if(Table->Slots[Index])
        {
            TableSet(&NewTable, Table->Keys[Index], Table->Values[Index]);
        }
    }
    FreeTable(Table);
    *Table = NewTable;
}

static void TableSet(table* Table, key Key, value Value)
{
    if(Table->Count >= 0.666 * Table->Capacity)
    {
        TableGrow(Table);
    }
    int Hash = (int)XXH3_64bits(&Key, sizeof(key));
    int CapacityMask = Table->Capacity - 1;
    int Index = Hash & CapacityMask;
    for(;;)
    {
        if(Table->Slots[Index])
        {
            if(!memcmp(&Table->Keys[Index], &Key, sizeof(key)))
            {
                Table->Values[Index] = Value;
                return;
            }
        }
        else
        {
            Table->Slots[Index] = 1;
            Table->Keys[Index] = Key;
            Table->Values[Index] = Value;
            Table->Count++;
            return;
        }
        Index = (Index + 1) & CapacityMask;
    }
}

static bool TableGet(table* Table, key Key, value* Value)
{
    if(Table->Count == 0) return false;
    uint64_t Hash = (int)XXH3_64bits(&Key, sizeof(key));
    int CapacityMask = Table->Capacity - 1;
    int Index = Hash & CapacityMask;
    for(;;)
    {
        if(Table->Slots[Index])
        {
            if(!memcmp(&Table->Keys[Index], &Key, sizeof(key)))
            {
                *Value = Table->Values[Index];
                return true;
            }
        }
        else
        {
            return false;
        }
        Index = (Index + 1) & CapacityMask;
    }
}

static int64_t CountArrangements(table* Cache, const char* Record, int Length, int At, int* Groups, int GroupCount, int GroupIndex, int Slack)
{
    key Key = (key){.At = At, .GroupIndex = GroupIndex, .Slack = Slack};
    int64_t Arrangements;
    if(TableGet(Cache, Key, &Arrangements))
    {
        return Arrangements;
    }

    if(GroupIndex == GroupCount)
    {
        // Check that all remaining springs could be operational before
        // affirming this arrangement counts.
        for(int Index = At; Index < Length; Index++)
        {
            if(IsBroken(Record[Index]))
            {
                TableSet(Cache, Key, 0);
                return 0;
            }
        }
        TableSet(Cache, Key, 1);
        return 1;
    }

    int Group = Groups[GroupIndex];
    Arrangements = 0;
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
        if(GroupEnd <= Length - 1)
        {
            if(IsBroken(Record[GroupEnd++])) goto NextOffset;
        }

        // We've found a good spot for our group.
        Arrangements += CountArrangements(Cache, Record, Length, GroupEnd, Groups, GroupCount, GroupIndex + 1, Slack - Offset);

    NextOffset:
        continue;
    }
    TableSet(Cache, Key, Arrangements);
    return Arrangements;
}

static int64_t Solve(const char* Input, int Folds)
{
    table Cache;
    InitTable(&Cache);
    int64_t Sum = 0;
    size_t GroupCapacity = 16;
    int* Groups = (int*)malloc(sizeof(int) * GroupCapacity);
    char* UnfoldedRecordBuffer = NULL;
    size_t UnfoldedRecordCapacity = 1;
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
        if(Folds > 1)
        {
            // Unfold the groups.
            size_t UnfoldedGroupCount = GroupCount * Folds;
            while(GroupCapacity < UnfoldedGroupCount)
            {
                GroupCapacity *= 2;
                Groups = (int*)realloc(Groups, sizeof(int) * GroupCapacity);
            }
            for(int Index = GroupCount; Index < UnfoldedGroupCount; Index += GroupCount)
            {
                memcpy(&Groups[Index], Groups, sizeof(int) * GroupCount);
            }
            GroupCount = UnfoldedGroupCount;
            NumBroken *= Folds;

            // Unfold the record.
            int UnfoldedRecordLength = Length * Folds + Folds - 1;
            if(UnfoldedRecordCapacity < UnfoldedRecordLength)
            {
                UnfoldedRecordCapacity = UnfoldedRecordLength;
                UnfoldedRecordBuffer = (char*)realloc(UnfoldedRecordBuffer, sizeof(char) * UnfoldedRecordCapacity);
            }
            char* Dest = UnfoldedRecordBuffer;
            for(int FoldIndex = 0; FoldIndex < Folds; FoldIndex++)
            {
                if(FoldIndex > 0)
                {
                    Dest[0] = '?';
                    Dest++;
                }
                memcpy(Dest, Record, Length);
                Dest += Length;
            }
            Record = UnfoldedRecordBuffer;
            Length = UnfoldedRecordLength;
        }
        int Slack = Length - (NumBroken + GroupCount - 1);
        Sum += CountArrangements(&Cache, Record, Length, 0, Groups, GroupCount, 0, Slack);;
        Input = SkipPastNewline(Input);
        TableReset(&Cache);
    }
    free(UnfoldedRecordBuffer);
    free(Groups);
    FreeTable(&Cache);
    return Sum;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, 1);
}

AOC_SOLVER(Part2)
{
    return Solve(Input, 5);
}
