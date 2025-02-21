//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/JsonNode.h>
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Core/Math.h>
#include <stdlib.h>

//======================================================================================
//																				  Public
//======================================================================================
namespace nemesis
{
	JsonNode_s* JsonNode_FindMember( const JsonNode_s* node, cstr_t name )
	{
		if (!name)
			return nullptr;
		for ( JsonNode_s* member = node->Member; member; member = member->Next )
		{
			if (!member->Name)
				continue;
			if (0 == Str_CmpNoCase( member->Name, name ))
				return member;
		}
		return nullptr;
	}

	int JsonNode_ReadCount( const JsonNode_s* node )
	{
		return node->Count;
	}

}

//======================================================================================
namespace nemesis 
{
	JsonReadResult_s JsonNode_ReadCount( const JsonNode_s* node, int* count )
	{
		NeAssert(node && count);
		*count = JsonNode_ReadCount( node );
		return { JsonRead::Ok, node };
	}

	JsonReadResult_s JsonNode_ReadCount( const JsonNode_s* node, JsonType::Enum type, int* count )
	{
		NeAssert(node && count);
		(*count) = 0;
		if (node->Type == JsonType::Array)
		{
			for ( const JsonNode_s* item = node->Member; item; item = item->Next, ++(*count) )
			{
				if (item->Type == type)
					continue;
				return { JsonRead::TypeMismatch, item };
			}
		}

		if (node->Type == type)
		{
			*count = 1;
			return { JsonRead::Ok, node };
		}

		return { JsonRead::TypeMismatch, node, JsonType::Array };
	}
}

//======================================================================================
namespace nemesis 
{
	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, bool* v )
	{
		NeAssert(node && v);
		if (node->Type != JsonType::Bool)
			return { JsonRead::TypeMismatch, node, JsonType::Bool };
		*v = (node->Value[0] == 't');
		return { JsonRead::Ok, node };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, int* v )
	{
		NeAssert(node && v);
		if (node->Type != JsonType::Int)
			return { JsonRead::TypeMismatch, node, JsonType::Int };
		*v = atoi( node->Value );
		return { JsonRead::Ok, node };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, float* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Int:
		case JsonType::Float:
			*v = (float)atof( node->Value );
			break;
		default:
			return { JsonRead::TypeMismatch, node, JsonType::Float };
		}
		return { JsonRead::Ok, node };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, cstr_t* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Null:
			*v = nullptr;
			return { JsonRead::Ok, node };

		case JsonType::String:
			*v = node->Value;
			return { JsonRead::Ok, node };

		default:
			break;
		}

		return { JsonRead::TypeMismatch, node, JsonType::String };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, uint16_t* v )
	{
		NeAssert(node && v);
		if (node->Type != JsonType::Int)
			return { JsonRead::TypeMismatch, node, JsonType::Int };
		const int v_int = atoi( node->Value );
		if (v_int < 0)
			return { JsonRead::Underflow, node, JsonType::Int };
		if (v_int > UINT16_MAX)
			return { JsonRead::Overflow, node, JsonType::Int };
		*v = (uint16_t)v_int;
		return { JsonRead::Ok, node };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, uint32_t* v )
	{
		NeAssert(node && v);
		if (node->Type != JsonType::Int)
			return { JsonRead::TypeMismatch, node, JsonType::Int };
		*v = (uint32_t)atoll( node->Value );
		return { JsonRead::Ok, node };
	}
}

