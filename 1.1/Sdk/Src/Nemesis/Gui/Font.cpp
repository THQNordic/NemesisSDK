//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Font.h"
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/Map.h>
#include <Nemesis/Core/New.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Font/All.h>
#include <memory.h>

//======================================================================================
#define	STB_RECT_PACK_IMPLEMENTATION
#define	STBRP_ASSERT NeAssert
#include "stb/stb_rect_pack.h"

//======================================================================================
namespace nemesis { namespace gui { namespace
{
	/// Texture Manipulation

	static void Texture_FillRect( const TextureLock_s& lr, const IntRect_s& r, uint32_t argb )
	{
		uint8_t* dst = ((uint8_t*)lr.Data) + r.y * lr.Pitch + r.x * sizeof(uint32_t);
		for ( int y = 0; y < r.h; ++y, dst += lr.Pitch )
			for ( int x = 0; x < r.w; ++x )
				((uint32_t*)dst)[x] = argb;
	}

	static void Texture_CopyRect( const TextureLock_s& dst_lr, const IntRect_s& dst_rect, const TextureLock_s& src_lr, int src_x, int src_y )
	{
		const int dst_x = dst_rect.x;
		const int dst_y = dst_rect.y;
		const uint8_t* src_pos = ((const uint8_t*)src_lr.Data) + (src_y * src_lr.Pitch) + (src_x * sizeof(uint32_t));
			  uint8_t* dst_pos = ((	     uint8_t*)dst_lr.Data) + (dst_y * dst_lr.Pitch) + (dst_x * sizeof(uint32_t));
		for ( int y = 0; y < dst_rect.h; ++y )
		{
			memcpy( dst_pos, src_pos, dst_rect.w * sizeof(uint32_t) );
			src_pos += src_lr.Pitch;
			dst_pos += dst_lr.Pitch;
		}
	}

	static void Texture_CopyGlyphA8( const TextureLock_s& lr, int dst_x, int dst_y, const uint8_t* bits, int glyph_w, int glyph_h, int glyph_stride )
	{
		uint32_t a;
		const uint8_t* src = bits;
			  uint8_t* dst = ((uint8_t*)lr.Data) + dst_y * lr.Pitch + dst_x * sizeof(uint32_t);
		for ( int y = 0; y < glyph_h; ++y, dst += lr.Pitch, src += glyph_stride )
		{
			uint32_t* pixel = (uint32_t*)dst;
			for ( int x = 0; x < glyph_w; ++x )
			{
				a = src[x];
				pixel[x] = (a << 24) | 0x00ffffff;
			}
		}
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace
{
	static void Rect_ToVec2_Scale( const IntRect_s& r, const Vec2_s& s, Vec2_s& a, Vec2_s& b )
	{
		a = Vec2_s {  r.x		 * s.x,  r.y		* s.y };
		b = Vec2_s { (r.x + r.w) * s.x, (r.y + r.h)	* s.y };
	}

	static IntRect_s ToRect( const stbrp_rect& pr )
	{
		const IntRect_s r = { pr.x, pr.y, pr.w, pr.h };
		return r;
	}

	struct CharInfoComparer
	{
		static int Compare( const CharInfo_s& lhs, wchar_t rhs )
		{ return (int)lhs.Char - (int)rhs; }
	};

} } }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Dynamic Font

	void DynamicFont_Initialize( DynamicFont_t item, Allocator_t alloc, Renderer_t client, font::Font_t font )
	{
		item->Alloc = alloc;
		item->Client = client;
		item->Font = font;
		item->Texture =  nullptr;
		item->WhiteUV = Vec2_s { 0.0f, 0.0f };
		item->BorderX = 1;
		item->BorderY = 1;
		item->Char.Init( alloc );
		item->Glyph.Init( alloc );
		item->Missing.Init( alloc );
		item->SubstFont.Init( alloc );
		item->SubstRange.Init( alloc );
	}

	void DynamicFont_Shutdown( DynamicFont_t item )
	{
		if (item->Client && item->Texture)
			item->Client->Api->ReleaseTexture( item->Client, item->Texture );
		item->Char.Clear();
		item->Glyph.Clear();
		item->Missing.Clear();
		item->SubstFont.Clear();
		item->SubstRange.Clear();
	}

