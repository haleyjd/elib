/*
  ELib
  
  ctype.h safety wrapper functions
  
  The MIT License (MIT)
  
  Copyright (c) 2016 James Haley
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <ctype.h>

#if defined(__cplusplus)
#define ect(fn) fn
#else
#define ect(fn) ectype_ ## fn
#endif

//
// Namespace for ELib character type functions.
//
#if defined(__cplusplus)
namespace ectype {
#endif

// Return 1 if c is a control character, 0 otherwise.
inline int ect(isCntrl)(char c)
{
    return iscntrl((unsigned char)(c));
}

// Return 1 if c is a blank character, 0 otherwise.
inline int ect(isBlank)(char c)
{
    return isblank((unsigned char)(c));
}

// Return 1 if c is a whitespace character, 0 otherwise.
inline int ect(isSpace)(char c)
{
    return isspace((unsigned char)(c));
}

// Return 1 if c is an uppercase alphabetic character, 0 otherwise.
inline int ect(isUpper)(char c)
{
    return isupper((unsigned char)(c));
}

// Return 1 if c is a lowercase alphabetic character, 0 otherwise.
inline int ect(isLower)(char c)
{
    return islower((unsigned char)(c));
}

// Return 1 if c is an alphabetic character, 0 otherwise.
inline int ect(isAlpha)(char c)
{
    return isalpha((unsigned char)(c));
}

// Return 1 if c is a base 10 numeral, 0 otherwise.
inline int ect(isDigit)(char c)
{
    return isdigit((unsigned char)(c));
}

// Return 1 if c is a base 16 numeral, 0 otherwise.
inline int ect(isXDigit)(char c)
{
    return isxdigit((unsigned char)(c));
}

// Return 1 if c is either a base 10 digit or an alphabetic character.
// Return 0 otherwise.
inline int ect(isAlnum)(char c)
{
    return isalnum((unsigned char)(c));
}

// Return 1 if c is a punctuation mark, 0 otherwise.
inline int ect(isPunct)(char c)
{
    return ispunct((unsigned char)(c));
}

// Return 1 if c is a character with a graphical glyph.
// Return 0 otherwise.
inline int ect(isGraph)(char c)
{
    return isgraph((unsigned char)(c));
}

// Return 1 if c is a printable character, 0 otherwise.
inline int ect(isPrint)(char c)
{
    return isprint((unsigned char)(c));
}

// Convert lowercase alphabetics to uppercase; leave other inputs alone.
inline int ect(toUpper)(char c)
{
    return toupper((unsigned char)(c));
}

// Convert uppercase alphabetics to lowercase; leave other inputs alone.
inline int ect(toLower)(char c)
{
    return tolower((unsigned char)(c));
}

#if defined(__cplusplus)
} // end namespace
#endif

// EOF
