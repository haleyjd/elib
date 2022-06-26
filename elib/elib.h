/*
  ELib
  
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

// C/C++ standard library
#if defined(__cplusplus)
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#else
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

#include <stddef.h>
#include <stdint.h>

// keywords
#if defined(__cplusplus)
#define EDEPRECATED(msg) [[deprecated(msg)]]
#define ENORETURN        [[noreturn]]
#define EUNUSED          [[maybe_unused]]
#else
#define EDEPRECATED(msg)
#define ENORETURN        _Noreturn
#define EUNUSED
#endif

// assertions
#include "eassert.h"

// ctype safety wrappers
#include "m_ctype.h"

// comparison functions
#include "compare.h"

// Memory handling
#include "zone.h"

#if defined(__cplusplus)
// Smart pointer types
#include "esmartptr.h"

// Non-throwing new (calls elib error instead, safe for calls by C code)
#include "ecnew.h"
#endif

// String case comparison
#if defined(_MSC_VER) && !defined(strcasecmp)
#define strcasecmp _stricmp
#endif

#if defined(_MSC_VER) && !defined(strncasecmp)
#define strncasecmp _strnicmp
#endif

// Get the size of a static array
#define earrlen(a) (sizeof(a) / sizeof(*a))

// Define a struct var and ensure it is fully initialized
#if defined(__cplusplus)
#define edefstructvar(type, name)  \
   type name;                      \
   std::memset(&name, 0, sizeof(name))
#else
#define edefstructvar(type, name)  \
   type name;                      \
   memset(&name, 0, sizeof(name))
#endif

// Empty-or-null testing
#define estrempty(str) ((str) == NULL || *(str) == '\0')

// Types
typedef uint8_t  ebyte;
typedef int32_t  efixed;
typedef uint32_t euint;

// Fixed-point defines
#define EFRACBITS 16
#define EFRACUNIT (1<<EFRACBITS)

//
// Structure packing via #pragma pack
//
#if defined(_MSC_VER) || defined(__GNUC__)
#define ELIB_HAS_PACKING 1
#endif

//
// Is 64-bit processor?
// Add your own processor define here if it's not covered.
//
#if defined(__x86_64__) || defined(__LP64__) || defined(_M_AMD64)
#define ELIB_IS_X64 1
#endif

//
// Some platforms may not allow an application to exit on its own
//
#if defined(_WIN32) || defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#define ELIB_ALLOW_EXIT 1
#endif

// EOF
