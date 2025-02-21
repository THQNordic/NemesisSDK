#pragma once

//======================================================================================
#include "Gfx_d3d9.h"
#include "Gui_d3d9.h"
NE_LINK("libNeGfx_d3d9")

//======================================================================================
namespace nemesis { namespace graphics
{
	typedef Gfx_d3d9_t Gfx_t;
	typedef Gui_d3d9_t Gui_t;

	inline Gfx_t Gfx_Create	 ( Allocator_t alloc, void* wnd )							{ return Gfx_d3d9_Create ( alloc, wnd ); }
	inline void	 Gfx_Resize  ( Gfx_t g )												{ return Gfx_d3d9_Resize ( g ); }
	inline void	 Gfx_Release ( Gfx_t g )												{ return Gfx_d3d9_Release( g ); }
	inline void	 Gfx_CacheOut( Gfx_t g )												{}
	inline void	 Gfx_Begin   ( Gfx_t g )												{ return Gfx_d3d9_Begin	 ( g ); }
	inline void	 Gfx_Clear   ( Gfx_t g, uint32_t color )								{ return Gfx_d3d9_Clear	 ( g, color ); }
	inline void	 Gfx_End     ( Gfx_t g )												{ return Gfx_d3d9_End	 ( g ); }
	inline void	 Gfx_Present ( Gfx_t g )												{ return Gfx_d3d9_Present( g ); }

	inline void	 Gfx_Draw_2D		( Gfx_t g, const DrawBatch_s* batch, int count )	{ return Gfx_d3d9_Draw_2D( g, batch, count ); }
	inline void	 Gfx_Draw_3D		( Gfx_t g, const MeshBatch_s* batch, int count )	{ return Gfx_d3d9_Draw_3D( g, batch, count ); }
	inline void	 Gfx_Draw_3D_Skinned( Gfx_t g, const SkinBatch_s* batch, int count )	{}

	inline Gui_t Gui_Create	  ( Allocator_t alloc, Gfx_t gfx )							{ return Gui_d3d9_Create   ( alloc, gfx ); }
	inline void	 Gui_Release  ( Gui_t gui )												{ return Gui_d3d9_Release  ( gui ); }
	inline void	 Gui_NextFrame( Gui_t gui )												{ return Gui_d3d9_NextFrame( gui ); }

} }
