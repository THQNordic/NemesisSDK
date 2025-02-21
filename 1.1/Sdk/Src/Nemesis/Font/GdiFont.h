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
#include "Font.h"

//======================================================================================
namespace nemesis { namespace font
{
	struct GdiFont
	{
	public:
		static Handle_t InstallResource  ( cptr_t resource, uint32_t size );
		static Result_t UninstallResource( Handle_t handle );

	public:
		static Result_t CreateInstance( Allocator_t alloc, const GdiFontSetup_s& setup, Font_t* font );

	private:
		static Result_t GetTextMetric( Font_t font, TextMetrics_s& tm );
		static Result_t GetGlyphInfos( Font_t font, wcstr_t t, int num_chars, GlyphInfo_s* infos );
		static Result_t RenderChar	 ( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer );
		static void		Release		 ( Font_t font );

	private:
		static Result_t RenderChar( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer, GlyphInfo_s* info );

	private:
		struct Instance_s;
		static const Font_v Api;
	};

} }