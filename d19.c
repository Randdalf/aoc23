#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d19.txt";

enum
{
    RATING_X,
    RATING_M,
    RATING_A,
    RATING_S,
    NUM_RATINGS
};

enum
{
    COND_NONE,
    COND_LESS,
    COND_GREATER
};

enum
{
    TARGET_WORKFLOW,
    TARGET_ACCEPT,
    TARGET_REJECT
};

typedef struct rule
{
    int Cond;
    int Rating;
    int Cmp;
    int Target;
    int Id;
    struct rule* Next;
    struct rule* FreeList;
} rule;

static bool IsLower(char C)
{
    return C >= 'a' && C <= 'z';
}

static const char* ParseId(const char* Input, int* OutId)
{
    int Id = 0;
    while(IsLower(*Input)) Id = Id * 27 + (*Input++) - 'a';
    *OutId = Id;
    return Input;
}

static const char* ParseNumber(const char* Input, int* OutNumber)
{
    int Number = 0;
    while(IsDigit(*Input)) Number = Number * 10 + (*Input++) - '0';
    *OutNumber = Number;
    return Input;
}

static const char* ParseCond(const char* Input, rule* Rule)
{
    switch(Input[1])
    {
    case '>': Rule->Cond = COND_GREATER; break;
    case '<': Rule->Cond = COND_LESS; break;
    default:  Rule->Cond = COND_NONE; return Input;
    }
    switch(*Input)
    {
    case 'x': Rule->Rating = RATING_X; break;
    case 'm': Rule->Rating = RATING_M; break;
    case 'a': Rule->Rating = RATING_A; break;
    case 's': Rule->Rating = RATING_S; break;
    }
    return ParseNumber(Input + 2, &Rule->Cmp) + 1;
}

static const char* ParseTarget(const char* Input, rule* Rule)
{
    switch(*Input)
    {
    case 'A': Rule->Target = TARGET_ACCEPT; return Input + 1;
    case 'R': Rule->Target = TARGET_REJECT; return Input + 1;
    default:  Rule->Target = TARGET_WORKFLOW; return ParseId(Input, &Rule->Id);
    }
}

static const char* ParseWorkflows(const char* Input, rule*** OutWorkflows, rule** OutFreeList)
{
    rule** Workflows = malloc(sizeof(rule*) * (27 * 27 * 27));
    rule* FreeList = NULL;
    while(IsLower(*Input))
    {
        int Id;
        Input = ParseId(Input, &Id) + 1;
        rule* Prev = NULL;
        while(*Input != '}')
        {
            rule* Rule = (rule*)malloc(sizeof(rule));
            Rule->Next = NULL;
            Rule->FreeList = FreeList;
            if(!Prev)
            {
                Workflows[Id] = Rule;
            }
            else
            {
                Prev->Next = Rule;
            }
            Prev = Rule;
            FreeList = Rule;
            Input = ParseCond(Input, Rule);
            Input = ParseTarget(Input, Rule);
            if(*Input == ',') Input++;
        }
        Input = SkipPastNewline(Input + 1);
    }
    *OutWorkflows = Workflows;
    *OutFreeList = FreeList;
    return SkipPastNewline(Input);
}

static void FreeWorkflows(rule** Workflows, rule* FreeList)
{
    while(FreeList)
    {
        rule* Next = FreeList->FreeList;
        free(FreeList);
        FreeList = Next;
    }
    free(Workflows);
}

