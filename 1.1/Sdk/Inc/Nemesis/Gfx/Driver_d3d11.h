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
#include "Gfx_d3d11.h"
#include "Gui_d3d11.h"
NE_LINK("libNeGfx_d3d11")

//======================================================================================
namespace nemesis { namespace graphics
{
	typedef Gfx_d3d11_t Gfx_t;
	typedef Gui_d3d11_t Gui_t;

	inline Gfx_t Gfx_Create	 ( Allocator_t alloc, void* wnd )							{ return Gfx_d3d11_Create	( alloc, wnd ); }
	inline void	 Gfx_Resize  ( Gfx_t g )												{ return Gfx_d3d11_Resize	( g ); }
	inline void	 Gfx_Release ( Gfx_t g )												{ return Gfx_d3d11_Release	( g ); }
	inline void	 Gfx_CacheOut( Gfx_t g )												{ return Gfx_d3d11_CacheOut	( g ); }
	inline void	 Gfx_Begin   ( Gfx_t g )												{ return Gfx_d3d11_Begin	( g ); }
	inline void	 Gfx_Clear   ( Gfx_t g, uint32_t color )								{ return Gfx_d3d11_Clear	( g, color ); }
	inline void	 Gfx_End     ( Gfx_t g )												{ return Gfx_d3d11_End		( g ); }
	inline void	 Gfx_Present ( Gfx_t g )												{ return Gfx_d3d11_Present	( g ); }

	inline void	 Gfx_Draw_2D		( Gfx_t g, const DrawBatch_s* batch, int count )	{ return Gfx_d3d11_Draw_2D		  ( g, batch, count ); }
	inline void	 Gfx_Draw_3D		( Gfx_t g, const MeshBatch_s* batch, int count )	{ return Gfx_d3d11_Draw_3D		  ( g, batch, count ); }
	inline void	 Gfx_Draw_3D_Skinned( Gfx_t g, const SkinBatch_s* batch, int count )	{ return Gfx_d3d11_Draw_3D_Skinned( g, batch, count ); }

	inline Gui_t Gui_Create	  ( Allocator_t alloc, Gfx_t gfx )							{ return Gui_d3d11_Create	( alloc, gfx ); }
	inline void	 Gui_Release  ( Gui_t gui )												{ return Gui_d3d11_Release	( gui ); }
	inline void	 Gui_NextFrame( Gui_t gui )												{ return Gui_d3d11_NextFrame( gui ); }

} }
