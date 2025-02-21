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
#include "DockCtrl.h"
#include "DockTheme.h"

//======================================================================================
namespace nemesis
{
	struct DragAction
	{
		enum Enum
		{ None
		, Push
		, Move
		, Release
		};
	};

	struct DockAction_s
	{
		DockSide::Enum	DropSide;
		DockArea::Enum	DropArea;
		DockCtrl_t		DragSource;
		DockCtrl_t		DropTarget;
		DockCtrl_t		TearOff;
		DockCtrl::Enum	TearWrap;
	};

	struct DockDrag_s
	{
		DockCtrl_t			Source;
		DragAction::Enum	Action;	
		Vec2_s				BeginPos;
		Vec2_s				MousePos;
		float				Threshold;
		DockCtrl_t			ThisTarget;
		DockCtrl_t			PrevTarget;
		DockSide::Enum		DropSide;
		DockArea::Enum		DropArea;
	};

	struct DockMgr_s
	{
		Allocator_t			Alloc;
		ptr_t				AppWnd;
		cstr_t				AppTitle;
		ptr_t				Instance;
		DockMgrApi_Gfx_s	Graphics;
		DockMgrApi_Os_s		Platform;
		DockOs_t			Os;
		gui::Id_t			Id;
		gui::Context_t		Dc;
		uint32_t			NextId;
		Array<DockCtrl_t>	Ctrls;
		Array<DockHost_t>	Order;
		Array<DockCtrl_t>	Temp;
		Array<DockCtrl_t>	Kill;
		DockTheme_s			Theme;
		DockDrag_s			Drag;
		DockCtrl_t			Main;
		DockCtrl_t			Overlay;
		DockAction_s		Pending;
	};
}

//======================================================================================
namespace nemesis
{
	DockCtrl_t	NE_API DockMgr_CreateCtrl	( DockMgr_t mgr, DockCtrl::Enum kind, cstr_t text, const Rect_s& rect, bool show, const DockCtrlApi_v& api );
	void		NE_API DockMgr_Drag			( DockMgr_t mgr, DockCtrl_t ctrl, const Vec2_s& screen_pos, DragAction::Enum action );
	void		NE_API DockMgr_BeginDrop	( DockMgr_t mgr );
	void		NE_API DockMgr_SetDropTarget( DockMgr_t mgr, DockCtrl_t ctrl );
	void		NE_API DockMgr_QueueDrop	( DockMgr_t mgr );
	void		NE_API DockMgr_QueueTearOff	( DockMgr_t mgr, DockCtrl_t ctrl, float float_x, float float_y, DockCtrl::Enum wrapper );
}
