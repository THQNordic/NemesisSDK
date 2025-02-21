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
#include "DockCtrl_Gui.h"
#include "DockCtrl_Layout.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	static const cstr_t DockCtrlKind_Name[] =
	{ "None"
	, "Main"
	, "Tab B"
	, "Tab T"
	, "Page B"
	, "Page T"
	, "Split L/R"
	, "Split T/B"
	, "User"
	, "Overlay"
	};
	NeStaticAssert(NeCountOf(DockCtrlKind_Name) == DockCtrl::COUNT);

	static cstr_t DockCtrlKind_ToStr( DockCtrl::Enum kind )
	{
		NeAssertBounds(kind, NeCountOf(DockCtrlKind_Name));
		return DockCtrlKind_Name[kind];
	}

	static cstr_t DockSide_ToStr( DockSide::Enum side )
	{
		switch (side)
		{
		case DockSide::None:	return "None";
		case DockSide::Left:	return "Left";
		case DockSide::Top:		return "Top";
		case DockSide::Right:	return "Right";
		case DockSide::Bottom:	return "Bottom";
		case DockSide::Center:	return "Center";
		default:
			break;
		}
		return "<invalid>";
	}

	static void DockCtrl_TraceHierarchy( DockCtrl_t ctrl, int level )
	{
		for ( int i = 0; i < level; ++i )
			NeTrace("| ");
		NeTrace( "[0x%p] (%s) %s\n", ctrl, DockCtrlKind_ToStr(ctrl->Kind), ctrl->Text );

		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			DockCtrl_TraceHierarchy( child, level+1 );
	}
}

//======================================================================================
namespace nemesis
{
	static bool DockCtrl_IsUnlinked( DockCtrl_t ctrl )
	{
		// @note: 
		//	it's ok for the ctrl to have children
		//	in order to be considered unlinked
		return (ctrl->Parent	  == nullptr)
			&& (ctrl->NextSibling == nullptr)
			&& (ctrl->PrevSibling == nullptr)
			;
	}

	static void DockCtrl_Unlink( DockCtrl_t ctrl )
	{
		NeAssert(ctrl->Parent);
		DockCtrl_t prnt = ctrl->Parent;
		DockCtrl_t prev = ctrl->PrevSibling;
		DockCtrl_t next = ctrl->NextSibling;
		if (next)
			next->PrevSibling = prev;
		if (prev)
			prev->NextSibling = next;
		else
		{
			NeAssert(prnt->FirstChild == ctrl);
			prnt->FirstChild = next;
		}
		ctrl->Parent	  = nullptr;
		ctrl->NextSibling = nullptr;
		ctrl->PrevSibling = nullptr;
		NeAssert(DockCtrl_IsUnlinked(ctrl));
	}

	static void DockCtrl_LinkAfter( DockCtrl_t parent, DockCtrl_t after, DockCtrl_t insert )
	{
		NeAssert(parent);
		NeAssert(!after || (after->Parent == parent));
		NeAssert(DockCtrl_IsUnlinked(insert));
		if (after)
		{
			DockCtrl_t next = after->NextSibling;
			if (next)
				next->PrevSibling = insert;
			insert->Parent = parent;
			insert->NextSibling = next;
			insert->PrevSibling = after;
			after->NextSibling = insert;
		}
		else
		{
			DockCtrl_t next = parent->FirstChild;
			if (next)
				next->PrevSibling = insert;
			insert->Parent = parent;
			insert->NextSibling = next;
			insert->PrevSibling = nullptr;
			parent->FirstChild = insert;
		}
		NeAssert(insert->Parent == parent);
		NeAssert(insert->PrevSibling == after);
		NeAssert(!after || (after->NextSibling == insert));
		NeAssert( after || (insert->Parent->FirstChild == insert));
	}

}

