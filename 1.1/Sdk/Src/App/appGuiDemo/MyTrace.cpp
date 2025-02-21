//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "MyTrace.h"

//======================================================================================
#define INITGUID	// Causes definition of SystemTraceControlGuid in evntrace.h.
#include <windows.h>
#include <strsafe.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <TlHelp32.h>

//======================================================================================
#define CLIENT_CONTEXT_TIMESTAMP_QPC		1
#define CLIENT_CONTEXT_TIMESTAMP_SYSTIME	2
#define CLIENT_CONTEXT_TIMESTAMP_CPU_CYCLE	3
#define EVENT_RECORD_OPCODE_CSWITCH			36
#define INVALID_TRACE_HANDLE_VALUE			((TRACEHANDLE)INVALID_HANDLE_VALUE)

//======================================================================================
using namespace nemesis;

//======================================================================================
static bool FindProcessEntry( HANDLE hSnapshot, uint32_t pid, PROCESSENTRY32& entry )
{
	PROCESSENTRY32 item = { sizeof(PROCESSENTRY32) };
	if (Process32First( hSnapshot, &item ))
	{
		do
		{
			if (item.th32ProcessID == pid)
			{
				entry = item;
				return true;
			}
		}
		while (Process32Next( hSnapshot, &item ));
	}
	return false;
}

//======================================================================================
struct EVENT_TRACE_PROPERTIES_EX
{
	EVENT_TRACE_PROPERTIES Properties;
	WCHAR				   LoggerName[ MAX_PATH ];
};

static void InitializeTracePropertiesEx( EVENT_TRACE_PROPERTIES_EX& trace_props )
{
	NeZero(trace_props);
	trace_props.Properties.Wnode.BufferSize = sizeof(EVENT_TRACE_PROPERTIES_EX);
	trace_props.Properties.Wnode.Guid = SystemTraceControlGuid;
	trace_props.Properties.Wnode.ClientContext = CLIENT_CONTEXT_TIMESTAMP_QPC;
	trace_props.Properties.Wnode.Flags = WNODE_FLAG_TRACED_GUID;
	trace_props.Properties.EnableFlags = EVENT_TRACE_FLAG_CSWITCH;
	trace_props.Properties.LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
	trace_props.Properties.LoggerNameOffset = offsetof( EVENT_TRACE_PROPERTIES_EX, LoggerName );
	wcscpy_s( trace_props.LoggerName, KERNEL_LOGGER_NAMEW );
}

//======================================================================================
struct MyTrace_s
{
	TRACEHANDLE	Consumer;
	HANDLE		Thread;
};

static MyTrace_s MyTraceInstance = { INVALID_TRACE_HANDLE_VALUE, INVALID_HANDLE_VALUE };

//======================================================================================
namespace
{
	static void WINAPI EnumEvent( EVENT_RECORD* ev )
	{
		typedef int8_t sint8_t;

		struct CSwitch
		{
			uint32_t	NewThreadId;
			uint32_t	OldThreadId;
			sint8_t		NewThreadPriority;
			sint8_t		OldThreadPriority;
			uint8_t		PreviousCState;
			sint8_t		SpareByte;
			sint8_t		OldThreadWaitReason;
			sint8_t		OldThreadWaitMode;
			sint8_t		OldThreadState;
			sint8_t		OldThreadWaitIdealProcessor;
			uint32_t	NewThreadWaitTime;
			uint32_t	Reserved;
		};

		// skip all but context switches
		if ((!ev->UserData) || (ev->UserDataLength != sizeof(CSwitch)))
			return;
		if (ev->EventHeader.EventDescriptor.Opcode != EVENT_RECORD_OPCODE_CSWITCH)
			return;

		// get the context switch
		CSwitch* cs = (CSwitch*)(ev->UserData);
		uint32_t core = ev->BufferContext.ProcessorNumber;
		uint32_t tid0 = cs->OldThreadId;
		uint32_t tid1 = cs->NewThreadId;

		// determine if out process is involved 
		DWORD pid = GetCurrentProcessId();
		HANDLE th0 = OpenThread( THREAD_QUERY_INFORMATION, FALSE, tid0 );
		HANDLE th1 = OpenThread( THREAD_QUERY_INFORMATION, FALSE, tid1 );
		DWORD pid0 = GetProcessIdOfThread( th0 );
		DWORD pid1 = GetProcessIdOfThread( th1 );
		CloseHandle( th0 );
		CloseHandle( th1 );
		if ((pid != pid0) && (pid != pid1))
			return;

		/*
		// find process names
		PROCESSENTRY32 pe0 = {};
		PROCESSENTRY32 pe1 = {};
		HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		FindProcessEntry( hSnapshot, pid0, pe0 );
		FindProcessEntry( hSnapshot, pid1, pe1 );
		CloseHandle( hSnapshot );

		// buffer event

		// trace
		/*
		char msg[256];
		sprintf_s( msg, "Core %2u: {%-40S (%8u) : %8u} -> {%-40S (%8u) : %8u}\n", core, pe0.szExeFile, pid0, tid0, pe1.szExeFile, pid1, tid1 );
		OutputDebugStringA( msg );
		*/
	}