	void DynamicFont_ReplaceRange( DynamicFont_t item, font::Font_t font, wchar_t first, wchar_t last )
	{
		item->SubstFont.Append( font );
		item->SubstRange.Append( CharRange_s { first, last } );
	}

	Result_t DynamicFont_GetFontInfo( DynamicFont_t item, FontInfo_s* info )
	{
		Result_t hr;
		font::TextMetrics_s tm = {};
		hr = font::Font_GetTextMetrics( item->Font, tm );
		info->FontHeight	= (float)tm.Height;
		info->LineHeight	= (float)tm.LineHeight;
		info->MaxCharWidth	= (float)tm.MaxCharWidth;
		info->BreakChar		= tm.BreakChar;
		info->DefaultChar	= tm.DefaultChar;
		info->Texture		= item->Texture;
		info->WhiteUV		= item->WhiteUV;
		info->Char			= item->Char.Data;
		info->Glyph			= item->Glyph.Data;
		info->NumChars		= item->Char.Count;
		info->NumGlyphs		= item->Glyph.Count;
		return NE_OK;
	}

	void DynamicFont_PreloadChar( DynamicFont_t item, wchar_t ch )
	{
		int pos = Array_BinaryFind< CharInfoComparer >( item->Char, ch );
		if (pos >= 0)
			return;

		pos = Array_BinaryFind( item->Missing, ch );
		if (pos >= 0)
			return;

		item->Missing.InsertAt( ~pos, ch );
	}

	void DynamicFont_PreloadText( DynamicFont_t item, cstr_t utf8, int count )
	{
		if (!utf8 || !utf8[0] || !count)
			return;

		int n = 0;
		wchar_t cp = 0;
		cstr_t end = utf8 + Utf8_Len( utf8, count );
		for ( cstr_t text = utf8; text < end; )
		{
			n = Utf8_Decode( text, end, &cp );
			if (!n)
				break;
			text += n;
			DynamicFont_PreloadChar( item, cp );
		}
	}

	void DynamicFont_PreloadText( DynamicFont_t item, wcstr_t utf16, int count )
	{
		if (!utf16 || !utf16[0] || !count)
			return;
		const int len = Wcs_Len( utf16, count );
		for ( int i = 0; i < len; ++i )
			DynamicFont_PreloadChar( item, utf16[i] );
	}

	void DynamicFont_PreloadChars( DynamicFont_t item, wchar_t first, wchar_t last )
	{
		for ( wchar_t ch = first; ch <= last; ++ch )
			DynamicFont_PreloadChar( item, ch );
	}

