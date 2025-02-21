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
#include "JsonNode.h"
#include "Stack.h"

//======================================================================================
namespace nemesis
{
	typedef struct JsonDoc_s  *JsonDoc_t;
	typedef struct JsonNode_s *JsonNode_t;

	struct JsonDoc_s
	{
		JsonNode_s*	Head;
		Stack_s		Stack;
	};

}

//======================================================================================
namespace nemesis
{
	void NE_API JsonDoc_Init( JsonDoc_s& doc, Allocator_t alloc );
	void NE_API JsonDoc_Init( JsonDoc_s& doc, Allocator_t alloc, size_t page_size );
	void NE_API JsonDoc_Reset( JsonDoc_s& doc );
	void NE_API JsonDoc_Clear( JsonDoc_s& doc );

	Result_t NE_API JsonDoc_Parse( JsonDoc_s& doc, cstr_t json, size_t size, JsonError_s& err );
} 
