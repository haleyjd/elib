/*
  ELib
  
  Win32 Directory Enumeration Implementation
  
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

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>

#include "../elib/elib.h"
#include "../elib/qstring.h"
#include "../elib/misc.h"
#include "../hal/hal_opendir.h"
#include "win32_util.h"
#include "win32_opendir.h"

//=============================================================================
//
// Win32 OpenDir Implementation
//
//=============================================================================

typedef struct hal_direntry_t
{
    char d_name[FILENAME_MAX]; // file name
} hal_direntry_t;

typedef struct hal_dir_t
{
    // disk transfer area for this dir
    struct _wfinddata64i32_t dd_dta;

    // dirent struct to return from dir (NOTE: this makes this thread safe as 
    // long as only one thread uses a particular DIR struct at a time)
    hal_direntry_t dd_dir;

    // _findnext handle
    intptr_t dd_handle = 0;

     // Status of search:
     //  0 = not started yet (next entry to read is first entry)
     // -1 = off the end
     //  positive = 0 based index of next entry
    int dd_stat = 0;

    // given path for dir with search pattern
    qstring dd_name;
} hal_dir_t;

//
// Open a directory into a hal_dir_t structure.
//
static hal_dir_t *Win32_OpenDir(const char *path)
{
    if(estrempty(path))
    {
        // Invalid input; EFAULT or ENOTDIR
        return nullptr;
    }

    // Attempt to determine if the given path really is a directory.
    const std::wstring wpath { Win32_UTF8ToWStr(path) };
    const unsigned int rc = GetFileAttributesW(wpath.data());
    if(rc == static_cast<unsigned int>(-1))
    {
        // Not found (ENOENT)
        return nullptr;
    }
    if(!(rc & FILE_ATTRIBUTE_DIRECTORY))
    {
        // Error, entry exists but not a directory. (ENOTDIR)
        return nullptr;
    }

    // Make an absolute pathname
    char szFullPath[MAX_PATH];
    ZeroMemory(szFullPath, MAX_PATH);
    _fullpath(szFullPath, path, MAX_PATH);

    std::unique_ptr<hal_dir_t> upDir { ecnew<hal_dir_t>() };
    hal_dir_t &dir = *upDir;

    // Create the search expression.
    dir.dd_name = szFullPath;

    // Add on a slash if the path does not end with one.
    if(!dir.dd_name.empty() && 
       !dir.dd_name.endsWith('/') && 
       !dir.dd_name.endsWith('\\'))
    {
        dir.dd_name += "\\";
    }

    // Add on the search pattern
    dir.dd_name += "*";

    // Initialize handle to -1 so that a premature closedir doesn't try to call _findclose on it
    dir.dd_handle = -1;

    // Initialize the status
    dir.dd_stat = 0;

    return upDir.release();
}

//
// Read next directory entry.
//
static hal_direntry_t *Win32_ReadDir(hal_dir_t *dir)
{
    if(dir == nullptr)
        return nullptr;

    if(dir->dd_stat < 0)
    {
        // We have already returned all files in the directory (or the structure
        // has an invalid dd_stat).
        return nullptr;
    }
    else if(dir->dd_stat == 0)
    {
        // We haven't started the search yet, start it now.
        std::wstring wdd_name { Win32_UTF8ToWStr(dir->dd_name.c_str()) };
        dir->dd_handle = _wfindfirst(wdd_name.data(), &dir->dd_dta);

        if(dir->dd_handle == -1)
        {
            // Whoops! Seems there are no files in that directory.
            dir->dd_stat = -1;
        }
        else
        {
            dir->dd_stat = 1;
        }
    }
    else
    {
        // Get the next search entry.
        if(_wfindnext(dir->dd_handle, &dir->dd_dta))
        {
            // We are off the end or otherwise error.
            _findclose(dir->dd_handle);
            dir->dd_handle = -1;
            dir->dd_stat   = -1;
        }
        else
        {
            // Update the status to indicate the correct number.
            dir->dd_stat++;
        }
    }

    if(dir->dd_stat > 0)
    {
        // Successfully got an entry. Everything about the file is already
        // appropriately filled in except the length of the file name.
        const std::string dd_dtaName { Win32_WideToStdString(dir->dd_dta.name) };
        M_Strlcpy(dir->dd_dir.d_name, dd_dtaName.c_str(), sizeof(dir->dd_dir.d_name));
        return &dir->dd_dir;
    }

    return nullptr;
}

//
// Close and free the directory.
//
static hal_bool Win32_CloseDir(hal_dir_t *dir)
{
    if(dir == nullptr)
        return HAL_FALSE;

    int rc = 0;
    if(dir->dd_handle != -1)
        rc = _findclose(dir->dd_handle);

    delete dir;

    return rc == 0 ? HAL_TRUE : HAL_FALSE;
}

//
// Rewind directory structure to the beginning of the enumeration.
//
static void Win32_RewindDir(hal_dir_t *dir)
{
    if(dir == nullptr)
        return;

    if(dir->dd_handle != -1)
        _findclose(dir->dd_handle);

    dir->dd_handle = -1;
    dir->dd_stat   =  0;
}

//
// Report position in the directory enumeration.
//
static long Win32_TellDir(hal_dir_t *dir)
{
    return dir ? dir->dd_stat : -1;
}

//
// Seek to a specific position in the directory enumeration.
//
static void Win32_SeekDir(hal_dir_t *dir, long lpos)
{
    if(dir == nullptr)
        return;

    if(lpos < -1)
    {
        // seeking to an invalid position (EINVAL)
        return;
    }
    else if(lpos == -1)
    {
        // seek past end
        if(dir->dd_handle != -1)
            _findclose(dir->dd_handle);
        dir->dd_handle = -1;
        dir->dd_stat   = -1;
    }
    else
    {
        // Rewind dir and read forward to the appropriate index
        Win32_RewindDir(dir);

        while(dir->dd_stat < lpos && Win32_ReadDir(dir) != nullptr)
            ; // nothing to do here
    }
}

//
// Get an entry name
//
static const char *Win32_GetEntryName(hal_direntry_t *ent)
{
    return ent ? ent->d_name : "";
}

//
// Load Win32 implementation function pointers into the hal_opendir interface
//
void Win32_InitOpenDir()
{
    hal_opendir.OpenDir      = Win32_OpenDir;
    hal_opendir.ReadDir      = Win32_ReadDir;
    hal_opendir.CloseDir     = Win32_CloseDir;
    hal_opendir.RewindDir    = Win32_RewindDir;
    hal_opendir.TellDir      = Win32_TellDir;
    hal_opendir.SeekDir      = Win32_SeekDir;
    hal_opendir.GetEntryName = Win32_GetEntryName;
}

#endif

// EOF
