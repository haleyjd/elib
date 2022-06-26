/*
  ELib
  
  Configuration file
  
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

#if defined(__cplusplus)

#include <variant>
#include "compare.h"

class qstring;

template<typename T>
struct ecfgrange_t
{
   T min;
   T max;
   T clamp(const T &value)
   {
      return eclamp<T>(value, min, max);
   }
};

class ECfgItem
{
public:
   enum itemtype_t
   {
      CFG_BOOL,
      CFG_INT,
      CFG_DOUBLE,
      CFG_STRING
   };

   enum chains_e { NUMCHAINS = 257 };

protected:
   static ECfgItem *items[NUMCHAINS];
   const char *m_name;
   ECfgItem   *m_next;
   itemtype_t  m_type;
   void       *m_var;
   void       *m_range;

   using default_t = std::variant<bool, int, double, const char *>;
   default_t   m_default;

   void init(const char *name, itemtype_t type, void *var);

public:
   ECfgItem(const char *name, int     *i, ecfgrange_t<int> *range = nullptr);
   ECfgItem(const char *name, bool    *b);
   ECfgItem(const char *name, double  *d, ecfgrange_t<double> *range = nullptr);
   ECfgItem(const char *name, char   **s, const char *pdefault = "");

   void readItem(const qstring &qstr);
   void writeItem(qstring &qstr) const;
   void resetToDefault();

   int    toInt()    const;
   bool   toBool()   const;
   double toDouble() const;
   void   toString(qstring &qstr) const;

   void setValue(int i);
   void setValue(bool b);
   void setValue(double d);
   void setValue(const char *str);

   itemtype_t  getType() const { return m_type; }
   const char *getName() const { return m_name; }

   static ECfgItem *FindByName(const char *name);
   static void GetValueAsString(const char *name, qstring &qstr);
   static void ItemIterator(void (*func)(ECfgItem *, void *), void *data);
};

extern "C" {
#endif

void E_CfgLoadFile(void);
void E_CfgWriteFile(void);

#if defined(__cplusplus)
}
#endif

// EOF

