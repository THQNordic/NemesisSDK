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
namespace nemesis
{
	struct DockCtrl
	{
		enum Enum
		{ None
		, Main
		, Tab_B
		, Tab_T
		, Page_B
		, Page_T
		, Split_LR
		, Split_TB
		, User
		, Overlay
		, COUNT
		};
	};

	struct DockCtrl_s
	{
		DockCtrl::Enum		Kind;
		gui::Id_t			Id;
		gui::Context_t		Dc;
		DockMgr_t			Mgr;
		DockCtrlApi_v		Api;
		str_t				Text;
		Vec2_s				FloatPos;
		Rect_s				LocalRect;
		Pad_s				NonClient;
		uint32_t			Hidden	  : 1;
		uint32_t			NoCaption : 1;
		DockHost_t			Host;
		DockCtrl_t			Parent;
		DockCtrl_t			FirstChild;
		DockCtrl_t			NextSibling;
		DockCtrl_t			PrevSibling;
		DockCtrl_t			ActivePage;
		float				SplitSize;
		DockSide::Enum		DockSide;
		Rect_s				RemainRect;
		DockScroll_s		ScrollState;
	};
}

//======================================================================================
namespace nemesis
{
	void		NE_API DockCtrl_Initialize		( DockCtrl_t ctrl, DockMgr_t mgr, DockCtrl::Enum kind, cstr_t text, const Rect_s& rect, bool show, const DockCtrlApi_v& api );
	bool		NE_API DockCtrl_IsMain			( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsOverlay		( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsTab			( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsPage			( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsSplitter		( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsContainer		( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_IsContainer		( DockCtrl_t ctrl, DockSide::Enum side );
	bool		NE_API DockCtrl_IsAncestor		( DockCtrl_t ctrl, DockCtrl_t test ); 
	bool		NE_API DockCtrl_IsDescendant	( DockCtrl_t ctrl, DockCtrl_t test ); 
	DockCtrl_t	NE_API DockCtrl_GetRoot			( DockCtrl_t ctrl );
	DockCtrl_t	NE_API DockCtrl_FindChild		( DockCtrl_t ctrl, DockCtrl::Enum kind );
	DockCtrl_t	NE_API DockCtrl_FindLastChild	( DockCtrl_t ctrl );
	int			NE_API DockCtrl_CountChildren	( DockCtrl_t ctrl );
	void		NE_API DockCtrl_ReorderAfter	( DockCtrl_t ctrl, DockCtrl_t after , DockCtrl_t reorder );
	void		NE_API DockCtrl_ReorderBefore	( DockCtrl_t ctrl, DockCtrl_t before, DockCtrl_t reorder );
	void		NE_API DockCtrl_ReorderFirst	( DockCtrl_t ctrl, DockCtrl_t reorder );
	void		NE_API DockCtrl_ReorderLast		( DockCtrl_t ctrl, DockCtrl_t reorder );
	void		NE_API DockCtrl_InsertAfter		( DockCtrl_t ctrl, DockCtrl_t after , DockCtrl_t insert );
	void		NE_API DockCtrl_InsertBefore	( DockCtrl_t ctrl, DockCtrl_t before, DockCtrl_t insert );
	void		NE_API DockCtrl_InsertFirst		( DockCtrl_t ctrl, DockCtrl_t insert );
	void		NE_API DockCtrl_InsertLast		( DockCtrl_t ctrl, DockCtrl_t insert );
	void		NE_API DockCtrl_RemoveChild		( DockCtrl_t ctrl, DockCtrl_t remove );
	void		NE_API DockCtrl_Wrap			( DockCtrl_t ctrl, DockCtrl_t container );
	void		NE_API DockCtrl_Unwrap			( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_TryUnwrap		( DockCtrl_t ctrl );
	void		NE_API DockCtrl_AttachHost		( DockCtrl_t ctrl );
	void		NE_API DockCtrl_DetachHost		( DockCtrl_t ctrl );
	void		NE_API DockCtrl_DoRoot			( DockCtrl_t ctrl );
	void		NE_API DockCtrl_DoChild			( DockCtrl_t ctrl );
	bool		NE_API DockCtrl_SetLocalRect	( DockCtrl_t ctrl, const Rect_s& rc );
	bool		NE_API DockCtrl_SetLocalSize	( DockCtrl_t ctrl, float w, float h );
	bool		NE_API DockCtrl_SetLocalPos		( DockCtrl_t ctrl, float x, float y );
	void		NE_API DockCtrl_SetNonClient	( DockCtrl_t ctrl, const Pad_s& pad );
	void		NE_API DockCtrl_UpdateNonClient	( DockCtrl_t ctrl );
	float		NE_API DockCtrl_GetClientX		( DockCtrl_t ctrl );
	float		NE_API DockCtrl_GetClientY		( DockCtrl_t ctrl );
	float		NE_API DockCtrl_GetClientW		( DockCtrl_t ctrl );
	float		NE_API DockCtrl_GetClientH		( DockCtrl_t ctrl );
	Rect_s		NE_API DockCtrl_GetClientRect	( DockCtrl_t ctrl );
	cstr_t		NE_API DockCtrl_GetKindString	( DockCtrl_t ctrl );
	cstr_t		NE_API DockCtrl_GetSideString	( DockCtrl_t ctrl );
	void		NE_API DockCtrl_Show			( DockCtrl_t ctrl, bool show );
	void		NE_API DockCtrl_Release			( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	void NE_API DockCtrl_TraceHierarchy	( DockCtrl_t ctrl, cstr_t label );
	void NE_API DockCtrl_Trace			( DockCtrl_t ctrl, cstr_t label );
}
