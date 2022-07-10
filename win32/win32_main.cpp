/*
  ELib

  Win32 Main Routine

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

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>
#include <exception>
#include "../elib/elib.h"
#include "../elib/m_argv.h"
#include "econfig.h"

ELIB_MAIN_FN_DECL();

#if defined(ELIB_WIN32_CONSOLE_PROGRAM)
// === Win32 console app support ==============================================

//
// ELib main program for Win32 console apps
//
int main(int argc, const char *const *argv)
{
    try 
    {
        // initialize global command line state
        EArgManager::GetGlobalArgs().setArgs(argc, argv);

        // call application's main function
        ELIB_MAIN_FN_CALL();
    }
    catch(const std::exception &ex)
    {
        std::fprintf(stderr, "\nException: %s\n", ex.what());
    }
    catch(...)
    {
        std::fprintf(stderr, "\nException: Unknown exception caught in main\n");
    }

    return 0;
}

#else
// === Win32 windowed app support ==============================================

//
// Remove slash characters from escaped quotation marks
//
static void Win32_unEscapeQuotes(char *arg)
{
    char *last = nullptr;
    
    while(*arg)
    {
        if(*arg == '"' && (last != nullptr && *last == '\\'))
        {
            char *c_curr = arg;
            char *c_last = last;
            
            while(*c_curr)
            {
                *c_last = *c_curr;
                c_last = c_curr;
                ++c_curr;
            }
            *c_last = '\0';
        }
        last = arg;
        ++arg;
    }
}

//
// Parse the command line buffer into arguments
//
static int Win32_parseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = nullptr;
    int argc, last_argc;

    argc = last_argc = 0;
    for(bufp = cmdline; *bufp; )
    {
        // skip leading whitespace
        while(ectype::isSpace(*bufp))
            ++bufp;

        // skip over argument
        if(*bufp == '"')
        {
            ++bufp;
            if(*bufp)
            {
                if(argv)
                    argv[argc] = bufp;
                ++argc;
            }

            // skip over word
            lastp = bufp;
            while(*bufp && (*bufp != '"' || *lastp == '\\'))
            {
                lastp = bufp;
                ++bufp;
            }
        }
        else
        {
            if(*bufp)
            {
                if(argv)
                    argv[argc] = bufp;
                ++argc;
            }

            // skip over word
            while(*bufp && !ectype::isSpace(*bufp))
                ++bufp;
        }

        if(*bufp)
        {
            if(argv)
                *bufp = '\0';
            ++bufp;
        }

        // strip out \ from \" sequences
        if(argv && last_argc != argc)
            Win32_unEscapeQuotes(argv[last_argc]);
        last_argc = argc;
    }

    if(argv)
        argv[argc] = nullptr;

    return argc;
}

//
// Show an error dialog
//
static void ShowError(const char *title, const char *message)
{
    MessageBoxA(nullptr, message, title, MB_ICONEXCLAMATION | MB_OK);
}

//
// Show an out-of-memory error dialog
//
static void OutOfMemory()
{
    ShowError("Fatal Error", "Out of memory");
}

//
// strdup polyfill
//
static char *mystrdup(const char *instr)
{
    const size_t len = std::strlen(instr) + 1;
    char *const buf = new char [len];
    std::strncpy(buf, instr, len);
    return buf;
}

//
// WinMain function
//
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    try 
    {
        // grab the command line
        std::unique_ptr<char []> cmdline { mystrdup(GetCommandLineA()) };

        // parse into argv, argc
        int    argc = Win32_parseCommandLine(cmdline.get(), nullptr);
        char **argv = static_cast<char **>(std::calloc(argc + 1, sizeof(char *)));
        if(!argv)
        {
            OutOfMemory();
            return 0;
        }
        Win32_parseCommandLine(cmdline.get(), argv);

        // initialize global command line state
        EArgManager::GetGlobalArgs().setArgs(argc, argv);

        // call application's main function
        ELIB_MAIN_FN_CALL();

        std::free(argv);
    }
    catch(const std::exception &ex)
    {
        ShowError("Exception", ex.what());
    }
    catch(...)
    {
        ShowError("Exception", "Unknown exception caught in main");
    }

    return 0;
}

#endif

#endif

// EOF
