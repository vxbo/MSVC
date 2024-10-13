; This is a part of the Microsoft Foundation Classes C++ library.
; Copyright (C) Microsoft Corporation
; All rights reserved.
;
; This source code is only intended as a supplement to the
; Microsoft Foundation Classes Reference and related
; electronic documentation provided with the library.
; See these sources for detailed information regarding the
; Microsoft Foundation Classes product.

; ARMWORKITEM: Completely entirely and utterly untested
;			   we should probably investigate letting DispCallFunc do all
;			   the work
;

#include "kxarm.h"

    TEXTAREA

    LEAF_ENTRY _AfxDispatchCall


;_AfxDispatchCall(AFX_PMSG /*pfn*/, void* /*pArgs*/, UINT /*nSizeArgs*/)

	; at this point R0 contains value of pfn, R1 contains value of pArgs 
	; and R2 contains value of nSizeArgs.
	; we leave LR alone so once we jump into the actual target this
	; function does not appear in the stack walk at all
	
	sub r1, r1, #64

	; save the pfn
	mov r12, r0

	; set the first four float/double arguments
	vldm r1!,{d0-d3}

	; set the first four integer arguments
	add r1, #32
	mov sp, r1
	ldm sp!,{r0-r3}

	; jump to the function
	mov pc, r12

    LEAF_END _AfxDispatchCall 

    END
