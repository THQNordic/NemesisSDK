//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "DockMgr.h"
#include "DockGui.h"
#include "DockTool.h"
#include "DockCtrl_Layout.h"
#include "DockCtrl_Typed.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
//																				    Rect
//======================================================================================
namespace nemesis
{
	static Rect_s Rect_Margin( const Rect_s& r, float x, float y )
	{
		return Rect_Margin( r, x, y, x, y );
	}

	static Rect_s Rect_Margin( const Rect_s& r, const Pad_s& pad )
	{
		return Rect_Margin( r, pad.l, pad.t, pad.r, pad.b );
	}
}

//======================================================================================
//																				    Drag
//======================================================================================
namespace nemesis
{
	static void DockMgr_Drag( DockCtrl_t ctrl, Id_t caption_id, PosGripAction::Enum action, const Vec2_s& screen_pos )
	{
		switch (action)
		{
		case PosGripAction::Pushed:
			DockMgr_Drag( ctrl->Mgr, ctrl, screen_pos, DragAction::Push );
			break;
		case PosGripAction::Dragged:
			{
				DockMgr_Drag( ctrl->Mgr, ctrl, screen_pos, DragAction::Move );
				if (ctrl->Parent)
				{
					// @note:
					//	this is the tear-off for docked controls
					//	and those never have to be wrapped in a 
					//	page control
					const Vec2_s diff = screen_pos - ctrl->Mgr->Drag.BeginPos;
					const bool is_tear 
						= ((fabsf(diff.x) > ctrl->Mgr->Theme.TearThreshold) 
						|| (fabsf(diff.y) > ctrl->Mgr->Theme.TearThreshold));
					if (is_tear)
						DockMgr_QueueTearOff( ctrl->Mgr, ctrl, screen_pos.x, screen_pos.y, DockCtrl::None );
				}
				else
				{
					ctrl->FloatPos.x = floorf(screen_pos.x);
					ctrl->FloatPos.y = floorf(screen_pos.y);
					ctrl->Mgr->Platform.Host_Move( ctrl->Host );
				}
			}
			break;
		case PosGripAction::Released:
			DockMgr_Drag( ctrl->Mgr, ctrl, screen_pos, DragAction::Release );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	static void TabCtrl_OnDrag( DockCtrl_t parent, DockCtrl_t ctrl, Id_t caption_id, PosGripAction::Enum action, const Vec2_s& screen_pos )
	{
		NeAssert(DockCtrl_IsTab(parent));
		const bool is_floating = (parent->Parent == nullptr);
		const Vec2_s actual_pos = 
			{ screen_pos.x - (is_floating ? parent->NonClient.l : 0.0f)
			, screen_pos.y - (is_floating ? parent->NonClient.t : 0.0f)
			};
		DockMgr_Drag( parent, caption_id, action, actual_pos );
	}

	static void SplitterCtrl_OnDrag( DockCtrl_t parent, DockCtrl_t ctrl, Id_t caption_id, PosGripAction::Enum action, const Vec2_s& screen_pos )
	{
		NeAssert(DockCtrl_IsSplitter(parent));
		return DockMgr_Drag( ctrl, caption_id, action, screen_pos );
	}

	static void MainCtrl_OnDrag( DockCtrl_t parent, DockCtrl_t ctrl, Id_t caption_id, PosGripAction::Enum action, const Vec2_s& screen_pos )
	{
		NeAssert(DockCtrl_IsMain(parent));
		return DockMgr_Drag( ctrl, caption_id, action, screen_pos );
	}

	static void DockCtrl_OnDrag( DockCtrl_t parent, DockCtrl_t ctrl, Id_t caption_id, PosGripAction::Enum action, const Vec2_s& screen_pos )
	{
		if (!parent)
			return DockMgr_Drag( ctrl, caption_id, action, screen_pos );

		switch (parent->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return TabCtrl_OnDrag( parent, ctrl, caption_id, action, screen_pos );
		case DockCtrl::Split_LR:
		case DockCtrl::Split_TB:
			return SplitterCtrl_OnDrag( parent, ctrl, caption_id, action, screen_pos );
		case DockCtrl::Main:
			return MainCtrl_OnDrag( parent, ctrl, caption_id, action, screen_pos );
		default:
			IMPLEMENT_ME;
			break;
		}
	}
}

//======================================================================================
//																				  Shared
//======================================================================================
namespace nemesis
{
	static bool DockCtrl_HasFrame( DockCtrl_t ctrl )
	{
		return (ctrl->NonClient.l != 0.0f)
			|| (ctrl->NonClient.t != 0.0f)
			|| (ctrl->NonClient.r != 0.0f)
			|| (ctrl->NonClient.b != 0.0f);
	}

	static void DockCtrl_DoCaption( DockCtrl_t ctrl )
	{
		const Id_t caption_id = Id_Cat( ctrl->Id, "Caption" );
		const Rect_s sr = DockCtrl_GetScreenRect( ctrl );
		const Context_t dc = ctrl->Dc;
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Rect_s caption_rect = Context_GetChild( dc );
		const bool is_active = ctrl->Mgr->Platform.Host_IsActive( ctrl->Host );
		const CtrlState::Enum ctrl_state 
			= is_active 
			? CtrlState::Pushed 
			: Ctrl_GetState( dc, caption_id );

		// draw
		Caption_Draw( dc, caption_id, caption_rect, ctrl->Text, ctrl_state );
	//	Context_DrawRect( dc, caption_rect, Color::Red );

		// input
		const Rect_s rc_screen = DockCtrl_GetScreenRect( ctrl );
		const Vec2_s pt_screen = { rc_screen.x, rc_screen.y };
		Vec2_s pos = pt_screen;
		const PosGripAction::Enum action = PosGrip_Mouse( dc, caption_id, caption_rect, pos );
		if (!action)
			return;

		// have the parent do this on our behalf
		DockCtrl_OnDrag( ctrl->Parent, ctrl, caption_id, action, pos );
	}

	static void DockCtrl_DoCaption( DockCtrl_t ctrl, const Rect_s& r )
	{
		NeGuiScopedChild( ctrl->Dc, r );
		DockCtrl_DoCaption( ctrl );
	}

	static void DockCtrl_DoBorder( DockCtrl_t ctrl )
	{
		if (ctrl->Parent)
			return;
		const Context_t dc = ctrl->Dc;
		const Rect_s r = Context_GetChild( dc );
		const Id_t border_id = Id_Cat( ctrl->Id, "Border" );
		const DockTheme_t th = &ctrl->Mgr->Theme;
		Rect_s screen_rect = DockCtrl_GetScreenRect( ctrl );
		if (Border_Do( dc, border_id, r, th->FrameBorder, th->MinFloatSize, screen_rect ))
			ctrl->Mgr->Platform.Host_Move_Rect( ctrl->Host, screen_rect );
	}
}

//======================================================================================
//																					User
//======================================================================================
namespace nemesis
{
	static Rect_s UserCtrl_CalcInnerRect( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s r = Context_GetChild( ctrl->Dc );
		const bool has_frame = (ctrl->Parent == nullptr);
		return has_frame 
			? Rect_Margin( r, ctrl->Mgr->Theme.FrameBorder ) 
			: r;
	}

	static Rect_s UserCtrl_CalcCaptionRect( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s inner_rect = UserCtrl_CalcInnerRect( ctrl );
		const float caption_h = ctrl->NoCaption ? 0.0f : Caption_CalcHeight( ctrl->Dc );
		return Rect_s { inner_rect.x, inner_rect.y, inner_rect.w, caption_h };
	}

	static Rect_s UserCtrl_CalcChildRect( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s inner_rect = UserCtrl_CalcInnerRect( ctrl );
		const float caption_h = ctrl->NoCaption ? 0.0f : Caption_CalcHeight( ctrl->Dc );
		return Rect_s { inner_rect.x, inner_rect.y + caption_h, inner_rect.w, inner_rect.h - caption_h };
	}

	static void UserCtrl_DoFrame( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s caption_rect = UserCtrl_CalcCaptionRect( ctrl );
		DockCtrl_DoBorder( ctrl );
		DockCtrl_DoCaption( ctrl, caption_rect );
	}

	static void UserCtrl_DoClient( DockCtrl_t ctrl )
	{
		if (!ctrl->Api.Do)
			return;
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s child_rect = UserCtrl_CalcChildRect( ctrl );
		NeGuiScopedChild( ctrl->Dc, child_rect );
		const Rect_s local_rect = Context_GetChild( ctrl->Dc );
		ScrollPanel_Begin( ctrl->Dc, ctrl->Id, local_rect, ctrl->ScrollState.Size, ctrl->ScrollState.Offset );
		ctrl->Api.Do( ctrl, ctrl->Api.User );
		ScrollPanel_End( ctrl->Dc, ctrl->Id, local_rect, ctrl->ScrollState.Size, ctrl->ScrollState.Step, ctrl->ScrollState.Offset );
	}

	static void UserCtrl_DoDrop( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		const Rect_s r = Context_GetChild( ctrl->Dc );
		const Vec2_s mouse_pos = Context_GetMousePos( ctrl->Dc );
		if (!Rect_Contains( r, mouse_pos ))
			return;
		DockMgr_SetDropTarget( ctrl->Mgr, ctrl );
	}

	static void UserCtrl_Do( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		UserCtrl_DoDrop  ( ctrl );
		UserCtrl_DoFrame ( ctrl );
		UserCtrl_DoClient( ctrl );
	}

}

//======================================================================================
//																					 Tab
//======================================================================================
namespace nemesis
{
	static void TabCtrl_Do( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		DockCtrl_DoBorder( ctrl );
		TabCtrl_DoClient ( ctrl );
	}
}

//======================================================================================
//																				Splitter
//======================================================================================
namespace nemesis
{
	static Rect_s SplitterCtrl_CalcCaptionRect( DockCtrl_t ctrl )
	{
		const Context_t dc = ctrl->Dc;
		const float caption_h = Caption_CalcHeight( ctrl->Dc );
		const Rect_s outer_rect = Context_GetChild( ctrl->Dc );
		const Rect_s inner_rect = Rect_Margin( outer_rect, ctrl->Mgr->Theme.FrameBorder );
		const Rect_s caption_rect = { inner_rect.x, inner_rect.y, inner_rect.w, caption_h };
		return caption_rect;
	}

	static void SplitterCtrl_DoFrame( DockCtrl_t ctrl )
	{
		const bool has_frame = DockCtrl_HasFrame( ctrl );
		if (!has_frame)
			return;
		NeAssert(!ctrl->Parent);
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Rect_s caption_rect = SplitterCtrl_CalcCaptionRect( ctrl );
		DockCtrl_DoBorder( ctrl );
		DockCtrl_DoCaption( ctrl, caption_rect );
	}

	static void SplitterCtrl_Do( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		SplitterCtrl_DoFrame ( ctrl );
		SplitterCtrl_DoClient( ctrl );
	}
}

//======================================================================================
//																					Main
//======================================================================================
namespace nemesis
{
	static void MainCtrl_DoFrame( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		const Context_t dc = ctrl->Dc;
		const Rect_s r = Context_GetChild( dc );
		Ctrl_DrawBox( dc, r, Visual::Window, CtrlState::Normal );
	}

	static void MainCtrl_DoDrop( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		const Vec2_s mouse_pos = Context_GetMousePos( ctrl->Dc );
		if (!Rect_Contains( ctrl->RemainRect, mouse_pos ))
			return;
		DockMgr_SetDropTarget( ctrl->Mgr, ctrl );
	}

	static void MainCtrl_Do( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		MainCtrl_DoDrop  ( ctrl );
		MainCtrl_DoFrame ( ctrl );
		MainCtrl_DoClient( ctrl );
	}
}

//======================================================================================
//																				   Typed
//======================================================================================
namespace nemesis
{
	void DockCtrl_DoGui( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return TabCtrl_Do( ctrl );
		case DockCtrl::Split_LR:
		case DockCtrl::Split_TB:
			return SplitterCtrl_Do( ctrl );
		case DockCtrl::User:
			return UserCtrl_Do( ctrl );
		case DockCtrl::Overlay:
			return OverlayCtrl_Do( ctrl );
		case DockCtrl::Main:
			return MainCtrl_Do( ctrl );
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	Pad_s DockCtrl_CalcNonClient( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
			return MainCtrl_CalcNonClient( ctrl );
		case DockCtrl::User:
			return UserCtrl_CalcNonClient( ctrl );
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return TabCtrl_CalcNonClient( ctrl );
		case DockCtrl::Split_LR:
		case DockCtrl::Split_TB:
			return SplitterCtrl_CalcNonClient( ctrl );
		default:
			IMPLEMENT_ME;
			break;
		}
		return Pad_s {};
	}
}
