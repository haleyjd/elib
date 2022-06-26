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

int     M_WriteFile(const char *filename, const void *source, size_t length);
long    M_FileLength(FILE *f);
size_t  M_ReadFile(const char *name, ebyte **buffer);
char   *M_LoadStringFromFile(const char *filename);

//
// C library polyfills
//

char *M_Strupr(char *string);
char *M_Strlwr(char *string);
char *M_Itoa(int value, char *string, int radix);

const char *M_StrCaseStr(const char *s, const char *find);
char       *M_StrCaseStrMutable(char *s, const char *find);

//
// BSD-like String Utilities
//

size_t M_Strlcpy(char *dst, const char *src, size_t siz);
size_t M_Strlcat(char *dst, const char *src, size_t siz);

//
// Filename and Path Utils
//

void  M_NormalizeSlashes(char *str);
int   M_StringAlloc(char **str, int numstrs, size_t extra, const char *str1, ...);
char *M_SafeFilePath(const char *basepath, const char *newcomponent);

#ifdef __cplusplus
}
#endif

// EOF

