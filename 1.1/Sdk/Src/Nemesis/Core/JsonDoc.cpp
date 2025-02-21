//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/JsonDoc.h>
#include <Nemesis/Core/String.h>

//======================================================================================
//																				 Private
//======================================================================================
namespace nemesis
{
	static void NE_CALLBK Json_ParseDoc( void* context, uint32_t pos, JsonEvent::Enum ev, JsonType::Enum type, cstr_t name, int name_len, cstr_t val, int val_len, int line )
	{
		JsonDoc_s* doc = (JsonDoc_s*)context;

		switch ( ev )
		{
		case JsonEvent::Begin:
		case JsonEvent::Value:
			{
				JsonNode_s* node = (JsonNode_s*)doc->Stack.Alloc( sizeof(JsonNode_s) );
				node->Type   = type;
				node->Line   = line;
				node->Pos	 = pos;
				node->End	 = 0;
				node->Name   = nullptr;
				node->Value  = nullptr;
				node->Next   = nullptr;
				node->Last   = nullptr;
				node->Member = nullptr;
				node->Parent = doc->Head;
				node->Count  = 0;
				node->_pad_	 = 0;

				if (name_len)
				{
					node->Name = (str_t)doc->Stack.Alloc( 1+name_len );
					Mem_Cpy( node->Name, name, name_len );
					node->Name[ name_len ] = 0;
				}

				if (val_len)
				{
					node->Value = (str_t)doc->Stack.Alloc( 1+val_len );
					Mem_Cpy( node->Value, val, val_len );
					node->Value[ val_len ] = 0;
				}

				if (node->Parent)
				{
					NeAssert((node->Parent->Type == JsonType::Array) || (node->Parent->Type == JsonType::Object));
					if (node->Parent->Last)
						node->Parent->Last->Next = node;
					else
						node->Parent->Member = node;
					node->Parent->Last = node;
					node->Parent->Count++;
				}

				if (ev == JsonEvent::Begin)
					doc->Head = node;
			}
			break;

		case JsonEvent::End:
			{
				doc->Head->End = pos;
				if (doc->Head->Parent)
					doc->Head = doc->Head->Parent;
			}
			break;
		}
	}
}

//======================================================================================
//																				  Public
//======================================================================================
namespace nemesis
{
	void JsonDoc_Init( JsonDoc_s& doc, Allocator_t alloc, size_t page_size )
	{
		NeZero(doc);
		doc.Stack.Init( alloc );
		doc.Stack.PageSize = page_size;
	}

	void JsonDoc_Init( JsonDoc_s& doc, Allocator_t alloc )
	{
		NeZero(doc);
		doc.Stack.Init( alloc );
	}

	void JsonDoc_Reset( JsonDoc_s& doc )
	{
		doc.Head = nullptr;
		doc.Stack.Reset();
	}

	void JsonDoc_Clear( JsonDoc_s& doc )
	{
		doc.Stack.Clear();
		NeZero(doc);
	}

	Result_t JsonDoc_Parse( JsonDoc_s& doc, cstr_t json, size_t size, JsonError_s& err )
	{
		const JsonClient_s json_client = { Json_ParseDoc, &doc };
		return Json_Parse( json, (uint32_t)size, json_client, &err );
	}

} 