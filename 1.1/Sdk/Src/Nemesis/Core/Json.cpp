//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Json.h>

//======================================================================================
namespace nemesis
{
	struct JsonPart
	{
		enum Enum
		{ Name
		, Name_End
		, Value
		, Value_End
		};
	};

	struct JsonName_s
	{
		cstr_t Pos;
		cstr_t End;
	};

	struct JsonStream_s
	{
		JsonName_s Caret;
		Result_t   Result;
	};

	static bool JsonStream_Continue( JsonStream_s& s )
	{
		if (NeSucceeded(s.Result))
		{
			if (s.Caret.Pos >= s.Caret.End)
			{
				s.Caret.Pos = s.Caret.End;
				s.Result = NE_ERR_JSON_END_OF_FILE;
			}
			else if (!s.Caret.Pos[0])
			{
				s.Result = NE_ERR_JSON_END_OF_STRING;
			}
		}
		return NeSucceeded(s.Result);
	}

	static void JsonStream_ParseString( JsonStream_s& s, JsonName_s& value )
	{
		cstr_t it = s.Caret.Pos+1;
		for ( ;; ++it )
		{
			if (it == s.Caret.End)
			{
				s.Caret.Pos = it;
				s.Result = NE_ERR_JSON_END_OF_FILE;
				return;
			}
			if (*it == '\0')
			{
				s.Caret.Pos = it;
				s.Result = NE_ERR_JSON_END_OF_STRING;
				return;
			}
			if (*it == '"')
			{
				value.Pos = s.Caret.Pos+1;
				value.End = it;
				s.Caret.Pos = it+1;
				return;
			}
		}
	}

	static void JsonStream_ParseConstant( JsonStream_s& s, JsonName_s& value, JsonType::Enum& type )
	{
		cstr_t it = s.Caret.Pos;
		switch (it[0])
		{
		case 't':
			if (s.Caret.End - s.Caret.Pos >= 4)
			{
				if ((it[1] == 'r') && (it[2] == 'u') && (it[3] == 'e'))
				{
					type = JsonType::Bool;
					value.Pos = it;
					value.End = it + 4;
					s.Caret.Pos += 4;
					return;
				}
				s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
				return;
			}
			s.Result = NE_ERR_JSON_END_OF_FILE;
			break;

		case 'f':
			if (s.Caret.End - s.Caret.Pos >= 5)
			{
				if ((it[1] == 'a') && (it[2] == 'l') && (it[3] == 's') && (it[4] == 'e'))
				{
					type = JsonType::Bool;
					value.Pos = it;
					value.End = it + 5;
					s.Caret.Pos += 5;
					return;
				}
				s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
				return;
			}
			s.Result = NE_ERR_JSON_END_OF_FILE;
			break;

		case 'n':
			if (s.Caret.End - s.Caret.Pos >= 4)
			{
				if ((it[1] == 'u') && (it[2] == 'l') && (it[3] == 'l'))
				{
					type = JsonType::Null;
					value.Pos = it;
					value.End = it + 4;
					s.Caret.Pos += 4;
					return;
				}
				s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
				return;
			}
			s.Result = NE_ERR_JSON_END_OF_FILE;
			break;
		}
	}

	static const bool Char_IsDigit( char c )
	{
		return ((c >= '0') && (c <= '9'));
	}

	static void JsonStream_ParseNumber( JsonStream_s& s, JsonName_s& value, JsonType::Enum& type )
	{
		enum NUMBER_PART
		{
			INTEGER
		,	FRACTION
		,	EXPONENT
		};

		NUMBER_PART part = INTEGER;

		cstr_t it = s.Caret.Pos;
		for ( ;; ++it )
		{
			// eof
			if (it == s.Caret.End)
			{
				s.Result = NE_ERR_JSON_END_OF_FILE;
				s.Caret.Pos = it;
				return;
			}

			// tokens
			switch (*it)
			{
			case '-':
				if ((part == INTEGER) && (it != s.Caret.Pos))
				{
					s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
					s.Caret.Pos = it;
					return;
				}
				else if (part == FRACTION)
				{
					s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
					s.Caret.Pos = it;
					return;
				}
				else if (part == EXPONENT)
				{
					if ((it[-1] != 'E') && (it[-1] != 'e'))
					{
						s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
						s.Caret.Pos = it;
						return;
					}
				}
				break;

			case '.':
				{
					if (part != INTEGER)
					{
						s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
						s.Caret.Pos = it;
						return;
					}
				
					part = FRACTION;
				}
				break;

			case 'e':
			case 'E':
				{
					if (part == INTEGER)
					{
						type = JsonType::Float;
						part = FRACTION;
					}

					if (part != FRACTION)
					{
						s.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
						s.Caret.Pos = it;
						return;
					}
					part = EXPONENT;
				}
				break;

			default:
				if (!Char_IsDigit(*it))
				{
					type = (part == INTEGER) ? JsonType::Int : JsonType::Float;
					value.Pos = s.Caret.Pos;
					value.End = it;
					s.Caret.Pos = it;
					return;
				}
				break;
			}
		}
	}

