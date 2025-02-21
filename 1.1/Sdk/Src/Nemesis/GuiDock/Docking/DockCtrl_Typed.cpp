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
//																					Ctrl
//======================================================================================
namespace nemesis
{
	static DockCtrl_t DockCtrl_GetFirstChild( DockCtrl_t ctrl, DockSide::Enum side )
	{
		for ( DockCtrl_t next = ctrl->FirstChild; next; next = next->NextSibling )
			if (next->DockSide == side)
				return next;
		return nullptr;
	}

	static DockCtrl_t DockCtrl_GetPrevSibling( DockCtrl_t ctrl, DockSide::Enum side )
	{
		for ( DockCtrl_t prev = ctrl->PrevSibling; prev; prev = prev->PrevSibling )
			if (prev->DockSide == side)
				return prev;
		return nullptr;
	}

	static DockCtrl_t DockCtrl_GetNextSibling( DockCtrl_t ctrl, DockSide::Enum side )
	{
		for ( DockCtrl_t next = ctrl->NextSibling; next; next = next->NextSibling )
			if (next->DockSide == side)
				return next;
		return nullptr;
	}

	static Id_t DockCtrl_MakeSplitterId( DockCtrl_t ctrl, DockCtrl_t child )
	{
		return Id_Cat( Id_Cat( ctrl->Id, "Splitter" ), child->Id );
	}

	static Id_t DockCtrl_MakePageId( DockCtrl_t ctrl )
	{
		return Id_Cat( ctrl->Id, "Page" );
	}
}

//======================================================================================
//																				 Generic
//======================================================================================
namespace nemesis
{
	static Rect_s GenericCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side )
	{
		const Rect_s r = DockCtrl_GetScreenRect( ctrl );
		const float ctl_w = DockCtrl_GetClientW( ctrl );
		const float ctl_h = DockCtrl_GetClientH( ctrl );
		const float rel_w = NeMin( 0.5f, insert->LocalRect.w / ctl_w );
		const float rel_h = NeMin( 0.5f, insert->LocalRect.h / ctl_h );
		const float ins_w = rel_w * ctl_w;
		const float ins_h = rel_h * ctl_h;
		const float rem_w = ctl_w - ins_w;
		const float rem_h = ctl_h - ins_h;
		switch (side)
		{
		case DockSide::Left:	return Rect_s { r.x, r.y, ins_w, r.h };
		case DockSide::Top:		return Rect_s { r.x, r.y, r.w, ins_h };
		case DockSide::Right:	return Rect_s { r.x + ins_w, r.y, rem_w, r.h };
		case DockSide::Bottom:	return Rect_s { r.x, r.y + ins_h, r.w, rem_h };
		case DockSide::Center:	return Rect_s { r.x, r.y, r.w, r.h };
		default:
			IMPLEMENT_ME;
			break;
		}
		return r;
	}

}

//======================================================================================
//																					User
//======================================================================================
namespace nemesis
{
	Pad_s UserCtrl_CalcNonClient( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		return {};
	}

	Rect_s UserCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side )
	{
		NeAssert(ctrl->Kind == DockCtrl::User);
		return GenericCtrl_CalcInsertRect( ctrl, insert, side );
	}
}

//======================================================================================
//																					Page
//======================================================================================
namespace nemesis
{
	struct PageAction
	{
		enum Enum
		{ None
		, After
		, Before
		, Tear
		};
	};
}

//======================================================================================
//																					 Tab
//======================================================================================
namespace nemesis
{
	static void TabCtrl_ActivatePage( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		if (ctrl->ActivePage)
			return;
		ctrl->ActivePage = ctrl->FirstChild;
	}

