#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d08.txt";

static bool IsUpper(char C)
{
    return C >= 'A' && C <= 'Z';
}

static const char* SkipToUpper(const char* At)
{
    while(!IsUpper(*At)) At++;
    return At;
}

static uint16_t MakeNode(char C0, char C1, char C2)
{
    return (C0 - 'A') * 26 * 26 + (C1 - 'A') * 26 + (C2 - 'A');
}

static const char* ParseNode(const char* At, uint16_t* OutNode)
{
    *OutNode = MakeNode(At[0], At[1], At[2]);
    return At + 3;
}

typedef struct
{
    uint16_t Left;
    uint16_t Right;
} ins;

AOC_SOLVER(Part1)
{
    // Parse the map.
    ins* Ins = (ins*)calloc(26 * 26 * 26, sizeof(ins));
    const char* At = SkipPastLine(Input);
    At = SkipPastLine(At);
    while(IsUpper(*At))
    {
        uint16_t Node, Left, Right;
        At = ParseNode(At, &Node) + 4;
        At = ParseNode(At, &Left) + 2;
        At = ParseNode(At, &Right) + 1;
        At = SkipPastNewline(At);
        Ins[Node] = (ins){.Left = Left, .Right = Right};
    }

    // Follow the instructions and count the number of steps.
    int64_t Steps = 0;
    uint16_t Curr = MakeNode('A', 'A', 'A');
    uint16_t Goal = MakeNode('Z', 'Z', 'Z');
    while(Curr != Goal)
    {
        switch(*At)
        {
        case 'L':
            Curr = Ins[Curr].Left;
            Steps++;
            At++;
            break;
        case 'R':
            Curr = Ins[Curr].Right;
            Steps++;
            At++;
            break;
        default:
            At = Input;
            break;
        }
    }

    free(Ins);
    return Steps;
}

AOC_SOLVER(Part2)
{
    return -1;
}
