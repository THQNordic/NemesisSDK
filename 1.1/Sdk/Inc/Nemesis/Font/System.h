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
#include "Types.h"

//======================================================================================
#include <Nemesis/Core/AllocTypes.h>

//======================================================================================
namespace nemesis { namespace font
{
	/// System

	Result_t		NE_API System_Initialize	( Allocator_t alloc );
	void			NE_API System_Shutdown		();

	int32_t			NE_API System_GetNumResources();
	FontResource_s	NE_API System_GetResources	 ( int32_t idx );
	FontResource_s	NE_API System_FindResource	 ( wcstr_t face );

	Handle_t		NE_API System_InstallGdiFont  ( cptr_t resource, uint32_t size );
	Result_t		NE_API System_UninstallGdiFont( Handle_t handle );

	Result_t		NE_API System_CreateGdiFont	( const GdiFontSetup_s& setup, Font_t* out );
	Result_t		NE_API System_CreateStbFont	( const StbFontSetup_s& setup, Font_t* out );

} } 

//======================================================================================
namespace nemesis { namespace font
{
	/// Font

	Result_t NE_API Font_GetTextMetrics	( Font_t font, TextMetrics_s& tm );
	Result_t NE_API Font_GetGlyphInfos  ( Font_t font, const wchar_t* t, int num_chars, GlyphInfo_s* infos );
	Result_t NE_API Font_RenderChar		( Font_t font, wchar_t ch, const GlyphBuffer_s& buffer );
	void	 NE_API Font_Release		( Font_t font );

} }
