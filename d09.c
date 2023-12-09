#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d09.txt";

typedef struct
{
    int64_t* Elements;
    size_t Count;
    size_t Capacity;
} sequence;

static void InitSequence(sequence* Sequence)
{
    Sequence->Elements = NULL;
    Sequence->Count = 0;
    Sequence->Capacity = 0;
}

static void FreeSequence(sequence* Sequence)
{
    free(Sequence->Elements);
    InitSequence(Sequence);
}

static void SequenceAdd(sequence* Sequence, int64_t Element)
{
    size_t Capacity = Sequence->Capacity;
    if(Sequence->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Sequence->Elements = (int64_t*)realloc(Sequence->Elements, sizeof(int64_t) * Capacity);
        Sequence->Capacity = Capacity;
    }
    Sequence->Elements[Sequence->Count++] = Element;
}

static void SequenceReset(sequence* Sequence)
{
    Sequence->Count = 0;
}

static void SequenceReverse(sequence* Sequence)
{
    size_t Count = Sequence->Count;
    size_t HalfCount = Count / 2;
    for(int Index = 0; Index < HalfCount; Index++)
    {
        int ReverseIndex = Count - Index - 1;
        int64_t Temp = Sequence->Elements[Index];
        Sequence->Elements[Index] = Sequence->Elements[ReverseIndex];
        Sequence->Elements[ReverseIndex] = Temp;
    }
}

static bool SequenceIsZero(sequence* Sequence)
{
    for(int Index = 0; Index < Sequence->Count; Index++)
    {
        if(Sequence->Elements[Index])
        {
            return false;
        }
    }
    return true;
}

static bool IsNumeric(char C)
{
    return IsDigit(C) || C == '-';
}

static inline const char* SkipPastNumeric(const char* Input)
{
    while(IsNumeric(*Input)) Input++;
    return Input;
}

int64_t Solve(const char* Input, bool Reverse)
{
    int64_t Sum = 0;
    sequence Sequence;
    InitSequence(&Sequence);
    while(IsNumeric(*Input))
    {
        // Parse the sequence.
        while(IsNumeric(*Input))
        {
            SequenceAdd(&Sequence, atoll(Input));
            Input = SkipPastNumeric(Input);
            Input = SkipPastWhitespace(Input);
        }
        Input = SkipPastNewline(Input);

        // Reverse the sequence if extrapolating backwards.
        if(Reverse)
        {
            SequenceReverse(&Sequence);
        }

        // Reduce the sequence to zero, adding the end of each step to the sum.
        // (As they will sum the next number in the sequence)
        while(!SequenceIsZero(&Sequence))
        {
            Sum += Sequence.Elements[Sequence.Count - 1];
            for(int Index = 1; Index < Sequence.Count; Index++)
            {
                Sequence.Elements[Index - 1] = Sequence.Elements[Index] - Sequence.Elements[Index - 1];
            }
            Sequence.Count--;
        }

        SequenceReset(&Sequence);
    }
    return Sum;
}

AOC_SOLVER(Part1)
{
    return Solve(Input, false);
}

AOC_SOLVER(Part2)
{
    return Solve(Input, true);
}