	static bool TabCtrl_IsBottom( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Page_B:
			break;
		case DockCtrl::Tab_T:
		case DockCtrl::Page_T:
			return false;
		default:
			IMPLEMENT_ME;
			break;
		}
		return true;
	}

	static Rect_s TabCtrl_CalcTabRect( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		const Rect_s r = Context_GetChild( ctrl->Dc );
		const Rect_s inner_rect = Rect_Margin( r, ctrl->NonClient );
		const float tab_h_small = TabItem_CalcHeight( ctrl->Dc, TabStyle::None );
		const float tab_h_large = TabItem_CalcHeight( ctrl->Dc, TabStyle::Enlarge );
		const bool is_bottom = TabCtrl_IsBottom( ctrl );
		const Rect_s tab_rect = is_bottom 
			? Rect_SplitB( inner_rect, tab_h_large ).Rect[0]
			: Rect_SplitT( inner_rect, tab_h_small ).Rect[0];
		return tab_rect;
	}

	static float TabCtrl_CalcTabWidth( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		const Context_t dc = ctrl->Dc;
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const float tab_space = th->TabItemSpacing.x;
		const Rect_s r = Context_GetChild( dc );

		float total_w = 0.0f;
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			const Text_s item_text = child->Text;
			const TabStyle::Mask_t item_style = TabStyle::None;
			total_w += TabItem_CalcWidth( dc, item_style, item_text );
			if (child->NextSibling)
				total_w += tab_space;
		}
		return total_w;
	}

	void TabCtrl_UpdateActivePage( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		if (ctrl->ActivePage != remove)
			return;
		DockCtrl_t next = ctrl->ActivePage->NextSibling;
		DockCtrl_t prev = ctrl->ActivePage->PrevSibling;
		if (next)
			ctrl->ActivePage = next;
		else
			ctrl->ActivePage = prev;
	}

	Pad_s TabCtrl_CalcNonClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const bool want_frame = (ctrl->Parent == nullptr);
		return want_frame 
			? Pad_s
				{ th->FrameBorder.x
				, th->FrameBorder.y
				, th->FrameBorder.x
				, th->FrameBorder.y
				}
			: Pad_s {};
	}

	Rect_s TabCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		NeAssert(child->Parent == ctrl);
		switch (side)
		{
		case DockSide::Left:
		case DockSide::Top:
		case DockSide::Right:
		case DockSide::Bottom:
			return GenericCtrl_CalcInsertRect( ctrl, insert, side );
		case DockSide::Center:
			return DockCtrl_GetScreenRect( child );
		default:
			IMPLEMENT_ME;
			break;
		}
		return DockCtrl_GetScreenRect( ctrl );
	}

	void TabCtrl_Layout( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		NeAssert(!insert || !remove);
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Page_B:
			{
				const float tab_h_large = TabItem_CalcHeight( ctrl->Dc, TabStyle::Enlarge );
				const Rect_s client_rect = DockCtrl_GetClientRect( ctrl );
				const Rect_s page_rect = Rect_Margin( client_rect, 0.0f, 0.0f, 0.0f, tab_h_large );
				for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
					DockCtrl_SetLocalRect( child, page_rect );
			}
			break;
		case DockCtrl::Tab_T:
		case DockCtrl::Page_T:
			{
				const float tab_h_small = TabItem_CalcHeight( ctrl->Dc, TabStyle::None );
				const Rect_s client_rect = DockCtrl_GetClientRect( ctrl );
				const Rect_s page_rect = Rect_Margin( client_rect, 0.0f, tab_h_small, 0.0f, 0.0f );
				for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
					DockCtrl_SetLocalRect( child, page_rect );
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	void TabCtrl_DoTabs( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		NeGuiScopedChild( ctrl->Dc, TabCtrl_CalcTabRect( ctrl ) );
		const Context_t dc = ctrl->Dc;
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Rect_s r = Context_GetChild( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s screen_rect = DockCtrl_GetScreenRect( ctrl );
		const float tab_space = th->TabItemSpacing.x;
		const float tab_h_small = TabItem_CalcHeight( ctrl->Dc, TabStyle::None );
		const float tab_h_large = TabItem_CalcHeight( ctrl->Dc, TabStyle::Enlarge );
		Ctrl_DrawBox( dc, r, Visual::Splitter, CtrlState::Normal );

		// do tab items
		Vec2_s tear_pos = {};
		Rect_s item_rect = r;
		DockCtrl_t hot_page = nullptr;
		PageAction::Enum page_action = PageAction::None;
		const float total_w = TabCtrl_CalcTabWidth( ctrl );
		const bool is_fixed_width = (total_w > r.w);
		const float fixed_width = is_fixed_width 
			? floorf(r.w / DockCtrl_CountChildren( ctrl ))
			: 1.0f;
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling, item_rect.x += item_rect.w + tab_space )
		{
			// setup item
			const Text_s item_text = child->Text;
			const Id_t item_id = DockCtrl_MakePageId( child );
			const bool is_selected = (ctrl->ActivePage == child);

			// make the active page stick out by enlaring it vertically
			const TabStyle::Mask_t item_style 
				= is_selected 
				? TabStyle::Enlarge
				: TabStyle::None;
			item_rect.h = is_selected 
				? tab_h_large
				: tab_h_small;

			// shrink the tab items when there isn't enough horizontal space.
			// the control shouldn't have to scroll them
			item_rect.w = is_fixed_width 
				? (fixed_width - tab_space)
				: TabItem_CalcWidth( dc, item_style, item_text );

			// hit test
			if (Rect_Contains( item_rect, mouse_pos ))
				hot_page = child;

			// draw item
			TabItem_Draw( dc, item_id, item_rect, item_style, item_text, is_selected );

			// mouse input
			Vec2_s screen_pos = { screen_rect.x + item_rect.x, screen_rect.y + item_rect.y };
			const PosGripAction::Enum grip_action = PosGrip_Mouse( dc, item_id, item_rect, screen_pos );

			// update drag info  
			switch (grip_action)
			{
			case PosGripAction::Pushed:
				DockMgr_Drag( ctrl->Mgr, child, screen_pos, DragAction::Push );
				break;
			case PosGripAction::Dragged:
				DockMgr_Drag( ctrl->Mgr, child, screen_pos, DragAction::Move );
				break;
			case PosGripAction::Released:
				DockMgr_Drag( ctrl->Mgr, child, screen_pos, DragAction::Release );
				break;
			default:
				break;
			}

			// translate input
			const bool is_page = DockCtrl_IsPage(ctrl);
			if (is_page)
			{
				// page control
				switch (grip_action)
				{
				case PosGripAction::None:
					break;
				case PosGripAction::Released:
					// unwrap the single page control when
					// we're done dragging the torn off page
					page_action = PageAction::Tear;
					break;
				case PosGripAction::Pushed:
					// shouldn't happen
					NeAssert(false);
					break;
				case PosGripAction::Dragged:
					// keep dragging the page
					ctrl->Mgr->Platform.Host_Move_Rect( ctrl->Host, { screen_pos.x, screen_pos.y, screen_rect.w, screen_rect.h } );
					break;
				}
			}
			else
			{
				// tab control
				switch (grip_action)
				{
				case PosGripAction::None:
					break;
				case PosGripAction::Released:
					break;
				case PosGripAction::Pushed:
					ctrl->ActivePage = child;
					break;
				case PosGripAction::Dragged:
					{
						const float tear_threshold = th->TearThreshold;

						// reorder / tear tab items
						const float mouse_off_l = (item_rect.x - mouse_pos.x);
						const float mouse_off_t = (item_rect.y - mouse_pos.y);
						const float mouse_off_r = (mouse_pos.x - Rect_Right (item_rect));
						const float mouse_off_b = (mouse_pos.y - Rect_Bottom(item_rect));
						if (mouse_off_l > 0.0f)
						{
							if (child->PrevSibling)
							{
								page_action = PageAction::Before;
							}
							else if (mouse_off_l >= tear_threshold)
							{
								page_action = PageAction::Tear;
								tear_pos = screen_pos;
							}
						}
						else if (mouse_off_r > 0.0f)
						{
							if (child->NextSibling)
							{
								page_action = PageAction::After;
							}
							else if (mouse_off_r >= tear_threshold)
							{
								page_action = PageAction::Tear;
								tear_pos = screen_pos;
							}
						}
						if (mouse_off_t >= tear_threshold)
						{
							page_action = PageAction::Tear;
						}
						else if (mouse_off_b >= tear_threshold)
						{
							page_action = PageAction::Tear;
							tear_pos = screen_pos;
						}
					}
					break;
				}
			}
		}

		// execute action
		switch (page_action)
		{
		case PageAction::None:
			break;
		case PageAction::After:
			if (hot_page && (hot_page != ctrl->ActivePage))
				DockCtrl_ReorderAfter( ctrl, hot_page, ctrl->ActivePage );
			break;
		case PageAction::Before:
			if (hot_page && (hot_page != ctrl->ActivePage))
				DockCtrl_ReorderBefore( ctrl, hot_page, ctrl->ActivePage );
			break;
		case PageAction::Tear:
			{
				//@note:
				//	tab controls cannot have nested children.
				//	it should therefore be fine to directly
				//	adjust the local size. 
				//	there aren't any children that would 
				//	require having their layout updated.
				//
				//	also, after the active page has been 
				//	torn off and got wrapped in a page control
				//	is ought to have the exact same size
				//	as before.
				NeAssert(ctrl->ActivePage->Kind == DockCtrl::User);

				//@todo:
				//	the size calculation is somehow off
				//	by a couple pixels. 
				//	figure out what the cause is....

				// adjust the torn off page's size
				ctrl->ActivePage->LocalRect.h += r.h;

				// determine the kind of wrapper to in which 
				// to embed the torn off page
				DockCtrl::Enum wrapper = DockCtrl::None;
				switch (ctrl->Kind)
				{
				case DockCtrl::Tab_B: 
					wrapper = DockCtrl::Page_B;
					break;
				case DockCtrl::Tab_T: 
					wrapper = DockCtrl::Page_T;
					break;
				case DockCtrl::Page_B:
				case DockCtrl::Page_T: 
					wrapper = DockCtrl::None;
					break;
				default:
					IMPLEMENT_ME;
					break;
				}

				// tear it off
				DockMgr_QueueTearOff( ctrl->Mgr, ctrl->ActivePage, tear_pos.x, tear_pos.y, wrapper );

				// pick a new active page
				TabCtrl_UpdateActivePage( ctrl, ctrl->ActivePage );
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	void TabCtrl_DoChildren( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		if (!ctrl->ActivePage)
			return;
		DockCtrl_DoChild( ctrl->ActivePage );
	}

	void TabCtrl_DoClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		TabCtrl_ActivatePage( ctrl );
		TabCtrl_DoTabs		( ctrl );
		TabCtrl_DoChildren	( ctrl );
	}
}

//======================================================================================
//																				Splitter
//======================================================================================
namespace nemesis
{
	static Rect_s SplitterCtrl_OuterToInner( DockCtrl_t ctrl, DockCtrl_t child, const Rect_s& outer_rect )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Vec2_s prev_half = Vec2_Floor( 0.5f * th->FrameBorder );
		const Vec2_s next_half = th->FrameBorder - prev_half;
		Rect_s inner_rect = outer_rect;
		switch (ctrl->Kind)
		{
		case DockCtrl::Split_LR:
			if (child->PrevSibling) { inner_rect.w -= prev_half.x; inner_rect.x += prev_half.x; }
			if (child->NextSibling) { inner_rect.w -= next_half.x; }
			break;
		case DockCtrl::Split_TB:
			if (child->PrevSibling) { inner_rect.h -= prev_half.y; inner_rect.y += prev_half.y; }
			if (child->NextSibling) { inner_rect.h -= next_half.y; }
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
		return inner_rect;
	}

	static Rect_s SplitterCtrl_InnerToOuter( DockCtrl_t ctrl, DockCtrl_t child, const Rect_s& inner_rect )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Vec2_s prev_half = Vec2_Floor( 0.5f * th->FrameBorder );
		const Vec2_s next_half = th->FrameBorder - prev_half;
		Rect_s outer_rect = inner_rect;
		switch (ctrl->Kind)
		{
		case DockCtrl::Split_LR:
			if (child->PrevSibling) { outer_rect.w += prev_half.x; outer_rect.x -= prev_half.x; }
			if (child->NextSibling) { outer_rect.w += next_half.x; }
			break;
		case DockCtrl::Split_TB:
			if (child->PrevSibling) { outer_rect.h += prev_half.y; outer_rect.y -= prev_half.y; }
			if (child->NextSibling) { outer_rect.h += next_half.y; }
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
		return outer_rect;
	}

	Pad_s SplitterCtrl_CalcNonClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const bool want_frame 
			=  (ctrl->Parent == nullptr)
			&& ctrl->FirstChild 
			&& ctrl->FirstChild->NextSibling;
		if (!want_frame)
			return {};
		const float caption_h = Caption_CalcHeight( ctrl->Dc );
		return Pad_s
			{ th->FrameBorder.x
			, th->FrameBorder.y + caption_h
			, th->FrameBorder.x
			, th->FrameBorder.y
			};
	}

	Rect_s SplitterCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		NeAssert(DockCtrl_IsContainer(ctrl, side));
		NeAssert(child->Parent == ctrl);
		const bool is_lr = (ctrl->Kind == DockCtrl::Split_LR);
		const Rect_s pr = DockCtrl_GetScreenRect( ctrl );
		const Rect_s cr = DockCtrl_GetScreenRect( child );
		if (is_lr)
		{
			const float client_w = DockCtrl_GetClientW( ctrl );
			const float insert_f = NeMin( 0.5f, insert->LocalRect.w / client_w );
			const float scale_f	 = (1.0f - insert_f);
			const float insert_w = client_w * insert_f;

			float client_x0 = 0.0f;
			float client_x1 = 0.0f;
			for ( DockCtrl_t iter = ctrl->FirstChild; iter != child->NextSibling; iter = iter->NextSibling )
			{
				const float split_size = iter->SplitSize * scale_f;
				client_x0 = client_x1;
				client_x1 += floorf(split_size * client_w);
			}

			const float screen_x0 = pr.x + ctrl->NonClient.l + client_x0;
			const float screen_x1 = pr.x + ctrl->NonClient.l + client_x1;

			switch (side)
			{
			case DockSide::Left:	return Rect_s { screen_x0, pr.y, insert_w, pr.h };
			case DockSide::Right:	return Rect_s { screen_x1, pr.y, insert_w, pr.h };
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
		}
		else
		{
			const float client_h = DockCtrl_GetClientH( ctrl );
			const float insert_f = NeMin( 0.5f, insert->LocalRect.h / client_h );
			const float scale_f	 = (1.0f - insert_f);
			const float insert_h = client_h * insert_f;

			float client_y0 = 0.0f;
			float client_y1 = 0.0f;
			for ( DockCtrl_t iter = ctrl->FirstChild; iter != child->NextSibling; iter = iter->NextSibling )
			{
				const float split_size = iter->SplitSize * scale_f;
				client_y0 = client_y1;
				client_y1 += floorf(split_size * client_h);
			}

			const float screen_y0 = pr.y + ctrl->NonClient.t + client_y0;
			const float screen_y1 = pr.y + ctrl->NonClient.t + client_y1;

			switch (side)
			{
			case DockSide::Top:		return Rect_s { pr.x, screen_y0, pr.w, insert_h };
			case DockSide::Bottom:	return Rect_s { pr.x, screen_y1, pr.w, insert_h };
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
		}
		return cr;
	}

	void SplitterCtrl_Layout( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		NeAssert(!insert || !remove);
		NeAssert(!insert || ((insert->SplitSize >= 0.0f) && (insert->SplitSize <= 1.0f)));
		const bool is_lr = (ctrl->Kind == DockCtrl::Split_LR);

		// scale other children down on insert
		if (insert)
		{
			const float scale = (1.0f - insert->SplitSize);
			for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			{
				if (child == insert)
					continue;
				child->SplitSize *= scale;
			}
		}

		// scale other children up on remove
		if (remove)
		{
			const float scale = (1.0f / (1.0f - remove->SplitSize));
			for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
				child->SplitSize *= scale;
		}

		// move children into place
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Vec2_s prev_half = Vec2_Floor( 0.5f * th->FrameBorder );
		const Vec2_s next_half = th->FrameBorder - prev_half;
		const Rect_s client_rect = DockCtrl_GetClientRect( ctrl );
		Rect_s inner_rect = {};
		Rect_s outer_rect = client_rect;
		DockCtrl_t last = ctrl->FirstChild;
		for ( DockCtrl_t child = ctrl->FirstChild; child; last = child, child = child->NextSibling )
		{
			if (is_lr)
			{
				outer_rect.w = floorf(child->SplitSize * client_rect.w);
				inner_rect = SplitterCtrl_OuterToInner( ctrl, child, outer_rect );
				outer_rect.x += outer_rect.w;
			}
			else
			{
				outer_rect.h = floorf(child->SplitSize * client_rect.h);
				inner_rect = SplitterCtrl_OuterToInner( ctrl, child, outer_rect );
				outer_rect.y += outer_rect.h;
			}

			DockCtrl_SetLocalRect( child, inner_rect );
		}
	}

	void SplitterCtrl_DoSplitter( DockCtrl_t ctrl, DockCtrl_t child )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		DockCtrl_t prev = child->PrevSibling;
		DockCtrl_t next = child;
		if (!prev)
			return;
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const Rect_s r = Context_GetChild( ctrl->Dc );
		const bool is_lr = (ctrl->Kind == DockCtrl::Split_LR);
		const Id_t splitter_id = DockCtrl_MakeSplitterId( ctrl, next );
		const Vec2_s prev_half = Vec2_Floor( 0.5f * th->FrameBorder );
		const Rect_s old_prev_outer = SplitterCtrl_InnerToOuter( ctrl, prev, prev->LocalRect );
		const Rect_s old_next_outer = SplitterCtrl_InnerToOuter( ctrl, next, next->LocalRect );
		if (is_lr)
		{
			const float bounds_l = old_prev_outer.x			  + NeMin(th->MinDockedSize.x, old_prev_outer.w);
			const float bounds_r = Rect_Right(old_next_outer) - NeMin(th->MinDockedSize.x, old_next_outer.w);

			const Rect_s splitter_rect = 
				{			 bounds_l, old_next_outer.y
				, bounds_r - bounds_l, old_next_outer.h
				};

		//	Context_DrawRect( ctrl->Dc, splitter_rect, Color::Red );

			float pos_x = old_next_outer.x;
			if (Splitter2_DoV( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.x, 0.0f, pos_x ))
			{
			//	Context_DrawRect( ctrl->Dc, { pos_x - prev_half.x, old_next_outer.y, th->FrameBorder.x, old_next_outer.h }, Color::Red );

				// calc new outer local rects
				const Rect_s new_prev_outer = 
					{		  old_prev_outer.x, old_prev_outer.y
					, pos_x - old_prev_outer.x, old_prev_outer.h 
					};
				const Rect_s new_next_outer =
					{								 pos_x, old_next_outer.y
					, Rect_Right( old_next_outer ) - pos_x, old_next_outer.h
					};

				// calc new inner local rects
				const Rect_s new_prev_inner = SplitterCtrl_OuterToInner( ctrl, prev, new_prev_outer );
				const Rect_s new_next_inner = SplitterCtrl_OuterToInner( ctrl, next, new_next_outer );

				// calc new split sizes
				const float prev_split = new_prev_outer.w / DockCtrl_GetClientW( ctrl );
				const float next_split = new_next_outer.w / DockCtrl_GetClientW( ctrl );

				// apply split and rects
				prev->SplitSize = prev_split;
				next->SplitSize = next_split;
				DockCtrl_SetLocalRect( prev, new_prev_inner );
				DockCtrl_SetLocalRect( next, new_next_inner );
			}
		}
		else
		{
			const float bounds_t = old_prev_outer.y			   + NeMin(th->MinDockedSize.y, old_prev_outer.h);
			const float bounds_b = Rect_Bottom(old_next_outer) - NeMin(th->MinDockedSize.y, old_next_outer.h);

			const Rect_s splitter_rect = 
				{ old_next_outer.x,			   bounds_t
				, old_next_outer.w, bounds_b - bounds_t
				};

			//Context_DrawRect( ctrl->Dc, splitter_rect, Color::Red );

			float pos_y = old_next_outer.y;
			if (Splitter2_DoH( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.y, 0.0f, pos_y ))
			{
			//	Context_DrawRect( ctrl->Dc, { old_next_outer.x, pos_y - prev_half.y, old_next_outer.w, th->FrameBorder.y }, Color::Red );

				// calc new outer local rects
				const Rect_s new_prev_outer = 
					{ old_prev_outer.x, 	    old_prev_outer.y
					, old_prev_outer.w, pos_y - old_prev_outer.y  
					};
				const Rect_s new_next_outer =
					{ old_prev_outer.x, 							    pos_y
					, old_prev_outer.w, Rect_Bottom( old_next_outer ) - pos_y
					};

				// calc new inner local rects
				const Rect_s new_prev_inner = SplitterCtrl_OuterToInner( ctrl, prev, new_prev_outer );
				const Rect_s new_next_inner = SplitterCtrl_OuterToInner( ctrl, next, new_next_outer );

				// calc new split sizes
				const float prev_split = new_prev_outer.h / DockCtrl_GetClientH( ctrl );
				const float next_split = new_next_outer.h / DockCtrl_GetClientH( ctrl );

				// apply split and rects
				prev->SplitSize = prev_split;
				next->SplitSize = next_split;
				DockCtrl_SetLocalRect( prev, new_prev_inner );
				DockCtrl_SetLocalRect( next, new_next_inner );
			}
		}
	}

	void SplitterCtrl_DoSplitters( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			SplitterCtrl_DoSplitter( ctrl, child );
	}

	void SplitterCtrl_DoChildren( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			DockCtrl_DoChild( child );
	}

	void SplitterCtrl_DoClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		SplitterCtrl_DoChildren ( ctrl );
		SplitterCtrl_DoSplitters( ctrl );
	}
}

//======================================================================================
//																					Main
//======================================================================================
namespace nemesis
{
	Pad_s MainCtrl_CalcNonClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		return {};
	}

	Rect_s MainCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		const Rect_s c = ctrl->RemainRect;
		const Rect_s r = DockCtrl_GetScreenRect( ctrl );
		switch (area)
		{
		case DockArea::Outer:
			{
				// edge
				const float ctl_w = DockCtrl_GetClientW( ctrl );
				const float ctl_h = DockCtrl_GetClientH( ctrl );
				const float rel_w = NeMin( 0.5f, insert->LocalRect.w / ctl_w );
				const float rel_h = NeMin( 0.5f, insert->LocalRect.h / ctl_h );
				const float ins_w = rel_w * ctl_w;
				const float ins_h = rel_h * ctl_h;
				const float rem_w = ctl_w - ins_w;
				const float rem_h = ctl_h - ins_h;
				switch (side)
				{
				case DockSide::Left:	return Rect_s { r.x, r.y, ins_w, r.h };
				case DockSide::Top:		return Rect_s { r.x, r.y, r.w, ins_h };
				case DockSide::Right:	return Rect_s { r.x + rem_w, r.y, ins_w, r.h };
				case DockSide::Bottom:	return Rect_s { r.x, r.y + rem_h, r.w, ins_h };
				case DockSide::Center:	return Rect_s { r.x + c.x, r.y + c.y, c.w, c.h };
				default:
					IMPLEMENT_ME;
					break;
				}
			}
			break;
		case DockArea::Default:
			{
				// remainder
				const float ctl_w = DockCtrl_GetClientW( ctrl );
				const float ctl_h = DockCtrl_GetClientH( ctrl );
				const float rel_w = NeMin( 0.5f, insert->LocalRect.w / ctl_w );
				const float rel_h = NeMin( 0.5f, insert->LocalRect.h / ctl_h );
				const float ins_w = rel_w * ctl_w;
				const float ins_h = rel_h * ctl_h;
				const float rem_w = c.w - ins_w;
				const float rem_h = c.h - ins_h;
				const float x0 = r.x + c.x;
				const float y0 = r.y + c.y;
				const float x1 = r.x + c.x + c.w;
				const float y1 = r.y + c.y + c.h;
				switch (side)
				{
				case DockSide::Left:	return Rect_s { x0, y0, ins_w, c.h };
				case DockSide::Top:		return Rect_s { x0, y0, c.w, ins_h };
				case DockSide::Right:	return Rect_s { x1 - ins_w, y0, ins_w, c.h };
				case DockSide::Bottom:	return Rect_s { x0, y1 - ins_h, c.w, ins_h };
				case DockSide::Center:	return Rect_s { r.x + c.x, r.y + c.y, c.w, c.h };
				default:
					IMPLEMENT_ME;
					break;
				}
			}
			break;
		case DockArea::Inner:
			{
				// documents
				const float rel_w = NeMin( 0.5f, insert->LocalRect.w / c.w );
				const float rel_h = NeMin( 0.5f, insert->LocalRect.h / c.h );
				const float ins_w = rel_w * c.w;
				const float ins_h = rel_h * c.h;
				const float rem_w = c.w - ins_w;
				const float rem_h = c.h - ins_h;
				const float x0 = r.x + c.x;
				const float y0 = r.y + c.y;
				const float x1 = r.x + c.x + c.w;
				const float y1 = r.y + c.y + c.h;
				switch (side)
				{
				case DockSide::Left:	return Rect_s { x0, y0, ins_w, c.h };
				case DockSide::Top:		return Rect_s { x0, y0, c.w, ins_h };
				case DockSide::Right:	return Rect_s { x1 - ins_w, y0, ins_w, c.h };
				case DockSide::Bottom:	return Rect_s { x0, y1 - ins_h, c.w, ins_h };
				case DockSide::Center:	return Rect_s { r.x + c.x, r.y + c.y, c.w, c.h };
				default:
					IMPLEMENT_ME;
					break;
				}
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
		return r;
	}

	void MainCtrl_Layout( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(!insert || (insert->DockSide != DockSide::None));

		// measure docked children 
		float split_sum_w = 0.0;
		float split_sum_h = 0.0;
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			switch (child->DockSide)
			{
			case DockSide::Left:
			case DockSide::Right:
				split_sum_w += child->SplitSize;
				break;
			case DockSide::Top:
			case DockSide::Bottom:
				split_sum_h += child->SplitSize;
				break;
			case DockSide::Center:
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
		}

		// down-scale docked children
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const float avail_w = DockCtrl_GetClientW( ctrl );
		const float avail_h = DockCtrl_GetClientW( ctrl );
		const float min_remain_w = th->MinDockedSize.x;
		const float min_remain_h = th->MinDockedSize.y;
		const float min_remain_split_w = min_remain_w / avail_w;
		const float min_remain_split_h = min_remain_h / avail_h;
		const float max_used_split_w = 1.0f - min_remain_split_w;
		const float max_used_split_h = 1.0f - min_remain_split_h;
		const float scale_factor_w = split_sum_w ? (max_used_split_w / split_sum_w) : 1.0f;
		const float scale_factor_h = split_sum_h ? (max_used_split_h / split_sum_h) : 1.0f;
		const bool is_scale_w = (scale_factor_w < 1.0f);
		const bool is_scale_h = (scale_factor_h < 1.0f);
		if (is_scale_w || is_scale_h)
		{
			for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			{
				if (child->DockSide == DockSide::Center)
					continue;

				switch (child->DockSide)
				{
				case DockSide::Left:
				case DockSide::Right:
					if (is_scale_w)
						child->SplitSize *= scale_factor_w;
					break;
				case DockSide::Top:
				case DockSide::Bottom:
					if (is_scale_h)
						child->SplitSize *= scale_factor_h;
					break;
				default:
					IMPLEMENT_ME;
					break;
				}
			}
		}

		// move children into place
		const Rect_s client_rect = DockCtrl_GetClientRect( ctrl );
		Rect_s remain_rect = client_rect;
		Rect_s child_rect = {};
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			if (child->DockSide == DockSide::Center)
				continue;

			// calculate child rect (incl. splitter)
			switch (child->DockSide)
			{
			case DockSide::Left:
				{
					const float child_w = floorf(child->SplitSize * client_rect.w);
					child_rect.x = remain_rect.x;
					child_rect.y = remain_rect.y;
					child_rect.w = child_w;
					child_rect.h = remain_rect.h;
					remain_rect.w -= child_w;
					remain_rect.x += child_w;
				}
				break;
			case DockSide::Right:
				{
					const float child_w = floorf(child->SplitSize * client_rect.w);
					child_rect.x = remain_rect.x + remain_rect.w - child_w;
					child_rect.y = remain_rect.y;
					child_rect.w = child_w;
					child_rect.h = remain_rect.h;
					remain_rect.w -= child_w;
				}
				break;
			case DockSide::Top:
				{
					const float child_h = floorf(child->SplitSize * client_rect.h);
					child_rect.x = remain_rect.x;
					child_rect.y = remain_rect.y;
					child_rect.w = remain_rect.w;
					child_rect.h = child_h;
					remain_rect.h -= child_h;
					remain_rect.y += child_h;
				}
				break;
			case DockSide::Bottom:
				{
					const float child_h = floorf(child->SplitSize * client_rect.h);
					child_rect.x = remain_rect.x;
					child_rect.y = remain_rect.y + remain_rect.h - child_h;
					child_rect.w = remain_rect.w;
					child_rect.h = child_h;
					remain_rect.h -= child_h;
				}
				break;
			default:
				IMPLEMENT_ME;
				break;
			}

			// make room for the splitter
			switch (child->DockSide)
			{
			case DockSide::Left:
				child_rect.w -= th->FrameBorder.x;
				break;
			case DockSide::Top:
				child_rect.h -= th->FrameBorder.y;
				break;
			case DockSide::Right:
				child_rect.x += th->FrameBorder.x;
				child_rect.w -= th->FrameBorder.x;
				break;
			case DockSide::Bottom:
				child_rect.y += th->FrameBorder.y;
				child_rect.h -= th->FrameBorder.y;
				break;
			default:
				IMPLEMENT_ME;
				break;
			}

			DockCtrl_SetLocalRect( child, child_rect );
		}

		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			if (child->DockSide != DockSide::Center)
				continue;
			DockCtrl_SetLocalRect( child, remain_rect );
		}

		ctrl->RemainRect = remain_rect;
	}

	void MainCtrl_DoSplitter( DockCtrl_t ctrl, DockCtrl_t child, bool* recalc_layout )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(child->DockSide != DockSide::None);
		NeAssert(child->DockSide != DockSide::Center);

		// calculate client size
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const float client_w = DockCtrl_GetClientW( ctrl );
		const float client_h = DockCtrl_GetClientH( ctrl );
		const float offset_x = floorf(th->FrameBorder.x * 0.5f);
		const float offset_y = floorf(th->FrameBorder.y * 0.5f);

		// expand the child rect
		Rect_s child_rect = child->LocalRect;
		switch (child->DockSide)
		{
		case DockSide::Left:
			child_rect.w += th->FrameBorder.x;
			break;
		case DockSide::Top:
			child_rect.h += th->FrameBorder.y;
			break;
		case DockSide::Right:
			child_rect.x -= th->FrameBorder.x;
			child_rect.w += th->FrameBorder.x;
			break;
		case DockSide::Bottom:
			child_rect.y -= th->FrameBorder.y;
			child_rect.h += th->FrameBorder.y;
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

		// calculate bounding rect
		Rect_s splitter_rect = child_rect;
		switch (child->DockSide)
		{
		case DockSide::Left:
			{
				const float min_w = NeMin( th->MinDockedSize.x, child_rect.w );
				splitter_rect.x = min_w;
				splitter_rect.w = client_w - th->MinDockedSize.x - splitter_rect.x;
			}
			break;
		case DockSide::Top:
			{
				const float min_h = NeMin( th->MinDockedSize.y, child_rect.h );
				splitter_rect.y = min_h;
				splitter_rect.h = client_h - th->MinDockedSize.y - splitter_rect.y;
			}
			break;
		case DockSide::Right:
			{
				const float min_w = NeMin( th->MinDockedSize.x, child_rect.w );
				splitter_rect.x = min_w;
				splitter_rect.w = child_rect.x + child_rect.w - splitter_rect.x - min_w;
			}
			break;
		case DockSide::Bottom:
			{
				const float min_h = NeMin( th->MinDockedSize.y, child_rect.h );
				splitter_rect.y = min_h;
				splitter_rect.h = child_rect.y + child_rect.h - splitter_rect.y - min_h;
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

	//	Context_DrawRect( ctrl->Dc, splitter_rect, Color::Red );

		const Id_t splitter_id = DockCtrl_MakeSplitterId( ctrl, child );
		switch (child->DockSide)
		{
		case DockSide::Left:
			{
				float pos = child_rect.x + child_rect.w;
				if (Splitter2_DoV( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.x, -offset_x, pos ))
				{
					const float local_r = pos;
					const float local_w = local_r - child_rect.x;
					child->SplitSize = local_w / client_w;
					DockCtrl_SetLocalSize( child, local_w, child_rect.h );
					if (recalc_layout)
						*recalc_layout = true;
				}
			}
			break;
		case DockSide::Top:
			{
				float pos = child_rect.y + child_rect.h;
				if (Splitter2_DoH( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.y, -offset_y, pos ))
				{
					const float local_b = pos;
					const float local_h = local_b - child_rect.y;
					child->SplitSize = local_h / client_h;
					DockCtrl_SetLocalSize( child, child_rect.w, local_h );
					if (recalc_layout)
						*recalc_layout = true;
				}
			}
			break;
		case DockSide::Right:
			{
				float pos = child_rect.x;
				if (Splitter2_DoV( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.x, offset_x, pos ))
				{
					const float local_x = pos;
					const float local_w = child_rect.x + child_rect.w - local_x;
					child->SplitSize = local_w / client_w;
					DockCtrl_SetLocalPos( child, local_x, child_rect.y );
					DockCtrl_SetLocalSize( child, local_w, child_rect.h );
					if (recalc_layout)
						*recalc_layout = true;
				}
			}
			break;
		case DockSide::Bottom:
			{
				float pos = child_rect.y;
				if (Splitter2_DoH( ctrl->Dc, splitter_id, splitter_rect, th->FrameBorder.y, offset_y, pos ))
				{
					const float local_y = pos;
					const float local_h = child_rect.y + child_rect.h - local_y;
					child->SplitSize = local_h / client_h;
					DockCtrl_SetLocalPos( child, child_rect.x, local_y );
					DockCtrl_SetLocalSize( child, child_rect.w, local_h );
					if (recalc_layout)
						*recalc_layout = true;
				}
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	void MainCtrl_DoSplitters( DockCtrl_t ctrl, bool* recalc_layout )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			if (child->DockSide == DockSide::Center)
				continue;
			MainCtrl_DoSplitter( ctrl, child, recalc_layout );
		}
	}

	void MainCtrl_DoChildren( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			DockCtrl_DoChild( child );
	}

	void MainCtrl_DoClient( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		bool recalc_layout = false;
		MainCtrl_DoChildren	 ( ctrl );
		MainCtrl_DoSplitters ( ctrl, &recalc_layout );
		if (recalc_layout)
			DockCtrl_OnResize( ctrl );
	}
}


//======================================================================================
//																				 Overlay
//======================================================================================
namespace nemesis
{
	struct DropTargetInfo_s
	{
		DockCtrl_t	Target;
		int			NumAreas;
		bool		IsDocument;
	};

	static DropTargetInfo_s OverlayCtrl_GetDropTargetInfo( DockCtrl_t drop )
	{
		DockCtrl_t target = drop;
		int num_areas = 1;
		bool is_doc = false;

		DockCtrl_t root = DockCtrl_GetRoot( drop );
		if (DockCtrl_IsMain(root))
		{
			target = root;
			is_doc
				=  DockCtrl_IsMain( drop )
				|| (drop->DockSide == DockSide::Center);
			num_areas = is_doc ? 3 : 2;
		}
		if (target->Parent)
		{
			switch (target->Parent->Kind)
			{
			case DockCtrl::Tab_B:
			case DockCtrl::Tab_T:
			case DockCtrl::Split_LR:
			case DockCtrl::Split_TB:
				target = target->Parent;
				break;
			default:
				break;
			}
		}

		return { target, num_areas, is_doc };
	}

	Rect_s OverlayCtrl_CalcScreenRect( DockCtrl_t drop )
	{
		// determine the top-most target for the insert operation
		// and the number of dock areas
		const DropTargetInfo_s info = OverlayCtrl_GetDropTargetInfo( drop );

		// calculate the number of icons and spacings (for each direction)
		const int num_icons = info.NumAreas * 2 + 1;
		const int num_space = num_icons-1;

		// calculate the screen space bounding rectangle
		const DockTheme_t th = &drop->Mgr->Theme;
		const Vec2_s indicator_size		= float(num_icons) * th->OverlayIconSize 
										+ float(num_space) * th->OverlayIconSpace;
		const Rect_s actual_screen_rect = DockCtrl_GetScreenRect( info.Target );
		const Rect_s target_screen_rect = DockCtrl_GetScreenRect( drop	 );
		const Vec2_s indicator_screen_min = 
			{ target_screen_rect.x + (target_screen_rect.w - indicator_size.x) * 0.5f
			, target_screen_rect.y + (target_screen_rect.y - indicator_size.y) * 0.5f
			};
		const Vec2_s indicator_screen_max = 
			{ target_screen_rect.x + (target_screen_rect.w + indicator_size.x) * 0.5f
			, target_screen_rect.y + (target_screen_rect.y + indicator_size.y) * 0.5f
			};
		const float overlay_screen_l = NeMin(actual_screen_rect.x, indicator_screen_min.x);
		const float overlay_screen_t = NeMin(actual_screen_rect.y, indicator_screen_min.y);
		const float overlay_screen_r = NeMax(Rect_Right(actual_screen_rect) , indicator_screen_max.x);
		const float overlay_screen_b = NeMax(Rect_Bottom(actual_screen_rect), indicator_screen_max.y);
		const Rect_s overlay_screen_rect = 
			{ floorf(overlay_screen_l)
			, floorf(overlay_screen_t)
			, floorf(overlay_screen_r - overlay_screen_l)
			, floorf(overlay_screen_b - overlay_screen_t)
			};
		return overlay_screen_rect;
	}

	void OverlayCtrl_Do( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsOverlay(ctrl));
		DockCtrl_t drag_source = ctrl->Mgr->Drag.Source;
		DockCtrl_t drop_target = ctrl->Mgr->Drag.PrevTarget;
		if (!drag_source || !drop_target)
			return;

		const Context_t dc = ctrl->Dc;
		const Rect_s r = Context_GetChild( dc );
		const Graphics_t g = Context_GetGraphics( dc );
		if (NE_CONFIG == NE_CONFIG_DEBUG)
		{
			Graphics_FillRect( g, r, 0x2f000000 );
			Graphics_DrawRect( g, r, Color::Goldenrod );
		}

		// determine whether we have additional work areas
		const DropTargetInfo_s info = OverlayCtrl_GetDropTargetInfo( drop_target );

		// get overlay's screen rect
		const Rect_s overlay_screen_rect = DockCtrl_GetScreenRect( ctrl );

		// calculate center of target in local space
		Vec2_s center = {};
		if (info.IsDocument)
		{
			// we're targeting the document area of the main control
			NeAssert(DockCtrl_IsMain(info.Target));
			const Rect_s main_screen_rect = DockCtrl_GetScreenRect( info.Target );
			const Rect_s remain_screen_rect = Rect_Offset( info.Target->RemainRect, Rect_Pos( main_screen_rect ) );
			const Rect_s remain_local_rect = Rect_Offset( remain_screen_rect, -Rect_Pos( overlay_screen_rect ) );
			center = Rect_Center( remain_local_rect );
		}
		else
		{
			const Rect_s target_screen_rect = DockCtrl_GetScreenRect( drop_target );
			const Rect_s target_local_rect = Rect_Offset( target_screen_rect, -Rect_Pos( overlay_screen_rect ) );
			center = Rect_Center( target_local_rect );
		}

		// setup colors
		const DockTheme_t th = &ctrl->Mgr->Theme;
		const uint32_t icon_base_color = th->OverlayIconColor;
		const uint32_t icon_hot_color = th->OverlayIconColorHot;
		const uint32_t area_base_color = th->OverlayAreaColor;
		const uint32_t area_alpha_color = 0xaf000000 | (area_base_color & 0x00ffffff);

		// setup metrics
		const float cx = center.x;
		const float cy = center.y;
		const float ix = th->OverlayIconSize.x;
		const float iy = th->OverlayIconSize.y;
		const float hx = th->OverlayIconSize.x * 0.5f;
		const float hy = th->OverlayIconSize.y * 0.5f;
		const float sx = th->OverlayIconSpace.x;
		const float sy = th->OverlayIconSpace.y;

		// calculate rects
		Rect_s rect_c				   = {};
		Rect_s rect_l[DockArea::COUNT] = {};
		Rect_s rect_t[DockArea::COUNT] = {};
		Rect_s rect_r[DockArea::COUNT] = {};
		Rect_s rect_b[DockArea::COUNT] = {};
		for ( int i = 0; i < DockArea::COUNT; ++i ) 
		{
			// determine whether area is used
			bool has_area = false;
			switch (info.NumAreas)
			{
			case 1:
				has_area = (i == DockArea::Default);
				break;
			case 2:
				has_area 
					 = (i == DockArea::Default)
					|| (i == DockArea::Outer);
				break;
			case 3:
				has_area = true;
				break;
			}

			if (!has_area)
				continue;

			// calculate icon rects
			rect_c	  = { cx - hx, cy - hy			, ix, iy };
			rect_t[i] = { cx - hx, cy - hy - sx - iy, ix, iy };
			rect_b[i] = { cx - hx, cy + hy + sx		, ix, iy };
			rect_l[i] = { cx - hx - sx - ix, cy - hy, ix, iy };
			rect_r[i] = { cx + hx + sx, cy - hy		, ix, iy };

			switch (i)
			{
			case DockArea::Default:
				if (info.NumAreas == 3)
				{
					rect_t[i].y -= (iy + sy);
					rect_b[i].y += (iy + sy);
					rect_l[i].x -= (ix + sx);
					rect_r[i].x += (ix + sx);
				}
				break;
			case DockArea::Inner:
				break;
			case DockArea::Outer:
				{
					NeAssert(DockCtrl_IsMain(info.Target));
					const Rect_s main_screen_rect = DockCtrl_GetScreenRect( info.Target );
					const Rect_s main_local_rect = Rect_Offset( main_screen_rect, -Rect_Pos( overlay_screen_rect ) );
					const Vec2_s main_local_center = Rect_Center( main_local_rect );
					const Rect_s mr = main_local_rect;
					const Vec2_s mc = main_local_center;
					rect_t[i] = { mc.x - hx				, mr.y					, ix, iy };
					rect_b[i] = { mc.x - hx				, Rect_Bottom( mr )	- iy, ix, iy };
					rect_l[i] = { mr.x					, mc.y - hy				, ix, iy };
					rect_r[i] = { Rect_Right( mr ) - ix	, mc.y - hy				, ix, iy };
				}
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
		}

		// hit test rects
		bool on_c = false;
		bool on_l[DockArea::COUNT] = {};
		bool on_r[DockArea::COUNT] = {};
		bool on_t[DockArea::COUNT] = {};
		bool on_b[DockArea::COUNT] = {};
		bool on_cross = false;
		DockSide::Enum target_side = DockSide::None;
		DockArea::Enum target_area = DockArea::Default;
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		for ( int i = 0; i < DockArea::COUNT; ++i ) 
		{
			// determine whether user is within cross bounds
			if (i != DockArea::Outer)
			{
				const float ox1 = rect_l[i].x;
				const float ox2 = rect_r[i].x + rect_r[i].w;
				const float oy1 = rect_t[i].y;
				const float oy2 = rect_b[i].y + rect_b[i].h;
				const Rect_s rect_o = { ox1, oy1, ox2-ox1, oy2-oy1 };
				on_cross |= Rect_Contains( rect_o   , mouse_pos );
			}

			// determine which icons are hot
			//	and whether the mouse is within the icon bounds
			on_c		 = Rect_Contains( rect_c   , mouse_pos );
			on_l[i]		 = Rect_Contains( rect_l[i], mouse_pos );
			on_t[i]		 = Rect_Contains( rect_t[i], mouse_pos );
			on_r[i]		 = Rect_Contains( rect_r[i], mouse_pos );
			on_b[i]		 = Rect_Contains( rect_b[i], mouse_pos );

			// hit test icons
			if		(on_l[i]) { target_side = DockSide::Left;	target_area = (DockArea::Enum)i; }
			else if (on_t[i]) { target_side = DockSide::Top;	target_area = (DockArea::Enum)i; }
			else if (on_r[i]) { target_side = DockSide::Right;	target_area = (DockArea::Enum)i; }
			else if (on_b[i]) { target_side = DockSide::Bottom; target_area = (DockArea::Enum)i; }
			else if (on_c)	  { target_side = DockSide::Center; }

		}

		// draw insert rect
		if (target_side != DockSide::None)
		{
			// select insert target
			DockCtrl_t parent_ctrl = drop_target; 
			if (target_area == DockArea::Outer)
				parent_ctrl = info.Target;

			const Rect_s r1 = DockCtrl_CalcInsertRect( parent_ctrl, drag_source, target_side, target_area );
			const Rect_s r2 = overlay_screen_rect;
			const Rect_s rect_insert = 
				{ r1.x - r2.x
				, r1.y - r2.y
				, r1.w
				, r1.h
				};
			Graphics_FillRect( g, rect_insert, area_alpha_color );
			Graphics_DrawRect( g, rect_insert, area_base_color );
		}

		// draw icons
		for ( int i = 0; i < DockArea::COUNT; ++i ) 
		{
			Graphics_FillRect( g, rect_l[i], on_l[i] ? icon_hot_color : icon_base_color );
			Graphics_FillRect( g, rect_r[i], on_r[i] ? icon_hot_color : icon_base_color );
			Graphics_FillRect( g, rect_t[i], on_t[i] ? icon_hot_color : icon_base_color );
			Graphics_FillRect( g, rect_b[i], on_b[i] ? icon_hot_color : icon_base_color );
		}
		Graphics_FillRect( g, rect_c, on_c ? icon_hot_color : icon_base_color );

		// keep the current drop target while
		//	the user is interacting with the
		//	cross icons
		if (on_cross)
			DockMgr_SetDropTarget( ctrl->Mgr, drop_target );

		// set the drop result
		ctrl->Mgr->Drag.DropSide = target_side;
		ctrl->Mgr->Drag.DropArea = target_area;
	}
}
