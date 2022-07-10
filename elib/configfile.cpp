/*
  ELib
  
  Configuration file
  
  The MIT License (MIT)
  
  Copyright (c) 2021 James Haley
  
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

#include <map>

#include "elib.h"
#include "econfig.h"
#include "../hal/hal_platform.h"
#include "../hal/hal_ml.h"
#include "atexit.h"
#include "configfile.h"
#include "misc.h"
#include "parser.h"
#include "qstring.h"

//=============================================================================
//
// Variable Binding
//

ECfgItem *ECfgItem::items[NUMCHAINS];

//
// Set value to an integer config binding
//
static void ECfg_SetInt(void *var, int i, void *range)
{
    auto i_var = static_cast<int *>(var);
    *i_var = i;
    if(range)
        *i_var = static_cast<ecfgrange_t<int> *>(range)->clamp(*i_var);
}

//
// Set value to a boolean config binding
//
static void ECfg_SetBool(void *var, bool b)
{
    *static_cast<bool *>(var) = b;
}

static void ECfg_SetDouble(void *var, double d, void *range)
{
    auto d_var = static_cast<double *>(var);
    *d_var = d;
    if(range)
        *d_var = static_cast<ecfgrange_t<double> *>(range)->clamp(*d_var);
}

//
// Set value to a string config binding
//
static void ECfg_SetString(void *var, const char *newvalue)
{
    char **dst = static_cast<char **>(var);
    if(*dst)
        efree(*dst);
    *dst = newvalue ? estrdup(newvalue) : nullptr;
}


//
// Initialize a configuration binding completely.
//
void ECfgItem::init(const char *name, itemtype_t type, void *var)
{
   m_name  = name;
   m_var   = var;
   m_type  = type;
   m_range = nullptr;

   const euint chain = qstring::HashCodeStatic(name) % NUMCHAINS;
   m_next = items[chain];
   items[chain] = this;
}

//
// Construct an integer config binding.
//
ECfgItem::ECfgItem(const char *name, int *i, ecfgrange_t<int> *range)
{
   init(name, CFG_INT, i);
   m_range = range;
   m_default = *i;
}
  
//
// Construct a boolean config binding.
//
ECfgItem::ECfgItem(const char *name, bool *b)
{
   init(name, CFG_BOOL, b);
   m_default = *b;
}
   
//
// Construct a double config binding.
//
ECfgItem::ECfgItem(const char *name, double *d, ecfgrange_t<double> *range)
{
   init(name, CFG_DOUBLE, d);
   m_range = range;
   m_default = *d;
}
 
//
// Construct a string config binding.
//
ECfgItem::ECfgItem(const char *name, char **s, const char *pdefault /*= ""*/)
{
   init(name, CFG_STRING, s);
   m_default = pdefault;
   if(!estrempty(pdefault))
       ECfg_SetString(m_var, pdefault);
}

//
// Read a config item in from input taken from file.
//
void ECfgItem::readItem(const qstring &qstr)
{
    switch(m_type)
    {
    case CFG_INT:
        ECfg_SetInt(m_var, qstr.toInt(), m_range);
        break;
    case CFG_BOOL:
        ECfg_SetBool(m_var, !!qstr.toInt());
        break;
    case CFG_DOUBLE:
        ECfg_SetDouble(m_var, qstr.toDouble(nullptr), m_range);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, qstr.c_str());
        break;
    default:
        break;
    }
}

//
// Write out the current value of a config item.
//
void ECfgItem::writeItem(qstring &qstr) const
{
   switch(m_type)
   {
   case CFG_INT:
      qstr << *static_cast<int *>(m_var);
      break;
   case CFG_BOOL:
      qstr << *static_cast<bool *>(m_var);
      break;
   case CFG_DOUBLE:
      qstr << *static_cast<double *>(m_var);
      break;
   case CFG_STRING:
      if(const char *const src = *static_cast<char **>(m_var); src != nullptr)
         qstr << src;
      break;
   default:
      break;
   }
}

//
// Reset an item to its default value
//
void ECfgItem::resetToDefault()
{
    switch(m_type)
    {
    case CFG_INT:
        *static_cast<int *>(m_var) = std::get<CFG_INT>(m_default);
        break;
    case CFG_BOOL:
        *static_cast<bool *>(m_var) = std::get<CFG_BOOL>(m_default);
        break;
    case CFG_DOUBLE:
        *static_cast<double *>(m_var) = std::get<CFG_DOUBLE>(m_default);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, std::get<CFG_STRING>(m_default));
        break;
    default:
        break;
    }
}

//
// Get value of config item as integer
//
int ECfgItem::toInt() const
{
    int val = 0;
    switch(m_type)
    {
    case CFG_INT:
        val = *static_cast<int *>(m_var);
        break;
    case CFG_BOOL:
        val = int(*static_cast<bool *>(m_var));
        break;
    case CFG_DOUBLE:
        val = int(*static_cast<double *>(m_var));
        break;
    case CFG_STRING:
        if(const char *const chvar = *static_cast<char **>(m_var); chvar != nullptr)
            val = std::atoi(chvar);
        break;
    default:
        break;
    }
    return val;
}

