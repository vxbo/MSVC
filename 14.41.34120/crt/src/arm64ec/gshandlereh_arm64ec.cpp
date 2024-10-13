/***
*gshandlereh_arm64ec.cpp - Defines __GSHandlerCheck_EH for ARM64EC
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines __GSHandlerCheck_EH, the ARM64 exception handler for functions
*       with /GS-protected buffers as well as C++ EH constructs.
*******************************************************************************/

#define __GSHandlerCheckCommon __GSHandlerCheckCommon_ARM64EC
#define __GSHandlerCheck_EH __GSHandlerCheck_EH_ARM64EC

#include "..\arm64\gshandlereh.cpp"

#undef __GSHandlerCheckCommon
#undef __GSHandlerCheck_EH

extern "C" EXCEPTION_DISPOSITION
__GSHandlerCheck_EH_AMD64 (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    );

extern "C" EXCEPTION_DISPOSITION
__GSHandlerCheck_EH (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
    )
{
    if (RtlIsEcCode(DispatcherContext->ControlPc)) {
        return __GSHandlerCheck_EH_ARM64EC (
            ExceptionRecord,
            EstablisherFrame,
            ContextRecord,
            DispatcherContext
        );
    }

    return __GSHandlerCheck_EH_AMD64 (
        ExceptionRecord,
        EstablisherFrame,
        ContextRecord,
        DispatcherContext
    );

}
