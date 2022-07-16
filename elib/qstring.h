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

#pragma once

#include <functional>
#include <string_view>

//
// Quasar's robust, secure string class.
//
class qstring
{
public:
    static constexpr size_t npos = ((size_t) -1);
    static constexpr size_t basesize = 16;

    static const qstring emptystr;

    // === Constructors / Destructor ====================================================

    qstring(size_t startSize = 0) noexcept
       : index(0), size(basesize)
    {
        buffer = local;
        std::memset(local, 0, sizeof(local));
        if(startSize)
            createSize(startSize);
    }

    qstring(const qstring &other) noexcept 
       : index(0), size(basesize)
    {
        buffer = local;
        std::memset(local, 0, sizeof(local));
        copy(other);
    }
    
    explicit qstring(const char *cstr) noexcept
       : index(0), size(basesize)
    {
        buffer = local;
        std::memset(local, 0, sizeof(local));
        copy(cstr);
    }

    qstring(const char *start, size_t len) noexcept
        : index(0), size(basesize)
    {
        buffer = local;
        std::memset(local, 0, sizeof(local));
        copy(start, len);
    }

    qstring(qstring &&other) noexcept
        : index(0), size(basesize)
    {
        moveFrom(std::move(other));
    }
    
    ~qstring() { freeBuffer(); }
    
    // === Basic Property Getters =======================================================
    
    //
    // Retrieves a pointer to the internal buffer. This pointer shouldn't be 
    // cached, and is not meant for writing into (although it is safe to do so, it
    // circumvents the encapsulation and security of this structure).
    //
    char *getBuffer() { return buffer; }

    //
    // Like qstring::getBuffer, but casts to const to enforce safety.
    //
    const char *c_str() const { return buffer; }
    
    //
    // Because the validity of "index" is maintained by all insertion and editing
    // functions, we can bypass calling strlen.
    //
    size_t length() const { return index; }
    
    //
    // Test if the string is empty.
    //
    bool empty() const { return (index == 0); }

    //
    // Returns the amount of size allocated for this qstring (will be >= strlen).
    // You are allowed to index into the qstring up to size - 1, although any bytes
    // beyond the strlen will be zero.
    //
    size_t getSize() const { return size; }
    
    // === Initialization and Resizing ==================================================

    qstring &createSize(size_t size);
    
    //
    // Gives the qstring a buffer of the default size and initializes
    // it to zero. Resets insertion point to zero. This is safe to call
    // on an existing qstring to reinitialize it.
    //
    qstring &create() { return createSize(basesize); }
    
    qstring &grow(size_t len);

    //
    // Sets the entire qstring buffer to zero, and resets the insertion index. 
    // Does not reallocate the buffer.
    //
    qstring &clear()
    {
        std::memset(buffer, 0, size);
        index = 0;
    
        return *this;
    }

    void freeBuffer();

    // === Indexing operations ==========================================================

    char charAt(size_t idx) const;

    //
    // Gets a pointer into the buffer at the given position, if that position would
    // be valid. Returns NULL otherwise. The same caveats apply as with qstring::getBuffer.
    //
    char *bufferAt(size_t idx) { return idx < size ? buffer + idx : nullptr; }
    
    unsigned char ucharAt(size_t idx) const { return static_cast<unsigned char>(charAt(idx)); }
    
    // === Concatenation and insertion/deletion =========================================
    
    //
    // Adds a character to the end of the qstring, reallocating via buffer doubling
    // if necessary.
    //
    qstring &push(char ch)
    {
        if(index >= size - 1) // leave room for \0
            grow(size);       // double buffer size

        buffer[index++] = ch;

        return *this;
    }

    //
    // Deletes a character from the end of the qstring.
    //
    qstring &pop()
    {
        if(index > 0)
            buffer[--index] = '\0';

        return *this;
    }

    qstring &concat(const char *str);
    qstring &concat(const qstring &src) { return concat(src.buffer); }

    qstring &insert(const char *insertstr, size_t pos);
    qstring &insert(const qstring &insertstr, size_t pos) { return insert(insertstr.buffer, pos); }
    
    // === Comparisons: C and C++ style =================================================

