/***
*gshandlereh_amd64.cpp - Defines __GSHandlerCheck_EH for X64 frames in ARM64EC
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines __GSHandlerCheck_EH, the X64 exception handler for functions
*       with /GS-protected buffers as well as C++ EH constructs.
*******************************************************************************/

#define __GSHandlerCheckCommon __GSHandlerCheckCommon_AMD64
#define __GSHandlerCheck_EH __GSHandlerCheck_EH_AMD64

#include "..\amd64\gshandlereh.cpp"

#undef __GSHandlerCheckCommon
#undef __GSHandlerCheck_EH
