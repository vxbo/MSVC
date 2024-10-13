/***
*handlers_amd64.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Wrapper functions calling the "catch" handlers and notifying
*       the debugger of the Non-Linear-Goto transitions involved.
*
*N.B.
*       On x86_64, non-volatile registers are not restored before
*       calling the handler, so these can be implemented in C code.
*       For architectures where non-volatile registers are restored
*       before calling the handler (e.g. ARM32 and ARM64) these
*       have to be implemented in ASM.
*
****/

#include <eh.h>         // User-visible routines for eh
#include <ehassert.h>   // This project's versions of standard assert macros
#include <ehdata.h>     // Declarations of all types used for EH

#define NLG_CATCH_LEAVE (2)

extern "C" {

typedef PVOID (FN_HANDLER)(PVOID, EHRegistrationNode);

VOID
_NLG_Notify (
    PVOID,
    EHRegistrationNode,
    ULONG
    );

VOID
__NLG_Return2 (
    VOID
    );

DECLSPEC_GUARDNOCF
PVOID
_CallSettingFrame (
    PVOID handler,
    EHRegistrationNode* en,
    ULONG nlg_code
    )
{
    EHRegistrationNode ef;
    FN_HANDLER *fnh;
    PVOID continuation;

    ef = *en;
    fnh = (FN_HANDLER*)handler;

    // Notify the debugger that the handler is
    // about to be called
    _NLG_Notify(handler, ef, nlg_code);

    // Call the handler ("Catch block")
    continuation = (*fnh)(handler, ef);

    // Notify the debugger that the handler
    // has returned.
    __NLG_Return2();

    // Notify the debugger that the code following
    // the handler is about to be called.
    _NLG_Notify(continuation, ef, NLG_CATCH_LEAVE);

    return continuation;
}

DECLSPEC_GUARDNOCF
PVOID
_CallSettingFrame_LookupContinuationIndex (
    PVOID handler,
    EHRegistrationNode* en,
    ULONG nlg_code
    )
{
    EHRegistrationNode ef;
    FN_HANDLER *fnh;
    PVOID continuation;

    ef = *en;
    fnh = (FN_HANDLER*)handler;

    // Notify the debugger that the handler is
    // about to be called
    _NLG_Notify(handler, ef, nlg_code);

    // Call the handler ("Catch block")
    continuation = (*fnh)(handler, ef);

    // Notify the debugger that the handler
    // has returned.
    __NLG_Return2();

    return continuation;
}

PVOID
_CallSettingFrame_NotifyContinuationAddr (
    PVOID continuation,
    EHRegistrationNode* en
    )
{
    EHRegistrationNode ef;

    ef = *en;

    // Notify the debugger that the code following
    // the handler is about to be called.
    _NLG_Notify(continuation, ef, NLG_CATCH_LEAVE);

    return continuation;
}

DECLSPEC_GUARDNOCF
PVOID
_CallSettingFrameEncoded (
    PVOID handler,
    EHRegistrationNode ef,
    PVOID object,
    ULONG nlg_code
    )
{
    FN_HANDLER *fnh;
    PVOID continuation;

    fnh = (FN_HANDLER*)handler;

    // Notify the debugger that the handler is
    // about to be called
    _NLG_Notify(handler, ef, nlg_code);

    // Call the handler ("Catch block")
    continuation = (*fnh)(object, ef);

    // Notify the debugger that the handler
    // has returned.
    __NLG_Return2();

    // Notify the debugger that the code following
    // the handler is about to be called.
    _NLG_Notify(continuation, ef, NLG_CATCH_LEAVE);

    return continuation;
}

}
