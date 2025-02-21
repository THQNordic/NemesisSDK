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
#include <Nemesis/Core/MathTypes.h>

//======================================================================================
namespace nemesis { namespace font
{
	/// Embedded font resources

	struct FontPitch
	{
		enum Enum
		{ Fixed
		, Variable
		};
	};

	struct FontWeight
	{
		enum Enum
		{ Regular
		};
	};

	struct FontUsage
	{
		enum Enum
		{ Default
		, IconSet
		};
	};

	struct FontResource_s
	{
		const wchar_t*	 Family;
		const uint8_t*	 Buffer;
		uint32_t		 Size;
		FontWeight::Enum Weight;
		FontPitch::Enum  Pitch;
		FontUsage::Enum	 Usage;
	};

} }

//======================================================================================
namespace nemesis { namespace font
{
	typedef struct Font_s* Font_t;

	struct GdiFontSetup_s
	{
		wcstr_t			Face;
		int32_t			Height;
		FontUsage::Enum Usage;
	};

	struct StbFontSetup_s
	{
		wcstr_t			Face;
		int32_t			Height;
		FontUsage::Enum Usage;
		const void*		Data;
	};
	
	struct TextMetrics_s
	{
		int Height;
		int LineHeight;
		int MaxCharWidth;
		wchar_t BreakChar;
		wchar_t DefaultChar;
	};

	struct GlyphIndex
	{
		static const uint16_t Invalid = 0xffff;
	};

	struct GlyphInfo_s
	{
		IntRect_s BlackBox;
		int		  Advance;
		int		  Offset;
		uint16_t  Index;
	};

	struct GlyphBuffer_s
	{
		uint8_t* Data;
		int32_t Width;
		int32_t Height;
	};

} }