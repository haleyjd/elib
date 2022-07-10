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

#include "hal_opendir.h"

//=============================================================================
// 
// Default implementation - this is do-nothing.
//
//=============================================================================

static hal_dir_t *HAL_OpenDir(const char *path)
{
    return nullptr;
}

static hal_direntry_t *HAL_ReadDir(hal_dir_t *)
{
    return nullptr;
}

static hal_bool HAL_CloseDir(hal_dir_t *)
{
    return HAL_FALSE;
}

static void HAL_RewindDir(hal_dir_t *)
{
}

static long HAL_TellDir(hal_dir_t *)
{
    return -1;
}

static void HAL_SeekDir(hal_dir_t *, long lpos)
{
}

static const char *HAL_GetEntryName(hal_direntry_t *)
{
    return "";
}

// global singleton
hal_opendir_t hal_opendir =
{
    HAL_OpenDir,
    HAL_ReadDir,
    HAL_CloseDir,
    HAL_RewindDir,
    HAL_TellDir,
    HAL_SeekDir,
    HAL_GetEntryName
};

// EOF