//
// Get value of config item as boolean
//
bool ECfgItem::toBool() const
{
    bool b = false;
    switch(m_type)
    {
    case CFG_INT:
        b = !!(*static_cast<int *>(m_var));
        break;
    case CFG_BOOL:
        b = *static_cast<bool *>(m_var);
        break;
    case CFG_DOUBLE:
        b = (*static_cast<double *>(m_var) != 0.0);
        break;
    case CFG_STRING:
        if(const char *const chvar = *static_cast<char **>(m_var); chvar != nullptr)
            b = !!std::atoi(chvar);
        break;
    default:
        break;
    }
    return b;
}

//
// Get value of config item as double
//
double ECfgItem::toDouble() const
{
    double d = 0.0;
    switch(m_type)
    {
    case CFG_INT:
        d = double(*static_cast<int *>(m_var));
        break;
    case CFG_BOOL:
        d = double(*static_cast<bool *>(m_var));
        break;
    case CFG_DOUBLE:
        d = *static_cast<double *>(m_var);
        break;
    case CFG_STRING:
        if(const char *const chvar = *static_cast<char **>(m_var); chvar != nullptr)
            d = std::strtod(chvar, nullptr);
        break;
    default:
        break;
    }
    return d;
}

//
// Get value of config item as string
//
void ECfgItem::toString(qstring &qstr) const
{
    writeItem(qstr);
}

//
// Set a config item to an integer value
//
void ECfgItem::setValue(int i)
{
    switch(m_type)
    {
    case CFG_INT:
        ECfg_SetInt(m_var, i, m_range);
        break;
    case CFG_BOOL:
        ECfg_SetBool(m_var, !!i);
        break;
    case CFG_DOUBLE:
        ECfg_SetDouble(m_var, double(i), m_range);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, qstring::ToString(i).c_str());
        break;
    default:
        break;
    }
}

//
// Set a config item to a boolean value
//
void ECfgItem::setValue(bool b)
{
    switch(m_type)
    {
    case CFG_INT:
        ECfg_SetInt(m_var, int(b), m_range);
        break;
    case CFG_BOOL:
        ECfg_SetBool(m_var, b);
        break;
    case CFG_DOUBLE:
        ECfg_SetDouble(m_var, double(b), m_range);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, qstring::ToString(b).c_str());
        break;
    default:
        break;
    }
}

//
// Set a config item to a double value
//
void ECfgItem::setValue(double d)
{
    switch(m_type)
    {
    case CFG_INT:
        ECfg_SetInt(m_var, int(d), m_range);
        break;
    case CFG_BOOL:
        ECfg_SetBool(m_var, d != 0.0);
        break;
    case CFG_DOUBLE:
        ECfg_SetDouble(m_var, d, m_range);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, qstring::ToString(d).c_str());
        break;
    default:
        break;
    }
}

//
// Set a config item to a string value
//
void ECfgItem::setValue(const char *str)
{
    const char *const nval = (str != nullptr ? str : "");
    switch(m_type)
    {
    case CFG_INT:
        ECfg_SetInt(m_var, std::atoi(nval), m_range);
        break;
    case CFG_BOOL:
        ECfg_SetBool(m_var, !!std::atoi(nval));
        break;
    case CFG_DOUBLE:
        ECfg_SetDouble(m_var, std::strtod(nval, nullptr), m_range);
        break;
    case CFG_STRING:
        ECfg_SetString(m_var, nval);
        break;
    default:
        break;
    }
}

//
// Find a configuration binding item by name.
//
ECfgItem *ECfgItem::FindByName(const char *name)
{
   const euint chain = qstring::HashCodeStatic(name) % NUMCHAINS;
   ECfgItem *item = items[chain];

   while(item && strcasecmp(name, item->m_name))
      item = item->m_next;

   return item;
}

//
// Get a variable's string representation.
//
void ECfgItem::GetValueAsString(const char *name, qstring &qstr)
{
   if(const ECfgItem *const item = ECfgItem::FindByName(name); item != nullptr)
      item->writeItem(qstr);
}

//
// Iterate over all configuration bindings and call the provided function
// for each.
//
void ECfgItem::ItemIterator(void (*func)(ECfgItem *, void *), void *data)
{
   for(int i = 0; i < NUMCHAINS; i++)
   {
      ECfgItem *item = items[i];
      while(item)
      {
         func(item, data);
         item = item->m_next;
      }
   }
}

//=============================================================================
//
// Configuration Loading
//

//
// Configuration file parser class
//
class ECfgFileParser : public EParser
{
protected:
   // state table declaration
   static bool (ECfgFileParser::*States[])(ETokenizer &);

   // parser state enumeration
   enum pstate_e
   {
      STATE_EXPECTKEYWORD,
      STATE_EXPECTVALUE
   };

