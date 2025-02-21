#pragma once

//======================================================================================
#include "Gfx.h"
#include <Nemesis/Core/AllocTypes.h>

//======================================================================================
namespace nemesis { namespace graphics 
{
	typedef struct Gfx_d3d9_s		*Gfx_d3d9_t;
	typedef struct Texture_d3d9_s	*Texture_d3d9_t;
	typedef struct SwapChain_d3d9_s *SwapChain_d3d9_t;

	Gfx_d3d9_t	NE_API Gfx_d3d9_Create ( Allocator_t alloc, void* wnd );
	void		NE_API Gfx_d3d9_Resize ( Gfx_d3d9_t g );
	void		NE_API Gfx_d3d9_Release( Gfx_d3d9_t g );

	void		NE_API Gfx_d3d9_Begin  ( Gfx_d3d9_t g );
	void		NE_API Gfx_d3d9_Clear  ( Gfx_d3d9_t g, uint32_t color );
	void		NE_API Gfx_d3d9_End    ( Gfx_d3d9_t g );
	void		NE_API Gfx_d3d9_Present( Gfx_d3d9_t g );

	bool		NE_API Gfx_d3d9_Texture_Create	( Gfx_d3d9_t g, const TextureDesc_s& info, Texture_d3d9_t* texture );
	bool		NE_API Gfx_d3d9_Texture_GetDesc	( Gfx_d3d9_t g, Texture_d3d9_t texture, TextureDesc_s& desc );
	bool		NE_API Gfx_d3d9_Texture_Lock	( Gfx_d3d9_t g, Texture_d3d9_t texture, Lock::Mode mode, LockDesc_s& lock );
	void		NE_API Gfx_d3d9_Texture_Unlock	( Gfx_d3d9_t g, Texture_d3d9_t texture );
	void		NE_API Gfx_d3d9_Texture_Release	( Gfx_d3d9_t g, Texture_d3d9_t texture );

	bool		NE_API Gfx_d3d9_SwapChain_Create ( Gfx_d3d9_t g, const SwapChainDesc_s& setup, SwapChain_d3d9_t* out );
	void		NE_API Gfx_d3d9_SwapChain_Resize ( SwapChain_d3d9_t swap, int w, int h );
	void		NE_API Gfx_d3d9_SwapChain_Begin	 ( SwapChain_d3d9_t swap );
	void		NE_API Gfx_d3d9_SwapChain_End	 ( SwapChain_d3d9_t swap );
	void		NE_API Gfx_d3d9_SwapChain_Present( SwapChain_d3d9_t swap );
	void		NE_API Gfx_d3d9_SwapChain_Release( SwapChain_d3d9_t swap );

	void		NE_API Gfx_d3d9_Draw_2D( Gfx_d3d9_t g, const DrawBatch_s* batch, int count );
	void		NE_API Gfx_d3d9_Draw_3D( Gfx_d3d9_t g, const MeshBatch_s* batch, int count );

} };
