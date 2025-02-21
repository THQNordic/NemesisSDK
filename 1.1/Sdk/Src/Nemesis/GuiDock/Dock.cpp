//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Docking/DockMgr.h"
#include "Docking/DockTool.h"
#include "Docking/DockHost_Null.h"
#include "Docking/DockHost_Win32.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
#define USE_NULL_OS	0

//======================================================================================
//																				 Private
//======================================================================================
namespace nemesis
{
	static void DockMgr_ExecKilled( DockMgr_t mgr )
	{
		NeAssert(!mgr->Order.Count);
		const int num_killed = mgr->Kill.Count;
		for ( int i = 0; i < num_killed; ++i )
			DockCtrl_Release( mgr->Kill[i] );
		mgr->Kill.Count = 0;
	}

	static void DockMgr_AttachHosts( DockMgr_t mgr )
	{
		for ( int i = 0; i < mgr->Ctrls.Count; ++i )
		{
			if (mgr->Ctrls[i]->Parent)
			{
				NeAssert(!mgr->Ctrls[i]->Host);
				continue;
			}
			DockCtrl_AttachHost( mgr->Ctrls[i] );
		}
	}

	static void DockMgr_RenderHosts( DockMgr_t mgr )
	{
		// gather hosts
		mgr->Order.Count = 0;
		mgr->Order.Reserve( mgr->Ctrls.Count );
		mgr->Order.Count = mgr->Platform.Os_OrderZ( mgr->Os, mgr->Ctrls.Data, mgr->Ctrls.Count, mgr->Order.Data );

		// render hosts in reverse z-order
		for ( int i = mgr->Order.Count-1; i >= 0; --i )
			mgr->Platform.Host_Render( mgr->Order[i] );

		// reset hosts
		mgr->Order.Count = 0;

		// release controls
		DockMgr_ExecKilled( mgr );
	}

	static void DockMgr_ExecPending( DockMgr_t mgr )
	{
		if (mgr->Pending.TearOff)
		{
			DockTool_TearOff( mgr->Pending.TearOff, mgr->Pending.TearWrap );
			mgr->Pending.TearOff = nullptr;
			mgr->Pending.TearWrap = DockCtrl::None;
		}

		if (mgr->Pending.DropSide != DockSide::None)
		{
			DockTool_Dock( mgr->Pending.DropTarget, mgr->Pending.DragSource, mgr->Pending.DropSide, mgr->Pending.DropArea );
			mgr->Pending.DropSide = DockSide::None;
			mgr->Pending.DropArea = DockArea::Default;
			mgr->Pending.DragSource = nullptr;
			mgr->Pending.DropTarget = nullptr;
		}
	}

	static void DockMgr_ReleaseUserCtrls( DockMgr_t mgr )
	{
		for ( ;; )
		{
			DockCtrl_t victim = nullptr;
			for ( int i = 0; i < mgr->Ctrls.Count; ++i )
			{
				DockCtrl_t ctrl = mgr->Ctrls[i];
				if (ctrl->Kind != DockCtrl::User)
					continue;
				victim = ctrl;
				break;
			}
			if (!victim)
				return;
			DockCtrl_Release( victim );
		}
	}

}

//======================================================================================
//																				  DockOs
//======================================================================================
namespace nemesis
{
	DockMgrApi_Os_s	DockOs_GetNull()
	{
		return DockOs_Null_GetApi();
	}

	DockMgrApi_Os_s	DockOs_GetWin32()
	{
		return DockOs_Win32_GetApi();
	}
}

