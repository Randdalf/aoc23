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
    TOKEN_BLUE,
    TOKEN_RED,
    TOKEN_GAME,
    TOKEN_GREEN,
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
                        switch(Scanner.PrevToken.Type)
                        {
                        case TOKEN_RED:
                            Possible &= Cubes <= 12;
                            break;
                        case TOKEN_GREEN:
                            Possible &= Cubes <= 13;
                            break;
                        case TOKEN_BLUE:
                            Possible &= Cubes <= 14;
                            break;
                        default:
                            fprintf(stderr, "Unexpected color.");
                            exit(EXIT_FAILURE);
                        }
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
    return -1;
}
