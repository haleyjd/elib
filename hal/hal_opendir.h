/*
  ELib
  
  HAL Directory Enumeration Interface
  
  The MIT License (MIT)
  
  Copyright (c) 2022 James Haley
  
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

#include "hal_types.h"

// hal_dir_t is an opaque type equivalent to POSIX DIR
typedef struct hal_dir_t hal_dir_t;

// hal_direntry_t is an opaque type equivalent to POSIX DIRENT
typedef struct hal_direntry_t hal_direntry_t;

typedef struct hal_opendir_s
{
    hal_dir_t      *(*OpenDir)(const char *path);
    hal_direntry_t *(*ReadDir)(hal_dir_t *dir);
    hal_bool        (*CloseDir)(hal_dir_t *dir);
    void            (*RewindDir)(hal_dir_t *dir);
    long            (*TellDir)(hal_dir_t *dir);
    void            (*SeekDir)(hal_dir_t *dir, long lpos);
    const char     *(*GetEntryName)(hal_direntry_t *ent);
} hal_opendir_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern hal_opendir_t hal_opendir;

#if defined(__cplusplus)
}
#endif

// EOF
