//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "dock_d3d11.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::graphics;

//======================================================================================
static DockSwapChain_t NE_CALLBK DockApi_SwapChain_Create( ptr_t user, ptr_t wnd, int w, int h, bool overlay )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = nullptr;
	SwapChainDesc_d3d11_s setup = { { wnd, w, h }, overlay };
	Gfx_d3d11_SwapChain_Create( g, setup, &sc );
	return (DockSwapChain_t)sc;
}

static void NE_CALLBK DockApi_SwapChain_Resize( ptr_t user, DockSwapChain_t swap_chain, int w, int h )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = (SwapChain_d3d11_t)swap_chain;
	Gfx_d3d11_SwapChain_Resize( sc, w, h );
}

static void NE_CALLBK DockApi_SwapChain_Begin( ptr_t user, DockSwapChain_t swap_chain )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = (SwapChain_d3d11_t)swap_chain;
	Gfx_d3d11_SwapChain_Begin( sc );
}

static void NE_CALLBK DockApi_SwapChain_End( ptr_t user, DockSwapChain_t swap_chain )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = (SwapChain_d3d11_t)swap_chain;
	Gfx_d3d11_SwapChain_End( sc );
}

static void NE_CALLBK DockApi_SwapChain_Present( ptr_t user, DockSwapChain_t swap_chain )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = (SwapChain_d3d11_t)swap_chain;
	Gfx_d3d11_SwapChain_Present( sc );
}

static void NE_CALLBK DockApi_SwapChain_Release( ptr_t user, DockSwapChain_t swap_chain )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	SwapChain_d3d11_t sc = (SwapChain_d3d11_t)swap_chain;
	Gfx_d3d11_SwapChain_Release( sc );
}

static void NE_CALLBK DockApi_Scene_Begin( ptr_t user )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	Gfx_d3d11_Begin( g );
}

static void NE_CALLBK DockApi_Scene_Clear( ptr_t user, uint32_t color )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	Gfx_d3d11_Clear( g, color );
}

static void NE_CALLBK DockApi_Scene_Draw( ptr_t user, const DrawBatch_s* batches, int count )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	Gfx_d3d11_Draw_2D( g, batches, count );
}

static void NE_CALLBK DockApi_Scene_End( ptr_t user )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	Gfx_d3d11_End( g );
}

static void NE_CALLBK DockApi_Scene_Present( ptr_t user )
{
	Gfx_d3d11_t g = (Gfx_d3d11_t)(user);
	Gfx_d3d11_Present( g );
}

//======================================================================================
ne::DockMgrApi_Gfx_s Dock_d3d11_GetApi( ne::gfx::Gfx_d3d11_t g )
{
	const DockMgrApi_Gfx_s api = 
	{ DockApi_SwapChain_Create
	, DockApi_SwapChain_Resize
	, DockApi_SwapChain_Begin
	, DockApi_SwapChain_End
	, DockApi_SwapChain_Present
	, DockApi_SwapChain_Release
	, DockApi_Scene_Begin
	, DockApi_Scene_Clear
	, DockApi_Scene_Draw
	, DockApi_Scene_End
	, DockApi_Scene_Present
	, g
	};
	return api;
}