    int  strCmp(const char *str)                       const { return std::strcmp(buffer, str);            }
    int  strnCmp(const char *str, size_t maxcount)     const { return std::strncmp(buffer, str, maxcount); }
    int  strCaseCmp(const char *str)                   const { return strcasecmp(buffer, str);             }
    int  strnCaseCmp(const char *str, size_t maxcount) const { return strncasecmp(buffer, str, maxcount);  }
    bool compare(const char *str)                      const { return !std::strcmp(buffer, str);           }
    bool compare(const qstring &other)                 const { return !std::strcmp(buffer, other.buffer);  }
    
    bool operator < (const qstring &other) const { return std::strcmp(buffer, other.buffer) < 0; }
    bool operator > (const qstring &other) const { return std::strcmp(buffer, other.buffer) > 0; }

    // === Hashing ======================================================================

    static unsigned int HashCodeStatic(const char *str);
    static unsigned int HashCodeCaseStatic(const char *str);
    
    unsigned int hashCode()     const { return HashCodeStatic(buffer);     } // case-ignoring
    unsigned int hashCodeCase() const { return HashCodeCaseStatic(buffer); } // case-considering

    struct charhash
    {
        size_t operator () (const char *str) const noexcept { return HashCodeStatic(str); }
    };

    struct charcasehash
    {
        size_t operator () (const char *str) const noexcept { return HashCodeCaseStatic(str); }
    };
    
    // === Copying and Swapping =========================================================

    //
    // Copies a C string into the qstring. The qstring is cleared first,
    // and then set to the contents of *str.
    //
    qstring &copy(const char *str)
    {
        if(index > 0)
            clear();

        return concat(str);
    }

    qstring &copy(const qstring &src) { return copy(src.buffer); }
    qstring &copy(const char *str, size_t count);

    //
    // Copies the qstring into a C string buffer.
    //
    char *copyInto(char *dest, size_t pSize) const { return std::strncpy(dest, buffer, pSize); }

    // Copies one qstring into another.
    qstring &copyInto(qstring &dest) const { return dest.copy(*this); }
    
    void swapWith(qstring &str2);

    // === Case Conversions ====================================================

    qstring &toUpper(); // mutating in-place
    qstring &toLower(); // mutating in-place

    qstring asUpperCase() const { return qstring(*this).toUpper(); } // const, copying
    qstring asLowerCase() const { return qstring(*this).toLower(); } // const, copying

    // === Substring Extraction =========================================================

    qstring substr(size_t start, size_t len) const;
    
    // === Substring Replacements =======================================================

    size_t replace(const char *filter, char repl);
    size_t replaceNotOf(const char *filter, char repl);
    
    // === File Path Utilities ==========================================================

    qstring &normalizeSlashes();
    qstring &pathConcatenate(const char *addend);
    qstring &pathConcatenate(const qstring &other) { return pathConcatenate(other.buffer); }
    qstring &addDefaultExtension(const char *ext);
    qstring &removeFileSpec();
    qstring &stripExtension();
    qstring extractFileBase() const;
    
    // === strdup wrapper ===============================================================

    char *duplicate() const { return estrdup(buffer); }
    
    // === Conversions ==================================================================

    int    toInt()                          const { return std::atoi(buffer); }
    long   toLong(char **endptr, int radix) const { return std::strtol(buffer, endptr, radix); }
    double toDouble(char **endptr)          const { return std::strtod(buffer, endptr); }

    static qstring ToString(int i, int radix = 10);
    static qstring ToString(euint u);
    static qstring ToString(float f);
    static qstring ToString(double d);
    static qstring ToString(int64_t i64);
    static qstring ToString(uint64_t u64);
    static qstring ToString(bool b) { return qstring(b ? "1" : "0"); }

    // === View =========================================================================

    std::string_view getView() const { return { buffer, index }; }
    operator std::string_view () const noexcept { return { buffer, index }; }

    // === Searching/Substring Finding Routines =========================================

    const char *strchr(char c)  const { return std::strchr(buffer, c);  }
    const char *strrchr(char c) const { return std::strrchr(buffer, c); }
    
    size_t findFirstOf(char c) const;
    size_t findFirstNotOf(char c) const;
    size_t findLastOf(char c) const;

    size_t findFirstOf   (const char *c, size_t offs = 0) const { return getView().find_first_of(c, offs);     }
    size_t findFirstNotOf(const char *c, size_t offs = 0) const { return getView().find_first_not_of(c, offs); }

