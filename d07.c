#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d07.txt";

enum
{
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

void InitHandArray(hand_array* Array)
{
    Array->Elements = NULL;
    Array->Count = 0;
    Array->Capacity = 0;
}

void FreeHandArray(hand_array* Array)
{
    free(Array->Elements);
    InitHandArray(Array);
}

void HandArrayAdd(hand_array* Array, uint32_t Cards, int32_t Bid)
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

static uint8_t ToType(int* HandCounts)
{
    switch(HandCounts[1])
    {
    case 0:
        return HandCounts[5] ? TYPE_FIVE_OF_A_KIND : TYPE_FULL_HOUSE;
    case 1:
        return HandCounts[4] ? TYPE_FOUR_OF_A_KIND : TYPE_TWO_PAIR;
    case 2:
        return TYPE_THREE_OF_A_KIND;
    case 3:
        return TYPE_ONE_PAIR;
    case 5:
    default:
        return TYPE_HIGH_CARD;
    }
}

AOC_SOLVER(Part1)
{
    // Create the ASCII-to-rank lookup table.
    uint8_t CharToRank[UINT8_MAX + 1];
    memset(CharToRank, RANK_2, sizeof(CharToRank));
    CharToRank['3'] = RANK_3;
    CharToRank['4'] = RANK_4;
    CharToRank['5'] = RANK_5;
    CharToRank['6'] = RANK_6;
    CharToRank['7'] = RANK_7;
    CharToRank['8'] = RANK_8;
    CharToRank['9'] = RANK_9;
    CharToRank['T'] = RANK_T;
    CharToRank['J'] = RANK_J;
    CharToRank['Q'] = RANK_Q;
    CharToRank['K'] = RANK_K;
    CharToRank['A'] = RANK_A;

    // Parse and determine the type of each hand.
    hand_array Hands;
    InitHandArray(&Hands);
    while(*Input != '\0')
    {
        int HandRanks[NUM_RANKS];
        memset(HandRanks, 0, sizeof(HandRanks));
        uint32_t Cards = 0;
        for(int Card = 0; Card < 5; Card++)
        {
            uint8_t Rank = CharToRank[*Input++];
            Cards = (Cards << 4) | Rank;
            HandRanks[Rank]++;
        }

        int HandCounts[6];
        memset(HandCounts, 0, sizeof(HandCounts));
        for(int Rank = 0; Rank < NUM_RANKS; Rank++)
        {
            HandCounts[HandRanks[Rank]]++;
        }
        Cards |= ToType(HandCounts) << 20;

        Input++;
        int Bid = atol(Input);
        Input = SkipPastDigits(Input);
        Input = SkipPastNewline(Input);

        HandArrayAdd(&Hands, Cards, Bid);
    }

    // Sort the hands (insertion sort).
    for(int Index = 1; Index < Hands.Count; Index++)
    {
        hand Hand = Hands.Elements[Index];
        int TestIndex = Index;
        while(--TestIndex >= 0 && Hands.Elements[TestIndex].Cards > Hand.Cards)
        {
            Hands.Elements[TestIndex + 1] = Hands.Elements[TestIndex];
        }
        Hands.Elements[TestIndex + 1] = Hand;
    }

    // Determine the total winnings.
    int Sum = 0;
    for(int Index = 0; Index < Hands.Count; Index++)
    {
        Sum += (Index + 1) * Hands.Elements[Index].Bid;
    }

    FreeHandArray(&Hands);
    return Sum;
}

AOC_SOLVER(Part2)
{
    return -1;
}
