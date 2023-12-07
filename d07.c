#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d07.txt";

enum
{
    RANK_JOKER,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_9,
    RANK_T,
    RANK_J,
    RANK_Q,
    RANK_K,
    RANK_A,
    NUM_RANKS
};

enum
{
    TYPE_HIGH_CARD,
    TYPE_ONE_PAIR,
    TYPE_TWO_PAIR,
    TYPE_THREE_OF_A_KIND,
    TYPE_FULL_HOUSE,
    TYPE_FOUR_OF_A_KIND,
    TYPE_FIVE_OF_A_KIND
};

typedef struct
{
    uint32_t Cards;
    int32_t Bid;
} hand;

typedef struct
{
    hand* Elements;
    size_t Count;
    size_t Capacity;
} hand_array;

static void InitHandArray(hand_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

static void FreeHandArray(hand_array* Array)
{
    free(Array->Elements);
    InitHandArray(Array);
}

static void HandArrayAdd(hand_array* Array, uint32_t Cards, int32_t Bid)
{
    size_t Capacity = Array->Capacity;
    if(Array->Count == Capacity)
    {
        Capacity = Capacity ? 2 * Capacity : 8;
        Array->Elements = (hand*)realloc(Array->Elements, sizeof(hand) * Capacity);
        Array->Capacity = Capacity;
    }
    Array->Elements[Array->Count++] = (hand){.Cards = Cards, .Bid = Bid};
}

static void HandArraySwap(hand* Hands, int FromIndex, int ToIndex)
{
    hand Temp = Hands[ToIndex];
    Hands[ToIndex] = Hands[FromIndex];
    Hands[FromIndex] = Temp;
}

static int HandArrayPartition(hand* Hands, int LoIndex, int HiIndex)
{
    hand Pivot = Hands[HiIndex];
    int PivotIndex = LoIndex - 1;
    for(int TestIndex = LoIndex; TestIndex < HiIndex; TestIndex++)
    {
        if(Hands[TestIndex].Cards <= Pivot.Cards)
        {
            HandArraySwap(Hands, ++PivotIndex, TestIndex);
        }
    }
    HandArraySwap(Hands, ++PivotIndex, HiIndex);
    return PivotIndex;
}

static void HandArrayQuickSort(hand* Hands, int LoIndex, int HiIndex)
{
    if(LoIndex >= HiIndex || LoIndex < 0) return;
    int Pivot = HandArrayPartition(Hands, LoIndex, HiIndex);
    HandArrayQuickSort(Hands, LoIndex, Pivot - 1);
    HandArrayQuickSort(Hands, Pivot + 1, HiIndex);
}

static void HandArraySort(hand_array* Array)
{
    HandArrayQuickSort(Array->Elements, 0, Array->Count - 1);
}

static uint8_t ToType(int* CountCounts)
{
    switch(CountCounts[1])
    {
    case 0:
        return CountCounts[5] ? TYPE_FIVE_OF_A_KIND : TYPE_FULL_HOUSE;
    case 1:
        return CountCounts[4] ? TYPE_FOUR_OF_A_KIND : TYPE_TWO_PAIR;
    case 2:
        return TYPE_THREE_OF_A_KIND;
    case 3:
        return TYPE_ONE_PAIR;
    case 5:
    default:
        return TYPE_HIGH_CARD;
    }
}

int64_t Solve(const char* Input, bool UseJokers)
{
    // Create the ASCII-to-rank lookup table.
    uint8_t CharToRank[UINT8_MAX + 1];
    memset(CharToRank, RANK_JOKER, sizeof(CharToRank));
    CharToRank['2'] = RANK_2;
    CharToRank['3'] = RANK_3;
    CharToRank['4'] = RANK_4;
    CharToRank['5'] = RANK_5;
    CharToRank['6'] = RANK_6;
    CharToRank['7'] = RANK_7;
    CharToRank['8'] = RANK_8;
    CharToRank['9'] = RANK_9;
    CharToRank['T'] = RANK_T;
    CharToRank['J'] = UseJokers ? RANK_JOKER : RANK_J;
    CharToRank['Q'] = RANK_Q;
    CharToRank['K'] = RANK_K;
    CharToRank['A'] = RANK_A;

    // Parse and determine the type of each hand.
    hand_array Hands;
    InitHandArray(&Hands);
    while(*Input != '\0')
    {
        int RankCounts[NUM_RANKS];
        memset(RankCounts, 0, sizeof(RankCounts));
        uint32_t Cards = 0;
        for(int Card = 0; Card < 5; Card++)
        {
            uint8_t Rank = CharToRank[*Input++];
            Cards = (Cards << 4) | Rank;
            RankCounts[Rank]++;
        }

        if(UseJokers)
        {
            int JokerCount = RankCounts[RANK_JOKER];
            if(JokerCount > 0)
            {
                int BestCount = 0;
                int BestRank = RANK_JOKER;
                for(int Rank = RANK_2; Rank < NUM_RANKS; Rank++)
                {
                    int RankCount = RankCounts[Rank];
                    if(RankCount >= BestCount)
                    {
                        BestCount = RankCount;
                        BestRank = Rank;
                    }
                }
                RankCounts[RANK_JOKER] = 0;
                RankCounts[BestRank] += JokerCount;
            }
        }

        int CountCounts[6];
        memset(CountCounts, 0, sizeof(CountCounts));
        for(int Rank = RANK_2; Rank < NUM_RANKS; Rank++)
        {
            CountCounts[RankCounts[Rank]]++;
        }
        Cards |= ToType(CountCounts) << 20;

        Input++;
        int Bid = atol(Input);
        Input = SkipPastDigits(Input);
        Input = SkipPastNewline(Input);

        HandArrayAdd(&Hands, Cards, Bid);
    }

    // Sort the hands and determine the total winnings.
    HandArraySort(&Hands);
    int Sum = 0;
    for(int Index = 0; Index < Hands.Count; Index++)
    {
        Sum += (Index + 1) * Hands.Elements[Index].Bid;
    }

    FreeHandArray(&Hands);
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
