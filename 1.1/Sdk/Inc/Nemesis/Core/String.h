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
namespace nemesis
{	
	/// Codepage

	int		NE_API Str_Len		( cstr_t lhs );
	int		NE_API Str_Cmp		( cstr_t lhs, cstr_t rhs );
	int		NE_API Str_Cmp		( cstr_t lhs, cstr_t rhs, size_t len );
	int		NE_API Str_CmpNoCase( cstr_t lhs, cstr_t rhs );
	str_t	NE_API Str_Chr		( str_t s, int ch );
	cstr_t	NE_API Str_Chr		( cstr_t s, int ch );
	str_t	NE_API Str_ChrR		( str_t s, int ch );
	cstr_t	NE_API Str_ChrR		( cstr_t s, int ch );
	str_t	NE_API Str_Str		( str_t text, cstr_t find );
	cstr_t	NE_API Str_Str		( cstr_t text, cstr_t find );
	str_t	NE_API Str_Cpy		( str_t dst, size_t dst_size, cstr_t src );
	str_t	NE_API Str_Cat		( str_t dst, size_t dst_size, cstr_t src );
	int		NE_API Str_FmtV		( str_t out, size_t size, cstr_t fmt, va_list args );
	int		NE_API Str_Fmt		( str_t out, size_t size, cstr_t fmt, ... );
	int		NE_API Str_FmtCntV	( cstr_t fmt, va_list args );

	inline bool Str_Eq		( cstr_t lhs, cstr_t rhs )				{ return Str_Cmp( lhs, rhs )	   == 0; }
	inline bool Str_EqN		( cstr_t lhs, cstr_t rhs, size_t len )	{ return Str_Cmp( lhs, rhs, len )  == 0; }
	inline bool Str_EqNoCase( cstr_t lhs, cstr_t rhs )				{ return Str_CmpNoCase( lhs, rhs ) == 0; }

	template< size_t S > inline str_t Str_Cpy	( char (&dst)[S], cstr_t src )					{ return Str_Cpy( dst, S, src ); }
	template< size_t S > inline str_t Str_Cat	( char (&dst)[S], cstr_t src )					{ return Str_Cat( dst, S, src ); }
	template< size_t S > inline int	  Str_FmtV	( char (&out)[S], cstr_t fmt, va_list args )	{ return Str_FmtV( out, S, fmt, args ); }
	template< size_t S > inline int	  Str_Fmt	( char (&out)[S], cstr_t fmt, ... )			
	{ 
		va_list args;
		va_start( args, fmt );
		const int n = Str_FmtV( out, fmt, args );
		va_end( args );
		return n;
	}
}

//======================================================================================
namespace nemesis
{	
	/// UTF-16

	int		NE_API Wcs_Len( wcstr_t t );
	int		NE_API Wcs_Len( wcstr_t t, int len );
	int		NE_API Wcs_Cmp( wcstr_t lhs, wcstr_t rhs );
	int		NE_API Wcs_Cpy( wstr_t out, size_t count, wcstr_t in );
	int		NE_API Wcs_Fmt( wstr_t out, size_t count, wcstr_t fmt, va_list args );
	int		NE_API Wcs_Fmt( wstr_t out, size_t count, wcstr_t fmt, ... );
	int		NE_API Wcs_Decode( wcstr_t start, wcstr_t end, wchar_t* ch );

	int		NE_API Wcs_ToInt( wcstr_t t );
	float	NE_API Wcs_ToFloat( wcstr_t t );

	template< size_t S > inline int Wcs_Fmt( wchar_t (&out)[S], wcstr_t fmt, va_list args ) { return Wcs_Fmt( out, S, fmt, args ); }
}

//======================================================================================
namespace nemesis
{	
	/// UTF-8

	int NE_API Utf8_Len( cstr_t t );
	int NE_API Utf8_Len( cstr_t t, int len );
	int NE_API Utf8_Decode( cstr_t start, cstr_t end, wchar_t* ch );
}

//======================================================================================
namespace nemesis
{	
	template < typename T > int Utf_Len( const T* t, int l );
	template < typename T > int Utf_Decode( const T* start, const T* end, wchar_t* ch );

	template <>	inline int Utf_Len( cstr_t  t, int l ) { return Utf8_Len( t, l ); }
	template <>	inline int Utf_Len( wcstr_t t, int l ) { return Wcs_Len ( t, l ); }

	template <>	inline int Utf_Decode( cstr_t  start, cstr_t  end, wchar_t* ch ) { return Utf8_Decode( start, end, ch ); }
	template <>	inline int Utf_Decode( wcstr_t start, wcstr_t end, wchar_t* ch ) { return Wcs_Decode ( start, end, ch ); }
}

//======================================================================================
namespace nemesis
{	
	int  NE_API Utf16_Cmp( const wchar_t* lhs, const wchar_t* rhs );
	void NE_API Utf16_Cpy( wchar_t* out, size_t count, const wchar_t* in );

	template< size_t S > inline void Utf16_Cpy( wchar_t (&out)[S], const wchar_t* in ) { return Utf16_Cpy( out, S, in ); }
}

//======================================================================================
namespace nemesis
{	
	size_t NE_API Utf8_Length( cstr_t text );
	size_t NE_API Utf8_Decode( cstr_t start, cstr_t end, uint32_t& codepoint );

	size_t NE_API Utf16_Length( const wchar_t* text );
	size_t NE_API Utf16_Decode( const wchar_t* start, const wchar_t* end, uint32_t& codepoint );
}

//======================================================================================
namespace nemesis
{
	template < typename T >
	struct CharTrait
	{
		static size_t Length( const T* text );
		static size_t Decode( const T* start, const T* end, uint32_t& codepoint );
	};

	template <>
	struct CharTrait< char >
	{
		static size_t Length( cstr_t text )										{ return Utf8_Length( text ); }
		static size_t Decode( cstr_t start, cstr_t end, uint32_t& codepoint )	{ return Utf8_Decode( start, end, codepoint ); }
	};

	template <>
	struct CharTrait< wchar_t >
	{
		static size_t Length( const wchar_t* text )											  { return Utf16_Length( text ); }
		static size_t Decode( const wchar_t* start, const wchar_t* end, uint32_t& codepoint ) { return Utf16_Decode( start, end, codepoint ); }
	};
} 

//======================================================================================
namespace nemesis
{
	/// Comparer for zero-terminated strings.
	struct StringComparer
	{
		static bool Equals( cstr_t lhs, cstr_t rhs )
		{ return 0 == Compare( lhs, rhs ); }

		static int Compare( cstr_t lhs, cstr_t rhs )
		{ return Str_Cmp( lhs, rhs ); }
	};

	/// Case insensitive comparer for zero-terminated strings.
	struct StringNoCaseComparer
	{
		static bool Equals( cstr_t lhs, cstr_t rhs )
		{ return 0 == Compare( lhs, rhs ); }

		static int Compare( cstr_t lhs, cstr_t rhs )
		{ return Str_CmpNoCase( lhs, rhs ); }
	};
}
