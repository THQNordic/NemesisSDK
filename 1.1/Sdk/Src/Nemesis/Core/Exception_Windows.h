//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once

//======================================================================================
#include <Nemesis/Core/Debug.h>
#include <Nemesis/Core/Memory.h>
#include <stdio.h>
#pragma warning ( push )
#pragma warning ( disable: 4091 )
#	include <dbghelp.h>
#pragma warning ( pop )
#pragma comment (lib, "dbghelp.lib")

//======================================================================================
namespace nemesis { namespace system
{
	//--------------------------------------------------------------------------------------
	static inline void InitStackFrame
		( const CONTEXT& Context
		, STACKFRAME64& StackFrame 
		, DWORD& MachineType
		)
	{
		NeZero( StackFrame );

	#ifdef _M_IX86
  		MachineType                 = IMAGE_FILE_MACHINE_I386;
  		StackFrame.AddrPC.Offset    = Context.Eip;
  		StackFrame.AddrPC.Mode      = AddrModeFlat;
  		StackFrame.AddrFrame.Offset = Context.Ebp;
  		StackFrame.AddrFrame.Mode   = AddrModeFlat;
  		StackFrame.AddrStack.Offset = Context.Esp;
  		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif _M_X64
  		MachineType                 = IMAGE_FILE_MACHINE_AMD64;
  		StackFrame.AddrPC.Offset    = Context.Rip;
  		StackFrame.AddrPC.Mode      = AddrModeFlat;
  		StackFrame.AddrFrame.Offset = Context.Rsp;
  		StackFrame.AddrFrame.Mode   = AddrModeFlat;
  		StackFrame.AddrStack.Offset = Context.Rsp;
  		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif _M_IA64
  		MachineType                 = IMAGE_FILE_MACHINE_IA64;
  		StackFrame.AddrPC.Offset    = Context.StIIP;
  		StackFrame.AddrPC.Mode      = AddrModeFlat;
  		StackFrame.AddrFrame.Offset = Context.IntSp;
  		StackFrame.AddrFrame.Mode   = AddrModeFlat;
  		StackFrame.AddrBStore.Offset= Context.RsBSP;
  		StackFrame.AddrBStore.Mode  = AddrModeFlat;
  		StackFrame.AddrStack.Offset = Context.IntSp;
  		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#else
	#	error Unsupported platform...
	#endif
	}

	///	Iterator for captured call stacks
	class StackWalk
	{
	public:
		explicit StackWalk( EXCEPTION_POINTERS* exceptionInfo );
		StackWalk();

	public:
		void Begin( EXCEPTION_POINTERS* exceptionInfo );
		bool Next();

	public:
		const void* GetFrameAddress() const;

	private:
		STACKFRAME64 StackFrame;
		DWORD		 MachineType;
		CONTEXT		 Context;
		BOOL		 Done;
	};

	StackWalk::StackWalk( EXCEPTION_POINTERS* exceptionInfo )
	{ Begin(exceptionInfo); }

	StackWalk::StackWalk()
	{ Begin(nullptr); }

	void StackWalk::Begin( EXCEPTION_POINTERS* exceptionInfo )
	{
		if (exceptionInfo)
		{
			Done = FALSE;
			Context = *exceptionInfo->ContextRecord;
			InitStackFrame( this->Context, this->StackFrame, this->MachineType );
		}
		else
		{
			Done = TRUE;
			Context = CONTEXT();
			StackFrame = STACKFRAME64();
			MachineType = 0;
		}
	}

	bool StackWalk::Next()
	{
		const BOOL bResult = 
			::StackWalk64
			( MachineType
			, GetCurrentProcess()
			, GetCurrentThread()
			, &StackFrame
			, &Context
			, NULL	// ReadMemoryRoutine
			, SymFunctionTableAccess64
			, SymGetModuleBase64
			, NULL	// TranslateAddress
			);

		if ( !bResult || (0 == StackFrame.AddrPC.Offset) )
			Done = TRUE;
		return !Done;
	}

