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
#include "DockTool.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	static DockCtrl::Enum DockSide_ToCtrl( DockSide::Enum side )
	{
		switch (side)
		{
		case DockSide::Center:
			return DockCtrl::Tab_B;
		case DockSide::Left:
		case DockSide::Right:
			return DockCtrl::Split_LR;
		case DockSide::Top:
		case DockSide::Bottom:
			return DockCtrl::Split_TB;
		default:
			break;
		}
		return DockCtrl::None;
	}
}

//======================================================================================
namespace nemesis
{
	static void DockTool_AppendLeaves( DockCtrl_t ctrl, Array<DockCtrl_t>& leaves )
	{
		NeAssert(DockCtrl_IsContainer(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
		{
			if (DockCtrl_IsContainer( child ))
				DockTool_AppendLeaves( child, leaves );
			else
				leaves.Append( child );
		}
	}

	static void DockTool_AppendChildren( DockCtrl_t ctrl, Array<DockCtrl_t>& children )
	{
		NeAssert(DockCtrl_IsContainer(ctrl));
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			children.Append( child );
	}

	static void DockTool_InsertFirst( DockCtrl_t container, DockCtrl_t insert )
	{
		if (insert->Kind != container->Kind)
			return DockCtrl_InsertFirst( container, insert );

		DockMgr_t mgr = insert->Mgr;
		mgr->Temp.Reserve( 64 );
		mgr->Temp.Count = 0;
		DockTool_AppendChildren( insert, mgr->Temp );
		const int count = mgr->Temp.Count;
		for ( int i = count-1; i >= 0; --i )
		{
			DockCtrl_t child = mgr->Temp[i];
			DockTool_Undock( child );
			DockCtrl_InsertFirst( container, child );
		}
		mgr->Temp.Count = 0;
	}

	static void DockTool_InsertLast( DockCtrl_t container, DockCtrl_t insert )
	{
		if (insert->Kind != container->Kind)
			return DockCtrl_InsertLast( container, insert );

		DockMgr_t mgr = insert->Mgr;
		mgr->Temp.Reserve( 64 );
		mgr->Temp.Count = 0;
		DockTool_AppendChildren( insert, mgr->Temp );
		const int count = mgr->Temp.Count;
		for ( int i = 0; i < count; ++i )
		{
			DockCtrl_t child = mgr->Temp[i];
			DockTool_Undock( child );
			DockCtrl_InsertLast( container, child );
		}
		mgr->Temp.Count = 0;
	}

	static void DockTool_InsertCenter( DockCtrl_t container, DockCtrl_t insert )
	{
		if (!DockCtrl_IsContainer(insert))
			return DockCtrl_InsertFirst( container, insert );

		DockMgr_t mgr = insert->Mgr;
		mgr->Temp.Reserve( 64 );
		mgr->Temp.Count = 0;
		DockTool_AppendLeaves( insert, mgr->Temp );
		const int count = mgr->Temp.Count;
		for ( int i = 0; i < count; ++i )
		{
			DockCtrl_t leaf = mgr->Temp[i];
			NeAssert(!leaf->FirstChild);
			NeAssert(leaf->Kind == DockCtrl::User);
			DockTool_Undock( leaf );
			DockCtrl_InsertFirst( container, leaf );
		}
		mgr->Temp.Count = 0;
	}
}

//======================================================================================
namespace nemesis
{
	static void DockTool_InsertChild( DockCtrl_t container, DockCtrl_t insert, DockSide::Enum side )
	{
		NeAssert(DockCtrl_IsContainer( container, side ));
		NeAssert(!insert->Parent);
		switch (side)
		{
		case DockSide::Left:
		case DockSide::Top:
			DockTool_InsertFirst( container, insert );
			break;
		case DockSide::Right:
		case DockSide::Bottom:
			DockTool_InsertLast( container, insert );
			break;
		case DockSide::Center:
			DockTool_InsertCenter( container, insert );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	static DockCtrl_t DockTool_WrapCtrl( DockCtrl_t ctrl, DockCtrl::Enum kind )
	{
		const Rect_s float_rect = DockCtrl_GetScreenRect( ctrl );
		DockCtrl_t container = DockMgr_CreateCtrl( ctrl->Mgr, kind, nullptr, float_rect, true, {} );
		DockCtrl_Wrap( ctrl, container );
		return container;
	}

	static void DockTool_WrapCtrl( DockCtrl_t ctrl, DockCtrl_t other, DockSide::Enum side )
	{
		NeAssert(!other->Parent);
		const DockCtrl::Enum kind = DockSide_ToCtrl( side );
		DockCtrl_t container = DockTool_WrapCtrl( ctrl, kind );
		DockTool_InsertChild( container, other, side );
	}

	static void DockTool_TryWrapCtrl( DockCtrl_t ctrl, DockCtrl::Enum kind )
	{
		if (kind == DockCtrl::None)
			return;
		DockTool_WrapCtrl( ctrl, kind );
	}

}

//======================================================================================
namespace nemesis
{
	static void DockTool_GatherChildren( DockCtrl_t ctrl )
	{
		DockMgr_t mgr = ctrl->Mgr;
		mgr->Temp.Reserve( 64 );
		mgr->Temp.Count = 0;
		DockTool_AppendChildren( ctrl, mgr->Temp );
	}

	static void DockTool_GatherLeaves( DockCtrl_t ctrl )
	{
		DockMgr_t mgr = ctrl->Mgr;
		mgr->Temp.Reserve( 64 );
		mgr->Temp.Count = 0;
		DockTool_AppendLeaves( ctrl, mgr->Temp );
	}

	static void TabCtrl_Dock( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area )
	{
		NeAssert(DockCtrl_IsTab(ctrl));
		NeAssert(child->Parent == ctrl);
		NeAssert(!insert->Parent);

		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_T:
			// @todo:
			//	redirect this to the parent,
			//	ie. the main control
			NeAssert(DockCtrl_IsMain(ctrl->Parent));
			return DockTool_Dock( ctrl->Parent, insert, side, DockArea::Default ); 
		}

		switch (side)
		{
		case DockSide::Center:
			switch (insert->Kind)
			{
			case DockCtrl::Tab_B:
			case DockCtrl::Tab_T:
			case DockCtrl::Split_LR:
			case DockCtrl::Split_TB:
				{
					NeTrace("Tab: Multi\n");
					DockTool_GatherLeaves( insert );
					DockMgr_t mgr = ctrl->Mgr;
					const int num_children = mgr->Temp.Count;
					for ( int i = 0; i < num_children; ++i )
					{
						DockTool_Undock( mgr->Temp[i] );
						DockCtrl_InsertFirst( ctrl, mgr->Temp[i] );
					}
				}
				break;
			case DockCtrl::User:
				{
					NeTrace("Tab: Single\n");
					DockCtrl_InsertFirst( ctrl, insert );
				}
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
			break;
		case DockSide::Left:
		case DockSide::Top:
		case DockSide::Right:
		case DockSide::Bottom:
			{
				NeTrace("Tab: Wrap\n");
				DockTool_WrapCtrl( ctrl, insert, side );
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	static void SplitterCtrl_Dock( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area )
	{
		NeAssert(DockCtrl_IsSplitter(ctrl));
		NeAssert(!DockCtrl_IsPage(ctrl));
		NeAssert(child->Parent == ctrl);
		NeAssert(!insert->Parent);

		// if the dock side doesn't match the splitter's direction
		//	then wrap the child in a new control of the matching
		//	direction.
		switch (side)
		{
		case DockSide::Left:
		case DockSide::Right:
			if (ctrl->Kind != DockCtrl::Split_LR)
			{
				NeTrace("Splitter: Wrap L/R\n");
				return DockTool_WrapCtrl( child, insert, side );
			}
			break;
		case DockSide::Top:
		case DockSide::Bottom:
			if (ctrl->Kind != DockCtrl::Split_TB)
			{
				NeTrace("Splitter: Wrap T/B\n");
				return DockTool_WrapCtrl( child, insert, side );
			}
			break;
		case DockSide::Center:
			{
				NeTrace("Splitter: Wrap C\n");
				return DockTool_WrapCtrl( child, insert, side );
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

		// prevent nested splitters of the same kind
		if (insert->Kind == ctrl->Kind)
		{
			DockTool_GatherChildren( insert );
			DockMgr_t mgr = ctrl->Mgr;
			const int num_children = mgr->Temp.Count;
			switch (side)
			{
			case DockSide::Left:
			case DockSide::Top:
				NeTrace("Splitter: Multi (Before)\n");
				for ( int i = 0; i < num_children; ++i )
				{
					DockTool_Undock( mgr->Temp[i] );
					DockCtrl_InsertBefore( ctrl, child, mgr->Temp[i] );
				}
				break;
			case DockSide::Right:
			case DockSide::Bottom:
				NeTrace("Splitter: Multi (After)\n");
				for ( int i = num_children-1; i >= 0; --i )
				{
					DockTool_Undock( mgr->Temp[i] );
					DockCtrl_InsertAfter( ctrl, child, mgr->Temp[i] );
				}
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
			mgr->Temp.Count = 0;
			return;
		}

		// append to splitter
		switch (side)
		{
		case DockSide::Left:
		case DockSide::Top:
			NeTrace("Splitter: Single (Before)\n");
			DockCtrl_InsertBefore( ctrl, child, insert );
			break;
		case DockSide::Right:
		case DockSide::Bottom:
			NeTrace("Splitter: Single (After)\n");
			DockCtrl_InsertAfter( ctrl, child, insert );
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
	}

	static bool MainCtrl_Dock( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(!insert->Parent);

		// leave regular docking to the child
		if (area == DockArea::Default)
		{
			const bool on_page = !child || (child->DockSide == DockSide::Center);
			if (!on_page)
				return false;
		}

		switch (side)
		{
		case DockSide::Left:
		case DockSide::Top:
		case DockSide::Right:
		case DockSide::Bottom:
			switch (area)
			{
			case DockArea::Outer:
				NeTrace("Main: Edge\n");
				insert->DockSide = side;
				DockCtrl_InsertFirst( ctrl, insert );
				break;
			case DockArea::Default:
				//@todo:
				//	merge adjacent controls, ie. when inserting
				//	a control and the last control's 
				//	dock side equals the insert side
				//	then perform a dock rather than an insert
				NeTrace("Main: Remainder\n");
				insert->DockSide = side;
				DockCtrl_InsertLast( ctrl, insert );
				break;
			case DockArea::Inner:
				NeTrace("Main: Document (Side)\n");
				break;
			default:
				IMPLEMENT_ME;
				break;
			}
			break;
		case DockSide::Center:
			{
				if (DockCtrl_IsContainer(insert))
				{
					NeTrace("Main: Center (Multi)\n");
					DockTool_GatherLeaves( insert );

					const int num_children = ctrl->Mgr->Temp.Count;
					for ( int i = 0; i < num_children; ++i )
						DockTool_Undock( ctrl->Mgr->Temp[i] );
				}
				else
				{
					NeTrace("Main: Center (Single)\n");
					ctrl->Mgr->Temp.Reserve( 64 );
					ctrl->Mgr->Temp.Count = 0;
					ctrl->Mgr->Temp.Append( insert );
				}

				DockCtrl_t tab_ctrl = DockCtrl_FindChild( ctrl, DockCtrl::Tab_T );
				const int first_idx = tab_ctrl ? 0 : 1;

				if (!tab_ctrl)
				{
					tab_ctrl = DockTool_WrapCtrl( ctrl->Mgr->Temp[0], DockCtrl::Tab_T );
					tab_ctrl->DockSide = side;
					DockCtrl_InsertLast( ctrl, tab_ctrl ); 
				}

				const int num_children = ctrl->Mgr->Temp.Count;
				for ( int i = first_idx; i < num_children; ++i )
					DockCtrl_InsertFirst( tab_ctrl, ctrl->Mgr->Temp[i] );
			}
			break;
		default:
			IMPLEMENT_ME;
			break;
		}

		return true;
	}

}

//======================================================================================
namespace nemesis
{
	void DockTool_Dock( DockCtrl_t ctrl, DockCtrl_t other, DockSide::Enum side, DockArea::Enum area )
	{
		DockCtrl_Undock( other );
		NeAssert(!other->Parent);

		if (ctrl->Parent)
		{
			switch (ctrl->Parent->Kind)
			{
			case DockCtrl::Main:
				if (MainCtrl_Dock( ctrl->Parent, ctrl, other, side, area ))
					return;
				break;
			case DockCtrl::Tab_B:
			case DockCtrl::Tab_T:
				return TabCtrl_Dock( ctrl->Parent, ctrl, other, side, area );
			case DockCtrl::Split_LR:
			case DockCtrl::Split_TB:
				return SplitterCtrl_Dock( ctrl->Parent, ctrl, other, side, area );
			default:
				IMPLEMENT_ME;
				break;
			}
		}

		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
			if (MainCtrl_Dock( ctrl, nullptr, other, side, area ))
				return;
			break;
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
			if (side == DockSide::Center)
				return DockTool_InsertChild( ctrl, other, side );
			break;
		case DockCtrl::Split_LR:
			switch (side)
			{
			case DockSide::Left:
			case DockSide::Right:
				return DockTool_InsertChild( ctrl, other, side );
			default:
				break;
			};
			break;
		case DockCtrl::Split_TB:
			switch (side)
			{
			case DockSide::Top:
			case DockSide::Bottom:
				return DockTool_InsertChild( ctrl, other, side );
			default:
				break;
			};
			break;
		default:
			break;
		}

		NeAssert(ctrl->Kind == DockCtrl::User);
		return DockTool_WrapCtrl( ctrl, other, side );
	}

	void DockTool_Undock( DockCtrl_t ctrl )
	{
		if (!ctrl->Parent)
			return;
		DockCtrl_Trace( ctrl, "Undock" );
		DockCtrl_t parent = ctrl->Parent;
		DockCtrl_RemoveChild( parent, ctrl );
		DockCtrl_TryUnwrap( parent );
	}

	void DockTool_TearOff( DockCtrl_t ctrl, DockCtrl::Enum wrapper )
	{
		DockCtrl_Trace( ctrl, "TearOff" );
		DockTool_Undock( ctrl );
		DockTool_TryWrapCtrl( ctrl, wrapper );
	}

}
