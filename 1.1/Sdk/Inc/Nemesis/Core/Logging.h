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
#include "LoggingTypes.h"

//======================================================================================
namespace nemesis 
{
	void NE_API Logger_SetHook( const Logger_s& log );
	void NE_API Logger_Log	  ( const LogInfo_s& info, cstr_t fmt, ... );
}

//======================================================================================
#ifndef NE_ENABLE_LOGGING
#	define NE_ENABLE_LOGGING	(NE_CONFIG <= NE_CONFIG_RELEASE)
#endif

//======================================================================================
/// General purpose logging
#if NE_ENABLE_LOGGING
#	define NeLog(severity, ...)\
		::nemesis::Logger_Log\
			( ::nemesis::LogInfo_s\
				( severity\
				, __FUNCTION__\
				, __FILE__\
				, __LINE__\
				)\
			, __VA_ARGS__\
			)
#else
#	define NeLog(severity, ...) //__noop( severity, __VA_ARGS__ )
#endif

#define NeTrace(...)\
	NeLog( ::nemesis::LogSeverity::Info, __VA_ARGS__ )

//======================================================================================
#if NE_ENABLE_LOGGING
#	define NeTraceIf(cond, ...) do { if (cond) NeTrace(__VA_ARGS__); } while( false )
#else
#	define NeTraceIf(cond, ...) //__noop( cond, __VA_ARGS__ )
#endif
