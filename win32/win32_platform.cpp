/*
  ELib
  
  Win32 Basic Platform Functions
  
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

#if defined(_WIN32)

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <io.h>
#include <Windows.h>

// COUGAR_FIXME: icon
//#include "../../vc2015/resource.h"

#include "../econfig.h"
#include "../elib/elib.h"
#include "../elib/misc.h"
#include "../hal/hal_ml.h"
#include "../hal/hal_platform.h"
#include "../hal/hal_video.h"
#include "win32_platform.h"

//
// Display a debug message. For Windows, this will open a debug console
// window the first time it is called.
//
static void Win32_DebugMsg(const char *msg, ...)
{
#if defined(_DEBUG)
    va_list args;
    static BOOL debugInit = FALSE;
    size_t len = std::strlen(msg);
    
    if(!debugInit)
    {
        if(AllocConsole())
        {
            if(std::freopen("CONOUT$", "w", stdout) == nullptr)
                std::fprintf(stderr, "Warning: could not redirect stdout to console\n");
        }
        debugInit = TRUE;
    }

    va_start(args, msg);
    std::vprintf(msg, args);
    va_end(args);

    if(len >= 1 && msg[len - 1] != '\n')
        std::printf("\n");
#endif
}

//
// Exit with a message that is not necessarily an error condition.
//
static void Win32_ExitWithMsg(const char *msg, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, msg);
    std::vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    // try the media layer's method first; if it fails, use native
    if(hal_medialayer.msgbox(ELIB_APP_NAME, buf, HAL_FALSE))
        MessageBoxA(nullptr, buf, ELIB_APP_NAME, MB_OK | MB_ICONINFORMATION);

    hal_medialayer.exit();
}

//
// Exit with a fatal error
//
static void Win32_FatalError(const char *msg, ...)
{
    va_list args;
    char buf[1024];

    va_start(args, msg);
    std::vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    // try the media layer's method first; if it fails, use native
    if(hal_medialayer.msgbox(ELIB_APP_NAME, buf, HAL_TRUE))
        MessageBoxA(nullptr, buf, ELIB_APP_NAME, MB_OK | MB_ICONERROR);

    hal_medialayer.error();
}

//
// Set the main application window icon. This implementation uses a native
// Win32 icon resource.
//
static void Win32_SetIcon()
{
    // COUGAR_FIXME: icon
#if 0
    HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
    if(hIcon)
    {
        HWND hWnd = (HWND)(hal_video.getWindowHandle());
        if(hWnd)
        {
            SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage(hWnd, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
        }
    }
#endif
}

static std::wstring UTF8ToWStr(const char *instr)
{
    const int clen = int(std::strlen(instr));
    const int size_needed = MultiByteToWideChar(CP_UTF8, 0, instr, clen, nullptr, 0);
    std::wstring wStrTo(size_t(size_needed) + 1, '\0');
    MultiByteToWideChar(CP_UTF8, 0, instr, clen, &wStrTo[0], size_needed);
    return wStrTo;
}

//
// Open a file with the input path being assumed to contain a UTF-8 encoded string
//
static FILE *Win32_FileOpen(const char *path, const char *mode)
{
    const std::wstring wpath { UTF8ToWStr(path) };
    const std::wstring wmode { UTF8ToWStr(mode) };

    return _wfopen(wpath.c_str(), wmode.c_str());
}

//
// Check if a file exists and is not a directory
//
static hal_bool Win32_FileExists(const char *path)
{
    hal_bool res = HAL_FALSE;
    const std::wstring wpath { UTF8ToWStr(path) };

    if(const DWORD attribs = GetFileAttributesW(wpath.c_str()); attribs != INVALID_FILE_ATTRIBUTES)
    {
        if(!(attribs & FILE_ATTRIBUTE_DIRECTORY))
            res = HAL_TRUE;
    }

    return res;
}

//
// Populate the HAL platform interface with Win32 implementation function pointers
//
void Win32_InitHAL()
{
    hal_platform.debugMsg    = Win32_DebugMsg;
    hal_platform.exitWithMsg = Win32_ExitWithMsg;
    hal_platform.fatalError  = Win32_FatalError;
    hal_platform.setIcon     = Win32_SetIcon;
    hal_platform.fileOpen    = Win32_FileOpen;
    hal_platform.fileExists  = Win32_FileExists;
}

#endif

// EOF