	Result_t DynamicFont_CommitChanges( DynamicFont_t item )
	{
		Result_t hr;

		// noop?
		if (item->Glyph.Count && !item->Missing.Count)
			return NE_OK;

		///
		///	...	Setup Phase
		///

		// custom glyphs
		enum 
		{ WHITE_GLYPH
		, NUM_CUSTOM_GLYPHS
		};

		// get text metrics
		font::TextMetrics_s tm = {};
		font::Font_GetTextMetrics( item->Font, tm );

		// lazy init
		if (!item->Glyph.Count)
		{
			item->Char .Reserve( 64 );
			item->Glyph.Reserve( 64 );
			item->Glyph.Resize( NUM_CUSTOM_GLYPHS );

			GlyphInfo_s& glyph = item->Glyph[ WHITE_GLYPH ];
			NeZero( glyph );
			glyph.Pos[1].x = 4.0f;
			glyph.Pos[1].y = 4.0f;
			glyph.Advance = 4.0f;

			DynamicFont_PreloadChar( item, tm.DefaultChar );
		}

		///
		///	...	Gather Phase
		///

		const int num_subst = item->SubstRange.Count;
		const int num_missing = item->Missing.Count;
		Array<int32_t> missing_subst( item->Alloc );
		missing_subst.Resize( num_missing );
		{
			for ( int i = 0; i < num_missing; ++i )
			{
				const wchar_t missing_char = item->Missing[i];
				for ( int j = 0; j < num_subst; ++j )
				{
					const CharRange_s& range = item->SubstRange[j];
					if ( missing_char >= range.First && missing_char <= range.Last )
					{
						missing_subst[i] = 1+j;
						break;
					}
				}
			}
		}

		// get current texture's info
		Texture_t old_texture = item->Texture;
		TextureInfo_s old_texture_info = {};
		if (old_texture)
			item->Client->Api->GetTextureInfo( item->Client, old_texture, old_texture_info );

		// gather glyph infos
		Array<font::GlyphInfo_s> glyph_info( item->Alloc );
		glyph_info.Resize( item->Glyph.Count + item->Missing.Count );
		const int first_new_glyph = item->Glyph.Count;
		{
			// existing glyphs
			for ( int i = 0; i < item->Glyph.Count; ++i )
			{
				glyph_info[i].BlackBox.x = (int)(item->Glyph[i].Pos[0].x);
				glyph_info[i].BlackBox.y = (int)(item->Glyph[i].Pos[0].y);
				glyph_info[i].BlackBox.w = (int)(item->Glyph[i].Pos[1].x - item->Glyph[i].Pos[0].x);
				glyph_info[i].BlackBox.h = (int)(item->Glyph[i].Pos[1].y - item->Glyph[i].Pos[0].y);
				glyph_info[i].Advance	 = (int)(item->Glyph[i].Advance);
				glyph_info[i].Offset	 = (int)(item->Glyph[i].Offset);
			}

			// missing glyphs
			for ( int i = 0; i < num_missing; ++i )
			{
				const int32_t subst = missing_subst[i]-1;
				font::Font_t fnt = (subst < 0) ? item->Font : item->SubstFont[subst];
				hr = font::Font_GetGlyphInfos( fnt, item->Missing.Data + i, 1, glyph_info.Data + first_new_glyph + i );
			}
		}

		///
		///	...	Packing Phase
		///

		// pack glyph rects
		int packer_w = old_texture_info.Width  ? old_texture_info.Width  : 128;
		int packer_h = old_texture_info.Height ? old_texture_info.Height : 128;
		stbrp_context pack_context;
		Array<stbrp_node> packed_node( item->Alloc );
		Array<stbrp_rect> packed_rect( item->Alloc );
		{
			// apply border and pack rects
			for ( ;; )
			{
				packed_node.Resize( packer_w );
				stbrp_init_target( &pack_context, packer_w - item->BorderX, packer_h - item->BorderY, packed_node.Data, packed_node.Count );

				packed_rect.Resize( glyph_info.Count );
				for ( int i = 0; i < packed_rect.Count; ++i )
				{
					packed_rect[i].id = i;
					packed_rect[i].w = (stbrp_coord)(glyph_info[i].BlackBox.w + item->BorderX);
					packed_rect[i].h = (stbrp_coord)(glyph_info[i].BlackBox.h + item->BorderY);
				}

				int stbpr_hr = stbrp_pack_rects( &pack_context, packed_rect.Data, packed_rect.Count );
				if (stbpr_hr)
					break;

				if (packer_w > packer_h)
					packer_h *= 2;
				else 
					packer_w *= 2;

				if (packer_w > 4096)
					return NE_ERR_OUT_OF_MEMORY;
			}

			// remove border
			for ( int i = 0; i < packed_rect.Count; ++i )
			{
				packed_rect[i].x += (stbrp_coord)item->BorderX;
				packed_rect[i].y += (stbrp_coord)item->BorderY;
				packed_rect[i].w -= (stbrp_coord)item->BorderX;
				packed_rect[i].h -= (stbrp_coord)item->BorderY;
			}
		}

		///
		///	...	Render Phase
		///

		// @todo: determine if any glyph moved
		bool glyph_moved = true;

		// alloc new texture
		Texture_t new_texture = old_texture;
		{
			const bool texture_resized = (old_texture_info.Width != packer_w) || (old_texture_info.Height != packer_h);
			if (texture_resized || glyph_moved)
			{
				const TextureInfo_s new_texture_info = { packer_w, packer_h, TextureFormat::A8R8G8B8 };
				hr = item->Client->Api->CreateTexture( item->Client, new_texture_info, &new_texture );
				if (NeFailed( hr ))
					return hr;
			}
		}

		// lock texture
		TextureLock_s new_locked_texture = {};
		hr = item->Client->Api->LockTexture( item->Client, new_texture, TextureLock::Write, new_locked_texture );
		if (NeFailed( hr ))
			return hr;

		// clear texture
		if (old_texture != new_texture)
		{
			const IntRect_s r = { 0, 0, packer_w, packer_h };
			Texture_FillRect( new_locked_texture, r, 0x00ffffff );
		}

		// generate custom glyphs
		if (!old_texture)
		{
			const stbrp_rect& pr = packed_rect[ WHITE_GLYPH ];
			Texture_FillRect( new_locked_texture, ToRect(pr), 0xffffffff );
			item->WhiteUV.x = (0.5f + pr.x) / (float)packer_w;
			item->WhiteUV.y = (0.5f + pr.y) / (float)packer_h;
		}

		// copy existing glyphs to new texture
		if (old_texture && (old_texture != new_texture))
		{
			TextureLock_s old_locked_texture = {};
			hr = item->Client->Api->LockTexture( item->Client, old_texture, TextureLock::Read, old_locked_texture );
			if (NeFailed( hr ))
			{
				item->Client->Api->UnlockTexture( item->Client, new_texture );
				return hr;
			}

			for ( int i = 0; i < first_new_glyph; ++i )
			{
				const GlyphInfo_s& src = item->Glyph[i]; 
				const stbrp_rect&  dst = packed_rect[i];

				const int src_x = (int)(old_texture_info.Width  * src.UV[0].x);	// @note: we have to reconstruct the pixel coords from the UVs of the old glyph
				const int src_y = (int)(old_texture_info.Height * src.UV[0].y);	//		  X and Y refer to screen space draw displacements, *NOT* texel coordinates

				Texture_CopyRect( new_locked_texture, ToRect(dst), old_locked_texture, src_x, src_y );
			}

			{
				const stbrp_rect& r = packed_rect[ 0 ]; 
				item->WhiteUV.x = (0.5f + r.x) / (float)packer_w;
				item->WhiteUV.y = (0.5f + r.y) / (float)packer_h;
			}

			item->Client->Api->UnlockTexture( item->Client, old_texture );
		}

		// render missing glyphs to texture
		{
			Array<uint8_t> glyph_a8( item->Alloc );
			glyph_a8.Resize( tm.MaxCharWidth * tm.LineHeight );

			const font::GlyphBuffer_s glyph_buffer = 
			{ glyph_a8.Data
			, tm.MaxCharWidth
			, tm.Height
			};

			// render each glyph
			for ( int i = 0; i < item->Missing.Count; ++i )
			{
				const int32_t subst = missing_subst[i]-1;
				font::Font_t fnt = (subst < 0) ? item->Font : item->SubstFont[subst];

				// @note: missing glyphs will have an empty rect

				// skip empty glyphs
				const stbrp_rect& rect = packed_rect[ first_new_glyph + i ];
				if ((rect.w == 0) || (rect.h == 0))
					continue;

				// render glyph to buffer
				const font::GlyphInfo_s& gi = glyph_info[ first_new_glyph + i ];
				hr = font::Font_RenderChar( fnt, item->Missing[i], glyph_buffer );

				// copy buffer to texture
				NeAssert( gi.BlackBox.w == rect.w );
				NeAssert( gi.BlackBox.h == rect.h );
				NeAssert( first_new_glyph + i == rect.id );
				Texture_CopyGlyphA8( new_locked_texture, rect.x, rect.y, glyph_a8.Data, gi.BlackBox.w, gi.BlackBox.h, glyph_buffer.Width );
			}
		}

		// unlock texture
		{
			item->Client->Api->UnlockTexture( item->Client, new_texture );
		}

		// keep the new texture
		if (new_texture != old_texture)
		{
			item->Texture = new_texture;
			item->Client->Api->ReleaseTexture( item->Client, old_texture );
		}

		///
		///	...	Indexing Phase
		///

		// rebuild glyph infos
		{
			const Vec2_s tex_scale = Vec2_s { 1.0f / packer_w, 1.0f / packer_h };

			item->Glyph.Resize( glyph_info.Count );
			for ( int i = 0; i < item->Glyph.Count; ++i )
			{
				const stbrp_rect&		rect = packed_rect[i];
				const font::GlyphInfo_s& src = glyph_info[i];
				gui::GlyphInfo_s&		 dst = item->Glyph[i];

				dst.Pos[0].x = (float)src.BlackBox.x;
				dst.Pos[0].y = (float)src.BlackBox.y;
				dst.Pos[1].x = (float)(src.BlackBox.x + src.BlackBox.w);
				dst.Pos[1].y = (float)(src.BlackBox.y + src.BlackBox.h);
				Rect_ToVec2_Scale( ToRect(rect), tex_scale, dst.UV[0], dst.UV[1] );
				dst.Advance = (float)src.Advance;
				dst.Offset  = (float)src.Offset;
			}
		}

		// find missing glyph index 
		int default_glyph = WHITE_GLYPH;
		{
			int pos = Array_BinaryFind< CharInfoComparer >( item->Char, tm.DefaultChar );
			if (pos >= 0)
			{
				default_glyph = pos;
			}
			else
			{
				pos = Array_BinaryFind( item->Missing, tm.DefaultChar );
				if (pos >= 0)
					default_glyph = pos + first_new_glyph;
			}
			NeAssert( pos >= 0 );
		}

		// map new chars to glyphs
		Array<int> dead_glyph( item->Alloc );
		for ( int i = 0; i < item->Missing.Count; ++i )
		{
			// point missing chars to the default char's glyph
			const int glyph_idx = first_new_glyph + i;
			const font::GlyphInfo_s& gi = glyph_info[ glyph_idx ];
			const bool is_missing = (gi.Index == font::GlyphIndex::Invalid);
			const bool id_default = (item->Missing[i] == tm.DefaultChar);
			if (is_missing && !id_default)
				dead_glyph.Append( glyph_idx );

			// map char to glyph
			const int pos = Array_BinaryFind< CharInfoComparer >( item->Char, item->Missing[i] );
			NeAssert(pos < 0);
			const CharInfo_s mapping = { item->Missing[i], (uint16_t)(is_missing ? default_glyph : glyph_idx) };
			item->Char.InsertAt( ~pos, mapping );
		}

		// remove dead glyphs
		for ( int i = dead_glyph.Count-1; i >= 0; --i )
		{
			const int glyph_idx = dead_glyph[i];

			// patch chars referencing subsequent glyphs
			for ( int j = 0; j < item->Char.Count; ++j )
			{
				CharInfo_s& mapping = item->Char[j];
				NeAssert( mapping.Glyph != glyph_idx );
				if (mapping.Glyph > glyph_idx)
					mapping.Glyph--;
			}

			// remove the glyph
			item->Glyph.RemoveAt( glyph_idx );
		}

		// reset missing glyphs
		item->Missing.Reset();
		return NE_OK;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Font Cache Internals

	static font::Font_t FontCache_CreateFontHandleGdi( const FontKey_s& key ) 
	{
		font::Font_t font = nullptr;
		const font::FontUsage::Enum usage 
			= NeHasFlag(key.Style, FontStyle::IconSet) 
			? font::FontUsage::IconSet
			: font::FontUsage::Default;
		font::System_CreateGdiFont( { key.Name, key.Height, usage }, &font );
		return font;
	}

	static font::FontResource_s FontCache_FindFontResource( font::FontPitch::Enum pitch )
	{
		const int count = font::System_GetNumResources();
		for ( int i = 0; i < count; ++i )
		{
			const font::FontResource_s resource = font::System_GetResources( i );
			if (resource.Pitch == pitch)
				return resource;
		}	
		return font::FontResource_s {};
	}

	static font::FontResource_s FontCache_FindFontResource( const FontKey_s& key )
	{
		const font::FontResource_s resource = font::System_FindResource( key.Name );
		if (resource.Buffer)
			return resource;

		const bool is_fixed_size = (Utf16_Cmp( key.Name, L"Consolas" ) == 0);
		const font::FontPitch::Enum pitch = is_fixed_size 
			? font::FontPitch::Fixed 
			: font::FontPitch::Variable;
		return FontCache_FindFontResource( pitch );
	}

	static font::Font_t FontCache_CreateFontHandleStb( const FontKey_s& key ) 
	{
		const font::FontResource_s resource = FontCache_FindFontResource( key );
		if (!resource.Buffer)
			return nullptr;

		font::Font_t font = nullptr;
		const font::FontUsage::Enum usage 
			= NeHasFlag(key.Style, FontStyle::IconSet) 
			? font::FontUsage::IconSet
			: font::FontUsage::Default;
		const font::StbFontSetup_s stb_setup = { key.Name, key.Height, usage, resource.Buffer };
		font::System_CreateStbFont( stb_setup, &font );
		return font;
	}

	static font::Font_t FontCache_CreateFontHandle( const FontKey_s& key ) 
	{
		switch ( key.Mapper )
		{
		case FontMapper::Gdi:
			return FontCache_CreateFontHandleGdi( key );

		case FontMapper::Stb:
			return FontCache_CreateFontHandleStb( key );

		default:
			NeAssertOut(false, "Invalid font mapper: %d.", key.Mapper );
			break;
		}
		return nullptr;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Font Cache Api

	void FontCache_Initialize( FontCache_t cache, Allocator_t alloc, Renderer_t client )
	{
		cache->Alloc = alloc;
		cache->Client = client;
		cache->Map.Init( alloc );
	}

	void FontCache_Shutdown( FontCache_t cache )
	{
		const int num_items = cache->Map.Values.Count;
		for ( int i = 0; i < num_items; ++i )
		{
			DynamicFont_t item = cache->Map.Values[i];
			const int num_subst = item->SubstFont.Count;
			for ( int j = 0; j < num_subst; ++j )
				font::Font_Release( item->SubstFont[j] );
			font::Font_Release( item->Font );
			DynamicFont_Shutdown( item );
			Mem_Free( cache->Alloc, item );
		}
		cache->Map.Clear();
	}

	int FontCache_GetNumFonts( FontCache_t cache )
	{
		return cache->Map.Keys.Count;
	}

	bool FontCache_GetFontKey( FontCache_t cache, int idx, FontKey_s& key )
	{
		if ((idx < 0) || (idx >= cache->Map.Keys.Count))
		{
			key = {};
			return false;
		}
		key = cache->Map.Keys[idx];
		return true;
	}

	Font_t FontCache_GetFont( FontCache_t cache, int idx )
	{
		if ((idx < 0) || (idx >= cache->Map.Keys.Count))
			return nullptr;
		return cache->Map.Values[idx];
	}

	int	FontCache_FindFont( FontCache_t cache, Font_t font )
	{
		const int pos = Array_BinaryFind( cache->Map.Values, (DynamicFont_t)font );
		return (pos >= 0) ? pos : -1;
	}

	Font_t FontCache_CreateFont( FontCache_t cache, const FontKey_s& key )
	{
		// lookup
		DynamicFont_t item = nullptr; 
		if (cache->Map.Lookup( key, item ))
			return item;

		// create font
		font::Font_t fnt = FontCache_CreateFontHandle( key );
		if (!fnt)
			return nullptr;

		// create dynamic font
		item = Mem_Calloc<DynamicFont_s>( cache->Alloc );
		DynamicFont_Initialize( item, cache->Alloc, cache->Client, fnt );

		// register
		cache->Map.Register( key, item );
		return item;
	}

	Font_t FontCache_CreateFont( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style )
	{
		Font_t font = FontCache_CreateGdiFont( cache, name, size, style );
		if (font)
			return font;
		return FontCache_CreateStbFont( cache, name, size, style );
	}

	Font_t FontCache_CreateGdiFont( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style )
	{
		// @todo: 
		//	- ignore case
		//	- intern strings
		//	- pixel size vs point size vs em size
		FontKey_s key;
		NeZero(key);
		key.Mapper	= FontMapper::Gdi;
		key.Style	= style;
		key.Height	= -(int)size;
		Utf16_Cpy( key.Name, name );

		// create
		return FontCache_CreateFont( cache, key );
	}

	Font_t FontCache_CreateStbFont( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style )
	{
		// @todo: 
		//	- ignore case
		//	- intern strings
		//	- pixel size vs point size vs em size
		FontKey_s key;
		NeZero(key);
		key.Mapper	= FontMapper::Stb;
		key.Style	= style;
		key.Height	= -(int)size;
		Utf16_Cpy( key.Name, name );

		// create
		return FontCache_CreateFont( cache, key );
	}

	Font_t FontCache_CreateScaledFont( FontCache_t cache, Font_t font, float scale, float min_h, float max_h )
	{
		const int idx = Array_LinearFind( cache->Map.Values, (DynamicFont_t)font );
		if (idx < 0)
			return font;

		FontKey_s key = cache->Map.Keys[idx];
		const int min = (int)min_h;
		const int max = (int)max_h;
		int h = (int)(-key.Height * scale);
		if (min)
			h = NeMax( h, min );
		if (max)
			h = NeMin( h, max );
		key.Height = -h;

		return FontCache_CreateFont( cache, key );
	}

	Result_t FontCache_ReplaceRange( FontCache_t cache, Font_t f, wcstr_t name, FontStyle::Mask style, wchar_t first, wchar_t last )
	{
		DynamicFont_t font = (DynamicFont_t)f;
		const int idx = Array_LinearFind( cache->Map.Values, font );
		if (idx < 0)
			return NE_ERR_NOT_FOUND;

		FontKey_s key = cache->Map.Keys[idx];
		key.Style = style;
		Utf16_Cpy( key.Name, name );

		font::Font_t fnt = FontCache_CreateFontHandle( key );
		if (!fnt)
			return NE_ERR_INVALID_CALL;

		DynamicFont_ReplaceRange( font, fnt, first, last );
		return NE_OK;
	}

	Result_t FontCache_PreloadText( FontCache_t cache, Font_t f, wcstr_t t, int l, FontInfo_s& info )
	{
		DynamicFont_t font = (DynamicFont_t)f;
		if (t && l)
		{
			DynamicFont_PreloadText( font, t, l );
			DynamicFont_CommitChanges( font );
		}
		return DynamicFont_GetFontInfo( font, &info );
	}

	Result_t FontCache_PreloadText( FontCache_t cache, Font_t f, cstr_t t, int l, FontInfo_s& info )
	{
		DynamicFont_t font = (DynamicFont_t)f;
		if (t && l)
		{
			DynamicFont_PreloadText( font, t, l );
			DynamicFont_CommitChanges( font );
		}
		return DynamicFont_GetFontInfo( font, &info );
	}

	Result_t FontCache_PreloadChars( FontCache_t cache, Font_t f, wchar_t s, wchar_t e, FontInfo_s& info )
	{
		DynamicFont_t font = (DynamicFont_t)f;
		DynamicFont_PreloadChars( font, s, e );
		DynamicFont_CommitChanges( font );
		return DynamicFont_GetFontInfo( font, &info );
	}

	Result_t FontCache_GetFontInfo( FontCache_t cache, Font_t f, FontInfo_s& info )
	{
		DynamicFont_t font = (DynamicFont_t)f;
		return DynamicFont_GetFontInfo( font, &info );
	}

	ScaledFont_s FontCache_ScaleFont( FontCache_t cache, Font_t f, float scale, float min_h, float max_h )
	{
		if (fabsf(scale - 1.0f) < FLT_SMALL)
			return ScaledFont_s { f, scale };

		FontInfo_s info = {};
		FontCache_GetFontInfo( cache, f, info );
		const float font_height_scaled = info.FontHeight * scale;
		Font_t font = FontCache_CreateScaledFont( cache, f, scale, min_h, max_h );
		FontCache_GetFontInfo( cache, font, info );
		const float draw_scale = font_height_scaled / info.FontHeight;
		return ScaledFont_s { font, draw_scale };
	}

} }