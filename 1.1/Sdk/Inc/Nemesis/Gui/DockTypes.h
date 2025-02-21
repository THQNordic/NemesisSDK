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
#include <Nemesis/Gfx/Gfx_d3d11.h>

//======================================================================================
namespace nemesis
{
	typedef struct DockMgr_s  *DockMgr_t;
	typedef struct DockCtrl_s *DockCtrl_t;

	struct DockSide
	{
		enum Enum
		{ None
		, Left
		, Top
		, Right
		, Bottom
		, Center
		};
	};

	struct DockArea
	{
		enum Enum
		{ Default
		, Inner
		, Outer
		, COUNT
		};
	};

	typedef struct DockSwapChain_s *DockSwapChain_t;

	struct DockMgrApi_Gfx_s
	{
		DockSwapChain_t (NE_CALLBK* SwapChain_Create) ( ptr_t user, ptr_t wnd, int w, int h, bool overlay );
		void			(NE_CALLBK* SwapChain_Resize) ( ptr_t user, DockSwapChain_t swap_chain, int w, int h );
		void			(NE_CALLBK* SwapChain_Begin)  ( ptr_t user, DockSwapChain_t swap_chain );
		void			(NE_CALLBK* SwapChain_End)	  ( ptr_t user, DockSwapChain_t swap_chain );
		void			(NE_CALLBK* SwapChain_Present)( ptr_t user, DockSwapChain_t swap_chain );
		void			(NE_CALLBK* SwapChain_Release)( ptr_t user, DockSwapChain_t swap_chain );

		void (NE_CALLBK* Scene_Begin)	( ptr_t user );
		void (NE_CALLBK* Scene_Clear)	( ptr_t user, uint32_t color );
		void (NE_CALLBK* Scene_Draw)	( ptr_t user, const gui::DrawBatch_s* batches, int count );
		void (NE_CALLBK* Scene_End)		( ptr_t user );
		void (NE_CALLBK* Scene_Present) ( ptr_t user );

		ptr_t User;
	};

	typedef struct DockOs_s  *DockOs_t;
	typedef struct DockHost_s *DockHost_t;

	struct DockMgrApi_Os_s
	{
		DockOs_t	(NE_CALLBK *Os_Create) ( ptr_t user );
		void		(NE_CALLBK *Os_Release)( DockOs_t os );
		int			(NE_CALLBK *Os_OrderZ) ( DockOs_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts );

		DockHost_t	(NE_CALLBK *Host_Create)	( DockOs_t os, DockCtrl_t ctrl );
		bool		(NE_CALLBK *Host_IsActive)	( DockHost_t host );
		void	  	(NE_CALLBK *Host_Move)		( DockHost_t host );
		void		(NE_CALLBK *Host_Move_Rect)	( DockHost_t host, const Rect_s& r );
		void		(NE_CALLBK *Host_Show)		( DockHost_t host );
		void		(NE_CALLBK *Host_Render)	( DockHost_t host );
		void		(NE_CALLBK *Host_Release)	( DockHost_t host );

		ptr_t User;
	};

	struct DockMgrSetup_s
	{
		Allocator_t		 Alloc;
		ptr_t			 AppWnd;
		Rect_s			 AppRect;
		cstr_t			 AppTitle;
		ptr_t			 Instance;
		DockMgrApi_Gfx_s Graphics;
		DockMgrApi_Os_s	 Platform;
	};

	struct DockCtrlApi_v
	{
		void (NE_CALLBK* Do)( DockCtrl_t ctrl, ptr_t user );
		ptr_t User;
	};

	struct DockCtrlSetup_s
	{
		cstr_t Text;
		IntRect_s Rect;
		DockCtrlApi_v Api;
	};

	struct DockScroll_s
	{
		Vec2_s Offset;
		Vec2_s Size;
		Vec2_s Step;
	};

}
