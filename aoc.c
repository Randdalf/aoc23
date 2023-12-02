#include "aoc.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

extern const char* DefaultInputPath;
extern AOC_SOLVER(Part1);
extern AOC_SOLVER(Part2);

static void PrintUsage(const char* Prog)
{
    printf("usage: %s [<options>]\n\n", Prog);
    printf("    -1  run part 1 only\n");
    printf("    -2  run part 2 only\n");
    printf("    -h  print this help\n");
    printf("    -i  read input from stdin\n");
    printf("    -q  quiet mode\n");
    printf("    -e  echo puzzle input to stdout, then exit\n");
}

static char* ReadStandardInput(void)
{
    size_t Length = 0;
    size_t Capacity = 128;
    char* Chars = (char*)malloc(Capacity);
    Chars[0] = '\0';
    char Buffer[128];
    while(fgets(Buffer, sizeof(Buffer), stdin))
    {
        size_t BufferLength = strlen(Buffer) + 1;
        size_t OldCapacity = Capacity;
        while(Length + BufferLength > Capacity)
        {
            Capacity *= 2;
        }
        if(Capacity > OldCapacity)
        {
            Chars = (char*)realloc(Chars, Capacity);
        }
        memcpy(Chars + Length, Buffer, BufferLength);
        Length += BufferLength - 1;
    }
    if(!feof(stdin))
    {
        free(Chars);
        return NULL;
    }
    return Chars;
}

static char* ReadInputFile(const char* Path)
{
    FILE* File = fopen(Path, "rb");
    if(File == NULL) return NULL;
    fseek(File, 0L, SEEK_END);
    size_t Size = ftell(File);
    rewind(File);
    char* Buffer = (char*)malloc(Size + 1);
    if(!Buffer) return NULL;
    size_t Read = fread(Buffer, sizeof(char), Size, File);
    if(Read < Size)
    {
        free(Buffer);
        return NULL;
    }
    Buffer[Size] = '\0';
    return Buffer;
}

static double Clock(void)
{
    static double Frequency = 0;
    if(!Frequency)
    {
        LARGE_INTEGER FrequencyInt;
        QueryPerformanceFrequency(&FrequencyInt);
        Frequency = (double)FrequencyInt.QuadPart;
    }
    LARGE_INTEGER Counter;
    QueryPerformanceCounter(&Counter);
    return (double)Counter.QuadPart / Frequency;
}

static void RunSolver(aoc_solver* Solver, const char* Input, bool Quiet)
{
    double Start = Clock();
    int64_t Result = Solver(Input);
    double End = Clock();
    if(Result == -1) return;
    printf("%-30lld", Result);
    if(!Quiet)
    {
        printf(" %.4fms", 1000 * (End - Start));
    }
    printf("\n");
}

int main(int ArgCount, const char** Args)
{
    // Parse command line arguments.
    int ExclusiveSolver = -1;
    bool EchoInput = false;
    bool UseStandardInput = false;
    bool Quiet = false;
    for(int ArgIndex = 1; ArgIndex < ArgCount; ArgIndex++)
    {
        const char* Arg = Args[ArgIndex];
        if(Arg[0] != '-')
        {
            PrintUsage(Args[0]);
            return EXIT_FAILURE;
        }
        int Index = 1;
        for(int CharIndex = 1; Arg[CharIndex] != '\0'; CharIndex++)
        {
            switch(Arg[CharIndex])
            {
            case '1': ExclusiveSolver = 1; break;
            case '2': ExclusiveSolver = 2; break;
            case 'e': EchoInput = true; break;
            case 'i': UseStandardInput = true; break;
            case 'h':
                PrintUsage(Args[0]);
                return EXIT_SUCCESS;
            case 'q': Quiet = true; break;
            default:
                PrintUsage(Args[0]);
                return EXIT_FAILURE;
            }
        }
    }

    // Read puzzle input.
    char* Input;
    if(UseStandardInput)
    {
        Input = ReadStandardInput();
    }
    else
    {
        Input = ReadInputFile(DefaultInputPath);
    }
    if(!Input)
    {
        fprintf(stderr, "Failed to read input.");
        return EXIT_FAILURE;
    }

    // Echo input and exit, if instructed.
    if(EchoInput)
    {
        puts(Input);
        return EXIT_SUCCESS;
    }

    // Run part 1.
    if(ExclusiveSolver < 0 || ExclusiveSolver == 1)
    {
        RunSolver(Part1, Input, Quiet);
    }

    // Run part 2.
    if(ExclusiveSolver < 0 || ExclusiveSolver == 2)
    {
        RunSolver(Part2, Input, Quiet);
    }

    // Tidy up and return.
    free(Input);
    return EXIT_SUCCESS;
}