	static DWORD WINAPI PollEventTrace( void* p )	
	{
		MyTrace_s* my_trace = (MyTrace_s*)p;
		ULONG hr;
		hr = ProcessTrace( &my_trace->Consumer, 1, 0, 0 );
		return 0;
	}
}

//======================================================================================

Result_t MyTrace::Initialize()
{
	// don't init twice
	if (MyTraceInstance.Consumer != INVALID_TRACE_HANDLE_VALUE)
		return NE_ERR_INVALID_CALL;

	ULONG hr;
	EVENT_TRACE_PROPERTIES_EX trace_props;

	// stop trace
	{
		InitializeTracePropertiesEx( trace_props );
		hr = ControlTraceW( 0, KERNEL_LOGGER_NAMEW, &trace_props.Properties, EVENT_TRACE_CONTROL_STOP );
	}

	// start trace
	{
		InitializeTracePropertiesEx( trace_props );
		TRACEHANDLE hStartTrace = INVALID_TRACE_HANDLE_VALUE;
		hr = StartTraceW( &hStartTrace, KERNEL_LOGGER_NAMEW, &trace_props.Properties );
		if (hr != ERROR_SUCCESS)
			return NE_ERROR;
	}

	// open trace
	{
		EVENT_TRACE_LOGFILEW trace_log;
		NeZero(trace_log);
		trace_log.LoggerName = KERNEL_LOGGER_NAMEW;
		trace_log.ProcessTraceMode = (PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_RAW_TIMESTAMP);
		trace_log.EventRecordCallback = EnumEvent;

		TRACEHANDLE hOpenTrace = OpenTraceW( &trace_log );
		if (hOpenTrace == INVALID_TRACE_HANDLE_VALUE)
			return NE_ERROR;

		MyTraceInstance.Consumer = hOpenTrace;
	}

	// listen for events
	{
		DWORD thread_id;
		HANDLE hThread = CreateThread( NULL, 0, PollEventTrace, &MyTraceInstance, 0, &thread_id );
		if (hThread == INVALID_HANDLE_VALUE)
		{
			CloseTrace(MyTraceInstance.Consumer);
			MyTraceInstance.Consumer = INVALID_TRACE_HANDLE_VALUE;
			return NE_ERROR;
		}
		MyTraceInstance.Thread = hThread;
	}

	// store trace handle
	return NE_OK;
}

int MyTrace::PollEvents( MyEvent* ev, int count )
{
	return 0;
}

void MyTrace::Shutdown()
{
	if (MyTraceInstance.Consumer == INVALID_TRACE_HANDLE_VALUE)
		return;

	CloseTrace( MyTraceInstance.Consumer );
	MyTraceInstance.Consumer = INVALID_TRACE_HANDLE_VALUE;

	WaitForSingleObject( MyTraceInstance.Thread, INFINITE );
	MyTraceInstance.Thread = INVALID_HANDLE_VALUE;
}