//======================================================================================
namespace nemesis
{
	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Vec2_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				// [ 1.0, 2.0 ]
				int count = 0;
				return JsonNode_Read_Array( node, &v->x, &count, 2 );
			}
			break;
		case JsonType::Object:
			{
				// { "x" : 1.0, "y" : 2.0 }
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
				return { JsonRead::Ok, node };
			}
			break;
		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Vec3_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				// [ 1.0, 2.0, 3.0 ]
				int count = 0;
				return JsonNode_Read_Array( node, &v->x, &count, 3 );
			}
			break;
		case JsonType::Object:
			{
				// { "x" : 1.0, "y" : 2.0, "z" : 3.0 }
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "z", &v->z ); if (result.Result != JsonRead::Ok) return result;
				return { JsonRead::Ok, node };
			}
			break;
		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Vec4_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				// [ 1.0, 2.0, 3.0, 4.0f ]
				int count = 0;
				return JsonNode_Read_Array( node, &v->x, &count, 4 );
			}
			break;
		case JsonType::Object:
			{
				// { "x" : 1.0, "y" : 2.0, "z" : 3.0, "w": 4.0 }
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "z", &v->z ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "w", &v->w ); if (result.Result != JsonRead::Ok) return result;
				return { JsonRead::Ok, node };
			}
			break;
		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Pos3_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				// [ 1.0, 2.0, 3.0 ]
				int count = 0;
				return JsonNode_Read_Array( node, &v->x, &count, 3 );
			}
			break;
		case JsonType::Object:
			{
				// { "x" : 1.0, "y" : 2.0, "z" : 3.0 }
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "z", &v->z ); if (result.Result != JsonRead::Ok) return result;
				return { JsonRead::Ok, node };
			}
			break;
		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Quat_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				switch ( node->Count )
				{
					// [ 1.0, 2.0, 3.0, 4.0 ]
				case 4:
					return JsonNode_Read_Array( node, &v->x, 4 );

					// [ 1.0, 2.0, 3.0 ]
				case 3:
					{
						Pos3_s euler = {};
						JsonReadResult_s result;
						result = JsonNode_Read_Array( node, &euler.x, 3 );
						if (result.Result != JsonRead::Ok)
							return result;
						*v = Quat_FromEulerAngles_Deg( euler.x, euler.y, euler.z );
						return { JsonRead::Ok, node };
					}
					break;
				}
			}
			break;
		case JsonType::Object:
			{
				if (JsonNode_FindMember( node, "w" ))
				{
					// { "x" : 1.0, "y" : 2.0, "z" : 3.0, "w": 4.0 }
					JsonReadResult_s result;
					result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "z", &v->z ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "w", &v->w ); if (result.Result != JsonRead::Ok) return result;
					return { JsonRead::Ok, node };
				}
				if (JsonNode_FindMember( node, "a" ))
				{
					// { "x" : 1.0, "y" : 2.0, "z" : 3.0, "a": 4.0 }
					float x,y,z,a;

					JsonReadResult_s result;
					result = JsonNode_Read_Member( node, "x", &x ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "y", &y ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "z", &z ); if (result.Result != JsonRead::Ok) return result;
					result = JsonNode_Read_Member( node, "a", &a ); if (result.Result != JsonRead::Ok) return result;
					*v = Quat_FromAxisAngle( Pos3_s { x,y,z }, a );
					return { JsonRead::Ok, node };
				}

				// euler angle
				float x,y,z;
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &y ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "z", &z ); if (result.Result != JsonRead::Ok) return result;
				*v = Quat_FromEulerAngles_Deg( x, y, z );
				return { JsonRead::Ok, node };
			}
			break;
		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}

	JsonReadResult_s JsonNode_Read( const JsonNode_s* node, Rect_s* v )
	{
		NeAssert(node && v);
		switch (node->Type)
		{
		case JsonType::Array:
			{
				// [ 10.0, 10.0, 30.0, 50.0 ]
				int count = 0;
				return JsonNode_Read_Array( node, &v->x, &count, 4 );
			}
			break;

		case JsonType::Object:
			{
				// { "x" : 1.0, "y" : 2.0, "z" : 3.0 }
				JsonReadResult_s result;
				result = JsonNode_Read_Member( node, "x", &v->x ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "y", &v->y ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "w", &v->w ); if (result.Result != JsonRead::Ok) return result;
				result = JsonNode_Read_Member( node, "h", &v->h ); if (result.Result != JsonRead::Ok) return result;
				return { JsonRead::Ok, node };
			}
			break;

		case JsonType::String:
			{
				cstr_t pos = node->Value;
				float* dst = &v->x;
				for ( int i = 0; pos && (i < 4); ++i, ++pos )
				{
					*dst++ = (float)atoi(pos);
					pos = Str_Chr( pos, ';' );
				}
			}
			break;

		default:
			break;
		}
		return { JsonRead::TypeMismatch, node, JsonType::Object };
	}
}
