//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "DockGui.h"

//======================================================================================
namespace nemesis { namespace gui 
{
	static Vec2_s Context_GetCursorPos( Context_t dc )
	{
		return Context_GetMouse( dc ).Screen;
	}

} }

//======================================================================================
namespace nemesis
{
	// Rect

	static Rect_s Rect_BorderL( const Rect_s& r, float w )
	{
		return Rect_s { r.x, r.y, w, r.h };
	}

	static Rect_s Rect_BorderT( const Rect_s& r, float h )
	{
		return Rect_s { r.x, r.y, r.w, h };
	}

	static Rect_s Rect_BorderR( const Rect_s& r, float w )
	{
		return Rect_s { r.x + r.w - w, r.y, w, r.h };
	}

	static Rect_s Rect_BorderB( const Rect_s& r, float h )
	{
		return Rect_s { r.x, r.y + r.h - h, r.w, h };
	}
}

//======================================================================================
namespace nemesis { namespace gui
{
	/// Context

	void Context_DrawRect( Context_t dc, const Rect_s& r, uint32_t c )
	{
		Graphics_DrawRect( Context_GetGraphics( dc ), r, c );
	}

	void Context_FillRect( Context_t dc, const Rect_s& r, uint32_t c )
	{
		Graphics_FillRect( Context_GetGraphics( dc ), r, c );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Ctrl

	void Ctrl_DrawBox( Context_t dc, const Rect_s& r, const Visual_s& v, CtrlState::Enum s )
	{
		Graphics_DrawBox( Context_GetGraphics( dc ), r, v.Back[s], v.Front[s] );
	}
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// PosGrip

	PosGripAction::Enum PosGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, MouseButton::Enum button, Vec2_s& pos )
	{
		const Vec2_s cursor_pos = Context_GetCursorPos( dc );
		if (Button_Mouse( dc, id, r, ButtonClick::Press, button ))
		{
			Context_BeginDragOnce( dc, id, pos - cursor_pos, button );
			return PosGripAction::Pushed;
		}

		if (Context_IsDragging( dc, id ))
		{
			const DragInfo_s& drag = Context_GetDrag( dc );
			if (drag.Button == button)
			{
				pos = drag.Pos + cursor_pos;

				return Context_GetMouse( dc ).Button[button].WentUp 
					? PosGripAction::Released 
					: PosGripAction::Dragged;
			}
		}

		return PosGripAction::None;
	}

	PosGripAction::Enum PosGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, Vec2_s& pos )
	{
		return PosGrip_Mouse( dc, id, r, MouseButton::Left, pos );
	}
	
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// BorderGrip

	struct BorderPart
	{
		enum Flag
		{ None		= 0
		, Left		= 1<<0
		, Top		= 1<<1
		, Right		= 1<<2
		, Bottom	= 1<<3
		};

		typedef uint32_t Mask_t;
	};

	struct BorderDrag_s
	{
		Id_t				Id;
		BorderPart::Mask_t	PartMask;
		Rect_s				ScreenRect;
	};

	static BorderDrag_s TheBorderDrag = {};

