/*
  ELib
  
  Miscellaneous utilities
  
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

#include <errno.h>
#include "elib.h"
#include "../hal/hal_platform.h"
#include "misc.h"

//=============================================================================
//
// Basic File IO Utils
//

//
// Write a file from a data source
//
int M_WriteFile(const char *filename, const void *source, size_t length)
{
   FILE *fp;
   int result;

   errno = 0;

   if(!(fp = hal_platform.fileOpen(filename, "wb")))
      return 0;

   result = (std::fwrite(source, 1, length, fp) == length);
   std::fclose(fp);

   if(!result)
      std::remove(filename);

   return result;
}

//
// Get the length of a file
//
long M_FileLength(FILE *f)
{
   long curpos, len;
   curpos = std::ftell(f);
   std::fseek(f, 0, SEEK_END);
   len = std::ftell(f);
   std::fseek(f, 0, SEEK_SET);

   return len;
}

//
// Read in a file
//
size_t M_ReadFile(const char *name, ebyte **buffer)
{
   errno = 0;

   if(FILE *const fp = hal_platform.fileOpen(name, "rb"); fp != nullptr)
   {
      const size_t length = static_cast<size_t>(M_FileLength(fp));

      *buffer = ecalloc(uint8_t, 1, length);

      if(std::fread(*buffer, 1, length, fp) == length)
      {
         std::fclose(fp);
         return length;
      }
      std::fclose(fp);
   }

   return 0;
}

//
// Read in a file; C++ version using EUniquePtr for ownership management
//
size_t M_ReadFileUnique(const char *name, EUniquePtr<ebyte> &upbuffer)
{
    ebyte *pbuf = nullptr;
    const size_t ret = M_ReadFile(name, &pbuf);
    if(pbuf != nullptr)
        upbuffer.reset(pbuf);
    return ret;
}

//
// Assume the contents of a file are a string, null-terminating the buffer
// when reading it in.
//
char *M_LoadStringFromFile(const char *filename)
{
    char *buf = nullptr;

    if(FILE *const f = hal_platform.fileOpen(filename, "rb"); f != nullptr)
    {
        // allocate at length+1 for null termination
        const size_t len = static_cast<size_t>(M_FileLength(f));
        buf = ecalloc(char, 1, len + 1);
        if(std::fread(buf, 1, len, f) != len)
            hal_platform.debugMsg("Warning: short read of file %s\n", filename);
        std::fclose(f);
    }

    return buf;
}

//=============================================================================
//
// Polyfills for non-standard C functions
//

// Convert a string to uppercase in-place.
char *M_Strupr(char *string)
{
   char *s = string;

   while(*s)
   {
      int c = ectype::toUpper(*s);
      *s++ = c;
   }

   return string;
}

// Convert a string to lowercase in-place.
char *M_Strlwr(char *string)
{
   char *s = string;

   while(*s)
   {
      int c = ectype::toLower(*s);
      *s++ = c;
   }

   return string;
}

// Itoa - integer to string
char *M_Itoa(int value, char *string, int radix)
{
   int i;
   char temp[33];
   char *sp, *rover = temp;
   unsigned int v;

   // check for valid base
   if(radix <= 1 || radix > 36)
      return nullptr;

   const int sign = (radix == 10 && value < 0);

   if(sign)
      v = -value;
   else
      v = static_cast<unsigned int>(value);

   while(v || rover == temp)
   {
      i = v % radix;
      v = v / radix;

      if(i < 10)
         *rover++ = i + '0';
      else
         *rover++ = i + 'a' - 10;
   }

   if(string == nullptr)
      string = ecalloc(char, (rover - temp) + sign + 1, 1);
   sp = string;

   if(sign)
      *sp++ = '-';

   while(rover > temp)
   {
      *sp = *--rover;
      ++sp;
   }
   *sp = '\0';

   return string;
}

//=============================================================================
//
// Case-Insensitive Substring Search
//

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

//
// Find the first occurrence of find in s, ignore case.
//
const char *M_StrCaseStr(const char *s, const char *find)
{
    char c, sc;

    if((c = *find++) != 0)
    {
        c = ectype::toLower(c);
        const size_t len = strlen(find);
        do 
        {
            do
            {
                if((sc = *s++) == 0)
                    return nullptr;
            }
            while(static_cast<char>(ectype::toLower(sc)) != c);
        }
        while(strncasecmp(s, find, len) != 0);
        s--;
    }
    return s;
}

//
// Version for mutable strings
//
char *M_StrCaseStrMutable(char *s, const char *find)
{
    return const_cast<char *>(M_StrCaseStr(s, find));
}

//=============================================================================
//
// BSD-Style String Utilities
//

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// Copy src to string dst of size siz.  At most siz-1 characters
// will be copied.  Always NUL terminates (unless siz == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
//
size_t M_Strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    // Copy as many bytes as will fit
    if(n != 0 && --n != 0)
    {
        do
        {
            if((*d++ = *s++) == '\0')
                break;
        }
        while(--n != 0);
    }

    // Not enough room in dst, add NUL and traverse rest of src
    if(n == 0)
    {
        if(siz != 0)
            *d = '\0'; // NUL-terminate dst
        while(*s++)
            ; // counter loop
    }

    return (s - src - 1); // count does not include NUL
}

// 
// Appends src to string dst of size siz (unlike strncat, siz is the
// full size of dst, not space left).  At most siz-1 characters
// will be copied.  Always NUL terminates (unless siz == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
// 
size_t M_Strlcat(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    size_t dlen;

    // Find the end of dst and adjust bytes left but don't go past end
    while(*d != '\0' && n-- != 0)
        d++;
    dlen = d - dst;
    n = siz - dlen;

    if(n == 0)
        return (dlen + std::strlen(s));
    while(*s != '\0')
    {
        if(n != 1)
        {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return (dlen + (s - src)); // count does not include NUL
}

//=============================================================================
//
// Filename and Path Routines
//

void M_NormalizeSlashes(char *str)
{
   char *p;
   char useSlash     = '/';  // slash type to use for normalization
   char replaceSlash = '\\'; // type of slash to replace
   bool isUNC        = false;

   if(std::strlen(str) > 2 &&
      ((str[0] == '\\' || str[0] == '/') && str[0] == str[1]))
   {
      useSlash     = '\\';
      replaceSlash = '/';
      isUNC        = true;
   }

   // convert all replaceSlash to useSlash
   for(p = str; *p; p++)
   {
      if(*p == replaceSlash)
         *p = useSlash;
   }

   // remove trailing slashes
   while(p > str && *--p == useSlash)
      *p = '\0';

   // collapse multiple slashes
   for(p = str + (isUNC ? 2 : 0); (*str++ = *p); )
   {
      if(*p++ == useSlash)
      {
         while(*p == useSlash)
            ++p;
      }
   }
}

//
// M_StringAlloc
//
// haleyjd: This routine takes any number of strings and a number of extra
// characters, calculates their combined length, and calls ecalloc to create
// a temporary buffer of that size. This is extremely useful for allocation of
// file paths. The pointer returned in *str must be manually freed.
//
int M_StringAlloc(char **str, int numstrs, size_t extra, const char *str1, ...)
{
    va_list args;
    size_t len = extra;

    if(numstrs < 1)
        hal_platform.fatalError("M_StringAlloc: invalid input\n");

    len += std::strlen(str1);

    --numstrs;

    if(numstrs != 0)
    {   
        va_start(args, str1);

        while(numstrs != 0)
        {
            const char *argstr = va_arg(args, const char *);

            len += std::strlen(argstr);

            --numstrs;
        }

        va_end(args);
    }

    ++len;

    *str = ecalloc(char, 1, len);

    return len;
}

//
// M_SafeFilePath
//
// haleyjd 20110210 - original routine.
// This routine performs safe, portable concatenation of a base file path
// with another path component or file name. The returned string is ecalloc'd
// and should be freed when it has exhausted its usefulness.
//
char *M_SafeFilePath(const char *basepath, const char *newcomponent)
{
    int   newstrlen = 0;
    char *newstr = nullptr;

    if(!std::strcmp(basepath, ""))
    {
        basepath = ".";
    }

    // Always throw in a slash. M_NormalizeSlashes will remove it in the case
    // that either basepath or newcomponent includes a redundant slash at the
    // end or beginning respectively.
    newstrlen = M_StringAlloc(&newstr, 3, 1, basepath, "/", newcomponent);
    std::snprintf(newstr, newstrlen, "%s/%s", basepath, newcomponent);
    M_NormalizeSlashes(newstr);

    return newstr;
}

// EOF

