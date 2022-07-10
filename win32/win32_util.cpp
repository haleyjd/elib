/*
  ELib
  
  Win32 Utilities
  
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "win32_util.h"

//
// Convert UTF-8 multibyte characters to wide string
//
std::wstring Win32_UTF8ToWStr(const char *instr)
{
    const int clen = int(std::strlen(instr));
    const int size_needed = MultiByteToWideChar(CP_UTF8, 0, instr, clen, nullptr, 0);
    std::wstring wStrTo(size_t(size_needed) + 1, '\0');
    MultiByteToWideChar(CP_UTF8, 0, instr, clen, &wStrTo[0], size_needed);
    return wStrTo;
}

//
// Convert wide string to UTF-8 multibyte characters.
//
std::string Win32_WideToStdString(const std::wstring &wstr)
{
    const size_t len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    const std::unique_ptr<char []> szTo { new char [len + 1] };
    szTo[len] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szTo.get(), static_cast<int>(len), nullptr, nullptr);
    return std::string(szTo.get());
}

// EOF
