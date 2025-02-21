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
#define NE_ERR_JSON							(NE_ERROR | NE_ERR_CORE | 0x0001000)
#define NE_ERR_JSON_INVALID_ARG				NE_ERR_INVALID_ARG
#define NE_ERR_JSON_OUT_OF_MEMORY			NE_ERR_OUT_OF_MEMORY
#define NE_ERR_JSON_BRACE_MISMATCH			(NE_ERR_JSON | 0x1)
#define NE_ERR_JSON_MISSING_DELIMITER		(NE_ERR_JSON | 0x2)
#define NE_ERR_JSON_MULTIPLE_ROOT_OBJECTS	(NE_ERR_JSON | 0x3)
#define NE_ERR_JSON_END_OF_STRING			(NE_ERR_JSON | 0x4)
#define NE_ERR_JSON_END_OF_FILE				(NE_ERR_JSON | 0x5)
#define NE_ERR_JSON_UNEXPECTED_CHAR			(NE_ERR_JSON | 0x6)

//======================================================================================
namespace nemesis
{
	/// Json

	struct JsonType
	{
		enum Enum
		{ Null
		, Int
		, Float
		, Bool
		, String
		, Object
		, Array
		};
	};

	struct JsonEvent
	{
		enum Enum
		{ Begin
		, Value
		, End
		};
	};

	struct JsonError_s
	{
		cstr_t Msg;
		cstr_t Pos;
		int Line;
	};

	struct JsonClient_s
	{
		void (NE_CALLBK *HandleEvent)( void* context, uint32_t pos, JsonEvent::Enum ev, JsonType::Enum type, cstr_t name, int name_len, cstr_t val, int val_len, int line );
		void* Context;
	};
}

//======================================================================================
namespace nemesis
{
	/// Json

	Result_t NE_API Json_Parse( cstr_t src, uint32_t len, const JsonClient_s& client, JsonError_s* error );

}
