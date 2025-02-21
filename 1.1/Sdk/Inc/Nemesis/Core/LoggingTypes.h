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
#include "Types.h"

//======================================================================================
namespace nemesis
{
	struct LogSeverity
	{
		enum Enum 
		{ Info
		, Warning
		, Error 
		};
	};


	struct LogInfo_s
	{
		LogInfo_s() {};
		LogInfo_s( LogSeverity::Enum severity, cstr_t function, cstr_t file, unsigned long line )
			: Severity	( severity )
			, Source	( function, file, line )
		{};

		LogSeverity::Enum Severity;
		Location		  Source;
	};

	struct Logger_s
	{
		void (NE_CALLBK *Log)( void* context, const LogInfo_s& info, cstr_t msg );
		void* Context;
	};
}