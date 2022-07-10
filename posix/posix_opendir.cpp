/*
  ELib
  
  POSIX Directory Enumeration Implementation
  
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

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)

#include <dirent.h>
#include "../elib/elib.h"
#include "../hal/hal_opendir.h"
#include "posix_opendir.h"

//=============================================================================
//
// POSIX OpenDir Implementation
//
//=============================================================================

typedef struct hal_direntry_t
{
    struct dirent *m_pent = nullptr;
} hal_direntry_t;

typedef struct hal_dir_t
{
    hal_direntry_t m_ent;
    DIR *m_dir = nullptr;
} hal_dir_t;

//
// Open a directory into a hal_dir_t structure.
//
static hal_dir_t *POSIX_OpenDir(const char *path)
{
    hal_dir_t *pRet = nullptr;
    if(DIR *const pDir = opendir(path); pDir != nullptr)
    {
        pRet = ecnew<hal_dir_t>();
        pRet->m_dir = pDir;
    }
    return pRet;
}

//
// Read next directory entry.
//
static hal_direntry_t *POSIX_ReadDir(hal_dir_t *dir)
{
    if(dir == nullptr || dir->m_dir == nullptr)
        return nullptr;
    
    dir->m_ent.m_pent = readdir(dir->m_dir);
    return dir->m_ent.m_pent ? &dir->m_ent : nullptr;
}

//
// Close and free the directory.
//
static hal_bool POSIX_CloseDir(hal_dir_t *dir)
{
    if(dir == nullptr)
        return HAL_FALSE;
    
    int rc = 0;
    if(dir->m_dir != nullptr)
        rc = closedir(dir->m_dir);
    
    delete dir;
    
    return rc == 0 ? HAL_TRUE : HAL_FALSE;
}

//
// Rewind directory structure to the beginning of the enumeration.
//
static void POSIX_RewindDir(hal_dir_t *dir)
{
    if(dir == nullptr || dir->m_dir == nullptr)
        return;
    rewinddir(dir->m_dir);
}

//
// Report position in the directory enumeration.
//
static long POSIX_TellDir(hal_dir_t *dir)
{
    if(dir == nullptr || dir->m_dir == nullptr)
        return -1;
    
    return telldir(dir->m_dir);
}

//
// Seek to a specific position in the directory enumeration.
//
static void POSIX_SeekDir(hal_dir_t *dir, long lpos)
{
    if(dir == nullptr || dir->m_dir == nullptr)
        return;
    
    return seekdir(dir->m_dir, lpos);
}

//
// Get an entry name
//
static const char *POSIX_GetEntryName(hal_direntry_t *ent)
{
    return (ent && ent->m_pent) ? ent->m_pent->d_name : "";
}

//
// Load POSIX implementation function pointers into the hal_opendir interface
//
void POSIX_InitOpenDir()
{
    hal_opendir.OpenDir      = POSIX_OpenDir;
    hal_opendir.ReadDir      = POSIX_ReadDir;
    hal_opendir.CloseDir     = POSIX_CloseDir;
    hal_opendir.RewindDir    = POSIX_RewindDir;
    hal_opendir.TellDir      = POSIX_TellDir;
    hal_opendir.SeekDir      = POSIX_SeekDir;
    hal_opendir.GetEntryName = POSIX_GetEntryName;
}

#endif

// EOF
