/*
  ELib

  Command line argument management

  The MIT License (MIT)

  Copyright (c) 2015 James Haley

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

#include "../hal/hal_types.h"

#if defined(__cplusplus)

class EArgManager
{
public:
    EArgManager() {}

    EArgManager(int argc, const char *const *argv)
        : m_argc(argc), m_argv(argv)
    {
    }

    void setArgs(int argc, const char *const *argv)
    {
        m_argc = argc;
        m_argv = argv;
    }

    int getArgc() const { return m_argc; }

    const char *const *getArgv() const { return m_argv; }

    //
    // Returns the index of the specified argument if it was passed to the
    // program, and 0 otherwise.
    //
    int indexOf(const char *arg) const;

    //
    // Returns the index of the specified argument if it was passed to the
    // program, starting from the given index. Returns 0 if not found.
    //
    int indexOf(const char *arg, int from) const;
    
    //
    // Returns true if the specified argument was passed to the program,
    // and false otherwise.
    //
    bool findArgument(const char *arg) const;

    //
    // Returns the index of the first parameter to the specified argument, if
    // the argument was passed and can have at least "count" required arguments
    // after it on the command line. Returns 0 if either the parameter was not
    // passed at all or it is too near the end of the command line.
    //
    int getArgParameters(const char *arg, int count) const;

    //
    // Allows several aliases for a command line parameter to be checked in turn.
    // The list of parameters to check must be terminated with a null pointer. If
    // the indicated number of arguments is not available after a parameter found,
    // it will be ignored.
    //
    int findMultiParam(const char *const *params, int numargs) const;

    //
    // Returns an object which holds the global command-line arguments passed to the
    // program at startup.
    //
    static EArgManager &GetGlobalArgs()
    {
        static EArgManager globalArgs;
        return globalArgs;
    }

protected:
    int m_argc = 0;
    const char *const *m_argv = nullptr;
};

#endif

#if defined(__cplusplus)
extern "C" {
#endif

// Versions callable from C; these target the global argument manager object.

int M_GetArgc(void);
const char *const *M_GetArgv(void);

hal_bool M_FindArgument(const char *arg);
int      M_FindArgumentIndex(const char *arg);
int      M_GetArgParameters(const char *arg, int count);
int      M_FindMultiParam(const char *const *params, int numargs);

#if defined(__cplusplus)
}
#endif

// EOF