	const void* StackWalk::GetFrameAddress() const
	{
		if (!Done)
			return (const void*)StackFrame.AddrPC.Offset;
		return 0l;
	}

} }

//======================================================================================
namespace nemesis { namespace system
{
	//==================================================================================
	// internals
	static const char* GetExceptionName( DWORD code )
	{
	#define CASE(x) case x:	return #x

		switch ( code )
		{
		CASE( STATUS_NO_MEMORY );
		CASE( EXCEPTION_ACCESS_VIOLATION );
		CASE( EXCEPTION_ARRAY_BOUNDS_EXCEEDED );
		CASE( EXCEPTION_BREAKPOINT );
		CASE( EXCEPTION_DATATYPE_MISALIGNMENT );
		CASE( EXCEPTION_FLT_DENORMAL_OPERAND );
		CASE( EXCEPTION_FLT_DIVIDE_BY_ZERO );
		CASE( EXCEPTION_FLT_INEXACT_RESULT );
		CASE( EXCEPTION_FLT_INVALID_OPERATION );
		CASE( EXCEPTION_FLT_OVERFLOW );
		CASE( EXCEPTION_FLT_STACK_CHECK );
		CASE( EXCEPTION_FLT_UNDERFLOW );
		CASE( EXCEPTION_GUARD_PAGE );
		CASE( EXCEPTION_ILLEGAL_INSTRUCTION );
		CASE( EXCEPTION_IN_PAGE_ERROR );
		CASE( EXCEPTION_INT_DIVIDE_BY_ZERO );
		CASE( EXCEPTION_INT_OVERFLOW );
		CASE( EXCEPTION_INVALID_DISPOSITION );
		CASE( EXCEPTION_INVALID_HANDLE );
		CASE( EXCEPTION_NONCONTINUABLE_EXCEPTION );
		CASE( EXCEPTION_PRIV_INSTRUCTION );
		CASE( EXCEPTION_SINGLE_STEP );
		CASE( EXCEPTION_STACK_OVERFLOW );
		}

	#undef CASE
		return "EXCEPTION_UNKNOWN";
	}

	static const char* GetExceptionDescription( DWORD code )
	{
		switch ( code )
		{
		case STATUS_NO_MEMORY:						return "Not Enough Quota.";
		case EXCEPTION_ACCESS_VIOLATION:			return "The thread attempts to read from or write to a virtual address for which it does not have access.";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		return "The thread attempts to access an array element that is out of bounds, and the underlying hardware supports bounds checking.";
		case EXCEPTION_BREAKPOINT:					return "A breakpoint is encountered.";
		case EXCEPTION_DATATYPE_MISALIGNMENT:		return "The thread attempts to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte boundaries, and so on.";
		case EXCEPTION_FLT_DENORMAL_OPERAND:		return "One of the operands in a floating point operation is denormal. A denormal value is one that is too small to represent as a standard floating point value.";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:			return "The thread attempts to divide a floating point value by a floating point divisor of 0 (zero).";
		case EXCEPTION_FLT_INEXACT_RESULT:			return "The result of a floating point operation cannot be represented exactly as a decimal fraction.";
		case EXCEPTION_FLT_INVALID_OPERATION:		return "A floating point exception that is not included in this list.";
		case EXCEPTION_FLT_OVERFLOW:				return "The exponent of a floating point operation is greater than the magnitude allowed by the corresponding type.";
		case EXCEPTION_FLT_STACK_CHECK:				return "The stack has overflowed or underflowed, because of a floating point operation.";
		case EXCEPTION_FLT_UNDERFLOW:				return "The exponent of a floating point operation is less than the magnitude allowed by the corresponding type.";
		case EXCEPTION_GUARD_PAGE:					return "The thread accessed memory allocated with the PAGE_GUARD modifier.";
		case EXCEPTION_ILLEGAL_INSTRUCTION:			return "The thread tries to execute an invalid instruction.";
		case EXCEPTION_IN_PAGE_ERROR:				return "The thread tries to access a page that is not present, and the system is unable to load the page. For example, this exception might occur if a network connection is lost while running a program over a network.";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:			return "The thread attempts to divide an integer value by an integer divisor of 0 (zero).";
		case EXCEPTION_INT_OVERFLOW:				return "The result of an integer operation causes a carry out of the most significant bit of the result.";
		case EXCEPTION_INVALID_DISPOSITION:			return "An exception handler returns an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
		case EXCEPTION_INVALID_HANDLE:				return "The thread used a handle to a kernel object that was invalid (probably because it had been closed.)";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:	return "The thread attempts to continue execution after a non-continuable exception occurs.";
		case EXCEPTION_PRIV_INSTRUCTION:			return "The thread attempts to execute an instruction with an operation that is not allowed in the current computer mode.";
		case EXCEPTION_SINGLE_STEP:					return "A trace trap or other single instruction mechanism signals that one instruction is executed.";
		case EXCEPTION_STACK_OVERFLOW:				return "The thread uses up its stack.";
		}
		return "The reason is unknown.";
	}

