; This is a part of the Active Template Library.
; Copyright (C) Microsoft Corporation
; All rights reserved.
;
; This source code is only intended as a supplement to the
; Active Template Library Reference and related
; electronic documentation provided with the library.
; See these sources for detailed information regarding the	
; Active Template Library product.

#include "ksarm64.h"

        LEAF_ENTRY A64NAME(CComStdCallThunkHelper)

        ldr     x16, [x0, #16]      ; get pThunk->pfn
        ldr     x0, [x0, #8]        ; replace the 1st argument (pThunk) with the pThunk->pThis
        br      x16                 ; skip over to the target

        LEAF_END A64NAME(CComStdCallThunkHelper)

        END
