#include "aoc.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

const char* DefaultInputPath = "d02.txt";

enum
{
    TOKEN_COLON = ':',
    TOKEN_SEMICOLON = ';',
    TOKEN_COMMA = ',',
    TOKEN_EOL = '\n',
    TOKEN_EOF = '\0',
    TOKEN_NUMBER = 256,
    TOKEN_RED,
    TOKEN_GREEN,
    TOKEN_BLUE,
    TOKEN_GAME,
};

typedef struct
{
    const char* Start;
    int Type;
} token;

typedef struct
{
    const char* At;
    const char* Start;
    token PrevToken;
    token CurrToken;
} scanner;

static bool IsWhitespace(char C)
{
    return C == ' ' || C == '\r';
}

static bool IsDigit(char C)
{
    return C >= '0' && C <= '9';
}

static token Token(scanner* Scanner, int Type)
{
    token Token;
    Token.Start = Scanner->Start;
    Token.Type = Type;
    return Token;
}

static token Next(scanner* Scanner)
{
    while(IsWhitespace(*Scanner->At)) Scanner->At++;
    Scanner->Start = Scanner->At;
    char C = *Scanner->At;
    if(IsDigit(C))
    {
        do
        {
            Scanner->At++;
        } while(IsDigit(*Scanner->At));
        return Token(Scanner, TOKEN_NUMBER);
    }
    switch(C)
    {
    case ':':
    case ';':
    case ',':
    case '\n':
    case '\0':
        Scanner->At++;
        return Token(Scanner, C);
    case 'G':
        Scanner->At += 4;
        return Token(Scanner, TOKEN_GAME);
    case 'b':
        Scanner->At += 4;
        return Token(Scanner, TOKEN_BLUE);
    case 'g':
        Scanner->At += 5;
        return Token(Scanner, TOKEN_GREEN);
    case 'r':
        Scanner->At += 3;
        return Token(Scanner, TOKEN_RED);
    default:
        fprintf(stderr, "Unexpected character '%d' (%d).", C, '\n');
        exit(EXIT_FAILURE);
    }
}

static bool Check(scanner* Scanner, int Type)
{
    return Scanner->CurrToken.Type == Type;
}

static void Advance(scanner* Scanner)
{
    Scanner->PrevToken = Scanner->CurrToken;
    Scanner->CurrToken = Next(Scanner);
}

static void Expect(scanner* Scanner, int Type)
{
    if(Check(Scanner, Type))
    {
        return Advance(Scanner);
    }
    fprintf(stderr, "Unexpected token.");
    exit(EXIT_FAILURE);
}

static int Number(scanner* Scanner)
{
    Expect(Scanner, TOKEN_NUMBER);
    return atoi(Scanner->PrevToken.Start);
}

static bool Accept(scanner* Scanner, int Type)
{
    if(Check(Scanner, Type))
    {
        Advance(Scanner);
        return true;
    }
    return false;
}

AOC_SOLVER(Part1)
{
    int64_t Sum = 0;
    scanner Scanner = { .At = Input };
    Advance(&Scanner);
    if(Check(&Scanner, TOKEN_GAME))
    {
        do
        {
            Expect(&Scanner, TOKEN_GAME);
            int GameID = Number(&Scanner);
            bool Possible = true;
            Expect(&Scanner, TOKEN_COLON);
            if(Check(&Scanner, TOKEN_NUMBER))
            {
                do
                {
                    do
                    {
                        int Cubes = Number(&Scanner);
                        Advance(&Scanner);
                        int Color = Scanner.PrevToken.Type - TOKEN_RED;
                        Possible &= Cubes <= 12 + Color;
                    } while(Accept(&Scanner, TOKEN_COMMA));
                } while(Accept(&Scanner, TOKEN_SEMICOLON));
            }
            if(Possible)
            {
                Sum += GameID;
            }
        } while(Accept(&Scanner, TOKEN_EOL));
    }
    return Sum;
}

AOC_SOLVER(Part2)
{
    int64_t Sum = 0;
    scanner Scanner = { .At = Input };
    Advance(&Scanner);
    if(Check(&Scanner, TOKEN_GAME))
    {
        do
        {
            Expect(&Scanner, TOKEN_GAME);
            Expect(&Scanner, TOKEN_NUMBER);
            int MaxCubes[3] = {0, 0, 0};
            Expect(&Scanner, TOKEN_COLON);
            if(Check(&Scanner, TOKEN_NUMBER))
            {
                do
                {
                    do
                    {
                        int Cubes = Number(&Scanner);
                        Advance(&Scanner);
                        int Color = Scanner.PrevToken.Type - TOKEN_RED;
                        if(MaxCubes[Color] < Cubes) MaxCubes[Color] = Cubes;
                    } while(Accept(&Scanner, TOKEN_COMMA));
                } while(Accept(&Scanner, TOKEN_SEMICOLON));
            }
            Sum += MaxCubes[0] * MaxCubes[1] * MaxCubes[2];
        } while(Accept(&Scanner, TOKEN_EOL));
    }
    return Sum;
}
