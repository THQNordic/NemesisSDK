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
//																				 Generic
//======================================================================================
namespace nemesis
{
	static void DockCtrl_PostRemove( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		const Rect_s parent_screen_rect  = DockCtrl_GetScreenRect( ctrl );
		const Rect_s removed_screen_rect = Rect_Offset( parent_screen_rect, Rect_Pos( remove->LocalRect ) );
		remove->FloatPos.x = removed_screen_rect.x;
		remove->FloatPos.y = removed_screen_rect.y;
		remove->LocalRect.x = 0.0f;
		remove->LocalRect.y = 0.0f;
		remove->NoCaption = false;
		remove->SplitSize = 0.0f;
		remove->DockSide = DockSide::None;
	}
}

//======================================================================================
//																					 Tab
//======================================================================================
namespace nemesis
{
	static void TabCtrl_OnInsert( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		TabCtrl_Layout( ctrl, insert, nullptr );
		ctrl->ActivePage = insert;

		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_T:
		case DockCtrl::Page_T:
			insert->NoCaption = true;
			break;
		default:
			break;
		}
	}

	static void TabCtrl_OnRemove( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		TabCtrl_UpdateActivePage( ctrl, remove );
		TabCtrl_Layout( ctrl, nullptr, remove );
		DockCtrl_PostRemove( ctrl, remove );
	}

	static void TabCtrl_OnReorder( DockCtrl_t ctrl, DockCtrl_t reorder )
	{
		// order does not affect layout
	}

	static void TabCtrl_OnResize( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		TabCtrl_Layout( ctrl, nullptr, nullptr );
	}
}

//======================================================================================
//																				Splitter
//======================================================================================
namespace nemesis
{
	static void SplitterCtrl_OnInsert( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		NeAssert(ctrl->FirstChild);
		NeAssert(insert->Parent == ctrl);
		const bool was_empty = (!insert->PrevSibling) && (!insert->NextSibling);
		const float insert_rel = insert->LocalRect.w / DockCtrl_GetClientW( ctrl );
		const float split_size = was_empty ? 1.0f : NeMin( 0.5f, insert_rel );
		insert->SplitSize = split_size;
		SplitterCtrl_Layout( ctrl, insert, nullptr );
	}

	static void SplitterCtrl_OnRemove( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		NeAssert(remove->Parent != ctrl);
		SplitterCtrl_Layout( ctrl, nullptr, remove );
		DockCtrl_PostRemove( ctrl, remove );
	}

	static void SplitterCtrl_OnResize( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		SplitterCtrl_Layout( ctrl, nullptr, nullptr );
	}
}

//======================================================================================
//																					Main
//======================================================================================
namespace nemesis
{
	static void MainCtrl_OnInsert( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(insert->Parent == ctrl);
		NeAssert(insert->DockSide != DockSide::None);
		switch (insert->DockSide)
		{
		case DockSide::Left:
		case DockSide::Right:
			insert->SplitSize = NeMin( 0.5f, insert->LocalRect.w / DockCtrl_GetClientW( ctrl ) );
			break;
		case DockSide::Top:
		case DockSide::Bottom:
			insert->SplitSize = NeMin( 0.5f, insert->LocalRect.h / DockCtrl_GetClientH( ctrl ) );
			break;
		case DockSide::Center:
			insert->SplitSize = 0.0f;
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
		MainCtrl_Layout( ctrl, insert, nullptr );
	}

	static void MainCtrl_OnRemove( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(remove->Parent != ctrl);
		MainCtrl_Layout( ctrl, nullptr, remove );
		DockCtrl_PostRemove( ctrl, remove );
	}

	static void MainCtrl_OnReorder( DockCtrl_t ctrl, DockCtrl_t reorder )
	{
		// order does not affect layout
	}

	static void MainCtrl_OnResize( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		MainCtrl_Layout( ctrl, nullptr, nullptr );
	}
}

//======================================================================================
//																				   Typed
//======================================================================================
namespace nemesis
{
	void DockCtrl_OnWrap( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t container )
	{
		if (ctrl->ActivePage == child)
			ctrl->ActivePage = container;
		container->DockSide = child->DockSide;
		container->SplitSize = child->SplitSize;
		container->LocalRect = child->LocalRect;
		child->DockSide = DockSide::None;
		child->SplitSize = 0.0f;
		child->LocalRect.x = 0.0f;
		child->LocalRect.y = 0.0f;
		DockCtrl_UpdateNonClient( child );
		DockCtrl_UpdateNonClient( container );
	}

