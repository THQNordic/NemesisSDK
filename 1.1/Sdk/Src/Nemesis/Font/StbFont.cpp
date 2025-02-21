//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include <stdafx.h>
#include "StbFont.h"

//======================================================================================
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/Logging.h>
#include <Nemesis/Core/Memory.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Core/New.h>

//======================================================================================
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_memcpy       memcpy
#define STBTT_memset       memset
#include "stb/stb_truetype.h"

//======================================================================================
namespace nemesis { namespace font
{
	namespace
	{
		struct stbtt_textmetrics
		{
			int Ascent;
			int Descent;
			int LineGap;
			int X0, Y0;
			int X1, Y1;
		};

		static void stbtt_GetTextMetrics( stbtt_fontinfo* info, stbtt_textmetrics* tm )
		{
			stbtt_GetFontVMetrics( info, &tm->Ascent, &tm->Descent, &tm->LineGap );
			stbtt_GetFontBoundingBox( info, &tm->X0, &tm->Y0, &tm->X1, &tm->Y1 );
		}
	}

	static const float fugde_em_scale_x = 80.0f/72.0f;
	static const float fugde_em_scale_y = 1.0f;

} }

//======================================================================================
namespace nemesis { namespace font
{
	const Font_v StbFont::Api = 
	{ &GetTextMetric
	, &GetGlyphInfos
	, &RenderChar
	, &Release
	};

	struct StbFontData
	{
		const void*			Data;
		stbtt_fontinfo		Info;
		Vec2_s				EmToPixel;
		stbtt_textmetrics	TextMetricsEm;
		stbtt_textmetrics	TextMetricsPx;
		wchar_t				BreakChar;
		wchar_t				DefaultChar;
	};

	struct StbFont::Instance_s
	{ 
		Font_s Header;
		Allocator_t	Alloc;
		StbFontData Data;
	};

	Result_t StbFont::CreateInstance( Allocator_t alloc, const StbFontSetup_s& setup, Font_t* font )
	{
		if (!font)
			return NE_ERR_INVALID_ARG;

		*font = nullptr;

		if (!setup.Data)
			return NE_ERR_INVALID_ARG;

		stbtt_fontinfo stb_font_info = {};
		if (!stbtt_InitFont( &stb_font_info, (const uint8_t*)setup.Data, 0 ))
			return NE_ERR_INVALID_ARG;

		stbtt_textmetrics stb_text_metrics_em = {};
		stbtt_GetTextMetrics( &stb_font_info, &stb_text_metrics_em );

		const float em_to_pixel_y = (setup.Height > 0)
			? stbtt_ScaleForPixelHeight		 ( &stb_font_info,  1.0f * setup.Height )
			: stbtt_ScaleForMappingEmToPixels( &stb_font_info, -1.0f * setup.Height )
			;

		const float aspect = 10.0f/9.0f;

		const Vec2_s em_to_pixel =
		{ em_to_pixel_y * aspect
		, em_to_pixel_y
		};

		stbtt_textmetrics stb_text_metrics_px =
		{ (int)(em_to_pixel.y * stb_text_metrics_em.Ascent	)
		, (int)(em_to_pixel.y * stb_text_metrics_em.Descent	)
		, (int)(em_to_pixel.y * stb_text_metrics_em.LineGap	)
		, (int)(em_to_pixel.x * stb_text_metrics_em.X0		)
		, (int)(em_to_pixel.y * stb_text_metrics_em.Y0		)
		, (int)(em_to_pixel.x * stb_text_metrics_em.X1		)
		, (int)(em_to_pixel.y * stb_text_metrics_em.Y1		)
		};

		wchar_t default_char = 1;
		for ( default_char = 1; default_char <= 0xff; ++default_char )
		{
			if ( 0 == stbtt_FindGlyphIndex( &stb_font_info, default_char ) )
				break;
		}

		Instance_s* instance = NE_ALLOC_NEW( alloc, Instance_s );
		instance->Header.Api = &Api;
		instance->Alloc = alloc;
		instance->Data.Data				= setup.Data;
		instance->Data.Info				= stb_font_info;
		instance->Data.EmToPixel		= em_to_pixel;
		instance->Data.TextMetricsEm	= stb_text_metrics_em;
		instance->Data.TextMetricsPx	= stb_text_metrics_px;
		instance->Data.DefaultChar		= default_char;
		instance->Data.BreakChar		= 0;
		*font = &instance->Header;
		return NE_OK;
	}


