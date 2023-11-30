#include "aoc.h"

#include <stdio.h>
#include <stdlib.h>

extern const char* DefaultInputPath;
extern AOC_SOLVER(Part1);
extern AOC_SOLVER(Part2);

static const char* ReadInputFile(const char* Path)
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

int main(int ArgCount, char** Args)
{
    const char* Input = ReadInputFile(DefaultInputPath);
    if(!Input) return EXIT_FAILURE;
    printf("%s\n", Input);
    return EXIT_SUCCESS;
}
