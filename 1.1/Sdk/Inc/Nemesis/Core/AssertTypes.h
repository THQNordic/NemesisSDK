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
	struct AssertAction
	{
		enum Enum
		{ Continue
		, Terminate
		, Break
		};
	};

	struct AssertInfo_s
	{
		AssertInfo_s() {};
		AssertInfo_s( cstr_t expression, cstr_t function, cstr_t file, int32_t line )
			: Expression( expression )
			, Function( function )
			, File( file )
			, Line( line )
		{};

		cstr_t Expression;
		cstr_t Function;
		cstr_t File;
		int32_t Line;
	};

	struct AssertHook_s
	{
		AssertAction::Enum (NE_CALLBK *AssertFailed)( void* context, const AssertInfo_s& info, cstr_t msg );
		void* Context;
	};

}