	bool Border_Mouse( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen )
	{
		const Vec2_s cursor_pos = Context_GetCursorPos( dc );
		const MouseState_s& mouse = Context_GetMouse( dc );
		if (Context_IsDragging( dc, id ))
		{
			const DragInfo_s& drag = Context_GetDrag( dc );
			const Vec2_s delta = cursor_pos - drag.Pos;
		//	NeTrace("DragOffset: %.2f, %.2f\n", delta.x, delta.y);

			Rect_s screen_rect = TheBorderDrag.ScreenRect;
			const bool on_l = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Left);
			const bool on_t = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Top);
			const bool on_r = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Right);
			const bool on_b = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Bottom);
			const float min_w = NeMin(TheBorderDrag.ScreenRect.w, min_size.x);
			const float min_h = NeMin(TheBorderDrag.ScreenRect.h, min_size.y);
			if (on_l)
			{
				screen_rect.w = NeMax(screen_rect.w - delta.x, min_w);
				screen_rect.x += (TheBorderDrag.ScreenRect.w - screen_rect.w);
			}
			if (on_r)
			{
				screen_rect.w = NeMax(screen_rect.w + delta.x, min_w);
			}
			if (on_t)
			{
				screen_rect.h = NeMax(screen_rect.h - delta.y, min_h);
				screen_rect.y += (TheBorderDrag.ScreenRect.h - screen_rect.h);
			}
			if (on_b)
			{
				screen_rect.h = NeMax(screen_rect.h + delta.y, min_h);
			}

			/*
			NeTrace("[Drag] ScreenRect: %.2f,%.2f [%.2f,%.2f] (%c%c%c%c) %.2f,%.2f\n"
				, screen_rect.x, screen_rect.y
				, screen_rect.w, screen_rect.h
				, on_l ? 'L' : '-'
				, on_t ? 'T' : '-'
				, on_r ? 'R' : '-'
				, on_b ? 'B' : '-'
				, delta.x
				, delta.y
				);
			*/
			screen = screen_rect;
			return true;
		}

		const Vec2_s mouse_pos = cursor_pos;
		const Rect_s outer_rect = screen;
		const Rect_s inner_rect = Rect_Margin( outer_rect, border );
		const bool on_outer = Rect_Contains( outer_rect, mouse_pos );
		const bool on_inner = Rect_Contains( inner_rect, mouse_pos );
		if (!on_outer)
			return false;
		if (on_inner)
			return false;
		const bool on_l = (mouse_pos.x >= outer_rect.x) && (mouse_pos.x <= inner_rect.x);
		const bool on_t = (mouse_pos.y >= outer_rect.y) && (mouse_pos.y <= inner_rect.y);
		const bool on_r = (mouse_pos.x >= Rect_Right ( inner_rect )) && (mouse_pos.x <= Rect_Right ( outer_rect ));
		const bool on_b = (mouse_pos.y >= Rect_Bottom( inner_rect )) && (mouse_pos.y <= Rect_Bottom( outer_rect ));
		if (Context_IsHot( dc, id ))
		{
			TheBorderDrag.Id = id;
			TheBorderDrag.PartMask 
				= (on_l ? BorderPart::Left	 : 0)
				| (on_t ? BorderPart::Top	 : 0)
				| (on_r ? BorderPart::Right  : 0)
				| (on_b ? BorderPart::Bottom : 0)
				;
			TheBorderDrag.ScreenRect = screen;

			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetPushed( dc, id );
				Context_BeginDragOnce( dc, id, cursor_pos );
				/*
				NeTrace("[Begin] ScreenRect: %.2f,%.2f [%.2f,%.2f] (%c%c%c%c)\n"
					, screen_rect.x, screen_rect.y
					, screen_rect.w, screen_rect.h
					, on_l ? 'L' : '-'
					, on_t ? 'T' : '-'
					, on_r ? 'R' : '-'
					, on_b ? 'B' : '-'
					);
				*/
			}
		}
		Context_SetHot( dc, id );
		return false;
	}

	bool Border_Draw( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen )
	{
		if (Context_Cull( dc, r ))
			return false;

		const Rect_s rc_l = Rect_BorderL( r, border.x );
		const Rect_s rc_t = Rect_BorderT( r, border.y );
		const Rect_s rc_r = Rect_BorderR( r, border.x );
		const Rect_s rc_b = Rect_BorderB( r, border.y );
		const bool is_hot	 = Context_IsHot( dc, id );
		const bool is_pushed = Context_IsPushed( dc, id );
		const bool is_active = is_hot || is_pushed;
		const bool on_l = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Left);
		const bool on_t = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Top);
		const bool on_r = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Right);
		const bool on_b = NeHasFlag(TheBorderDrag.PartMask, BorderPart::Bottom);
		const CtrlState::Enum cs_ctrl 
			= is_pushed ? CtrlState::Pushed 
			: is_hot	? CtrlState::Hot
			:			  CtrlState::Normal;
		const CtrlState::Enum cs_l = (is_active && on_l) ? cs_ctrl : CtrlState::Normal;
		const CtrlState::Enum cs_t = (is_active && on_t) ? cs_ctrl : CtrlState::Normal;
		const CtrlState::Enum cs_r = (is_active && on_r) ? cs_ctrl : CtrlState::Normal;
		const CtrlState::Enum cs_b = (is_active && on_b) ? cs_ctrl : CtrlState::Normal;

		Ctrl_DrawBox( dc, rc_l, Visual::Splitter, cs_l );
		Ctrl_DrawBox( dc, rc_t, Visual::Splitter, cs_t );
		Ctrl_DrawBox( dc, rc_r, Visual::Splitter, cs_r );
		Ctrl_DrawBox( dc, rc_b, Visual::Splitter, cs_b );
		return true;
	}

	bool Border_Do( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen )
	{
		Border_Draw( dc, id, r, border, min_size, screen );
		return Border_Mouse( dc, id, r, border, min_size, screen );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	Rect_s Splitter2_CalcRect( Context_t dc, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float pos )
	{
		const float half_size = floorf(splitter_size * 0.5f);
		return (dir == Orientation::Vert)
			? Rect_s { pos - half_size + center_offset, r.y, splitter_size, r.h }
			: Rect_s { r.x, pos - half_size + center_offset, r.w, splitter_size };
	}

	bool Splitter2_Draw( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float pos )
	{
		if (Context_Cull( dc, r ))
			return false;
		const Rect_s rect = Splitter2_CalcRect( dc, r, dir, splitter_size, center_offset, pos );
		const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, id );
		Ctrl_DrawBox( dc, rect, Visual::Splitter, ctrl_state );
		return true;
	}

	bool Splitter2_Mouse( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float& pos )
	{
		const Rect_s splitter_rect = Splitter2_CalcRect( dc, r, dir, splitter_size, center_offset, pos );

		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_ctrl = Rect_Contains( splitter_rect, mouse_pos );

		if (Context_IsDragging( dc, id ))
		{
			// @note:
			//	the addition of half the size is there
			//	because we use the top-left corner
			//	of the bar rect as the drag coord
			const float half_size = floorf(splitter_size * 0.5f);
			const Vec2_s coord = mouse_pos - Context_GetDrag( dc ).Pos;
			pos = (dir == Orientation::Horz)
				? NeClamp( coord.y + half_size - center_offset, r.y, Rect_Bottom(r) )
				: NeClamp( coord.x + half_size - center_offset, r.x, Rect_Right (r) )
				;
			return true;
		}

		if (!on_ctrl)
			return false;

		const Vec2_s drag_pos = mouse_pos - Rect_Pos( splitter_rect );
		if (Button_Mouse( dc, id, splitter_rect, ButtonClick::Press ))
			Context_BeginDragOnce( dc, id, drag_pos );
		return false;
	}

	bool Splitter2_Do( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float& pos )
	{
		Splitter2_Draw( dc, id, r, dir, splitter_size, center_offset, pos );
		return Splitter2_Mouse( dc, id, r, dir, splitter_size, center_offset, pos );
	}

	bool Splitter2_DoH( Context_t dc, Id_t id, const Rect_s& r, float splitter_size, float center_offset, float& pos )
	{
		return Splitter2_Do( dc, id, r, Orientation::Horz, splitter_size, center_offset, pos );
	}

	bool Splitter2_DoV( Context_t dc, Id_t id, const Rect_s& r, float splitter_size, float center_offset, float& pos )
	{
		return Splitter2_Do( dc, id, r, Orientation::Vert, splitter_size, center_offset, pos );
	}

} }
