/*
  ELib

  Command line argument management
  
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

#include "elib.h"
#include "m_argv.h"

int EArgManager::indexOf(const char *arg) const
{
    for(int i = 1; i < m_argc; i++)
    {
        if(!std::strcmp(m_argv[i], arg))
            return i;
    }

    return 0;
}

bool EArgManager::findArgument(const char *arg) const
{
    return indexOf(arg) != 0;
}

int EArgManager::getArgParameters(const char *arg, int count) const
{
    for(int i = 1; i < m_argc; i++)
    {
        if(!std::strcmp(m_argv[i], arg))
        {
            if(i < m_argc - count)
                return i + 1;
            else
                return 0; // not enough arguments available
        }
    }

    return 0; // not found
}

int EArgManager::findMultiParam(const char *const *params, int numargs) const
{
   int i = 0;
   const char *param;

   while((param = params[i++]))
   {
      const int p = indexOf(param);

      // it is only found if the expected number of arguments are available
      if(p && p < m_argc - numargs)
         return p;
   }

   return 0; // none were found
}

int M_GetArgc()
{
    return EArgManager::GetGlobalArgs().getArgc();
}

const char *const *M_GetArgv()
{
    return EArgManager::GetGlobalArgs().getArgv();
}

int M_FindArgumentIndex(const char *arg)
{
    return EArgManager::GetGlobalArgs().indexOf(arg);
}

hal_bool M_FindArgument(const char *arg)
{
    return EArgManager::GetGlobalArgs().findArgument(arg) ? HAL_TRUE : HAL_FALSE;
}

int M_GetArgParameters(const char *arg, int count)
{
    return EArgManager::GetGlobalArgs().getArgParameters(arg, count);
}

int M_FindMultiParam(const char *const *params, int numargs)
{
    return EArgManager::GetGlobalArgs().findMultiParam(params, numargs);
}

// EOF
