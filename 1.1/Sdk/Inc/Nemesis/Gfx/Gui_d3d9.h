#pragma once

//======================================================================================
#include "Gfx_d3d9.h"

//======================================================================================
namespace nemesis { namespace graphics 
{
    typedef struct Gui_d3d9_s *Gui_d3d9_t;

    Gui_d3d9_t  NE_API Gui_d3d9_Create	 ( Allocator_t alloc, Gfx_d3d9_t gfx );
    void	    NE_API Gui_d3d9_Release  ( Gui_d3d9_t gui );
    void		NE_API Gui_d3d9_NextFrame( Gui_d3d9_t gui );

} }
