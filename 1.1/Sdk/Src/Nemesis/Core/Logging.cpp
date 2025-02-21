//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Logging.h>

//======================================================================================
#include <Nemesis/Core/Debug.h>
#include <Nemesis/Core/String.h>

//======================================================================================
namespace nemesis 
{
	static void NE_CALLBK Logger_DefaultHook( void*, const LogInfo_s& info, cstr_t msg )
	{
		system::Debug_Print( msg );
	}

	static Logger_s Logger_GetDefault()
	{
		const Logger_s log = { Logger_DefaultHook };
		return log;
	}
}

//======================================================================================
namespace nemesis 
{
	struct LoggerInstance_s
	{
		Logger_s Hook;
	};

	namespace global
	{
		static LoggerInstance_s LoggerInstance = { Logger_GetDefault() };
	}
}

//======================================================================================
namespace nemesis 
{
	void Logger_SetHook( const Logger_s& log )
	{
		global::LoggerInstance.Hook = log.Log ? log : Logger_GetDefault();
	}

	void Logger_Log( const LogInfo_s& info, cstr_t fmt, va_list args )
	{
		char text[1024];
		Str_FmtV( text, fmt, args ); 
		global::LoggerInstance.Hook.Log( global::LoggerInstance.Hook.Context, info, text );
	}

	void Logger_Log( const LogInfo_s& info, cstr_t fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		Logger_Log( info, fmt, args );
		va_end( args );
	}
}
