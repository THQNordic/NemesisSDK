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
#include "StrBuf.h"

//======================================================================================
namespace nemesis
{
	struct Json
	{
		enum Kind
		{ Root
		, Array
		, Object
		, Field
		, Value
		};
	};

	struct JsonOut_s
	{
		StrBuf_s			Text;
		Array<Json::Kind>	Stack;
		Json::Kind			Scope;
		Json::Kind			Item;
		int					Indent;
		cstr_t				Tab;
		StrBuf_s			Comment;
	};

	void NE_API JsonOut_Initialize	( JsonOut_s& json, Allocator_t alloc, int32_t text_size, int32_t page_size, int32_t comment_size );
	void NE_API JsonOut_Initialize	( JsonOut_s& json, Allocator_t alloc );
	void NE_API JsonOut_Shutdown	( JsonOut_s& json);
	void NE_API JsonOut_Reset		( JsonOut_s& json );

	void NE_API JsonOut_Comment	( JsonOut_s& json, cstr_t text );
	void NE_API JsonOut_Null	( JsonOut_s& json );
	void NE_API JsonOut_Value	( JsonOut_s& json, bool v );
	void NE_API JsonOut_Value	( JsonOut_s& json, int32_t v );
	void NE_API JsonOut_Value	( JsonOut_s& json, uint32_t v );
	void NE_API JsonOut_Value	( JsonOut_s& json, float v );
	void NE_API JsonOut_Value	( JsonOut_s& json, cstr_t v );
	void NE_API JsonOut_Field	( JsonOut_s& json, cstr_t name );
	void NE_API JsonOut_Array	( JsonOut_s& json );
	void NE_API JsonOut_Object	( JsonOut_s& json );
	void NE_API JsonOut_End		( JsonOut_s& json );

	void NE_API JsonOut_Array	( JsonOut_s& json, cstr_t name );
	void NE_API JsonOut_Object	( JsonOut_s& json, cstr_t name );

} 