	struct JsonItem_s
	{
		JsonType::Enum	Type;
		JsonPart::Enum	Part;
		JsonName_s		Name;
	};

	struct JsonStack_s
	{
		int Count;
		JsonItem_s Item[64];
	};

	static JsonItem_s* JsonStack_Push( JsonStack_s& stack )
	{
		if (stack.Count >= NeCountOf(stack.Item))
			return nullptr;
		return stack.Item + (stack.Count++);
	}

	static JsonItem_s* JsonStack_Pop( JsonStack_s& stack )
	{
		if (stack.Count < 1)
			return nullptr;
		--stack.Count;
		if (stack.Count < 1)
			return nullptr;
		return stack.Item + (stack.Count-1);
	}

	struct JsonState_s
	{
		JsonStream_s Stream;
		JsonItem_s*  Top;
		JsonItem_s*	 Root;
		JsonName_s	 Name;
		int			 Line;
		cstr_t		 Begin;
		cstr_t		 Start;
		JsonStack_s	 Stack;
		JsonClient_s Client;
	};

	static void JsonState_Initialize( JsonState_s& s, cstr_t src, uint32_t len, const JsonClient_s& client )
	{
		s.Stream.Caret.Pos = src;
		s.Stream.Caret.End = src + len;
		s.Stream.Result = NE_OK;
		s.Line = 1;
		s.Begin = src;
		s.Start = src;
		s.Client = client;
	}

	static void JsonState_Parse_Begin		( JsonState_s& s );
	static void JsonState_Parse_End			( JsonState_s& s );
	static void JsonState_Parse_Field		( JsonState_s& s );
	static void JsonState_Parse_Separator	( JsonState_s& s );
	static void JsonState_Parse_String		( JsonState_s& s );
	static void JsonState_Parse_Constant	( JsonState_s& s );
	static void JsonState_Parse_Number		( JsonState_s& s );
	static void JsonState_Parse_NewLine		( JsonState_s& s );
	static void JsonState_Parse_Whitespace	( JsonState_s& s );
	static void JsonState_Parse_Comment		( JsonState_s& s );

	struct JsonParserItem_s
	{
		char Char;
		void (*Handler)( JsonState_s& s );
	};

	static void JsonState_Parse( JsonState_s& s )
	{
		const JsonParserItem_s item[] =
		{ { '{'	, JsonState_Parse_Begin		 }
		, { '['	, JsonState_Parse_Begin		 }
		, { '}'	, JsonState_Parse_End		 }
		, { ']'	, JsonState_Parse_End		 }
		, { ':'	, JsonState_Parse_Field		 }
		, { ','	, JsonState_Parse_Separator	 }
		, { '"'	, JsonState_Parse_String	 }
		, { 't'	, JsonState_Parse_Constant	 }
		, { 'f'	, JsonState_Parse_Constant	 }
		, { 'n'	, JsonState_Parse_Constant	 }
		, { '-'	, JsonState_Parse_Number	 }
		, { '0'	, JsonState_Parse_Number	 }
		, { '1'	, JsonState_Parse_Number	 }
		, { '2'	, JsonState_Parse_Number	 }
		, { '3'	, JsonState_Parse_Number	 }
		, { '4'	, JsonState_Parse_Number	 }
		, { '5'	, JsonState_Parse_Number	 }
		, { '6'	, JsonState_Parse_Number	 }
		, { '7'	, JsonState_Parse_Number	 }
		, { '8'	, JsonState_Parse_Number	 }
		, { '9'	, JsonState_Parse_Number	 }
		, { '\n', JsonState_Parse_NewLine	 }
		, { ' '	, JsonState_Parse_Whitespace }
		, { '\t', JsonState_Parse_Whitespace }
		, { '\r', JsonState_Parse_Whitespace }
		, { '/' , JsonState_Parse_Comment	 }
		};

		for ( ; JsonStream_Continue( s.Stream ); )
		{
			bool handled = false;
			for ( int i = 0; i < NeCountOf(item); ++i )
			{
				if (s.Stream.Caret.Pos[0] == item[i].Char)
				{
					item[i].Handler( s );
					handled = true;
					break;
				}
			}
			if (!handled)
				s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
		}
	}

