//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include <stdafx.h>
#include "GdiFont.h"

//======================================================================================
#if NE_PLATFORM_IS_DESKTOP
#	include <Nemesis/Core/Array.h>
#	include <Nemesis/Core/String.h>
#	include <Nemesis/Core/Logging.h>
#	include <Nemesis/Core/New.h>
#	define _WIN32_WINNT 0x0600
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

//======================================================================================
namespace nemesis { namespace font
{
	/// GDI Helpers

	struct GdiChannel
	{ 
		enum Enum
		{ Red
		, Green
		, Blue
		, D3DXFont = Green
		};
	};

	static HRESULT GdiBitmap_ConvertPixels( BYTE* dst, int dst_w, int dst_h, const COLORREF* src, int src_w, int src_h, const IntRect_s& src_rect, GdiChannel::Enum ch )
	{
		if (!src || !dst)
			return E_INVALIDARG;
		if ((src_w > dst_w) || (src_h > dst_h))
			return E_INVALIDARG;

		uint8_t channel_index;
		switch ( ch )
		{
		case GdiChannel::Blue:
			channel_index = 0;
			break;
		default:
		case GdiChannel::Green:
			channel_index = 1;
			break;
		case GdiChannel::Red:
			channel_index = 2;
			break;
		}

		typedef BYTE byte4_t[4];

		BYTE* dst_line = dst;
		const COLORREF* src_line = src + src_rect.y * src_w + src_rect.x;
		for ( int y = 0; y < src_rect.h; ++y )
		{
			for ( int x = 0; x < src_rect.w; ++x )
			{
				const byte4_t& quad = (const byte4_t&)(src_line[x]);
				dst_line[x] = quad[channel_index];
			}
			src_line += src_w;
			dst_line += dst_w;
		}

		return S_OK;
	}

} }

//======================================================================================
namespace nemesis { namespace font
{
	/// Gdi Font

	const Font_v GdiFont::Api = 
	{ &GetTextMetric
	, &GetGlyphInfos
	, &RenderChar
	, &Release
	};

	struct GdiFontData_s
	{
		HDC				DC;
		HFONT			Font;
		LOGFONTW		LogFont;
		LONG			LineHeight;
		TEXTMETRICW		TextMetrics;
		SIZE			GlyphSize;
		HBITMAP			GlyphBitmap;
		COLORREF		GlyphBackColor;
		COLORREF		GlyphTextColor;
		HBRUSH			GlyphBackBrush;
		HRGN			GlyphClipRegion;
		FontUsage::Enum	Usage;
	};

	struct GdiFont::Instance_s
	{ 
		explicit Instance_s( Allocator_t alloc )
			: Alloc( alloc )
			, Scratch( alloc )
		{}

		Font_s Header;
		Allocator_t Alloc;
		GdiFontData_s Data;
		Array<COLORREF> Scratch;
	};

	Handle_t GdiFont::InstallResource( cptr_t resource, uint32_t size )
	{
		DWORD num_installed = 0;
		return AddFontMemResourceEx( (void*)resource, size, 0, &num_installed );
	}

	Result_t GdiFont::UninstallResource( Handle_t handle )
	{
		return RemoveFontMemResourceEx( handle ) ? NE_OK : NE_ERR_INVALID_ARG;
	}

	Result_t GdiFont::CreateInstance( Allocator_t alloc, const GdiFontSetup_s& setup, Font_t* font )
	{
		if (!font)
			return NE_ERR_INVALID_ARG;

		*font = nullptr;

		LOGFONTW lf = {};
		lf.lfHeight = setup.Height;
		wcscpy_s( lf.lfFaceName, setup.Face );

		GdiFontData_s data;
		NeZero( data );

		// meta
		data.Usage = setup.Usage;

		// font
		data.DC = CreateCompatibleDC( nullptr );
		data.Font = CreateFontIndirectW( &lf );
		SelectObject( data.DC, data.Font );
		GetTextMetricsW( data.DC, &data.TextMetrics );
		data.LineHeight = data.TextMetrics.tmHeight + data.TextMetrics.tmExternalLeading;

		// glyph
		data.GlyphSize.cx = data.TextMetrics.tmMaxCharWidth;
		data.GlyphSize.cy = data.TextMetrics.tmHeight;
		data.GlyphBitmap = CreateBitmap( data.GlyphSize.cx, data.GlyphSize.cy, 1, 32, nullptr );
		data.GlyphBackColor = RGB( 0x00, 0x00, 0x00 );
		data.GlyphTextColor = RGB( 0xff, 0xff, 0xff );
		data.GlyphBackBrush = CreateSolidBrush( data.GlyphBackColor );
		data.GlyphClipRegion = CreateRectRgn( 0, 0, data.GlyphSize.cx, data.GlyphSize.cy );

		// dc
		SelectObject ( data.DC, data.GlyphBitmap );
		SelectClipRgn( data.DC, data.GlyphClipRegion );
		SetTextColor ( data.DC, data.GlyphTextColor );
		SetTextAlign ( data.DC, TA_LEFT| TA_TOP | TA_UPDATECP );
		SetMapMode	 ( data.DC, MM_TEXT );
		SetBkMode	 ( data.DC, TRANSPARENT );
		SetBkColor	 ( data.DC, data.GlyphBackColor );

		// instance
		Instance_s* instance = NE_ALLOC_NEW( alloc, Instance_s )( alloc );
		instance->Header.Api = &Api;
		instance->Alloc = alloc;
		instance->Data = data;
		*font = &instance->Header;
		return NE_OK;
	}

