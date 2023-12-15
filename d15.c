#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d15.txt";

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    int Value = 0;
    for(;;)
    {
        char C = *Input++;
        switch(C)
        {
        case ',':
            Sum += Value;
            Value = 0;
            break;
        case '\0':
        case '\r':
        case '\n':
            return Sum + Value;
        default:
            Value = ((Value + C) * 17) & 255;
            break;
        }
    }
}

typedef struct lens
{
    uint64_t Id;
    struct lens* Next;
    int FocalLength;
} lens;

#if 0
void PrintBoxes(lens** Boxes)
{
    for(int Index = 0; Index < 256; Index++)
    {
        lens* Lens = Boxes[Index];
        bool HasLens = Lens != NULL;
        if(HasLens)
        {
            printf("Box %d:", Index);
        }
        while(Lens)
        {
            putchar(' ');
            putchar('[');
            uint64_t Id = Lens->Id;
            uint64_t Reverse = 0;
            while(Id)
            {
                Reverse = (Reverse << 8) | (Id & 255);
                Id >>= 8;
            }
            while(Reverse)
            {
                putchar((char)(Reverse & 255));
                Reverse >>= 8;
            }
            putchar(' ');
            putchar((char)(Lens->FocalLength + '0'));
            putchar(']');
            Lens = Lens->Next;
        }
        if(HasLens)
        {
            printf("\n");
        }
    }
}
#endif

AOC_SOLVER(Part2)
{
    lens* Boxes[256];
    memset(Boxes, 0, sizeof(Boxes));
    int Hash = 0;
    uint64_t Id = 0;
    for(;;)
    {
        char C = *Input++;
        switch(C)
        {
            case '=':
            {
                int FocalLength = *Input++ - '0';
                lens* Prev = NULL;
                lens* Head = Boxes[Hash];
                while(Head)
                {
                    if(Head->Id == Id)
                    {
                        Head->FocalLength = FocalLength;
                        break;
                    }
                    Prev = Head;
                    Head = Head->Next;
                }
                if(!Head)
                {
                    lens* Lens = (lens*)malloc(sizeof(lens));
                    Lens->Id = Id;
                    Lens->Next = NULL;
                    Lens->FocalLength = FocalLength;
                    if(Prev)
                    {
                        Prev->Next = Lens;
                    }
                    else
                    {
                        Boxes[Hash] = Lens;
                    }
                }
                break;
            }
            case '-':
            {
                lens* Prev = NULL;
                lens* Head = Boxes[Hash];
                while(Head)
                {
                    if(Head->Id == Id)
                    {
                        if(Prev)
                        {
                            Prev->Next = Head->Next;
                        }
                        else
                        {
                            Boxes[Hash] = Head->Next;
                        }
                        break;
                    }
                    Prev = Head;
                    Head = Head->Next;
                }
                break;
            }
            case ',':
                Hash = 0;
                Id = 0;
                break;
            case '\0':
            case '\r':
            case '\n':
            {
                int64_t Sum = 0;
                for(int Index = 0; Index < 256; Index++)
                {
                    lens* Lens = Boxes[Index];
                    int Slot = 1;
                    while(Lens)
                    {
                        Sum += (Index + 1) * (Slot++) * Lens->FocalLength;
                        lens* Next = Lens->Next;
                        free(Next);
                        Lens = Next;
                    }
                }
                return Sum;
            }
            default:
                Id = (Id << 8) | C;
                Hash = ((Hash + C) * 17) & 255;
                break;
        }
    }
}
