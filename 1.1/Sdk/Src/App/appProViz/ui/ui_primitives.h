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
namespace nemesis { namespace gui 
{
	void DrawBorder( const Rect_s& r, const Color_c& c1, const Color_c& c2 );
	void DrawBox   ( const Rect_s& r, const Color_c& c1, const Color_c& c2, const Color_c& c3 );
	void DrawBox   ( const Rect_s& r, const Color_c& c, LinearGradient::Mode m );
	void DrawBar   ( const Rect_s& r, const Color_c& c, float border );

	Font_t EnsureFont( float size, FontStyle::Mask style );

} }
