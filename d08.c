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
    return ((C0 - 'A') << 10) | ((C1 - 'A') << 5) | (C2 - 'A');
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
    uint16_t ZZZ = MakeNode('Z', 'Z', 'Z');;
    ins* Ins = (ins*)calloc(ZZZ + 1, sizeof(ins));
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
    while(Curr != ZZZ)
    {
        switch(*At)
        {
        case 'L':
            Curr = Ins[Curr].Left;
            break;
        case 'R':
            Curr = Ins[Curr].Right;
            break;
        default:
            At = Input;
            continue;
        }
        Steps++;
        At++;
    }

    free(Ins);
    return Steps;
}

static char LastChar(uint16_t Node)
{
    // Each char takes up 5 bits. 31 = 0b11111 to mask off the last char's offset.
    return 'A' + (Node & 31);
}

static int64_t GCD(int64_t A, int64_t B)
{
    while(B)
    {
        int64_t Temp = B;
        B = A % B;
        A = Temp;
    }
    return A;
}

static int64_t LCM(int64_t A, int64_t B)
{
    return (A * B) / GCD(A, B);
}

AOC_SOLVER(Part2)
{
    // Parse the map and detect the ghosts (oooOOOoooOOOOoo).
    uint16_t ZZZ = MakeNode('Z', 'Z', 'Z');
    ins* Ins = (ins*)calloc(ZZZ + 1, sizeof(ins));
    uint16_t* Ghosts = NULL;
    size_t GhostCount = 0;
    size_t GhostCapacity = 0;
    const char* At = SkipPastLine(Input);
    At = SkipPastLine(At);
    while(IsUpper(*At))
    {
        uint16_t Node, Left, Right;
        At = ParseNode(At, &Node) + 4;
        char Blah = At[2];
        At = ParseNode(At, &Left) + 2;
        At = ParseNode(At, &Right) + 1;
        At = SkipPastNewline(At);
        Ins[Node] = (ins){.Left = Left, .Right = Right};
        if(LastChar(Node) == 'A')
        {
            if(GhostCount == GhostCapacity)
            {
                GhostCapacity = GhostCapacity ? 2 * GhostCapacity : 8;
                Ghosts = (uint16_t*)realloc(Ghosts, GhostCapacity);
            }
            Ghosts[GhostCount++] = Node;
        }
    }

    // Compute the lowest common multiple of the steps taken for each ghost
    // to reach a node ending in 'Z', to work out the number of steps it will
    // take for every ghost to be on a node ending in 'Z'.
    int64_t OverallSteps;
    for(int GhostIndex = 0; GhostIndex < GhostCount; GhostIndex++)
    {
        int64_t Steps = 0;
        uint16_t Ghost = Ghosts[GhostIndex];
        while(LastChar(Ghost) != 'Z')
        {
            switch(*At)
            {
            case 'L':
                Ghost = Ins[Ghost].Left;
                break;
            case 'R':
                Ghost = Ins[Ghost].Right;
                break;
            default:
                At = Input;
                continue;
            }
            Steps++;
            At++;
        }
        OverallSteps = GhostIndex ? LCM(Steps, OverallSteps) : Steps;
    }

    free(Ins);
    return OverallSteps;
}
