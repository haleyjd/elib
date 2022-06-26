/*
  ELib

  Inline functions for portable binary data fetches

  The MIT License (MIT)

  Copyright (C) 2017 James Haley

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

#include "elib.h"

// Read a little-endian short without alignment assumptions
#define eread16_le(b, t) (t)((b)[0] | ((t)((b)[1]) << 8))

// Read a big-endian short without alignment assumptions
#define eread16_be(b, t) (t)((b)[1] | ((t)((b)[0]) << 8))

// Read a little-endian dword without alignment assumptions
#define eread32_le(b, t)      \
   (t)((b)[0] |              \
       ((t)((b)[1]) <<  8) | \
       ((t)((b)[2]) << 16) | \
       ((t)((b)[3]) << 24))

// Read a big-endian dword without alignment assumptions
#define eread32_be(b, t)      \
   (t)((b)[3] |              \
       ((t)((b)[2]) <<  8) | \
       ((t)((b)[1]) << 16) | \
       ((t)((b)[0]) << 24))

//
// Read an int16 from the lump data, but do not increment the read pointer.
//
inline int16_t E_ReadBinaryWord(ebyte *data)
{
   return eread16_le(data, int16_t);
}

inline void E_WriteBinaryWord(ebyte *data, int16_t val)
{
   ebyte sgn = (val < 0) ? 0x80 : 0x00;
   data[0]  = (ebyte)((val & 0x00ff) >> 0);
   data[1]  = (ebyte)((val & 0xff00) >> 8) | sgn;
}

//
// Reads an int16 from the lump data and increments the read pointer.
//
inline int16_t E_GetBinaryWord(ebyte **data)
{
   int16_t val = eread16_le(*data, int16_t);
   *data += 2;

   return val;
}

//
// Read a uint16 from the lump data, but do not increment the read pointer.
//
inline uint16_t E_ReadBinaryUWord(ebyte *data)
{
   return eread16_le(data, uint16_t);
}

//
// Reads a uint16 from the lump data and increments the read pointer.
//
inline uint16_t E_GetBinaryUWord(ebyte **data)
{
   uint16_t val = eread16_le(*data, uint16_t);
   *data += 2;

   return val;
}

//
// Reads an int32 from the lump data and increments the read pointer.
//
inline int32_t E_GetBinaryDWord(ebyte **data)
{
   int32_t val = eread32_le(*data, int32_t);
   *data += 4;

   return val;
}

//
// Reads an int32, big-endian, from the lump data and increments the read pointer.
//
inline int32_t E_GetBinaryDWordBE(ebyte **data)
{
   int32_t val = eread32_be(*data, int32_t);
   *data += 4;

   return val;
}

//
// Read a uint32 from the lump data, but do not increment the read pointer.
//
inline uint32_t E_ReadBinaryUDWord(ebyte *data)
{
   return eread32_le(data, uint32_t);
}

//
// Read a uint32, big-endian, from the lump data, but do not increment the read pointer.
//
inline uint32_t E_ReadBinaryUDWordBE(ebyte *data)
{
   return eread32_be(data, uint32_t);
}

//
// Reads a uint32 from the lump data and increments the read pointer.
//
inline uint32_t E_GetBinaryUDWord(ebyte **data)
{
   uint32_t val = eread32_le(*data, uint32_t);
   *data += 4;

   return val;
}

//
// Reads a uint32, big-endian, from the lump data and increments the read pointer.
//
inline uint32_t E_GetBinaryUDWordBE(ebyte **data)
{
   uint32_t val = eread32_be(*data, uint32_t);
   *data += 4;

   return val;
}

//
// Reads a "len"-byte string from the lump data and writes it into the 
// destination buffer. The read pointer is incremented by len bytes.
//
inline void E_GetBinaryString(ebyte **data, char *dest, int len)
{
   char *loc = (char *)(*data);

   std::memcpy(dest, loc, len);

   *data += len;
}

// EOF