	static void DisplayExceptionMessageBox( EXCEPTION_POINTERS* exceptionInfo )
	{
		char msg_text[2048] = "";
		const int msg_max = sizeof(msg_text)/sizeof(msg_text[0]);
		int msg_pos = 0;

	#define W(...) msg_pos += sprintf_s(msg_text + msg_pos, msg_max - msg_pos, __VA_ARGS__);

		// append description
		{
			W( "Exception Code:\n%s\n", GetExceptionName( exceptionInfo->ExceptionRecord->ExceptionCode ) );
			W( "\nDescription:\n%s\n", GetExceptionDescription( exceptionInfo->ExceptionRecord->ExceptionCode ) );
		}

		// append callstack
		{
			using namespace nemesis::system;
			PdbSymbolInfo_s symbol;
			PdbSymbolLineInfo_s line;
			uint64_t offset;
			W("\nCallstack:\n");
			for ( StackWalk iter( exceptionInfo ); iter.Next(); )
			{
				offset = 0;
				NeZero(line);
				NeZero(symbol);
				system::Pdb_FindSymbolInfoByAddress(iter.GetFrameAddress(), &symbol, &offset);
				system::Pdb_FindLineInfoByAddress(iter.GetFrameAddress(), &line, 0);
				W(" <-- %s", symbol.Name);
			}
		}

	#undef W
		
		MessageBoxA(nullptr, msg_text, "Unhandled Exception", MB_ICONERROR);
	}

	static LONG WINAPI FilterUnhandledException( EXCEPTION_POINTERS* exceptionInfo )
	{
		// exceptions might be thrown even before the core is fully initialized.
		// this call ensures that even so debug symbols are available.
		nemesis::system::Pdb_Initialize();

		DisplayExceptionMessageBox( exceptionInfo );

		// we have the following options:
		//
		//	EXCEPTION_EXECUTE_HANDLER		0x1			Return from UnhandledExceptionFilter and execute the associated exception handler. This usually results in process termination.
		//	EXCEPTION_CONTINUE_EXECUTION	0xffffffff	Return from UnhandledExceptionFilter and continue execution from the point of the exception. Note that the filter function is free to modify the continuation state by modifying the exception information supplied through its LPEXCEPTION_POINTERS parameter.
		//	EXCEPTION_CONTINUE_SEARCH		0x0			Proceed with normal execution of UnhandledExceptionFilter. That means obeying the SetErrorMode flags, or invoking the Application Error pop-up message box.
		return EXCEPTION_EXECUTE_HANDLER;
	}

	//==================================================================================
	// public interface
	void Exception_Raise()
	{
		RaiseException( EXCEPTION_NONCONTINUABLE_EXCEPTION, EXCEPTION_NONCONTINUABLE, 0, nullptr );
	}

	void Exception_SetHook()
	{
		SetUnhandledExceptionFilter( &FilterUnhandledException );
	}

	uint32_t Exception_Filter( void* info )
	{
		return FilterUnhandledException( (EXCEPTION_POINTERS*)info );
	}
} }
