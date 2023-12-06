#pragma once

#include <stdbool.h>

static inline bool IsDigit(char C)
{
    return C >= '0' && C <= '9';
}

static inline bool IsWhitespace(char C)
{
    return C == ' ' || C == '\r';
}

static inline const char* SkipPastDigits(const char* Input)
{
    while(IsDigit(*Input)) Input++;
    return Input;
}

static inline const char* SkipPastNewline(const char* Input)
{
    if(*Input == '\r') Input++;
    if(*Input == '\n') Input++;
    return Input;
}

static inline const char* SkipPastWhitespace(const char* Input)
{
    while(IsWhitespace(*Input)) Input++;
    return Input;
}

static inline const char* SkipToDigits(const char* Input)
{
    while(!IsDigit(*Input)) Input++;
    return Input;
}