AOC_SOLVER(Part1)
{
    // Parse the workflows into linked lists of rules, stored by numerical id.
    rule** Workflows;
    rule* FreeList;
    Input = ParseWorkflows(Input, &Workflows, &FreeList);

    // Run each part through the workflows, starting at "in", counting the
    // total ratings of the accepted parts.
    int64_t Result = 0;
    int StartId;
    ParseId("in", &StartId);
    rule* Start = Workflows[StartId];
    while(*Input == '{')
    {
        Input++;
        int Ratings[NUM_RATINGS];
        for(int Index = 0; Index < NUM_RATINGS; Index++)
        {
            Input = ParseNumber(Input + 2, &Ratings[Index]) + 1;
        }
        rule* Rule = Start;
        for(;;)
        {
            switch(Rule->Cond)
            {
            case COND_LESS:
                if(Ratings[Rule->Rating] < Rule->Cmp) break;
                Rule = Rule->Next;
                continue;
            case COND_GREATER:
                if(Ratings[Rule->Rating] > Rule->Cmp) break;
                Rule = Rule->Next;
                continue;
            }
            switch(Rule->Target)
            {
            case TARGET_WORKFLOW:
                Rule = Workflows[Rule->Id];
                continue;
            case TARGET_ACCEPT:
                Result += Ratings[0] + Ratings[1] + Ratings[2] + Ratings[3];
                goto NextPart;
            case TARGET_REJECT:
                goto NextPart;
            }
        }
    NextPart:
        Input = SkipPastNewline(Input);
    }
    FreeWorkflows(Workflows, FreeList);
    return Result;
}

typedef struct
{
    int64_t Min;
    int64_t Max;
} range;

static inline int64_t Min(int64_t A, int64_t B)
{
    return A < B ? A : B;
}

static inline int64_t Max(int64_t A, int64_t B)
{
    return A > B ? A : B;
}

static int64_t AcceptedCombinations(rule** Workflows, rule* Rule, range* Accepted)
{
    int64_t Result = 0;
    range AcceptedCopy[4];
    memcpy(AcceptedCopy, Accepted, sizeof(AcceptedCopy));
    Accepted = AcceptedCopy;
    while(Rule)
    {
        // Modify the accepted ranges based on the condition passing.
        range Range;
        if(Rule->Cond != COND_NONE)
        {
            Range = Accepted[Rule->Rating];
            if(Rule->Cond == COND_LESS)
            {
                Accepted[Rule->Rating].Max = Min(Range.Max, Rule->Cmp - 1);
            }
            else if(Rule->Cond == COND_GREATER)
            {
                Accepted[Rule->Rating].Min = Max(Range.Min, Rule->Cmp + 1);
            }
        }

        // Recurse into the linked workflow or, if the workflow terminates
        // here, count the number of accepted combinations.
        if(Rule->Target == TARGET_WORKFLOW)
        {
            Result += AcceptedCombinations(Workflows, Workflows[Rule->Id], Accepted);
        }
        else if(Rule->Target == TARGET_ACCEPT)
        {
            int64_t Product = 1;
            for(int Index = 0; Index < NUM_RATINGS; Index++)
            {
                Product *= 1 + Accepted[Index].Max - Accepted[Index].Min;
            }
            Result += Product;
        }

        // Modify the accepted ranges based on the condition failing.
        if(Rule->Cond != COND_NONE)
        {
            Accepted[Rule->Rating] = Range;
            if(Rule->Cond == COND_LESS)
            {
                Accepted[Rule->Rating].Min = Max(Range.Min, Rule->Cmp);
            }
            else if(Rule->Cond == COND_GREATER)
            {
                Accepted[Rule->Rating].Max = Min(Range.Max, Rule->Cmp);
            }
        }

        // Transition to the next part of the worklow.
        Rule = Rule->Next;
    }
    return Result;
}

AOC_SOLVER(Part2)
{
    rule** Workflows;
    rule* FreeList;
    Input = ParseWorkflows(Input, &Workflows, &FreeList);
    int StartId;
    ParseId("in", &StartId);
    rule* Start = Workflows[StartId];
    range Accepted[4];
    for(int Index = 0; Index < NUM_RATINGS; Index++)
    {
        Accepted[Index] = (range){.Min = 1, .Max = 4000};
    }
    int64_t Result = AcceptedCombinations(Workflows, Start, Accepted);
    FreeWorkflows(Workflows, FreeList);
    return Result;
}