//======================================================================================
//																				 DockMgr
//======================================================================================
namespace nemesis
{
	DockMgr_t DockMgr_Create( const DockMgrSetup_s& setup )
	{
		if (!setup.AppWnd)
			return nullptr;

		cstr_t title = setup.AppTitle ? setup.AppTitle : "(Unknown)";

		DockMgr_t mgr = Mem_Calloc<DockMgr_s>( setup.Alloc );
		DockTheme_Initialize( &mgr->Theme );

		mgr->Alloc		= setup.Alloc;
		mgr->AppWnd		= setup.AppWnd;
		mgr->AppTitle	= (str_t)Mem_Clone( mgr->Alloc, title, 1+Str_Len(title) );
		mgr->Instance	= setup.Instance;
		mgr->Graphics	= setup.Graphics;
	#if USE_NULL_OS
		mgr->Platform	= DockOs_Null_GetApi();
	#else
		mgr->Platform	= setup.Platform;
	#endif
		mgr->Os			= mgr->Platform.Os_Create( mgr->Platform.User );
		mgr->Id			= NE_UI_ID;
		mgr->Dc			= System_GetContext();
		mgr->NextId		= 0;			
		mgr->Ctrls		.Init( setup.Alloc );
		mgr->Order		.Init( setup.Alloc );
		mgr->Temp		.Init( setup.Alloc );
		mgr->Kill		.Init( setup.Alloc );
		mgr->Drag		= {};
		mgr->Pending	= {};
		mgr->Main		= DockMgr_CreateCtrl( mgr, DockCtrl::Main	, nullptr	, setup.AppRect			, true, {} );
		mgr->Overlay	= DockMgr_CreateCtrl( mgr, DockCtrl::Overlay, "Overlay"	, { 400, 100, 100, 100 }, true, {} );

		DockCtrl_AttachHost( mgr->Main );

		return mgr;
	}

	DockCtrl_t DockMgr_GetMain( DockMgr_t mgr )
	{
		return mgr->Main;
	}

	Context_t DockMgr_GetDc( DockMgr_t mgr )
	{
		return mgr->Dc;
	}

	void DockMgr_Resize( DockMgr_t mgr, const Rect_s& r )
	{
		DockCtrl_SetLocalRect( mgr->Main, r );
	}

	void DockMgr_Begin( DockMgr_t mgr, const MouseState_s& mouse, const KeyboardState_s& kb )
	{
		mgr->Graphics.Scene_Begin( mgr->Graphics.User );
		mgr->Graphics.Scene_Clear( mgr->Graphics.User, 0xff777777 );
		Context_BeginFrame( mgr->Dc, mgr->Main->LocalRect, mouse, kb );
	}

	void DockMgr_Draw( DockMgr_t mgr )
	{
		DockMgr_BeginDrop  ( mgr );
		DockMgr_AttachHosts( mgr );
		DockMgr_RenderHosts( mgr );
		DockMgr_ExecPending( mgr );
	}

	void DockMgr_End( DockMgr_t mgr )
	{
		DrawBatch_s batch = {};
		Context_EndFrame( mgr->Dc, &batch );
		mgr->Graphics.Scene_Draw	( mgr->Graphics.User, &batch, 1 );
		mgr->Graphics.Scene_End		( mgr->Graphics.User );
		mgr->Graphics.Scene_Present	( mgr->Graphics.User );
	}

	void DockMgr_Do( DockMgr_t mgr, const Rect_s& r, const MouseState_s& mouse, const KeyboardState_s& kb )
	{
		DockMgr_Resize( mgr, r );
		DockMgr_Begin( mgr, mouse, kb );
		DockMgr_Draw( mgr );
		DockMgr_End( mgr );
	}

	void DockMgr_Release( DockMgr_t mgr )
	{
		if (!mgr)
			return;
		DockMgr_ExecKilled( mgr );
		DockMgr_ReleaseUserCtrls( mgr );
		DockCtrl_Release( mgr->Overlay );
		DockCtrl_Release( mgr->Main );
		mgr->Overlay = nullptr;
		mgr->Main = nullptr;
		NeAssert(mgr->Ctrls.Count == 0);
		mgr->Ctrls.Clear();
		mgr->Order.Clear();
		mgr->Temp .Clear();
		mgr->Kill .Clear();
		mgr->Platform.Os_Release( mgr->Os );
		Mem_Free( mgr->Alloc, (str_t)mgr->AppTitle );
		Mem_Free( mgr->Alloc, mgr );
	}

}

//======================================================================================
//																				DockCtrl
//======================================================================================
namespace nemesis
{
	DockCtrl_t DockCtrl_Create( DockMgr_t mgr, const DockCtrlSetup_s& setup )
	{
		const Rect_s r = 
			{ (float)setup.Rect.x
			, (float)setup.Rect.y
			, (float)setup.Rect.w
			, (float)setup.Rect.h
			};
		return DockMgr_CreateCtrl( mgr, DockCtrl::User, setup.Text, r, true, setup.Api );
	}

