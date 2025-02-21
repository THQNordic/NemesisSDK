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

//======================================================================================
namespace nemesis { namespace graphics 
{
    typedef struct Gui_d3d11_s *Gui_d3d11_t;

    Gui_d3d11_t NE_API Gui_d3d11_Create	  ( Allocator_t alloc, Gfx_d3d11_t gfx );
    void		NE_API Gui_d3d11_Release  ( Gui_d3d11_t gui );
    void		NE_API Gui_d3d11_NextFrame( Gui_d3d11_t gui );

} }
