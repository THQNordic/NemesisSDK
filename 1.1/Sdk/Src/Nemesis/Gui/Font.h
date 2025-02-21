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
#include <Nemesis/Gui/Types.h>

//======================================================================================
namespace nemesis { namespace gui
{
	/// Dynamic Font

	typedef struct DynamicFont_s *DynamicFont_t;

	struct CharRange_s
	{
		wchar_t First;
		wchar_t Last;
	};

	struct DynamicFont_s
	{
		Allocator_t			Alloc;
		Renderer_t			Client;
		font::Font_t		Font;
		Texture_t			Texture;
		Vec2_s				WhiteUV;
		int					BorderX;
		int					BorderY;
		Array<CharInfo_s>	Char;
		Array<GlyphInfo_s>	Glyph;
		Array<wchar_t>		Missing;
		Array<font::Font_t> SubstFont;
		Array<CharRange_s>	SubstRange;
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Font Cache

	struct FontCache_s
	{
		Allocator_t Alloc;
		Renderer_t Client;
		BinaryArrayMap< FontKey_s, DynamicFont_t, BitwiseComparer > Map;
	};

	void FontCache_Initialize( FontCache_t cache, Allocator_t alloc, Renderer_t client );
	void FontCache_Shutdown  ( FontCache_t cache );

} }