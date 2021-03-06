/*
  CALICO
  
  HAL Basic Platform Functions
  
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

#include <stdio.h>
#include "hal_types.h"

typedef struct hal_platform_s
{
   void        (*debugMsg)(const char *msg, ...);
   void        (*exitWithMsg)(const char *msg, ...);
   void        (*fatalError)(const char *msg, ...);
   void        (*setIcon)(void);
   FILE       *(*fileOpen)(const char *path, const char *mode);
   hal_bool    (*fileExists)(const char *path);
   hal_bool    (*directoryExists)(const char *path);
   hal_bool    (*makeDirectory)(const char *path);
} hal_platform_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern hal_platform_t hal_platform;

#if defined(__cplusplus)
}
#endif

// EOF
