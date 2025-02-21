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
#include "DockCtrl_Typed.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	DockCtrl_t DockMgr_CreateCtrl( DockMgr_t mgr, DockCtrl::Enum kind, cstr_t text, const Rect_s& rect, bool show, const DockCtrlApi_v& api )
	{
		DockCtrl_t ctrl = Mem_Calloc<DockCtrl_s>( mgr->Alloc );
		DockCtrl_Initialize( ctrl, mgr, kind, text, rect, show, api );
		mgr->Ctrls.Append( ctrl );
		return ctrl;
	}

	void DockMgr_Drag( DockMgr_t mgr, DockCtrl_t ctrl, const Vec2_s& screen_pos, DragAction::Enum action )
	{
		NeAssert(mgr == ctrl->Mgr);
		switch (action)
		{
		case DragAction::Push:
			{
				NeAssert(mgr->Drag.Source == nullptr);
				mgr->Drag.Source = ctrl;
				mgr->Drag.Action = action;
				mgr->Drag.BeginPos = screen_pos;
				mgr->Drag.MousePos = screen_pos;
				mgr->Drag.Threshold = mgr->Theme.TearThreshold;
			}
			break;
		case DragAction::Move:
			{
				NeAssert(mgr->Drag.Source == ctrl);
				mgr->Drag.MousePos = screen_pos;
			}
			break;
		case DragAction::Release:
			{
				NeAssert(mgr->Drag.Source == ctrl);
				DockMgr_QueueDrop( mgr );
				mgr->Drag = {};
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	void DockMgr_BeginDrop( DockMgr_t mgr )
	{
		// flip targets
		mgr->Drag.PrevTarget = mgr->Drag.ThisTarget;
		mgr->Drag.ThisTarget = nullptr;

		// update the overlay
		if (mgr->Drag.PrevTarget)
		{
			// calculate overlay placement
			const Rect_s overlay_screen_rect = OverlayCtrl_CalcScreenRect( mgr->Drag.PrevTarget );

			// update the overlay
			mgr->Overlay->FloatPos = { overlay_screen_rect.x, overlay_screen_rect.y };
			mgr->Overlay->LocalRect = { 0.0f, 0.0f, overlay_screen_rect.w, overlay_screen_rect.h };
			mgr->Overlay->Hidden = false;
			mgr->Platform.Host_Move( mgr->Overlay->Host );
			mgr->Platform.Host_Show( mgr->Overlay->Host );
		}
		else
		{
			// hide the overlay
			mgr->Overlay->Hidden = true;
			mgr->Platform.Host_Show( mgr->Overlay->Host );
		}
	}

	void DockMgr_SetDropTarget( DockMgr_t mgr, DockCtrl_t ctrl )
	{
		if (!mgr->Drag.Source)
			return;
		if (ctrl == mgr->Drag.Source)
			return;
		if (DockCtrl_IsAncestor( ctrl, mgr->Drag.Source ))
			return;
		mgr->Drag.ThisTarget = ctrl;
	}

	void DockMgr_QueueDrop( DockMgr_t mgr )
	{
		NeAssert(mgr->Pending.DropSide == DockSide::None);
		NeAssert(!mgr->Pending.DragSource);
		NeAssert(!mgr->Pending.DropTarget);
		if (mgr->Drag.DropSide == DockSide::None)
			return;
		mgr->Pending.DropSide = mgr->Drag.DropSide;
		mgr->Pending.DropArea = mgr->Drag.DropArea;
		mgr->Pending.DragSource = mgr->Drag.Source;
		mgr->Pending.DropTarget = mgr->Drag.PrevTarget;
	}

	void DockMgr_QueueTearOff( DockMgr_t mgr, DockCtrl_t ctrl, float float_x, float float_y, DockCtrl::Enum wrapper )
	{
		NeAssert(!ctrl->Mgr->Pending.TearOff);
		ctrl->FloatPos.x = floorf(float_x);
		ctrl->FloatPos.y = floorf(float_y);
		ctrl->Mgr->Pending.TearOff = ctrl;
		ctrl->Mgr->Pending.TearWrap = wrapper;
	}
}