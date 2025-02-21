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
#include "GdiFont.h"
#include "StbFont.h"
#include <Nemesis/Core/Alloc.h>
#include <Nemesis/Core/Memory.h>
#include <Nemesis/Core/String.h>

//======================================================================================
namespace nemesis { namespace font { namespace data
{
	#include "ttf/inconsolata_ttf.inl" 
	#include "ttf/selawk_ttf.inl" 
	#include "ttf/nemesis_ttf.inl" 

} } }

//======================================================================================
namespace nemesis { namespace font
{
	static const FontResource_s BuiltInFont[] = 
	{ { L"Inconsolata"	, data::inconsolata_ttf	, sizeof(data::inconsolata_ttf)	, FontWeight::Regular, FontPitch::Fixed		, FontUsage::Default }
	, { L"Selawik"		, data::selawk_ttf		, sizeof(data::selawk_ttf)		, FontWeight::Regular, FontPitch::Variable	, FontUsage::Default }
	, { L"Nemesis"		, data::nemesis_ttf		, sizeof(data::nemesis_ttf)		, FontWeight::Regular, FontPitch::Variable	, FontUsage::IconSet }
	};

	const FontResource_s* FontResource_Lookup( wcstr_t face )
	{
		const int num_resources = NeCountOf( BuiltInFont );
		for ( int i = 0; i < num_resources; ++i )
		{
			if (0 == Utf16_Cmp( face, BuiltInFont[i].Family ))
				return BuiltInFont + i;
		}
		return nullptr;
	}
} }

//======================================================================================
namespace nemesis { namespace font
{
	/// System

	struct System_s
	{
		Allocator_t Alloc;
	};

	static System_s FontSystem = {};

	Result_t System_Initialize( Allocator_t alloc )
	{
		if (FontSystem.Alloc)
			return NE_ERR_INVALID_CALL;
		FontSystem.Alloc = alloc;
		return NE_OK;
	}

	void System_Shutdown()
	{
		NeZero(FontSystem);
	}

	int32_t System_GetNumResources()
	{
		return NeCountOf(BuiltInFont);
	}

	FontResource_s System_GetResources( int32_t idx )
	{
		if ((idx >= 0) && (idx < NeCountOf(BuiltInFont)))
			return BuiltInFont[idx];
		return FontResource_s {};
	}

	FontResource_s System_FindResource( wcstr_t face )
	{
		const FontResource_s* resource = FontResource_Lookup( face );
		if (resource)
			return *resource;
		return FontResource_s {};
	}

	Handle_t System_InstallGdiFont( cptr_t resource, uint32_t size )
	{
	#if NE_PLATFORM_IS_DESKTOP
		return GdiFont::InstallResource( resource, size );
	#else
		return nullptr;
	#endif
	}

	Result_t System_UninstallGdiFont( Handle_t handle )
	{
	#if NE_PLATFORM_IS_DESKTOP
		return GdiFont::UninstallResource( handle );
	#else
		return NE_ERR_NOT_SUPPORTED;
	#endif
	}

	Result_t System_CreateGdiFont( const GdiFontSetup_s& setup, Font_t* out )
	{
	#if NE_PLATFORM_IS_DESKTOP
		return GdiFont::CreateInstance( FontSystem.Alloc, setup, out );
	#else
		return NE_ERR_NOT_SUPPORTED;
	#endif
	}

	Result_t System_CreateStbFont( const StbFontSetup_s& setup, Font_t* out )
	{
		return StbFont::CreateInstance( FontSystem.Alloc, setup, out );
	}

} }

//======================================================================================
namespace nemesis { namespace font
{
	/// Font

	Result_t Font_GetTextMetrics( Font_t font, TextMetrics_s& tm )
	{
		return font->Api->GetTextMetric( font, tm );
	}

	Result_t Font_GetGlyphInfos( Font_t font, const wchar_t* t, int num_chars, GlyphInfo_s* infos )
	{
		return font->Api->GetGlyphInfos( font, t, num_chars, infos );
	}

	Result_t Font_RenderChar( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer )
	{
		return font->Api->RenderChar( font, ch, buffer );
	}

	void Font_Release( Font_t font )
	{
		return font->Api->Release( font );
	}

} }