	Result_t StbFont::GetTextMetric( Font_t font, TextMetrics_s& tm )
	{
		Instance_s* instance = (Instance_s*)font;
		const StbFontData& data = instance->Data;
		const stbtt_textmetrics& tm_px = data.TextMetricsPx;
		tm.Height		= tm_px.Y1 - tm_px.Y0;
		tm.Height		= tm_px.Ascent - tm_px.Descent;
		tm.LineHeight	= tm.Height + tm_px.LineGap;
		tm.MaxCharWidth = tm_px.X1 - tm_px.X0;
		tm.BreakChar	= data.BreakChar;
		tm.DefaultChar	= data.DefaultChar;
		return NE_OK;
	}

	Result_t StbFont::GetGlyphInfos( Font_t font, wcstr_t t, int num_chars, GlyphInfo_s* infos )
	{
		Instance_s* instance = (Instance_s*)font;
		const int len = Wcs_Len( t, num_chars );

		const float sx = instance->Data.EmToPixel.x;
		const float sy = instance->Data.EmToPixel.y;

		stbtt_textmetrics tms = instance->Data.TextMetricsPx;

		int glyph	= 0;
		int advance = 0;
		int offset	= 0;
		int x0 = 0;
		int y0 = 0;
		int x1 = 0;
		int y1 = 0;
		for ( int i = 0; i < len; ++i )
		{
			GlyphInfo_s& gi = infos[i];

			glyph = stbtt_FindGlyphIndex( &instance->Data.Info, t[i] );
			if (glyph <= 0)
			{
				if (t[i] != instance->Data.DefaultChar)
				{
					NeZero( gi );
					gi.Index = GlyphIndex::Invalid;
					continue;
				}
			}

			stbtt_GetGlyphHMetrics( &instance->Data.Info, glyph, &advance, &offset );
			stbtt_GetGlyphBitmapBox( &instance->Data.Info, glyph, sx, sy, &x0, &y0, &x1, &y1 );

			// @note: em coords are baseline-relative
			// @note: line gap seems to apply to the baseline, not the descent
			gi.BlackBox.x = x0;
			gi.BlackBox.y = y0 + tms.Ascent + tms.LineGap;	
			gi.BlackBox.w = x1-x0;
			gi.BlackBox.h = y1-y0;
			gi.Advance = (int)(sx * advance);
			gi.Offset = (int)(sx * offset);
			gi.Index = (uint16_t)glyph;
		}

		return NE_OK;
	}

	Result_t StbFont::RenderChar( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer )
	{
		Instance_s* instance = (Instance_s*)font;
		if (!buffer.Data || !buffer.Width || !buffer.Height)
			return NE_ERR_INVALID_ARG;
		const float sx = instance->Data.EmToPixel.x;
		const float sy = instance->Data.EmToPixel.y;
		const int glyph = stbtt_FindGlyphIndex( &instance->Data.Info, ch );
		stbtt_MakeGlyphBitmap( &instance->Data.Info, buffer.Data, buffer.Width, buffer.Height, buffer.Width, sx, sy, glyph );
		return NE_OK;
	}

	void StbFont::Release( Font_t font )
	{
		Instance_s* instance = (Instance_s*)font;
		NE_ALLOC_DELETE( instance->Alloc, instance );
	}

} }