    size_t findLastOf   (const char *c, size_t offs = npos) const { return getView().find_last_of(c, offs);     }
    size_t findLastNotOf(const char *c, size_t offs = npos) const { return getView().find_last_not_of(c, offs); }
  
    const char *findSubStr(const char *substr) const { return std::strstr(buffer, substr); }
    const char *findSubStrNoCase(const char *substr) const;
    
    size_t find(const char *s, size_t pos = 0) const;
    size_t find(const qstring &s, size_t pos = 0) const { return find(s.buffer, pos); }

    bool startsWith(char c) const { return buffer[0] == c; }
    bool endsWith(char c)   const { return index > 0 ? buffer[index - 1] == c : false; }

    bool startsWith(const char *prefix)    const { return (std::strstr(buffer, prefix) == buffer); }
    bool startsWith(const qstring &prefix) const { return startsWith(prefix.buffer); };

    bool contains(const char *needle)    const { return std::strstr(buffer, needle) != nullptr; }
    bool contains(const qstring &needle) const { return contains(needle.buffer); }

    bool containsNoCase(const char *needle)    const;
    bool containsNoCase(const qstring &needle) const { return containsNoCase(needle.buffer); }

    // === Stripping and Truncation =====================================================

    qstring &lstrip(char c);
    qstring &rstrip(char c);
    qstring &truncate(size_t pos);
    qstring &erase(size_t pos, size_t n = npos);
    
    // === Special Formatting  ==========================================================

    qstring &makeQuoted();
    int      vprintf(const char *fmt, va_list args);
    int      printf(const char *fmt, ...);

    // === Operators ====================================================================

    bool operator     == (const char *other)    const { return !std::strcmp(buffer, other); }
    bool operator     == (const qstring &other) const { return !std::strcmp(buffer, other.buffer); }
    bool operator     != (const char *other)    const { return std::strcmp(buffer, other) != 0; }
    bool operator     != (const qstring &other) const { return std::strcmp(buffer, other.buffer) != 0; }
    qstring &operator  = (const qstring &other)       { return copy(other); }
    qstring &operator  = (const char    *other)       { return copy(other); }
    qstring operator  +  (const qstring &other) const { return qstring(*this).concat(other); }
    qstring operator  +  (const char    *other) const { return qstring(*this).concat(other); }
    qstring &operator += (const qstring &other)       { return concat(other); }
    qstring &operator += (const char    *other)       { return concat(other); }
    qstring &operator += (char ch)                    { return push(ch);      }
    qstring &operator << (const qstring &other)       { return concat(other); }
    qstring &operator << (const char    *other)       { return concat(other); }
    qstring &operator << (char   ch)                  { return push(ch);      }
    qstring &operator << (int    i);
    qstring &operator << (double d);
    qstring operator  /  (const qstring &other) const { return qstring(*this).pathConcatenate(other); }
    qstring operator  /  (const char *other)    const { return qstring(*this).pathConcatenate(other); }
    qstring &operator /= (const qstring &other)       { return pathConcatenate(other); }
    qstring &operator /= (const char *other)          { return pathConcatenate(other); }

    qstring &operator = (qstring &&other) noexcept
    {
        freeBuffer();
        moveFrom(std::move(other));
        return *this;
    }

    friend bool operator == (const char *a, const qstring &b)
    {
        return b == a;
    }

    friend bool operator != (const char *a, const qstring &b)
    {
        return b != a;
    }

    friend qstring operator + (const char *a, const qstring &b)
    {
        return qstring(a).concat(b);
    }

    friend qstring operator / (const char *a, const qstring &b)
    {
        return qstring(a).pathConcatenate(b);
    }
    
    char       &operator [] (size_t idx);
    const char &operator [] (size_t idx) const;
    
private:
    char    local[basesize];
    char   *buffer;
    size_t  index;
    size_t  size;
   
    bool isLocal() const { return (buffer == local); }
    void unLocalize(size_t pSize);

    void moveFrom(qstring &&other) noexcept;
};

// Specialization of std::hash for qstring
template<>
struct std::hash<qstring>
{
    size_t operator () (const qstring &qstr) const noexcept { return qstr.hashCode(); }
};

// Literal operator
inline qstring operator ""_q (const char *ch, size_t size) noexcept { return qstring(ch, size); }

// EOF

