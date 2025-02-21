//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"

//======================================================================================
namespace nemesis { namespace gui 
{
	static const wchar_t* DEFAULT_FONT_NAME = L"Verdana"; //L"Tahoma";

	//==================================================================================
	// public interface
	void DrawBorder( const Rect_s& r, const Color_c& c1, const Color_c& c2 )
	{
		DrawLine( c1, r.x, r.y, r.x + r.w, r.y );					// top
		DrawLine( c1, r.x, r.y, r.x, r.y + r.h );					// left

		DrawLine( c2, r.x, r.y + r.h, r.x + r.w, r.y + r.h );		// bottom
		DrawLine( c2, r.x + r.w, r.y, r.x + r.w, r.y + r.h );		// right
	}

	void DrawBox( const Rect_s& r, const Color_c& c1, const Color_c& c2, const Color_c& c3 )
	{
		FillRectangle( r, c3 );
		DrawBorder( r, c1, c2 );
	}

	void DrawBox( const Rect_s& r, const Color_c& c, LinearGradient::Mode m )
	{
		FillRectangle( r, Color_Modulate( c, 0.75f ), Color_Modulate( c, 0.5f ), m );
		DrawBorder( r, c, Color_Modulate( c, 0.25f ) );
	}

	void DrawBar( const Rect_s& r, const Color_c& c, float border )
	{
		Rect_s local_rect = r;
		const Color_c c1 = c + 0.25f;
		const Color_c c2 = c * 0.70f;
		const Color_c c3 = c;
		FillRectangle( local_rect, c3 );
		local_rect = Rect_Inflate( local_rect, -border, -border );
		FillRectangle( local_rect, c2, c1, LinearGradient::Vertical );
	}

	Font_t EnsureFont( float size, FontStyle::Mask style )
	{ return EnsureFont( DEFAULT_FONT_NAME, size, style ); }

} }