	void DockCtrl_OnUnwrap( DockCtrl_t ctrl, DockCtrl_t container, DockCtrl_t remainder )
	{
		if (ctrl->ActivePage == container)
			ctrl->ActivePage = remainder;
		remainder->DockSide = container->DockSide;
		remainder->SplitSize = container->SplitSize;
		remainder->LocalRect = container->LocalRect;
		container->LocalRect.x = 0.0f;
		container->LocalRect.y = 0.0f;
		container->DockSide = DockSide::None;
		container->SplitSize = 0.0f;
		DockCtrl_UpdateNonClient( container );
		DockCtrl_UpdateNonClient( remainder );
	}

	void DockCtrl_OnInsert( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		NeAssert(DockCtrl_IsContainer(ctrl));
		NeAssert(insert->Parent == ctrl);
		NeAssert(DockCtrl_GetClientW( ctrl ));
		NeAssert(DockCtrl_GetClientH( ctrl ));
		NeAssert(insert->LocalRect.w);
		NeAssert(insert->LocalRect.h);
		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
			MainCtrl_OnInsert( ctrl, insert );
			break;
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			TabCtrl_OnInsert( ctrl, insert );
			break;
		case DockCtrl::Split_LR:
			SplitterCtrl_OnInsert( ctrl, insert );
			break;
		case DockCtrl::Split_TB:
			SplitterCtrl_OnInsert( ctrl, insert );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

		DockCtrl_UpdateNonClient( insert );
		DockCtrl_UpdateNonClient( ctrl );
	}

	void DockCtrl_OnRemove( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(DockCtrl_IsContainer(ctrl));
		NeAssert(!remove->Parent);
		NeAssert(DockCtrl_GetClientW( ctrl ));
		NeAssert(DockCtrl_GetClientH( ctrl ));
		NeAssert(remove->LocalRect.w);
		NeAssert(remove->LocalRect.h);
		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
			MainCtrl_OnRemove( ctrl, remove );
			break;
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			TabCtrl_OnRemove( ctrl, remove );
			break;
		case DockCtrl::Split_LR:
			SplitterCtrl_OnRemove( ctrl, remove );
			break;
		case DockCtrl::Split_TB:
			SplitterCtrl_OnRemove( ctrl, remove );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

		DockCtrl_UpdateNonClient( ctrl );
		DockCtrl_UpdateNonClient( remove );
	}

	void DockCtrl_OnReorder( DockCtrl_t ctrl, DockCtrl_t reorder )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			TabCtrl_OnReorder( ctrl, reorder );
			break;
		case DockCtrl::Main:
			MainCtrl_OnReorder( ctrl, reorder );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	void DockCtrl_OnResize( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::User:
			break;
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			TabCtrl_OnResize( ctrl );
			break;
		case DockCtrl::Split_LR:
			SplitterCtrl_OnResize( ctrl );
			break;
		case DockCtrl::Split_TB:
			SplitterCtrl_OnResize( ctrl );
			break;
		case DockCtrl::Overlay:
			break;
		case DockCtrl::Main:
			MainCtrl_OnResize( ctrl );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}
}

//======================================================================================
namespace nemesis
{
	Rect_s DockCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area )
	{
		NeAssert(side != DockSide::None);

		if (ctrl->Parent)
		{
			if (DockCtrl_IsContainer( ctrl->Parent, side ))
			{
				switch (ctrl->Parent->Kind)
				{
				case DockCtrl::Tab_B:
				case DockCtrl::Tab_T:
					return TabCtrl_CalcInsertRect( ctrl->Parent, ctrl, insert, side );
				case DockCtrl::Split_LR:
				case DockCtrl::Split_TB:
					return SplitterCtrl_CalcInsertRect( ctrl->Parent, ctrl, insert, side );
				default:
					IMPLEMENT_ME;
					break;
				}
			}
			else
			{
				switch (ctrl->Parent->Kind)
				{
				case DockCtrl::Tab_B:
				case DockCtrl::Tab_T:
					return TabCtrl_CalcInsertRect( ctrl->Parent, ctrl, insert, side );
				default:
					break;
				}
			}
		}

		switch (ctrl->Kind)
		{
		case DockCtrl::User:
			return UserCtrl_CalcInsertRect( ctrl, insert, side );
		case DockCtrl::Main:
			return MainCtrl_CalcInsertRect( ctrl, insert, side, area );
		default:
			IMPLEMENT_ME;
			break;
		}
		return {};
	}
}