//======================================================================================
namespace nemesis
{
	static void DockCtrl_EatMouse( DockCtrl_t ctrl )
	{
		if (ctrl->Kind == DockCtrl::Overlay)
			return;
		const Context_t dc = ctrl->Dc;
		const Rect_s r = Context_GetChild( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		if (!Rect_Contains( r, mouse_pos ))
			return;
		Context_SetHot( dc, ctrl->Id );
	}
}

//======================================================================================
namespace nemesis
{
	void DockCtrl_Initialize( DockCtrl_t ctrl, DockMgr_t mgr, DockCtrl::Enum kind, cstr_t text, const Rect_s& rect, bool show, const DockCtrlApi_v& api )
	{
		cstr_t name = text ? text : mgr->AppTitle;
		ctrl->Kind = kind;
		ctrl->Id = Id_Cat( mgr->Id, ++mgr->NextId );
		ctrl->Dc = mgr->Dc;
		ctrl->Mgr = mgr;
		ctrl->Api = api;
		ctrl->Text = (str_t)Mem_Clone( mgr->Alloc, name, 1+Str_Len(name) );
		ctrl->FloatPos.x = floorf(rect.x);
		ctrl->FloatPos.y = floorf(rect.y);
		ctrl->LocalRect = { 0, 0, floorf(rect.w), floorf(rect.h) };
		ctrl->NonClient = {};
		ctrl->Hidden = !show;
		ctrl->NoCaption = false; 
		ctrl->Parent = nullptr;
		ctrl->FirstChild = nullptr;
		ctrl->NextSibling = nullptr;
		ctrl->PrevSibling = nullptr;
		ctrl->ActivePage = nullptr;
		ctrl->SplitSize = 0.0f;
		ctrl->DockSide = DockSide::None;
		ctrl->ScrollState = {};
	}

	bool DockCtrl_IsMain( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
			return true;
		default:
			break;
		}
		return false;
	}

