;***
;handlers_arm64ec.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

#include "ksarm64.h"

; _CallSettingFrameArm64Ec
;
; this version restores and preserves the permanent register
; state of the funclet
;
; as the funclet returns to this function, it
; would be possible to insert calls to NLG
; for the debugger (are these needed for wince?)
; _ARM_WORKITEM_ NLG support?
;
;;extern "C" void* _CallSettingFrameArm64Ec(
;;    void*               handler,
;;    EHRegistrationNode* pRN,
;;    PULONG  *           pNonVolatiles,
;;    ULONG               NLG_CODE)
;
; funclet is expected to return continuation address in r0.
;

#ifndef DISABLE_NLG

        IMPORT A64NAME(_NLG_Notify)
        IMPORT A64NAME(__NLG_Return2)

#endif

        NESTED_ENTRY_COMDAT A64NAME(_CallSettingFrameArm64Ec)

        PROLOG_STACK_ALLOC 96
        PROLOG_SAVE_REG_PAIR_NO_FP fp, lr, #16
        PROLOG_SAVE_REG_PAIR x19, x20, #32
        PROLOG_SAVE_REG_PAIR x21, x22, #48
        PROLOG_SAVE_REG_PAIR x25, x26, #64
        PROLOG_SAVE_REG x27, #80

        ldr     x1, [x1]                                ; get establisher frame
        str     x1, [sp]                                ; save establisher frame at [sp] for unwind

#ifndef DISABLE_NLG

        mov     x19, x0                                 ; copy registers to nonvolatile space
        mov     x20, x1                                 ;
        mov     x21, x2                                 ;
        mov     x22, x3                                 ;

        mov     x2, x3                                  ; x0, x1 already have the values we want
        bl      A64NAME(_NLG_Notify)                    ; call debugger hook

        mov     x0, x19                                 ; recover original parameters
        mov     x1, x20                                 ;
        mov     x2, x21                                 ;
        mov     x3, x22                                 ;

#endif

        ldp     x19, x20, [x2, #0]                      ; load non-volatiles from x2
        ldp     x21, x22, [x2, #16]                     ; load non-volatiles from x2
                                                        ; skip disallowed registers x23 & x24
        ldp     x25, x26, [x2, #48]                     ; load non-volatiles from x2
        ldr     x27, [x2, #64]                          ; load non-volatiles from x2
                                                        ; skip disallowed register x28
        ldr     fp, [x2, #80]                           ; load non-volatiles from x2
        blr     x0                                      ; branch to handler

#ifndef DISABLE_NLG

        mov     x19, x0                                 ; save the return from the handler

        bl      A64NAME(__NLG_Return2)                  ; call debugger hook (no params)

        mov     x0, x19
        ldr     x1, [sp, #24]                           ; load saved lr value
        mov     x2, #2                                  ; CATCH_LEAVE parameter for _NLG_Notify
        bl      A64NAME(_NLG_Notify)                    ; call debugger hook

        mov     x0, x19                                 ; return value from the handler

#endif

        EPILOG_RESTORE_REG x27, #80
        EPILOG_RESTORE_REG_PAIR x25, x26, #64
        EPILOG_RESTORE_REG_PAIR x21, x22, #48
        EPILOG_RESTORE_REG_PAIR x19, x20, #32
        EPILOG_RESTORE_REG_PAIR fp, lr, #16
        EPILOG_STACK_FREE 96
        EPILOG_RETURN

        NESTED_END


        END