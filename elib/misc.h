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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//
// File IO Utils
//

// Write a file from a data source
int     M_WriteFile(const char *filename, const void *source, size_t length);
// Get the length of a file
long    M_FileLength(FILE *f);
// Read in a file
size_t  M_ReadFile(const char *name, ebyte **buffer);
#if defined(__cplusplus)
// Read in a file; C++ version using EUniquePtr for ownership management
size_t  M_ReadFileUnique(const char *name, EUniquePtr<ebyte> &upbuffer);
#endif
// Assume the contents of a file are a string, null-terminating the buffer when reading it in.
char   *M_LoadStringFromFile(const char *filename);

//
// C library polyfills
//

// Convert a string to uppercase in-place.
char *M_Strupr(char *string);
// Convert a string to lowercase in-place.
char *M_Strlwr(char *string);
// Itoa - integer to string with specifiable base
char *M_Itoa(int value, char *string, int radix);

// Find the first occurrence of find in s, ignore case.
const char *M_StrCaseStr(const char *s, const char *find);
// Version of M_StrCaseStr for mutable strings
char       *M_StrCaseStrMutable(char *s, const char *find);

//
// BSD-like String Utilities
//

// Copy src to string dst of size siz.  At most siz-1 characters
// will be copied.  Always NUL terminates (unless siz == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
size_t M_Strlcpy(char *dst, const char *src, size_t siz);

// Appends src to string dst of size siz (unlike strncat, siz is the
// full size of dst, not space left).  At most siz-1 characters
// will be copied.  Always NUL terminates (unless siz == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
size_t M_Strlcat(char *dst, const char *src, size_t siz);

//
// Filename and Path Utils
//

// Normalizes slashes in a file path:
// * Replaces \\ with / except in UNC paths
// * Removes trailing slashes
// * Deduplicates consecutive slashes, except at beginning of an UNC path
void  M_NormalizeSlashes(char *str);

// This routine takes any number of strings and a number of extra
// characters, calculates their combined length, and calls ecalloc to create
// a temporary buffer of that size. This is extremely useful for allocation of
// file paths. The pointer returned in *str must be manually freed.
int   M_StringAlloc(char **str, int numstrs, size_t extra, const char *str1, ...);

// This routine performs safe, portable concatenation of a base file path
// with another path component or file name. The returned string is ecalloc'd
// and should be freed when it has exhausted its usefulness.
char *M_SafeFilePath(const char *basepath, const char *newcomponent);

#ifdef __cplusplus
}
#endif

// EOF

