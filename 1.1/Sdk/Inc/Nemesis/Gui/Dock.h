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
#include "DockTypes.h"

//======================================================================================
namespace nemesis
{
	DockMgrApi_Os_s	NE_API DockOs_GetNull ();
	DockMgrApi_Os_s	NE_API DockOs_GetWin32();
}

//======================================================================================
namespace nemesis
{
	DockMgr_t		NE_API DockMgr_Create ( const DockMgrSetup_s& setup );
	DockCtrl_t		NE_API DockMgr_GetMain( DockMgr_t mgr );
	gui::Context_t	NE_API DockMgr_GetDc  ( DockMgr_t mgr );
	void			NE_API DockMgr_Resize ( DockMgr_t mgr, const Rect_s& r );
	void			NE_API DockMgr_Begin  ( DockMgr_t mgr, const MouseState_s& mouse, const KeyboardState_s& kb );
	void			NE_API DockMgr_Draw	  ( DockMgr_t mgr );
	void			NE_API DockMgr_End    ( DockMgr_t mgr );
	void			NE_API DockMgr_Release( DockMgr_t mgr );
}

//======================================================================================
namespace nemesis
{
	void NE_API DockMgr_Do( DockMgr_t mgr, const Rect_s& r, const MouseState_s& mouse, const KeyboardState_s& kb );
}

//======================================================================================
namespace nemesis
{
	DockCtrl_t		NE_API DockCtrl_Create			( DockMgr_t mgr, const DockCtrlSetup_s& setup );
	gui::Id_t		NE_API DockCtrl_GetId			( DockCtrl_t ctrl );
	gui::Context_t	NE_API DockCtrl_GetDc			( DockCtrl_t ctrl );
	DockMgr_t		NE_API DockCtrl_GetMgr			( DockCtrl_t ctrl );
	cstr_t			NE_API DockCtrl_GetText			( DockCtrl_t ctrl );
	cstr_t			NE_API DockCtrl_GetSideString	( DockCtrl_t ctrl );
	cstr_t			NE_API DockCtrl_GetKindString	( DockCtrl_t ctrl );
	Vec2_s			NE_API DockCtrl_GetFloatPos		( DockCtrl_t ctrl );
	Rect_s			NE_API DockCtrl_GetLocalRect	( DockCtrl_t ctrl );
	Rect_s			NE_API DockCtrl_GetScreenRect	( DockCtrl_t ctrl );
	bool			NE_API DockCtrl_IsFloating		( DockCtrl_t ctrl );
	DockScroll_s	NE_API DockCtrl_GetScrollInfo	( DockCtrl_t ctrl );
	void			NE_API DockCtrl_SetScrollInfo	( DockCtrl_t ctrl, const DockScroll_s& scroll );
	void			NE_API DockCtrl_Dock			( DockCtrl_t ctrl, DockCtrl_t other, DockSide::Enum side );
	void			NE_API DockCtrl_Undock			( DockCtrl_t ctrl );
	void			NE_API DockCtrl_Destroy			( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	DockCtrl_t	NE_API DockCtrl_Dbg_GetParent	( DockCtrl_t ctrl );
	Vec2_s		NE_API DockCtrl_Dbg_GetFloatPos ( DockCtrl_t ctrl );
	Rect_s		NE_API DockCtrl_Dbg_GetLocalRect( DockCtrl_t ctrl );
	Pad_s		NE_API DockCtrl_Dbg_GetNonClient( DockCtrl_t ctrl );
	float		NE_API DockCtrl_Dbg_GetSplitSize( DockCtrl_t ctrl );

}

//======================================================================================
namespace nemesis
{
	int			NE_API DockMgr_Dbg_GetNumCtrls	( DockMgr_t mgr );
	DockCtrl_t	NE_API DockMgr_Dbg_GetCtrl		( DockMgr_t mgr, int idx );
	DockCtrl_t	NE_API DockMgr_Dbg_GetDragSource( DockMgr_t mgr );
	DockCtrl_t	NE_API DockMgr_Dbg_GetDragTarget( DockMgr_t mgr );
	Vec2_s		NE_API DockMgr_Dbg_GetDragBegin ( DockMgr_t mgr );
	Vec2_s		NE_API DockMgr_Dbg_GetDragMouse ( DockMgr_t mgr );
}