//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/JsonOut.h>

//======================================================================================
#include <math.h>

//======================================================================================
//																				 Private
//======================================================================================
namespace nemesis
{
	static void JsonOut_PopScope( JsonOut_s& json )
	{
		json.Scope = json.Stack[json.Stack.Count-1];
		json.Stack.Count--;
	}

	static void JsonOut_Write_Comma( JsonOut_s& json )
	{
		StrBuf_Print( json.Text, "," );
	}

	static void JsonOut_Write_Space( JsonOut_s& json )
	{
		StrBuf_Print( json.Text, " " );
	}

	static void JsonOut_Flush_Comment( JsonOut_s& json )
	{
		if (!json.Comment.Count)
			return;
		StrBuf_Print( json.Text, json.Tab ? json.Tab : "\t" );
		StrBuf_PrintF( json.Text, "// %s", json.Comment.Data );
		json.Comment.Count = 0;
	}

	static void JsonOut_Write_Endline( JsonOut_s& json )
	{
		if (!json.Comment.Count)
			return;
		JsonOut_Write_Space( json );
	}

	static void JsonOut_Write_Newline( JsonOut_s& json )
	{
		JsonOut_Flush_Comment( json );
		StrBuf_Print( json.Text, "\n" );
	}

	static void JsonOut_Write_Indent( JsonOut_s& json )
	{
		cstr_t tab = json.Tab ? json.Tab : "\t";
		for ( int i = 0; i < json.Indent; ++i )
			StrBuf_Print( json.Text, tab );
	}

	static void JsonOut_Delimit( JsonOut_s& json )
	{
		switch ( json.Item )
		{
		case Json::Array:
		case Json::Object:
			break;
		default:
			JsonOut_Write_Comma( json );
			break;
		}
	}

	static void JsonOut_Prefix( JsonOut_s& json )
	{
		JsonOut_Delimit( json );
		JsonOut_Write_Newline( json );
		JsonOut_Write_Indent( json );
	}

	static void JsonOut_Indent( JsonOut_s& json )
	{
		if (json.Scope == Json::Root)
			return;
		if (json.Item == Json::Field)
			return;
		JsonOut_Prefix( json );
	}
} 

//======================================================================================
//																				  Public
//======================================================================================
namespace nemesis
{
	void JsonOut_Initialize( JsonOut_s& json, Allocator_t alloc, int32_t text_size, int32_t page_size, int32_t comment_size )
	{
		json.Text.Init( alloc );
		json.Stack.Init( alloc );
		json.Comment.Init( alloc );
		json.Text.Reserve( text_size );
		json.Stack.Reserve( page_size );
		json.Comment.Reserve( comment_size );
	}

	void JsonOut_Initialize( JsonOut_s& json, Allocator_t alloc )
	{
		JsonOut_Initialize( json, alloc, 1024*1024, 256, 1024 );
	}

	void JsonOut_Shutdown( JsonOut_s& json )
	{
		json.Text.Clear();
		json.Stack.Clear();
		json.Comment.Clear();
	}

	void JsonOut_Reset( JsonOut_s& json )
	{
		json.Text.Reset();
		json.Stack.Reset();
		json.Comment.Reset();
	}

	void JsonOut_Comment( JsonOut_s& json, cstr_t text )
	{
		json.Comment.Reset();
		if (!text)
			return;
		StrBuf_Print( json.Comment, text );
	}

	void JsonOut_Null( JsonOut_s& json )
	{
		JsonOut_Indent( json );
		StrBuf_Print( json.Text, "null" );
		json.Item = Json::Value;
	}

	void JsonOut_Value( JsonOut_s& json, bool v )
	{
		JsonOut_Indent( json );
		StrBuf_PrintF( json.Text, "%s", v ? "true" : "false" );
		json.Item = Json::Value;
	}

	void JsonOut_Value( JsonOut_s& json, int32_t v )
	{
		JsonOut_Indent( json );
		StrBuf_PrintF( json.Text, "%d", v );
		json.Item = Json::Value;
	}

	void JsonOut_Value( JsonOut_s& json, uint32_t v )
	{
		JsonOut_Indent( json );
		StrBuf_PrintF( json.Text, "%u", v );
		json.Item = Json::Value;
	}

	void JsonOut_Value( JsonOut_s& json, float v )
	{
		JsonOut_Indent( json );
		if (v == floorf(v))
			StrBuf_PrintF( json.Text, "%d.0", (int32_t)v );
		else
			StrBuf_PrintF( json.Text, "%g", v );
		json.Item = Json::Value;
	}

	void JsonOut_Value( JsonOut_s& json, cstr_t v )
	{
		JsonOut_Indent( json );
		if (v)
			StrBuf_PrintF( json.Text, "\"%s\"", v );
		else
			StrBuf_Print( json.Text, "null" );
		json.Item = Json::Value;
	}

	void JsonOut_Field( JsonOut_s& json, cstr_t name )
	{
		JsonOut_Prefix( json );
		StrBuf_PrintF( json.Text, "\"%s\": ", name );
		json.Item = Json::Field;
	}

	void JsonOut_Object( JsonOut_s& json )
	{
		JsonOut_Indent( json );
		StrBuf_Print( json.Text, "{" );
		json.Stack.Append( json.Scope );
		json.Scope = Json::Object;
		json.Item = Json::Object;
		json.Indent++;
	}

	void JsonOut_Array( JsonOut_s& json )
	{
		JsonOut_Indent( json );
		StrBuf_Print( json.Text, "[" );
		json.Stack.Append( json.Scope );
		json.Scope = Json::Array;
		json.Item = Json::Array;
		json.Indent++;
	}

	void JsonOut_End( JsonOut_s& json )
	{
		NeAssert(json.Stack.Count);
		JsonOut_Write_Endline( json );
		JsonOut_Write_Newline( json );
		json.Indent--;
		JsonOut_Write_Indent( json );
		const char token = (json.Scope == Json::Object) ? '}' : ']';
		StrBuf_PrintF( json.Text, "%c", token );
		JsonOut_PopScope( json );
		json.Item = Json::Root;
	}

	void JsonOut_Array( JsonOut_s& json, cstr_t name )
	{
		JsonOut_Field( json, name );
		JsonOut_Array( json );
	}

	void JsonOut_Object( JsonOut_s& json, cstr_t name )
	{
		JsonOut_Field( json, name );
		JsonOut_Object( json );
	}
}
