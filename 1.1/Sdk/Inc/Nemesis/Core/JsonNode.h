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
#include "Json.h"
#include "Assert.h"

//======================================================================================
namespace nemesis
{
	struct JsonNode_s
	{
		JsonType::Enum	Type;
		int32_t			Line;
		uint32_t		Pos;
		uint32_t		End;
		str_t			Name;
		str_t			Value;
		JsonNode_s*		Next;
		JsonNode_s*		Last;
		JsonNode_s*		Member;
		JsonNode_s*		Parent;
		int32_t			Count;
		int32_t			_pad_;
	};

	struct JsonRead
	{
		enum Result
		{ Ok
		, NotFound
		, OutOfRange
		, TypeMismatch
		, Overflow
		, Underflow
		};
	};

	struct JsonReadResult_s
	{
		JsonRead::Result  Result;
		const JsonNode_s* Node;
		JsonType::Enum	  Type;
		cstr_t			  Name;
	};
}

//======================================================================================
namespace nemesis
{
	JsonNode_s* NE_API JsonNode_FindMember( const JsonNode_s* node, cstr_t name );
	int			NE_API JsonNode_ReadCount ( const JsonNode_s* node );

}

//======================================================================================
namespace nemesis 
{
	JsonReadResult_s NE_API JsonNode_ReadCount( const JsonNode_s* node, int* count );
	JsonReadResult_s NE_API JsonNode_ReadCount( const JsonNode_s* node, JsonType::Enum type, int* count );

}

//======================================================================================
namespace nemesis 
{
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, bool*	 v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, int*		 v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, float*	 v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, cstr_t*	 v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, uint16_t* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, uint32_t* v );

}

//======================================================================================
namespace nemesis 
{
	struct Vec2_s;
	struct Vec3_s;
	struct Vec4_s;
	struct Pos3_s;
	struct Quat_s;
	struct Rect_s;

	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Vec2_s* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Vec3_s* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Vec4_s* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Pos3_s* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Quat_s* v );
	JsonReadResult_s NE_API JsonNode_Read( const JsonNode_s* node, Rect_s* v );

}

//======================================================================================
namespace nemesis
{
	// Array (Static)

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Array( const JsonNode_s* node, T* v, int capacity )
	{
		NeAssert(node && v);
		int count = 0;
		switch (node->Type)
		{
		case JsonType::Array:
			{
				for ( const JsonNode_s* item = node->Member; item; item = item->Next, ++count )
				{
					if (count == capacity)
						return JsonReadResult_s { JsonRead::Overflow, item, JsonType::Array };

					const JsonReadResult_s result = JsonNode_Read( item, v + count );
					if (result.Result != JsonRead::Ok)
						return result;
				}
			}
			break;
		default:
			const JsonReadResult_s result = JsonNode_Read( node, v );
			if (result.Result != JsonRead::Ok)
				return result;
			++count;
			break;
		}
		if (count < (capacity-1))
			return JsonReadResult_s { JsonRead::Underflow, node, JsonType::Array };
		return JsonReadResult_s { JsonRead::Ok, node };
	}

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Array( const JsonNode_s* node, T* v, int* count )
	{
		NeAssert(node && v && count);
		(*count) = 0;
		switch (node->Type)
		{
		case JsonType::Array:
			{
				for ( const JsonNode_s* item = node->Member; item; item = item->Next, ++(*count) )
				{
					const JsonReadResult_s result = JsonNode_Read( item, v + (*count) );
					if (result.Result != JsonRead::Ok)
						return result;
				}
			}
			break;
		default:
			{
				const JsonReadResult_s result = JsonNode_Read( node, v );
				if (result.Result != JsonRead::Ok)
					return result;
				(*count) = 1;
			}
			break;
		}
		return JsonReadResult_s { JsonRead::Ok, node };
	}

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Array( const JsonNode_s* node, T* v, int* count, int capacity )
	{
		NeAssert(node && v && count);
		(*count) = 0;
		switch (node->Type)
		{
		case JsonType::Array:
			{
				for ( const JsonNode_s* item = node->Member; item && ((*count) < capacity); item = item->Next, ++(*count) )
				{
					const JsonReadResult_s result = JsonNode_Read( item, v + (*count) );
					if (result.Result != JsonRead::Ok)
						return result;
				}
			}
			break;
		default:
			{
				const JsonReadResult_s result = JsonNode_Read( node, v );
				if (result.Result != JsonRead::Ok)
					return result;
				(*count) = 1;
			}
			break;
		}
		return JsonReadResult_s { JsonRead::Ok, node };
	}

}

//======================================================================================
namespace nemesis
{
	// Member

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Member( const JsonNode_s* node, cstr_t name, T* v )
	{
		const JsonNode_s* member = JsonNode_FindMember( node, name );
		return member 
			? JsonNode_Read( member, v )
			: JsonReadResult_s { JsonRead::NotFound, node, JsonType::Null, name };
	}

	// Member Array (Static)

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Member( const JsonNode_s* node, cstr_t name, T* v, int capacity )
	{
		const JsonNode_s* member = JsonNode_FindMember( node, name );
		return member 
			? JsonNode_Read_Array( member, v, capacity )
			: JsonReadResult_s { JsonRead::NotFound, node, JsonType::Array, name };
	}

	// Member Array (Dynamic)

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Member( const JsonNode_s* node, cstr_t name, T* v, int* count )
	{
		const JsonNode_s* member = JsonNode_FindMember( node, name );
		return member 
			? JsonNode_Read_Array( member, v, count )
			: JsonReadResult_s { JsonRead::NotFound, node, JsonType::Array, name };
	}

	template < typename T >
	inline JsonReadResult_s JsonNode_Read_Member( const JsonNode_s* node, cstr_t name, T* v, int* count, int capacity )
	{
		const JsonNode_s* member = JsonNode_FindMember( node, name );
		return member 
			? JsonNode_Read_Array( member, v, count, capacity )
			: JsonReadResult_s { JsonRead::NotFound, node, JsonType::Array, name };
	}

}
