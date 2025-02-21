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
#include "DebugTypes.h"

//======================================================================================
namespace nemesis {	namespace system
{
	/// Stack Trace

	uint_t NE_API StackTrace_Capture( uint_t numSkip, uint_t numCapture, void** frames, uint32_t* hash );

	/// Debug Symbols

	void NE_API Pdb_Initialize				();
	void NE_API Pdb_Shutdown				();
	bool NE_API Pdb_FindSymbolInfoByName	( const char* name	 , PdbSymbolInfo_s*	    info );
	bool NE_API Pdb_FindSymbolInfoByAddress	( const void* address, PdbSymbolInfo_s*	    info, uint64_t* offset );
	bool NE_API Pdb_FindLineInfoByAddress	( const void* address, PdbSymbolLineInfo_s* info, uint32_t* offset );

	/// Debug Output

	void NE_API Debug_Print( const char* msg );

	/// Debug Popup

	DebugPopUp::Result NE_API Debug_PopUp( DebugPopUp::Mode mode, cstr_t caption, cstr_t msg );

} }
