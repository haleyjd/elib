/*
  ELib
  
  HAL Media Layer interface
  
  The MIT License (MIT)
  
  Copyright (C) 2016 James Haley
  
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

#include "../elib/elib.h"
#include "../elib/atexit.h"
#include "hal_ml.h"

//=============================================================================
//
// Default HAL medialayer functions
//
// These are dummies which basically do nothing, for situations where a media-
// layer object is not required.
//

static hal_bool hal_defaultexiting;

static hal_bool HAL_ML_DefaultInit(void)
{
    return HAL_TRUE;
}

static void HAL_ML_DefaultExit(void)
{
    if(hal_defaultexiting)
        return;

    hal_defaultexiting = HAL_TRUE;

    E_RunAtExitFuncs(0);

#if defined(ELIB_ALLOW_EXIT)
    exit(0);
#endif
}

static void HAL_ML_DefaultError(void)
{
    if(hal_defaultexiting)
        return;

    hal_defaultexiting = HAL_TRUE;

    E_RunAtExitFuncs(1);

#if defined(ELIB_ALLOW_EXIT)
    exit(-1);
#endif
}

static int HAL_ML_DefaultMsgBox(const char *title, const char *msg, hal_bool isError)
{
    return -1;
}

static hal_bool HAL_ML_DefaultIsExiting(void)
{
    return hal_defaultexiting;
}

static const char *HAL_ML_DefaultGetBaseDirectory(void)
{
    return "./";
}

static const char *HAL_ML_DefaultGetWriteDirectory(const char *app)
{
    return "./";
}

//=============================================================================
//
// Medialayer Object
//

hal_medialayer_t hal_medialayer =
{
    HAL_ML_DefaultInit,
    HAL_ML_DefaultExit,
    HAL_ML_DefaultError,
    HAL_ML_DefaultMsgBox,
    HAL_ML_DefaultIsExiting,
    HAL_ML_DefaultGetBaseDirectory,
    HAL_ML_DefaultGetWriteDirectory
};

// EOF