	bool DockCtrl_IsOverlay( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Overlay:
			return true;
		default:
			break;
		}
		return false;
	}

	bool DockCtrl_IsTab( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return true;
		default:
			break;
		}
		return false;
	}

	bool DockCtrl_IsPage( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return true;
		default:
			break;
		}
		return false;
	}

	bool DockCtrl_IsSplitter( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Split_LR:
		case DockCtrl::Split_TB:
			return true;
		default:
			break;
		}
		return false;
	}

	bool DockCtrl_IsContainer( DockCtrl_t ctrl )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Main:
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
		case DockCtrl::Split_LR:
		case DockCtrl::Split_TB:
			return true;
		case DockCtrl::User:
			break;
		default:
			IMPLEMENT_ME;
			break;
		}
		return false;
	}

	bool DockCtrl_IsContainer( DockCtrl_t ctrl, DockSide::Enum side )
	{
		switch (ctrl->Kind)
		{
		case DockCtrl::Tab_B:
		case DockCtrl::Tab_T:
			return (side == DockSide::Center);
		case DockCtrl::Page_B:
		case DockCtrl::Page_T:
			return false;
		case DockCtrl::Split_LR:
			return (side == DockSide::Left) || (side == DockSide::Right);
		case DockCtrl::Split_TB:
			return (side == DockSide::Top) || (side == DockSide::Bottom);
		case DockCtrl::Main:
		case DockCtrl::User:
			return false;
		default:
			IMPLEMENT_ME;
			break;
		}
		return false;
	}

	bool DockCtrl_IsAncestor( DockCtrl_t ctrl, DockCtrl_t test )
	{
		for ( DockCtrl_t ancestor = ctrl->Parent; ancestor; ancestor = ancestor->Parent )
		{
			if (ancestor == test)
				return true;
		}
		return false;
	}

	bool DockCtrl_IsDescendant( DockCtrl_t ctrl, DockCtrl_t test )
	{
		return DockCtrl_IsAncestor( test, ctrl );
	}

	DockCtrl_t DockCtrl_GetRoot( DockCtrl_t ctrl )
	{
		DockCtrl_t root = ctrl;
		while ( root->Parent )
			root = root->Parent;
		return root;
	}

	DockCtrl_t DockCtrl_FindChild( DockCtrl_t ctrl, DockCtrl::Enum kind )
	{
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			if (child->Kind == kind)
				return child;
		return nullptr;
	}

	DockCtrl_t DockCtrl_FindLastChild( DockCtrl_t ctrl )
	{
		DockCtrl_t last = nullptr;
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			last = child;
		return last;
	}

	int DockCtrl_CountChildren( DockCtrl_t ctrl )
	{
		int num_children = 0;
		for ( DockCtrl_t child = ctrl->FirstChild; child; child = child->NextSibling )
			++num_children;
		return num_children;
	}

	void DockCtrl_ReorderAfter( DockCtrl_t ctrl, DockCtrl_t after, DockCtrl_t reorder )
	{
		NeAssert(reorder->Parent == ctrl);
		NeAssert(!after || (after->Parent == ctrl));
		if (after && (after->NextSibling == reorder))
			return;
		if (!after && (ctrl->FirstChild == reorder))
			return;
		DockCtrl_Unlink( reorder );
		DockCtrl_LinkAfter( ctrl, after, reorder );
		DockCtrl_OnReorder( ctrl, reorder );
	}

	void DockCtrl_ReorderBefore( DockCtrl_t ctrl, DockCtrl_t before, DockCtrl_t reorder )
	{
		const DockCtrl_t after = before ? before->PrevSibling : nullptr;
		DockCtrl_ReorderAfter( ctrl, after, reorder );
	}

	void DockCtrl_ReorderFirst( DockCtrl_t ctrl, DockCtrl_t reorder )
	{
		DockCtrl_ReorderAfter( ctrl, nullptr, reorder );
	}

	void DockCtrl_ReorderLast( DockCtrl_t ctrl, DockCtrl_t reorder )
	{
		const DockCtrl_t last = DockCtrl_FindLastChild( ctrl );
		DockCtrl_ReorderAfter( ctrl, last, reorder );
	}

	void DockCtrl_InsertAfter( DockCtrl_t ctrl, DockCtrl_t after, DockCtrl_t insert )
	{
		NeAssert(!insert->Parent);
		DockCtrl_LinkAfter( ctrl, after, insert );
		DockCtrl_DetachHost( insert );
		DockCtrl_OnInsert( ctrl, insert );
	}

	void DockCtrl_InsertBefore( DockCtrl_t ctrl, DockCtrl_t before, DockCtrl_t insert )
	{
		const DockCtrl_t after = before ? before->PrevSibling : nullptr;
		DockCtrl_InsertAfter( ctrl, after, insert );
	}

	void DockCtrl_InsertFirst( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		DockCtrl_InsertAfter( ctrl, nullptr, insert );
	}

	void DockCtrl_InsertLast( DockCtrl_t ctrl, DockCtrl_t insert )
	{
		const DockCtrl_t last = DockCtrl_FindLastChild( ctrl );
		DockCtrl_InsertAfter( ctrl, last, insert );
	}

	void DockCtrl_RemoveChild( DockCtrl_t ctrl, DockCtrl_t remove )
	{
		NeAssert(remove->Parent == ctrl);
		NeAssert(remove->PrevSibling || (ctrl->FirstChild == remove));
		DockCtrl_Unlink( remove );
		DockCtrl_OnRemove( ctrl, remove );
	}

	static void DockCtrl_WrapRoot( DockCtrl_t ctrl, DockCtrl_t container )
	{
		NeAssert(DockCtrl_IsContainer(container));
		NeAssert(ctrl->Parent == nullptr);
		NeAssert(container->Parent == nullptr);
		NeAssert(container->FirstChild == nullptr);
		DockCtrl_InsertFirst( container, ctrl );
	}

	static void DockCtrl_WrapChild( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t container )
	{
		NeAssert(DockCtrl_IsContainer(container));
		NeAssert(child->Parent == ctrl);
		NeAssert(container->Parent == nullptr);
		NeAssert(container->FirstChild == nullptr);
		const DockCtrl_t prev = child->PrevSibling;
		NeAssert(prev || (ctrl->FirstChild == child));
		DockCtrl_LinkAfter( ctrl, child, container );
		DockCtrl_Unlink( child );
		DockCtrl_OnWrap( ctrl, child, container );
		DockCtrl_InsertFirst( container, child );
	}

	void DockCtrl_Wrap( DockCtrl_t ctrl, DockCtrl_t container )
	{
		NeAssert(container->Parent == nullptr);
		NeAssert(container->FirstChild == nullptr);
		if (!ctrl->Parent)
			return DockCtrl_WrapRoot( ctrl, container );
		return DockCtrl_WrapChild( ctrl->Parent, ctrl, container );
	}

	static void DockCtrl_UnwrapRoot( DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsContainer(ctrl));
		NeAssert(ctrl->Parent == nullptr);
		NeAssert(ctrl->FirstChild);
		NeAssert(!ctrl->FirstChild->NextSibling);
		ctrl->FirstChild->FloatPos = ctrl->FloatPos;
		DockCtrl_RemoveChild( ctrl, ctrl->FirstChild );
		DockCtrl_Release( ctrl );
	}

	static void DockCtrl_UnwrapChild( DockCtrl_t ctrl, DockCtrl_t container )
	{
		NeAssert(DockCtrl_IsContainer(container));
		NeAssert(container->Parent == ctrl);
		NeAssert(container->FirstChild && (!container->FirstChild->NextSibling));
		const DockCtrl_t remainder = container->FirstChild;
		DockCtrl_t prev = container->PrevSibling;
		NeAssert(prev || (ctrl->FirstChild == container));
		DockCtrl_Unlink( remainder );
		DockCtrl_LinkAfter( ctrl, prev, remainder );
		DockCtrl_Unlink( container );
		DockCtrl_OnUnwrap( ctrl, container, remainder );
		DockCtrl_Release( container );
	}

	void DockCtrl_Unwrap( DockCtrl_t ctrl )
	{
		return ctrl->Parent
			? DockCtrl_UnwrapChild( ctrl->Parent, ctrl )
			: DockCtrl_UnwrapRoot( ctrl );
	}

	bool DockCtrl_TryUnwrap( DockCtrl_t ctrl )
	{
		DockCtrl_Trace( ctrl, "TryUnwrap" );
		if (DockCtrl_IsMain( ctrl ))
			return false;	// main container

		if (!DockCtrl_IsContainer( ctrl ))
			return false;	// not a container

		if (ctrl->FirstChild && ctrl->FirstChild->NextSibling)
			return false;	// multiple children

		if (!ctrl->FirstChild)
		{
			DockCtrl_Release( ctrl );
			return true;
		}

		// @note:
		//	we must not unwrap document tabs of the
		//	main control while they're still containing
		//	items
		const bool is_main_tab = (ctrl->Kind == DockCtrl::Tab_T);
		if (is_main_tab)
		{
			NeAssert(ctrl->Parent->Kind == DockCtrl::Main);
			NeAssert(ctrl->DockSide == DockSide::Center);
			return false;
		}

		DockCtrl_Unwrap( ctrl );
		return true;
	}

	void DockCtrl_AttachHost( DockCtrl_t ctrl )
	{
		if (ctrl->Host)
			return;
		ctrl->Mgr->Platform.Host_Create( ctrl->Mgr->Os, ctrl );
	}

	void DockCtrl_DetachHost( DockCtrl_t ctrl )
	{
		if (!ctrl->Host)
			return;
		ctrl->Mgr->Platform.Host_Release( ctrl->Host );
		ctrl->Host = nullptr;
	}

	void DockCtrl_DoRoot( DockCtrl_t ctrl )
	{
		NeGuiScopedChild( ctrl->Dc, ctrl->LocalRect );
		DockCtrl_EatMouse( ctrl );
		DockCtrl_DoGui( ctrl );
	}

	void DockCtrl_DoChild( DockCtrl_t ctrl )
	{
		NeGuiScopedChild( ctrl->Dc, ctrl->LocalRect );
		DockCtrl_DoGui( ctrl );
	}

	bool DockCtrl_SetLocalSize( DockCtrl_t ctrl, float w, float h )
	{
		const bool no_change = (w == ctrl->LocalRect.w) && (h == ctrl->LocalRect.h);
		if (no_change)
			return false;
		NeAssert(w > 0.0f);
		NeAssert(h > 0.0f);
		ctrl->LocalRect.w = w;
		ctrl->LocalRect.h = h;
		DockCtrl_OnResize( ctrl );
		return true;
	}

	bool DockCtrl_SetLocalPos( DockCtrl_t ctrl, float x, float y )
	{
		const bool no_change = (x == ctrl->LocalRect.x) && (y == ctrl->LocalRect.y);
		if (no_change)
			return false;
		ctrl->LocalRect.x = x;
		ctrl->LocalRect.y = y;
		return true;
	}

	bool DockCtrl_SetLocalRect( DockCtrl_t ctrl, const Rect_s& rc )
	{
		const bool pos_changed  = DockCtrl_SetLocalPos ( ctrl, rc.x, rc.y );
		const bool size_changed = DockCtrl_SetLocalSize( ctrl, rc.w, rc.h );
		return pos_changed || size_changed;
	}

	void DockCtrl_UpdateNonClient( DockCtrl_t ctrl )
	{
		DockCtrl_SetNonClient( ctrl, DockCtrl_CalcNonClient( ctrl ) );
	}

	void DockCtrl_SetNonClient( DockCtrl_t ctrl, const Pad_s& pad )
	{
		const bool no_change 
			 = (ctrl->NonClient.l == pad.l)
			&& (ctrl->NonClient.t == pad.t)
			&& (ctrl->NonClient.r == pad.r)
			&& (ctrl->NonClient.b == pad.b)
			;
		if (no_change)
			return;
		ctrl->NonClient = pad;
		DockCtrl_OnResize( ctrl );
	}

	float DockCtrl_GetClientX( DockCtrl_t ctrl )
	{
		return ctrl->NonClient.l;
	}

	float DockCtrl_GetClientY( DockCtrl_t ctrl )
	{
		return ctrl->NonClient.t;
	}

	float DockCtrl_GetClientW( DockCtrl_t ctrl )
	{
		return ctrl->LocalRect.w - (ctrl->NonClient.l + ctrl->NonClient.r);
	}

	float DockCtrl_GetClientH( DockCtrl_t ctrl )
	{
		return ctrl->LocalRect.h - (ctrl->NonClient.t + ctrl->NonClient.b);
	}

	Rect_s DockCtrl_GetClientRect( DockCtrl_t ctrl )
	{
		return Rect_s 
			{ DockCtrl_GetClientX( ctrl )
			, DockCtrl_GetClientY( ctrl )
			, DockCtrl_GetClientW( ctrl )
			, DockCtrl_GetClientH( ctrl ) 
			};
	}

	cstr_t DockCtrl_GetKindString( DockCtrl_t ctrl )
	{
		return DockCtrlKind_ToStr( ctrl->Kind );
	}

	cstr_t DockCtrl_GetSideString( DockCtrl_t ctrl )
	{
		return DockSide_ToStr( ctrl->DockSide );
	}

	void DockCtrl_Show( DockCtrl_t ctrl, bool show )
	{
		ctrl->Hidden = !show;
		ctrl->Mgr->Platform.Host_Show( ctrl->Host );
	}

	void DockCtrl_TraceHierarchy( DockCtrl_t ctrl, cstr_t label )
	{
		NeTraceIf(label, "%s:\n", label);
		DockCtrl_TraceHierarchy( ctrl, 0 );
	}

	void DockCtrl_Trace( DockCtrl_t ctrl, cstr_t label )
	{
		NeTrace( "%-20s: [0x%p] (%s) %s\n", label, ctrl, DockCtrlKind_ToStr(ctrl->Kind), ctrl->Text );
	}

	void DockCtrl_Release( DockCtrl_t ctrl )
	{
		if (!ctrl)
			return;
		DockCtrl_Trace( ctrl, "Release" );
		NeAssert(!ctrl->Mgr->Order.Count);
		NeAssert(!ctrl->FirstChild);
		DockCtrl_Undock( ctrl );
		DockCtrl_DetachHost( ctrl );
		{
			const int idx = Array_LinearFind( ctrl->Mgr->Ctrls, ctrl );
			ctrl->Mgr->Ctrls.RemoveAt( idx );
		}
		Mem_Free( ctrl->Mgr->Alloc, ctrl->Text );
		Mem_Free( ctrl->Mgr->Alloc, ctrl );
	}

}
