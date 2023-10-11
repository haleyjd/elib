/*
  ELib
  
  Quick secure string library
  
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

#include <string>

#include "elib.h"
#include "../hal/hal_platform.h"
#include "misc.h"
#include "qstring.h"

const qstring qstring::emptystr;

//=============================================================================
//
// Constructors, Destructors, and Reinitializers
//

void qstring::unLocalize(size_t pSize)
{
   if(isLocal())
   {
      buffer = ecalloc(char, 1, pSize);
      size   = pSize;
      strcpy(buffer, local);

      memset(local, 0, basesize);
   }
}

//
// Creates a qstring with a given initial size, which helps prevent
// unnecessary initial reallocations. Resets insertion point to zero.
// This is safe to call on an existing qstring to reinitialize it.
//
qstring &qstring::createSize(size_t pSize)
{
   // Can remain local?
   if(isLocal() && pSize <= basesize)
      clear();
   else
   {
      unLocalize(pSize);

      // Don't realloc if not needed
      if(size < pSize)
      {
         buffer = erealloc(char, buffer, pSize);
         size   = pSize;
      }
      clear();
   }

   return *this;
}

//
// Frees the qstring object's buffer, if it is not localized. The qstring
// returns to being localized at that point, and will be empty.
//
void qstring::freeBuffer()
{
   if(buffer && !isLocal())
      efree(buffer);

   // return to being local
   buffer = local;
   size   = basesize;
   clear();
}

//
// Move from the other qstring instance
//
void qstring::moveFrom(qstring &&other) noexcept
{
   // When other is not localized, take direct ownership of its buffer
   if(!other.isLocal())
   {
      buffer = other.buffer;
      index  = other.index;
      size   = other.size;
      std::memset(local, 0, sizeof(local));

      other.buffer = nullptr;
      other.freeBuffer(); // returns to being localized
   }
   else
   {
      // Copy the local buffer
      std::memcpy(local, other.local, sizeof(local));
      buffer = local;
      index  = other.index;
   }
}

//=============================================================================
//
// Basic Properties
//

//
// Indexing function to access a character in a qstring. This is slower but 
// more secure than using qstring::getBuffer with array indexing.
//
char qstring::charAt(size_t idx) const
{
   if(idx >= size)
      hal_platform.fatalError("qstring::charAt: index out of range");

   return buffer[idx];
}

//
// Read-write variant.
//
char &qstring::operator [] (size_t idx)
{
   if(idx >= size)
      hal_platform.fatalError("qstring::operator []: index out of range");

   return buffer[idx];
}

//
// Read-only variant.
//
const char &qstring::operator [] (size_t idx) const
{
   if(idx >= size)
      hal_platform.fatalError("qstring::operator []: index out of range");

   return buffer[idx];
}


//=============================================================================
//
// Basic String Ops
//

//
// Grows the qstring's buffer by the indicated amount. This is automatically
// called by other qstring methods, so there is generally no need to call it 
// yourself.
//
qstring &qstring::grow(size_t len)
{   
   if(len > 0)
   {
      const size_t newsize = size + len;

      if(isLocal()) // are we local?
      {
         if(newsize > basesize) // can we stay local?
            unLocalize(newsize);
      }
      else
      {
         buffer = erealloc(char, buffer, newsize);
         std::memset(buffer + size, 0, len);
         size += len;
      }
   }

   return *this;
}

//=============================================================================
// 
// Concatenation and Insertion/Deletion/Copying Functions
//

//
// Concatenates a C string onto the end of a qstring, expanding the buffer if
// necessary.
//
qstring &qstring::concat(const char *str)
{
   const size_t cursize = size;
   const size_t newsize = index + std::strlen(str) + 1;

   if(newsize > cursize)
      grow(newsize - cursize);

   std::strcat(buffer, str);

   index = std::strlen(buffer);

   return *this;
}

//
// Inserts a string at a given position in the qstring. If the
// position is outside the range of the string, an error will occur.
//
qstring &qstring::insert(const char *insertstr, size_t pos)
{
   const size_t insertstrlen = std::strlen(insertstr);
   const size_t totalsize    = index + insertstrlen + 1; 
   
   // pos must be between 0 and dest->index - 1
   if(pos >= index)
      hal_platform.fatalError("qstring::insert: position out of range");

   // grow the buffer to hold the resulting string if necessary
   if(totalsize > size)
      grow(totalsize - size);

   char   *insertpoint = buffer + pos;
   size_t  charstomove = index  - pos;

   // use memmove for absolute safety
   std::memmove(insertpoint + insertstrlen, insertpoint, charstomove);
   std::memmove(insertpoint, insertstr, insertstrlen);

   index = std::strlen(buffer);

   return *this;
}

//
// Copies at most count bytes from the C string into the qstring.
//
qstring &qstring::copy(const char *str, size_t count)
{
    if(index > 0)
        clear();

    if(count > 0)
    {
        const size_t newsize = count + 1;
        if(newsize > size)
            grow(newsize - size);

        M_Strlcpy(buffer, str, newsize);
        index = std::strlen(buffer);
    }

    return *this;
}

//
// Exchanges the contents of two qstrings.
//
void qstring::swapWith(qstring &str2)
{
   // Both must be unlocalized.
   unLocalize(size);
   str2.unLocalize(str2.size);

   char *const  tmpbuffer = this->buffer; // make a shallow copy
   const size_t tmpsize   = this->size;
   const size_t tmpindex  = this->index;

   this->buffer = str2.buffer;
   this->size   = str2.size;
   this->index  = str2.index;

   str2.buffer = tmpbuffer;
   str2.size   = tmpsize;
   str2.index  = tmpindex;
}

//
// Removes occurrences of a specified character at the beginning of a qstring.
//
qstring &qstring::lstrip(char c)
{
   size_t i   = 0;
   size_t len = index;

   while(buffer[i] != '\0' && buffer[i] == c)
      ++i;

   if(i)
   {
      if((len -= i) == 0)
         clear();
      else
      {
         std::memmove(buffer, buffer + i, len);
         std::memset(buffer + len, 0, size - len);
         index -= i;
      }
   }

   return *this;
}

//
// Removes occurrences of a specified character at the end of a qstring.
//
qstring &qstring::rstrip(char c)
{
   while(index && buffer[index - 1] == c)
      pop();

   return *this;
}

//
// Truncates the qstring to the indicated position.
//
qstring &qstring::truncate(size_t pos)
{
   // pos must be between 0 and qstr->index - 1
   if(pos >= index)
      hal_platform.fatalError("qstring::truncate: position out of range");

   std::memset(buffer + pos, 0, index - pos);
   index = pos;

   return *this;
}

//
// std::string-compatible erasure function.
//
qstring &qstring::erase(size_t pos, size_t n)
{
   // truncate handles the case of n == qstring::npos
   if(!n)
      return *this;
   else if(n == npos)
      return truncate(pos);

   // pos must be between 0 and qstr->index - 1
   if(pos >= index)
      return *this; // can't do this

   size_t endPos = pos + n;
   if(endPos > index)
      endPos = index;

   char *to   = buffer + pos;
   char *from = buffer + endPos;
   char *const end = buffer + index;

   while(to != end)
   {
      *to = *from;
      ++to;
      if(from != end)
         ++from;
   }

   index -= (endPos - pos);
   return *this;
}

//=============================================================================
//
// Stream Insertion Operators
//

qstring &qstring::operator << (int i)
{
   char buf[33];
   M_Itoa(i, buf, 10);
   return concat(buf);
}

qstring &qstring::operator << (double d)
{
   return concat(std::to_string(d).c_str());
}

//=============================================================================
//
// Hash Code Functions
//
// These are just convenience wrappers.
//

//
// Static version, for convenience and so that the convention of hashing a
// null pointer to 0 hash code is enforceable without special checks, even
// if the thing being hashed isn't a qstring instance.
//
unsigned int qstring::HashCodeStatic(const char *str)
{
   auto ustr = reinterpret_cast<const unsigned char *>(str);
   int c;
   unsigned int h = 0;

   while((c = *ustr++))
      h = ectype::toUpper(c) + (h << 6) + (h << 16) - h;

   return h;
}

//
// As above, but with case sensitivity.
//
unsigned int qstring::HashCodeCaseStatic(const char *str)
{
   auto ustr = reinterpret_cast<const unsigned char *>(str);
   int c;
   unsigned int h = 0;

   while((c = *ustr++))
      h = c + (h << 6) + (h << 16) - h;

   return h;
}

//=============================================================================
//
// Conversion Functions
//

qstring qstring::ToString(int i, int radix)
{
    char buf[33] = { '\0' };
    M_Itoa(i, buf, radix);
    return qstring(buf);
}

qstring qstring::ToString(euint u)
{
    return qstring(std::to_string(u).c_str());
}

qstring qstring::ToString(float f)
{
    return qstring(std::to_string(f).c_str());
}

qstring qstring::ToString(double d)
{
    return qstring(std::to_string(d).c_str());
}

qstring qstring::ToString(int64_t i64)
{
    return qstring(std::to_string(i64).c_str());
}

qstring qstring::ToString(uint64_t u64)
{
    return qstring(std::to_string(u64).c_str());
}

//=============================================================================
//
// Search Functions
//

//
// Finds the first occurance of a character in the qstring and returns its 
// position. Returns qstring_npos if not found.
//
size_t qstring::findFirstOf(char c) const
{
   const char *rover = buffer;
   bool found = false;
   
   while(*rover)
   {
      if(*rover == c)
      {
         found = true;
         break;
      }
      ++rover;
   }

   return found ? rover - buffer : npos;
}

//
// Finds the first occurance of a character in the qstring which does not
// match the provided character. Returns qstring_npos if not found.
//
size_t qstring::findFirstNotOf(char c) const
{
   const char *rover = buffer;
   bool found = false;

   while(*rover)
   {
      if(*rover != c)
      {
         found = true;
         break;
      }
      ++rover;
   }

   return found ? rover - buffer : npos;
}

//
// Find the last occurrance of a character in the qstring which matches
// the provided character. Returns qstring::npos if not found.
//
size_t qstring::findLastOf(char c) const
{
   const char *rover;
   bool found = false;
   
   if(!index)
      return npos;
   
   rover = buffer + index - 1;
   do
   {
      if(*rover == c)
      {
         found = true;
         break;
      }
   }
   while((rover == buffer) ? false : (--rover, true));

   return found ? rover - buffer : npos;
}

//
// Case-insensitive substring find
//
const char *qstring::findSubStrNoCase(const char *substr) const
{
    return M_StrCaseStr(buffer, substr);
}

//
// Case-insensitive substring check
//
bool qstring::containsNoCase(const char *needle) const
{
    return (M_StrCaseStr(buffer, needle) != nullptr);
}

//
// std::string-compatible find function.
//
size_t qstring::find(const char *s, size_t pos) const
{
   // pos must be between 0 and index - 1
   if(pos >= index)
      hal_platform.fatalError("qstring::find: position out of range");

   const char *const base   = buffer + pos;
   const char *const substr = std::strstr(base, s);
   
   return substr ? substr - buffer : npos;
}

//=============================================================================
//
// Case Handling
//

//
// qstring::toLower
//
// Converts the string to lowercase.
//
qstring &qstring::toLower()
{
   M_Strlwr(buffer);
   return *this;
}

//
// qstring::toUpper
//
// Converts the string to uppercase.
//
qstring &qstring::toUpper()
{
   M_Strupr(buffer);
   return *this;
}

//=============================================================================
//
// Substring Extraction
//

qstring qstring::substr(size_t start, size_t len) const
{
    const size_t l = index;

    if(l == 0 || start >= l)
        return qstring();

    if(start + len >= l)
        len = l - start;

    return qstring(&buffer[start], len);
}

//=============================================================================
//
// Replacement Operations
//

static ebyte qstr_repltable[256];

//
// Static routine for replacement functions.
//
static size_t QStrReplaceInternal(qstring *qstr, char repl)
{
   size_t repcount = 0;
   ebyte *rptr = (ebyte *)(qstr->getBuffer());

   // now scan through the qstring buffer and replace any characters that
   // match characters in the filter table.
   while(*rptr)
   {
      if(qstr_repltable[*rptr])
      {
         *rptr = (ebyte)repl;
         ++repcount; // count characters replaced
      }
      ++rptr;
   }

   return repcount;
}

//
// Replaces characters in the qstring that match any character in the filter
// string with the character specified by the final parameter.
//
size_t qstring::replace(const char *filter, char repl)
{
   const unsigned char *fptr = (unsigned char *)filter;

   std::memset(qstr_repltable, 0, sizeof(qstr_repltable));

   // first scan the filter string and build the replacement filter table
   while(*fptr)
      qstr_repltable[*fptr++] = 1;

   return QStrReplaceInternal(this, repl);
}

//
// As above, but replaces all characters NOT in the filter string.
//
size_t qstring::replaceNotOf(const char *filter, char repl)
{
   const unsigned char *fptr = (unsigned char *)filter;
   
   std::memset(qstr_repltable, 1, sizeof(qstr_repltable));

   // first scan the filter string and build the replacement filter table
   while(*fptr)
      qstr_repltable[*fptr++] = 0;

   return QStrReplaceInternal(this, repl);
}

//=============================================================================
//
// File Path Specific Routines
//

//
// Calls M_NormalizeSlashes on a qstring, which replaces \ characters with /
// and eliminates any duplicate slashes. This isn't simply a convenience
// method, as the qstring structure requires a fix-up after this function is
// used on it, in order to keep the string length correct.
//
qstring &qstring::normalizeSlashes()
{
   M_NormalizeSlashes(buffer);
   index = std::strlen(buffer);

   return *this;
}

//
// Concatenate a C string assuming the qstring's current contents are a file
// path. Slashes will be normalized.
//
qstring &qstring::pathConcatenate(const char *addend)
{
   // Only add a slash if this is not the initial path component.
   if(index > 0)
      *this += '/';

   *this += addend;
   normalizeSlashes();

   return *this;
}

//
// Similar to M_AddDefaultExtension, but for qstrings.
// Note: an empty string will not be modified.
//
qstring &qstring::addDefaultExtension(const char *ext)
{
   char *p = buffer;

   if(p && index > 0)
   {
      p = p + index - 1;  // no need to seek for \0 here
      while(p-- > buffer && *p != '/' && *p != '\\')
      {
         if(*p == '.')
            return *this; // has an extension already.
      }
      if(*ext != '.') // need a dot?
         *this += '.';
      *this += ext;   // add the extension
   }

   return *this;
}

//
// Removes a filespec from a path.
// If called on a path without a file, the last path component is removed.
//
qstring &qstring::removeFileSpec()
{
   size_t lastSlash;

   lastSlash = findLastOf('/');
   if(lastSlash == npos)
      lastSlash = findLastOf('\\');
   if(lastSlash != npos)
      truncate(lastSlash);

   return *this;
}

//
// Removes a file extension if one exists.
//
qstring &qstring::stripExtension()
{
    if(const size_t pos = findLastOf('.'); pos != npos)
        truncate(pos);

    return *this;
}

//
// Similar to M_ExtractFileBase, but for qstrings.
// This one is not limited to 8 character file names, and will include any
// file extension, however, so it is not strictly equivalent.
//
qstring qstring::extractFileBase() const
{
    const char *src = buffer + index - 1;

    // back up until a \ or the start
    while(src != buffer && 
          *(src - 1) != ':' &&
          *(src - 1) != '\\' &&
          *(src - 1) != '/')
    {
        --src;
    }

   return qstring(src);
}

//=============================================================================
// 
// Formatting
//

// 
// Adds quotation marks to the qstring.
//
qstring &qstring::makeQuoted()
{
   // if the string is empty, make it "", else add quotes around the contents
   if(index == 0)
      return concat("\"\"");
   else
   {
      insert("\"", 0);
      return push('\"');
   }
}

//
// Performs formatted printing into a qstring. 
//
int qstring::vprintf(const char *fmt, va_list va)
{
    va_list vaLength;
    va_copy(vaLength, va);

    // This returns the required length
    int stringLength = std::vsnprintf(nullptr, 0, fmt, vaLength);
    va_end(vaLength);

    if(stringLength >= 0)
    {
        const size_t bufferLength = size_t(stringLength + 1);
        const size_t curLength    = size;
        
        if(bufferLength > curLength)
            grow(bufferLength - curLength);

        stringLength = std::vsnprintf(buffer, bufferLength, fmt, va);
        if(stringLength >= 0)
        {
            if(size_t(stringLength) >= bufferLength)
            {
                buffer[bufferLength - 1] = '\0';
            }
            index = size_t(stringLength);
        }
    }

    return stringLength;
}

//
// Performs formatted printing into a qstring. 
//
int qstring::printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    const int res = vprintf(fmt, va);

    va_end(va);
    return res;
}

//
// Create a new qstring instance with formatted contents
//
qstring qstring::VPrintf(const char *fmt, va_list args)
{
    va_list vaLength;
    va_copy(vaLength, args);

    // This returns the required length
    int stringLength = std::vsnprintf(nullptr, 0, fmt, vaLength);
    va_end(vaLength);

    if(stringLength >= 0)
    {
        qstring ret { size_t(stringLength) + 1 };

        const size_t bufferLength = size_t(stringLength + 1);

        stringLength = std::vsnprintf(ret.buffer, bufferLength, fmt, args);
        if(stringLength >= 0)
        {
            if(size_t(stringLength) >= bufferLength)
            {
                ret.buffer[bufferLength - 1] = '\0';
            }
            ret.index = size_t(stringLength);
        }

        return ret;
    }
    else
        return qstring::emptystr;
}

//
// Create a new qstring instance with formatted contents
//
qstring qstring::Printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    qstring ret = VPrintf(fmt, va);

    va_end(va);
    return ret;
}

// EOF

