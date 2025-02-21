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
#include "Gfx.h"
#include <Nemesis/Core/AllocTypes.h>

//======================================================================================
namespace nemesis { namespace graphics 
{
	struct SwapChainDesc_d3d11_s
	{
		SwapChainDesc_s Hdr;
		bool Composition;
	};

} }

//======================================================================================
namespace nemesis { namespace graphics 
{
	typedef struct Gfx_d3d11_s		 *Gfx_d3d11_t;
	typedef struct Texture_d3d11_s	 *Texture_d3d11_t;
	typedef struct SwapChain_d3d11_s *SwapChain_d3d11_t;

	Gfx_d3d11_t	NE_API Gfx_d3d11_Create  ( Allocator_t alloc, void* wnd );
	void		NE_API Gfx_d3d11_Resize  ( Gfx_d3d11_t g );
	void		NE_API Gfx_d3d11_CacheOut( Gfx_d3d11_t g );
	void		NE_API Gfx_d3d11_Release ( Gfx_d3d11_t g );

	void		NE_API Gfx_d3d11_Begin  ( Gfx_d3d11_t g );
	void		NE_API Gfx_d3d11_Clear  ( Gfx_d3d11_t g, uint32_t color );
	void		NE_API Gfx_d3d11_End    ( Gfx_d3d11_t g );
	void		NE_API Gfx_d3d11_Present( Gfx_d3d11_t g );
	void		NE_API Gfx_d3d11_Present( Gfx_d3d11_t g, bool v_sync );

	void		NE_API Gfx_d3d11_Scope_Begin( Gfx_d3d11_t g, wcstr_t name );
	void		NE_API Gfx_d3d11_Scope_End	( Gfx_d3d11_t g );

	bool		NE_API Gfx_d3d11_Texture_Create	( Gfx_d3d11_t g, const TextureDesc_s& info, Texture_d3d11_t* texture );
	bool		NE_API Gfx_d3d11_Texture_GetDesc( Gfx_d3d11_t g, Texture_d3d11_t texture, TextureDesc_s& desc );
	bool		NE_API Gfx_d3d11_Texture_Lock	( Gfx_d3d11_t g, Texture_d3d11_t texture, Lock::Mode mode, LockDesc_s& lock );
	void		NE_API Gfx_d3d11_Texture_Unlock ( Gfx_d3d11_t g, Texture_d3d11_t texture );
	void		NE_API Gfx_d3d11_Texture_Release( Gfx_d3d11_t g, Texture_d3d11_t texture );

	bool		NE_API Gfx_d3d11_SwapChain_Create ( Gfx_d3d11_t g, const SwapChainDesc_d3d11_s& setup, SwapChain_d3d11_t* out );
	bool		NE_API Gfx_d3d11_SwapChain_Create ( Gfx_d3d11_t g, const SwapChainDesc_s& setup, SwapChain_d3d11_t* out );
	void		NE_API Gfx_d3d11_SwapChain_Resize ( SwapChain_d3d11_t swap, int w, int h );
	void		NE_API Gfx_d3d11_SwapChain_Begin  ( SwapChain_d3d11_t swap );
	void		NE_API Gfx_d3d11_SwapChain_End	  ( SwapChain_d3d11_t swap );
	void		NE_API Gfx_d3d11_SwapChain_Present( SwapChain_d3d11_t swap );
	void		NE_API Gfx_d3d11_SwapChain_Present( SwapChain_d3d11_t swap, bool v_sync );
	void		NE_API Gfx_d3d11_SwapChain_Release( SwapChain_d3d11_t swap );

	void		NE_API Gfx_d3d11_Draw_2D		( Gfx_d3d11_t g, const DrawBatch_s* batch, int count );
	void		NE_API Gfx_d3d11_Draw_3D		( Gfx_d3d11_t g, const LineBatch_s* batch, int count );
	void		NE_API Gfx_d3d11_Draw_3D		( Gfx_d3d11_t g, const MeshBatch_s* batch, int count );
	void		NE_API Gfx_d3d11_Draw_3D_Lit	( Gfx_d3d11_t g, const MeshBatch_s* batch, int count, const Vec3_s& tint );
	void		NE_API Gfx_d3d11_Draw_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s* batch, int count );
	void		NE_API Gfx_d3d11_Draw_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s* batch, int count, const Vec3_s& tint );

} };
