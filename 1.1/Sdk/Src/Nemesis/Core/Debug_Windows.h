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
#pragma warning ( push )
#pragma warning ( disable: 4091 )
#	include <dbghelp.h>
#pragma warning ( pop )
#pragma comment (lib, "dbghelp.lib")

//======================================================================================
namespace nemesis { namespace system
{
	#define SHOW_LAST_ERROR 0

	#if SHOW_LAST_ERROR
	#	define CALL(expr) do { br = (expr); if (!br) DisplayLastError(); } while (false)
	#else
	#	define CALL(expr) (br = (expr))
	#endif

	//==================================================================================
	// implementation details
	#if SHOW_LAST_ERROR
	static void DisplayLastError()
	{
		LPVOID lpMsgBuf;
		::FormatMessageA( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPSTR) &lpMsgBuf,
			0,
			NULL 
		);
		::MessageBoxA( NULL, (LPCSTR)lpMsgBuf, "Error", MB_ICONERROR );
		::LocalFree( lpMsgBuf );
	}
	#endif

	//==================================================================================
	// public interface
	void Pdb_Initialize()
	{
		BOOL br;
		CALL(SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES ));
		CALL(SymInitialize( GetCurrentProcess(), NULL, TRUE ));
	}

	//----------------------------------------------------------------------------------
	void Pdb_Shutdown()
	{
		BOOL br;
		CALL(SymCleanup( GetCurrentProcess() ));
	}

	//----------------------------------------------------------------------------------
	uint_t StackTrace_Capture( uint_t numSkip, uint_t numCapture, void** frames, uint32_t* hash )
	{
		return ::CaptureStackBackTrace( numSkip, numCapture, frames, (DWORD*)hash );
	}

	//----------------------------------------------------------------------------------
	bool Pdb_FindSymbolInfoByName( const char* name, PdbSymbolInfo_s* info )
	{
		BOOL br;
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)info;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = info->MAX_NAME_CHARS;
		CALL(SymFromName( GetCurrentProcess(), name, pSymbol ));
		return br ? true : false;
	}

	bool Pdb_FindSymbolInfoByAddress( const void* address, PdbSymbolInfo_s* info, uint64_t* offset )
	{
		BOOL br;
		DWORD64 dwOffset = 0;
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)info;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = info->MAX_NAME_CHARS;
		CALL(SymFromAddr( GetCurrentProcess(), (DWORD64)address, &dwOffset, pSymbol ));
		if (offset)
			*offset = dwOffset;
		return br ? true : false;
	}

	bool Pdb_FindLineInfoByAddress( const void* address, PdbSymbolLineInfo_s* info, uint32_t* offset )
	{
		BOOL br;
		DWORD dwOffset = 0;
		PIMAGEHLP_LINE64 pLine = (PIMAGEHLP_LINE64)info;
		pLine->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		CALL(SymGetLineFromAddr64( GetCurrentProcess(), (DWORD64)address, &dwOffset, pLine ));
		if (offset)
			*offset = dwOffset;
		return br ? true : false;
	}
} }

//======================================================================================
namespace nemesis { namespace system
{
	void Debug_Print( const char* msg )
	{
		OutputDebugStringA( msg );
	}
} }

//======================================================================================
namespace nemesis { namespace system
{
	DebugPopUp::Result Debug_PopUp( DebugPopUp::Mode mode, cstr_t caption, cstr_t msg )
	{
		UINT type;
		switch ( mode )
		{
		case DebugPopUp::None:
			type = MB_OK;
			break;
		case DebugPopUp::OkCancel:
			type = MB_OKCANCEL;
			break;
		default:
			type = MB_CANCELTRYCONTINUE;
			break;
		}
		switch ( MessageBoxA( nullptr, msg, caption, MB_ICONERROR | type ) )
		{
		case IDOK:
			return DebugPopUp::Ok;
		case IDTRYAGAIN:
			break;
		case IDCONTINUE:
			return DebugPopUp::Continue;
		case IDCANCEL:
			return DebugPopUp::Cancel;
		}
		return DebugPopUp::Retry;
	}

} }