	Result_t GdiFont::GetTextMetric( Font_t font, TextMetrics_s& tm )
	{
		Instance_s* instance = (Instance_s*)font;
		tm.Height = instance->Data.TextMetrics.tmHeight;
		tm.LineHeight = instance->Data.LineHeight;
		tm.MaxCharWidth = instance->Data.TextMetrics.tmMaxCharWidth;
		tm.BreakChar = instance->Data.TextMetrics.tmBreakChar;
		tm.DefaultChar = instance->Data.TextMetrics.tmDefaultChar;
		return NE_OK;
	}

	Result_t GdiFont::GetGlyphInfos( Font_t font, wcstr_t t, int num_chars, GlyphInfo_s* infos )
	{
		Instance_s* instance = (Instance_s*)font;
		const GlyphBuffer_s scratch = {};
		const int len = Wcs_Len( t, num_chars );
		for ( int i = 0; i < len; ++i )
			RenderChar( font, t[i], scratch, infos + i );
		return NE_OK;
	}

	Result_t GdiFont::RenderChar( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer )
	{
		Instance_s* instance = (Instance_s*)font;
		return RenderChar( font, ch, buffer, nullptr );
	}

	Result_t GdiFont::RenderChar( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer, GlyphInfo_s* info )
	{
		Instance_s* instance = (Instance_s*)font;
		const GdiFontData_s& data = instance->Data;
		const bool no_kanji_hack = (instance->Data.Usage == FontUsage::IconSet);
		const bool is_default_char = (ch == instance->Data.TextMetrics.tmDefaultChar);
		const bool is_extended_char = is_default_char || ((ch > 0x1000) && (!no_kanji_hack));

		// char to glyph
		WORD glyph_index = 0;
		GetGlyphIndicesW( data.DC, &ch, 1, &glyph_index, is_default_char ? 0 : GGI_MARK_NONEXISTING_GLYPHS );

		// measure
		ABC abc;
		SIZE size;
		{
			const wchar_t str[2] = { ch, 0 };
			GetTextExtentPoint32W( data.DC, str, 1, &size );

			if (FALSE == GetCharABCWidthsW( data.DC, str[0], str[0], &abc ))
			{
				abc.abcA = 0;
				abc.abcB = size.cx;
				abc.abcC = 0;
			}
		}

		// clear
		{
			RECT r;
			SetRect( &r, 0, 0, data.GlyphSize.cx, data.GlyphSize.cy );
			FillRect( data.DC, &r, data.GlyphBackBrush );
		}

		// draw
		int x = 0;
		int y = 0;
		int w = abc.abcB;
		int h = size.cy;
		{
			// Hack to adjust for Kanji
			if (is_extended_char)
				w = h;

			// Adjust coordinates to account for the leading edge
			int sx = x;
			int sy = y;

			if ( abc.abcA >= 0 )
				x += abc.abcA;
			else
				sx -= abc.abcA;

			// Hack to adjust for Kanji
			if (is_extended_char)
				sx += abc.abcA;	

			// Draw
			BOOL ok;
			MoveToEx( data.DC, sx, sy, nullptr );
			if (is_default_char)
				ok = ExtTextOutW( data.DC, sx, sy, ETO_OPAQUE | ETO_GLYPH_INDEX, nullptr, (WCHAR*)&glyph_index, 1, nullptr );
			else
				ok = ExtTextOutW( data.DC, sx, sy, ETO_OPAQUE, nullptr, &ch, 1, nullptr );

			// Hack for extended characters (like Kanji) that don't seem to report
			// correct ABC widths. In this case, use the width calculated from
			// drawing the glyph.
			if (is_extended_char)
			{
				POINT pos;
				GetCurrentPositionEx( data.DC, &pos );
				abc.abcB = pos.x - sx;

				if ( abc.abcC < 0 )
					abc.abcB -= abc.abcC;

				w = abc.abcB;
			}
		}

		// info
		if (info)
		{
			// @note: some glyphs have a negative abcA but zero x (e.g. 'j' in Tahoma)
			info->BlackBox.x = NeMin(x, abc.abcA);
			info->BlackBox.y = y;
			info->BlackBox.w = w;
			info->BlackBox.h = h;
			info->Offset	= abc.abcA;
			info->Advance	= abc.abcA + abc.abcB + abc.abcC;
			info->Index		= glyph_index;

			if (glyph_index == 0xffff)
				NeZero(info->BlackBox);
		}

		// buffer
		if (buffer.Data)
		{
			// get bitmap bits
			const int num_pixels = data.GlyphSize.cx * data.GlyphSize.cy;
			instance->Scratch.Resize( num_pixels );
			GetBitmapBits( data.GlyphBitmap, num_pixels * sizeof(COLORREF), instance->Scratch.Data );

			// @note: the glyph buffer should contain only the black box
			//			and not the transparent left/top spacing (x,y)

			// copy/convert only the blackbox portion to the glyph buffer
			const IntRect_s black_box = { x,y, w,h };
			GdiBitmap_ConvertPixels
				( buffer.Data, buffer.Width, buffer.Height
				, instance->Scratch.Data, data.GlyphSize.cx, data.GlyphSize.cy
				, black_box
				, GdiChannel::D3DXFont
				);

		}
		return NE_OK;
	}

	void GdiFont::Release( Font_t font )
	{
		Instance_s* instance = (Instance_s*)font;
		NE_ALLOC_DELETE( instance->Alloc, instance );
	}

} }

//======================================================================================
#endif
