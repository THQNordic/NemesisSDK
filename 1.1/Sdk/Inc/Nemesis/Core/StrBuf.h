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
#include "Array.h"

//======================================================================================
namespace nemesis
{
	typedef Array<char> StrBuf_s;

	cstr_t	StrBuf_Text		( const StrBuf_s& buf );
	void	StrBuf_Print	( StrBuf_s& buf, cstr_t text );
	void	StrBuf_PrintN	( StrBuf_s& buf, cstr_t text, int count );
	void	StrBuf_PrintV	( StrBuf_s& buf, cstr_t fmt, va_list args );
	void	StrBuf_PrintF	( StrBuf_s& buf, cstr_t fmt, ... );
	void	StrBuf_PrintLn	( StrBuf_s& buf, cstr_t text );
	void	StrBuf_PrintLnV	( StrBuf_s& buf, cstr_t fmt, va_list args );
	void	StrBuf_PrintLnF	( StrBuf_s& buf, cstr_t fmt, ... );
	void	StrBuf_Replace	( StrBuf_s& buf, cstr_t find, cstr_t repl );

}