   // state handlers
   bool doStateExpectKeyword(ETokenizer &);
   bool doStateExpectValue(ETokenizer &);

   // parser state data
   int      m_state = STATE_EXPECTKEYWORD;
   qstring  m_key;

   // overrides
   virtual bool doToken(ETokenizer &token) override;
   virtual void startFile() override;
   virtual void initTokenizer(ETokenizer &token) override;

public:
    using EParser::EParser;
};

// state table
bool (ECfgFileParser::* ECfgFileParser::States[])(ETokenizer &) =
{
   &ECfgFileParser::doStateExpectKeyword,
   &ECfgFileParser::doStateExpectValue
};

//
// Dispatch token to appropriate state handler
//
bool ECfgFileParser::doToken(ETokenizer &token)
{
   return (this->*States[m_state])(token);
}

//
// Reinitialize parser at beginning of file parsing
//
void ECfgFileParser::startFile()
{
   m_state = STATE_EXPECTKEYWORD;
   m_key   = "";
}

//
// Setup tokenizer state before parsing begins
//
void ECfgFileParser::initTokenizer(ETokenizer &token)
{
   token.setTokenFlags(ETokenizer::TF_DEFAULT);
}

//
// Keyword state handler
//
bool ECfgFileParser::doStateExpectKeyword(ETokenizer &token)
{
   const int tokentype = token.getTokenType();

   switch(tokentype)
   {
   case ETokenizer::TOKEN_KEYWORD:
   case ETokenizer::TOKEN_STRING:
      // record as the current key and expect value to follow
      m_key   = token.getToken();
      m_state = STATE_EXPECTVALUE;
      break;
   default:
      // if we see anything else, keep scanning
      break;
   }

   return true;
}

//
// Value state handler
//
bool ECfgFileParser::doStateExpectValue(ETokenizer &token)
{
   qstring &value = token.getToken();

   if(ECfgItem *const item = ECfgItem::FindByName(m_key.c_str()); item != nullptr)
      item->readItem(value);
   m_state = STATE_EXPECTKEYWORD;
   m_key   = "";

   return true;
}

//=============================================================================
//
// External Interface
//

void E_CfgLoadFile()
{
    const char *const pGameName = ELIB_APP_NAME;
    const char *const pCfgName  = ELIB_CFG_NAME;

    const qstring fn = qstring(hal_medialayer.getWriteDirectory(pGameName)) / pCfgName;
    ECfgFileParser parser(fn.c_str());
    parser.parseFile();

    // schedule to write config file at exit, except in case of errors
    E_AtExit(E_CfgWriteFile, false);
}

using cfgitemmap_t = std::map<qstring, ECfgItem *>;

struct cfgwritedata_t
{
    cfgitemmap_t *itemMap;
};

static void AddItemToMap(ECfgItem *item, void *data)
{
   const auto cwd = static_cast<cfgwritedata_t *>(data);
   cwd->itemMap->emplace(qstring(item->getName()), item);
}

static bool WriteCfgItem(const ECfgItem *item, FILE *f)
{
   qstring value;
   item->writeItem(value);
   if(std::fprintf(f, "%s \"%s\"\n", item->getName(), value.c_str()) < 0)
      return false;
   return true;
}

void E_CfgWriteFile()
{
    const char *const  pGameName = ELIB_APP_NAME;
    const char *const  pCfgName  = ELIB_CFG_NAME;
    const char *const  pWriteDir = hal_medialayer.getWriteDirectory(pGameName);

    const qstring tmpName = qstring(pWriteDir) / "temp.cfg";
    const qstring dstName = qstring(pWriteDir) / pCfgName;

    cfgwritedata_t cwd;
    cfgitemmap_t   items;
    cwd.itemMap = &items;

    FILE *const f = hal_platform.fileOpen(tmpName.c_str(), "w");
    if(!f)
    {
        hal_platform.debugMsg("Warning: could not open temp.cfg\n");
        return;
    }
    
    if(std::fprintf(f, "// %s configuration file\n", pGameName) < 0)
    {
        hal_platform.debugMsg("Warning: failed write to temp.cfg\n");
        return;
    }
    
    // add config items to the map
    ECfgItem::ItemIterator(AddItemToMap, &cwd);
    
    bool error = false;
    for(const auto &item : items)
    {
        if(!WriteCfgItem(item.second, f))
        {
            error = true;
            break;
        }
    }
    
    if(error)
    {
        std::fclose(f);
        hal_platform.debugMsg("Warning: failed one or more cfg writes\n");
        return;
    }
    
    if(std::fclose(f) < 0)
    {
        hal_platform.debugMsg("Warning: failed to close temp.cfg\n");
        return;
    }
    
    std::remove(dstName.c_str());
    if(std::rename(tmpName.c_str(), dstName.c_str()))
        hal_platform.debugMsg("Warning: failed to write %s\n", pCfgName);
}

// EOF

