// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "sal.h"

#ifdef _DEBUG   // entire file


AFX_DATADEF CDumpContext afxDump;
AFX_DATADEF BOOL afxTraceEnabled = TRUE;
AFX_DATADEF UINT afxTraceFlags = 0;
static BOOL _afxDiagnosticInit = AfxDiagnosticInit();
static BOOL _afxMemoryLeakDump = TRUE;

/////////////////////////////////////////////////////////////////////////////
// _AFX_DEBUG_STATE implementation

static _CRT_DUMP_CLIENT pfnOldCrtDumpClient = NULL;


void __cdecl _AfxCrtDumpClient(void * pvData, size_t nBytes)
{
	if(_CrtReportBlockType(pvData) != _AFX_CLIENT_BLOCK)
		return;
	char sz[256];
	CObject* pObject = (CObject*)pvData;
	bool fCLRPresent=(::GetModuleHandleW(L"mscoree.dll")!=NULL);

	// use SEH (structured exception handling) to catch even GPFs
	//  that result from partially valid objects.
	__try
	{
		if(fCLRPresent) 
		{
			// If the CLR is in the process, we may well be dumping after the managed code execution environment has 
			// shut down. In this case, we can't be sure it is safe to call any method on the object at all, nor 
			// what exception we'd see if we did. So we resort to a terse form
			// 
			// To investigate leaks like this, you'll likely need to set a breakpoint here and look at the object itself
			// or force the leak code to run earlier in your process shutdown
			sprintf_s(sz, _countof(sz), "an object at $%p, %zu bytes long\n", pvData, nBytes);
			afxDump << sz;
		}
		// with vtable, verify that object and vtable are valid
		else if (!AfxIsValidAddress(*(void**)pObject, sizeof(void*), FALSE) ||
			!AfxIsValidAddress(pObject, pObject->GetRuntimeClass()->m_nObjectSize, FALSE))
		{
			// short form for invalid objects
			sprintf_s(sz, _countof(sz), "an invalid object at $%p, %zu bytes long\n", pvData, nBytes);
			afxDump << sz;
		}
		else if (afxDump.GetDepth() > 0)
		{
			// long form
			pObject->Dump(afxDump);
			afxDump << "\n";
		}
		else
		{
			// short form
			ATL_CRT_ERRORCHECK_SPRINTF(_snprintf_s(sz, _countof(sz), _countof(sz) - 1, "a %hs object at $%p, %zu bytes long\n",
				pObject->GetRuntimeClass()->m_lpszClassName, pvData, nBytes));
			afxDump << sz;
		}
	}
	__except(EXCEPTION_ACCESS_VIOLATION == GetExceptionCode())
	{
		// short form for trashed objects
		sprintf_s(sz, _countof(sz), "faulted while dumping object at $%p, %zu bytes long\n", pvData, nBytes);
		afxDump << sz;
	}

	if (pfnOldCrtDumpClient != NULL)
		(*pfnOldCrtDumpClient)(pvData, nBytes);
	return;
}

int __cdecl _AfxCrtReportHook(int nRptType, _In_ char *szMsg, int* pResult)
{
	// no hook on asserts or when m_pFile is NULL
	if (nRptType == _CRT_ASSERT || afxDump.m_pFile == NULL)
		return FALSE;

	ASSERT( pResult != NULL );
	if( pResult == NULL )
		AfxThrowInvalidArgException();

	ASSERT( szMsg != NULL );
	if( szMsg == NULL )
		AfxThrowInvalidArgException();

	// non-NULL m_pFile, so go through afxDump for the message
	*pResult = FALSE;
	afxDump << szMsg;
	//Allow other report hooks to be called.
	return FALSE;
}

static void _AFXDebugStateCommonInit()
{
	ASSERT(pfnOldCrtDumpClient == NULL);
	pfnOldCrtDumpClient = _CrtSetDumpClient(_AfxCrtDumpClient);

	ASSERT(_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, _AfxCrtReportHook) != -1);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
}
static void _AFXDebugStateCommonDestruct()
{
	int nOldState = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	_CrtSetDbgFlag(nOldState & ~_CRTDBG_LEAK_CHECK_DF);

	ASSERT(_CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, _AfxCrtReportHook) != -1);
	_CrtSetDumpClient(pfnOldCrtDumpClient);
}

_AFX_DEBUG_STATE::_AFX_DEBUG_STATE()
{
	_AFXDebugStateCommonInit();
}

_AFX_DEBUG_STATE::~_AFX_DEBUG_STATE()
{
	if (_afxMemoryLeakDump)
		_CrtDumpMemoryLeaks();
	_AFXDebugStateCommonDestruct();
}

_AFX_DEBUG_STATE2::_AFX_DEBUG_STATE2()
{
	_CrtMemCheckpoint(&mfcInitState);
	_AFXDebugStateCommonInit();
}

_AFX_DEBUG_STATE2::~_AFX_DEBUG_STATE2()
{
	if (_afxMemoryLeakDump)
	{
		_CrtMemState currentState, differnceBetweenStates;
		_CrtMemCheckpoint(&currentState);

		_CrtMemDifference(&differnceBetweenStates, &mfcInitState, &currentState);

		if (differnceBetweenStates.lCounts[_CLIENT_BLOCK] != 0 ||
			differnceBetweenStates.lCounts[_NORMAL_BLOCK] != 0 ||
			(_crtDbgFlag & _CRTDBG_CHECK_CRT_DF && differnceBetweenStates.lCounts[_CRT_BLOCK] != 0))
		{
			_RPT0(_CRT_WARN, "Detected memory leaks!\n");

			_CrtMemDumpAllObjectsSince(&mfcInitState);
		}
	}
	_AFXDebugStateCommonDestruct();
}

#pragma warning(disable: 4074)
#pragma init_seg(lib)

PROCESS_LOCAL(_AFX_DEBUG_STATE2, afxDebugState)

BOOL AFXAPI AfxDiagnosticInit(void)
{
	// just get the debug state to cause initialization
	_AFX_DEBUG_STATE2* pState = afxDebugState.GetData();
	ASSERT(pState != NULL);

	return TRUE;
}

BOOL AFXAPI AfxEnableMemoryLeakDump(BOOL bDump)
{
	// get the old flag state
	BOOL bRet = _afxMemoryLeakDump;

	// set the flag to the desired state
	_afxMemoryLeakDump = bDump;

	return bRet;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
