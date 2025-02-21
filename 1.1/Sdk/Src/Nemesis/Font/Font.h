//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once
#include <Nemesis/Font/System.h>

//======================================================================================
namespace nemesis { namespace font
{
	struct Font_v
	{
		Result_t (NE_CALLBK *GetTextMetric)	( Font_t font, TextMetrics_s& tm );
		Result_t (NE_CALLBK *GetGlyphInfos)	( Font_t font, const wchar_t* t, int num_chars, GlyphInfo_s* infos );
		Result_t (NE_CALLBK *RenderChar)	( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer );
		void	 (NE_CALLBK *Release)		( Font_t font );
	};

	struct Font_s
	{
		const Font_v* Api;
	};
} } 
