//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Debug.h>
#include <Nemesis/Core/String.h>
#include <stdlib.h>

//======================================================================================
namespace nemesis
{
	static AssertAction::Enum NE_CALLBK Assert_DefaultHook( void*, const AssertInfo_s& info, cstr_t msg )
	{
		char buffer[1024] = "";
		Str_Fmt
			( buffer
			, sizeof(buffer)
			, "Assertion Failed!\n"
			  "\n"
			  "File: %s\n"
			  "Line: %u\n"
			  "\n"
			  "Expression: %s\n"
			  "\n"
			  "Comment: %s\n"
			  "\n"
			  "Press Retry to debug the application (JIT must be enabled)\n"
			  "\n"
			, info.File
			, info.Line
			, info.Expression
			, msg ? msg : "(not available)"
			);

		switch (system::Debug_PopUp( system::DebugPopUp::RetryCancelContinue, "Assertion Failure", buffer ))
		{
		case system::DebugPopUp::Retry:
			break;
		case system::DebugPopUp::Continue:
			return AssertAction::Continue;
		default:
			return AssertAction::Terminate;
		}
		return AssertAction::Break;
	}
}

//======================================================================================
namespace nemesis
{
	struct AssertInstance_s
	{
		AssertHook_s Hook;
	};

	namespace global
	{
		static AssertInstance_s AssertInstance = {};
	}
}

//======================================================================================
namespace nemesis
{
	static AssertHook_s Assert_GetDefaultHook()
	{
		const AssertHook_s hook = { Assert_DefaultHook };
		return hook;
	}

	AssertHook_s Assert_GetHook()
	{ 
		return global::AssertInstance.Hook; 
	}

	void Assert_SetHook( const AssertHook_s& hook )
	{ 
		global::AssertInstance.Hook = hook; 
	}

	AssertAction::Enum Assert_Failure( const AssertInfo_s& info, cstr_t msg, va_list args )
	{
		char text[ 1024 ];
		Str_FmtV( text, msg, args );
		const AssertHook_s hook
			= global::AssertInstance.Hook.AssertFailed
			? global::AssertInstance.Hook 
			: Assert_GetDefaultHook()
			;
		return hook.AssertFailed( hook.Context, info, text );
	}

	AssertAction::Enum Assert_Failure( const AssertInfo_s& info, cstr_t msg, ... )
	{
		va_list args;
		va_start( args, msg );
		const AssertAction::Enum action = Assert_Failure( info, msg, args );
		va_end( args );
		switch ( action )
		{
		case AssertAction::Terminate:
			exit( 1 );
			break;
		default:
			break;
		}
		return action;
	}
}