	Id_t DockCtrl_GetId( DockCtrl_t ctrl )
	{
		return ctrl->Id;
	}

	Context_t DockCtrl_GetDc( DockCtrl_t ctrl )
	{
		return ctrl->Dc;
	}

	DockMgr_t DockCtrl_GetMgr( DockCtrl_t ctrl )
	{
		return ctrl->Mgr;
	}

	cstr_t DockCtrl_GetText( DockCtrl_t ctrl )
	{
		return ctrl->Text;
	}

	Vec2_s DockCtrl_GetFloatPos( DockCtrl_t ctrl )
	{
		return ctrl->FloatPos;
	}

	Rect_s DockCtrl_GetLocalRect( DockCtrl_t ctrl )
	{
		return ctrl->LocalRect;
	}

	Rect_s DockCtrl_GetScreenRect( DockCtrl_t ctrl )
	{
		Rect_s screen_rect = ctrl->LocalRect;
		DockCtrl_t root = ctrl;
		for ( DockCtrl_t parent = ctrl->Parent; parent; parent = parent->Parent )
		{
			root = parent;
			screen_rect.x += parent->LocalRect.x;
			screen_rect.y += parent->LocalRect.y;
		}
		screen_rect.x += root->FloatPos.x;
		screen_rect.y += root->FloatPos.y;
		return screen_rect;
	}

	bool DockCtrl_IsFloating( DockCtrl_t ctrl )
	{
		return ctrl->Parent == nullptr;
	}

	DockScroll_s DockCtrl_GetScrollInfo( DockCtrl_t ctrl )
	{
		return ctrl->ScrollState;
	}

	void DockCtrl_SetScrollInfo( DockCtrl_t ctrl, const DockScroll_s& scroll )
	{
		ctrl->ScrollState = scroll;
	}

	void DockCtrl_Dock( DockCtrl_t ctrl, DockCtrl_t other, DockSide::Enum side )
	{
		const DockArea::Enum area = DockCtrl_IsMain(ctrl) 
			? DockArea::Outer
			: DockArea::Default;
		DockTool_Dock( ctrl, other, side, area );
	}

	void DockCtrl_Undock( DockCtrl_t ctrl )
	{
		DockTool_Undock( ctrl );
	}

	void DockCtrl_Destroy( DockCtrl_t ctrl )
	{
		if (!ctrl)
			return;
		if (!ctrl->Mgr->Order.Count)
			return DockCtrl_Release( ctrl );
		ctrl->Mgr->Kill.Append( ctrl );
	}

}

//======================================================================================
namespace nemesis
{
	DockCtrl_t	DockCtrl_Dbg_GetParent	 ( DockCtrl_t ctrl ) { return ctrl->Parent; }
	Vec2_s		DockCtrl_Dbg_GetFloatPos ( DockCtrl_t ctrl ) { return ctrl->FloatPos; }
	Rect_s		DockCtrl_Dbg_GetLocalRect( DockCtrl_t ctrl ) { return ctrl->LocalRect; }
	Pad_s		DockCtrl_Dbg_GetNonClient( DockCtrl_t ctrl ) { return ctrl->NonClient; }
	float		DockCtrl_Dbg_GetSplitSize( DockCtrl_t ctrl ) { return ctrl->SplitSize; }

}

//======================================================================================
namespace nemesis
{
	int			DockMgr_Dbg_GetNumCtrls	 ( DockMgr_t mgr )			{ return mgr->Ctrls.Count; }
	DockCtrl_t	DockMgr_Dbg_GetCtrl		 ( DockMgr_t mgr, int idx )	{ return mgr->Ctrls[idx]; }
	DockCtrl_t	DockMgr_Dbg_GetDragSource( DockMgr_t mgr )			{ return mgr->Drag.Source; }
	DockCtrl_t	DockMgr_Dbg_GetDragTarget( DockMgr_t mgr )			{ return mgr->Drag.PrevTarget; }
	Vec2_s		DockMgr_Dbg_GetDragBegin ( DockMgr_t mgr )			{ return mgr->Drag.BeginPos; }
	Vec2_s		DockMgr_Dbg_GetDragMouse ( DockMgr_t mgr )			{ return mgr->Drag.MousePos; }
}