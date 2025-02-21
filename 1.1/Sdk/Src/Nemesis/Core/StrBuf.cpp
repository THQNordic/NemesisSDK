//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/StrBuf.h>
#include <Nemesis/Core/String.h>

//======================================================================================
namespace nemesis
{
	static str_t StrBuff_Append( StrBuf_s& buf, int count )
	{
		NeAssert(count > 0);
		const int start = (buf.Count == 0) ? buf.Count : buf.Count-1;
		const int grow  = (buf.Count == 0) ? count+1   : count;
		buf.GrowBy(grow);
		str_t pos = buf.Data + start;
		buf.Data[start + count] = 0;
		NeAssert(buf.Count == start + count + 1);
		return pos;
	}

	cstr_t StrBuf_Text( const StrBuf_s& buf )
	{
		return buf.Data ? buf.Data : "";
	}

	void StrBuf_Print( StrBuf_s& buf, cstr_t text )
	{
		StrBuf_PrintN(buf, text, Str_Len(text));
	}

	void StrBuf_PrintN( StrBuf_s& buf, cstr_t text, int count )
	{
		if (count == 0)
			return;
		str_t pos = StrBuff_Append( buf, count );
		Mem_Cpy( pos, count, text, count );
	}

	void StrBuf_PrintV( StrBuf_s& buf, cstr_t fmt, va_list args )
	{
		const int count = Str_FmtCntV(fmt, args);
		if (count == 0)
			return;
		str_t pos = StrBuff_Append(buf, count);
		const int written = Str_FmtV(pos, count+1, fmt, args);
		NeAssert(written == count);
	}

	void StrBuf_PrintF( StrBuf_s& buf, cstr_t fmt, ... )
	{
		va_list args;
		va_start(args, fmt);
		StrBuf_PrintV(buf, fmt, args);
		va_end(args);
	}

	void StrBuf_PrintLn( StrBuf_s& buf, cstr_t text )
	{
		StrBuf_Print(buf, text);
		StrBuf_Print(buf, "\n");
	}

	void StrBuf_PrintLnV( StrBuf_s& buf, cstr_t fmt, va_list args )
	{
		StrBuf_PrintV(buf, fmt, args);
		StrBuf_Print(buf, "\n");
	}

	void StrBuf_PrintLnF( StrBuf_s& buf, cstr_t fmt, ... )
	{
		va_list args;
		va_start(args, fmt);
		StrBuf_PrintLnV(buf, fmt, args);
		va_end(args);
	}

	void StrBuf_Replace( StrBuf_s& buf, cstr_t find, cstr_t repl )
	{
		if (!find)
			return;
		const int find_len = Str_Len(find);
		const int repl_len = Str_Len(repl);
		cstr_t pos;
		for (;;)
		{
			pos = Str_Str(buf.Data, find);
			if (!pos)
				return;
			const int idx = (int)(pos-buf.Data);
			buf.RemoveAt(idx, find_len);
			buf.InsertAt(idx, repl, repl_len);
			pos = pos + 1 + repl_len - find_len;
		}
	}

}