	static void JsonState_Parse_Begin( JsonState_s& s )
	{
		// determine type
		const JsonType::Enum type = ((*s.Stream.Caret.Pos) == '{') ? JsonType::Object : JsonType::Array;

		// handle syntax errors
		if (!s.Top)
		{
			if (!s.Root)
			{
				if (type != JsonType::Object)
				{
					s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
					return;
				}
			}
			else
			{
				s.Stream.Result = NE_ERR_JSON_MULTIPLE_ROOT_OBJECTS;
				return;
			}
		}

		// push item
		JsonItem_s* item = JsonStack_Push( s.Stack );
		if (item)
		{
			item->Type = type;
			item->Name = s.Name;
		}
		else
		{
			s.Stream.Result = NE_ERR_JSON_OUT_OF_MEMORY;
			return;
		}

		// notify 
		s.Client.HandleEvent( s.Client.Context, (uint32_t)(s.Stream.Caret.Pos-s.Begin), JsonEvent::Begin, type, item->Name.Pos, (uint32_t)(item->Name.End-item->Name.Pos), 0, 0, s.Line );

		// next, look for either a name (object) or another value (array)
		item->Part = (item->Type == JsonType::Object) ? JsonPart::Name : JsonPart::Value;

		// handle top and root
		s.Top = item;
		if (!s.Root)
			s.Root = item;

		// update state
		s.Name.Pos = s.Name.End = nullptr;

		// skip
		++s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_End( JsonState_s& s )
	{
		// determine type
		JsonType::Enum type = ((*s.Stream.Caret.Pos) == '}') ? JsonType::Object : JsonType::Array;

		// handle brace mismatch
		if (!s.Top || s.Top->Type != type)
		{
			s.Stream.Result = NE_ERR_JSON_BRACE_MISMATCH;
			return;
		}

		// handle part mismatch
		if ((s.Top->Type == JsonType::Object) && (s.Top->Part == JsonPart::Value))
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		// notify
		s.Client.HandleEvent( s.Client.Context, (uint32_t)(s.Stream.Caret.Pos-s.Begin), JsonEvent::End, type, s.Top->Name.Pos, (uint32_t)(s.Top->Name.End-s.Top->Name.Pos), 0, 0, s.Line );

		// pop
		s.Top = JsonStack_Pop( s.Stack );
		s.Name.Pos = s.Name.End = nullptr;

		// look for terminator next
		if (s.Top)
			s.Top->Part = JsonPart::Value_End;

		// skip
		++s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_Field( JsonState_s& s )
	{
		if (!s.Top || (s.Top->Type != JsonType::Object))
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		if (s.Top->Part != JsonPart::Name_End)
		{
			s.Stream.Result = NE_ERR_JSON_MISSING_DELIMITER;
			return;
		}

		// expect value next
		s.Top->Part = JsonPart::Value;
		++s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_Separator( JsonState_s& s )
	{
		// must have parsed a value
		if (!s.Top || s.Top->Part != JsonPart::Value_End)
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		// expect either a name (object) or another value (array) next
		s.Top->Part = (s.Top->Type == JsonType::Object) ? JsonPart::Name : JsonPart::Value;
		++s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_String( JsonState_s& s )
	{
		if (!s.Top)
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		if ((s.Top->Part == JsonPart::Name_End) || (s.Top->Part == JsonPart::Value_End))
		{
			s.Stream.Result = NE_ERR_JSON_MISSING_DELIMITER;
			return;
		}

		JsonName_s value = {};
		JsonStream_ParseString( s.Stream, value );
		if (NeFailed(s.Stream.Result))
			return;

		switch (s.Top->Part)
		{
		case JsonPart::Name:
			s.Top->Part = JsonPart::Name_End;
			s.Name = value;
			break;

		case JsonPart::Value:
			s.Top->Part = JsonPart::Value_End;
			s.Client.HandleEvent( s.Client.Context, (uint32_t)(s.Stream.Caret.Pos-s.Begin), JsonEvent::Value, JsonType::String, s.Name.Pos, (uint32_t)(s.Name.End-s.Name.Pos), value.Pos, (uint32_t)(value.End-value.Pos), s.Line );
			break;

		default:
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			break;
		}
	}

	static void JsonState_Parse_Constant( JsonState_s& s )
	{
		if (!s.Top)
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		if (s.Top->Part != JsonPart::Value)
		{
			s.Stream.Result = NE_ERR_JSON_MISSING_DELIMITER;
			return;
		}

		JsonName_s value = {};
		JsonType::Enum type = JsonType::String;
		JsonStream_ParseConstant( s.Stream, value, type );
		if (NeFailed(s.Stream.Result))
			return;

		s.Top->Part = JsonPart::Value_End;
		s.Client.HandleEvent( s.Client.Context, (uint32_t)(s.Stream.Caret.Pos-s.Begin), JsonEvent::Value, type, s.Name.Pos, (uint32_t)(s.Name.End-s.Name.Pos), value.Pos, (uint32_t)(value.End-value.Pos), s.Line );
	}

	static void JsonState_Parse_Number( JsonState_s& s )
	{
		if (!s.Top || s.Top->Part != JsonPart::Value)
		{
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}

		JsonName_s value = {};
		JsonType::Enum type = JsonType::Int;
		JsonStream_ParseNumber( s.Stream, value, type );
		if (NeFailed(s.Stream.Result))
			return;

		s.Top->Part = JsonPart::Value_End;
		s.Client.HandleEvent( s.Client.Context, (uint32_t)(s.Stream.Caret.Pos-s.Begin), JsonEvent::Value, type, s.Name.Pos, (uint32_t)(s.Name.End-s.Name.Pos), value.Pos, (uint32_t)(value.End-value.Pos), s.Line );
	}

	static void JsonState_Parse_NewLine( JsonState_s& s )
	{
		++s.Stream.Caret.Pos;
		++s.Line;
		s.Start = s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_Whitespace( JsonState_s& s )
	{
		++s.Stream.Caret.Pos;
	}

	static void JsonState_Parse_Comment( JsonState_s& s )
	{
		switch ( s.Stream.Caret.Pos[1] )
		{
		case '/':
			for ( ++s.Stream.Caret.Pos; s.Stream.Caret.Pos < s.Stream.Caret.End; ++s.Stream.Caret.Pos )
			{
				if (s.Stream.Caret.Pos[0] != '\n')
					continue;
				break;
			}
			break;
		case '*':
			for ( ++s.Stream.Caret.Pos; s.Stream.Caret.Pos < (s.Stream.Caret.End-1); ++s.Stream.Caret.Pos )
			{
				if (s.Stream.Caret.Pos[0] != '*')
					continue;
				if (s.Stream.Caret.Pos[1] != '/')
					continue;
				s.Stream.Caret.Pos += 2;
				break;
			}
			break;
		default:
			s.Stream.Result = NE_ERR_JSON_UNEXPECTED_CHAR;
			return;
		}
	}

	static cstr_t JsonResult_ToString( Result_t hr )
	{
		switch (hr)
		{
		case NE_OK								: return "Success";
		case NE_ERR_JSON_OUT_OF_MEMORY			: return "Out of memory";
		case NE_ERR_JSON_BRACE_MISMATCH			: return "Brace mismatch";
		case NE_ERR_JSON_MISSING_DELIMITER		: return "Missing delimiter";
		case NE_ERR_JSON_MULTIPLE_ROOT_OBJECTS	: return "Multiple root objects";
		case NE_ERR_JSON_END_OF_STRING			: return "End of string";
		case NE_ERR_JSON_END_OF_FILE			: return "End of file";
		case NE_ERR_JSON_UNEXPECTED_CHAR		: return "Unexpected character";
		default:
			break;
		}
		return "Internal error";
	}
}

//======================================================================================
namespace nemesis
{
	/// Json

	Result_t Json_Parse( cstr_t src, uint32_t len, const JsonClient_s& client, JsonError_s* error )
	{
		if (!src || !len || !client.HandleEvent)
			return NE_ERR_INVALID_ARG;

		JsonState_s state = {};
		JsonState_Initialize( state, src, len, client );
		JsonState_Parse( state );

		switch ( state.Stream.Result )
		{
		case NE_ERR_JSON_END_OF_FILE:
		case NE_ERR_JSON_END_OF_STRING:
			if (state.Root && !state.Top)
				state.Stream.Result = NE_OK;
			break;
		}

		if (error)
		{
			if (NeSucceeded(state.Stream.Result))
			{
				error->Line = 0;
				error->Pos = nullptr;
			}
			else
			{
				error->Line = state.Line;
				error->Pos = state.Stream.Caret.Pos;
			}
			error->Msg = JsonResult_ToString(state.Stream.Result);
		}

		return state.Stream.Result;
	}

}
