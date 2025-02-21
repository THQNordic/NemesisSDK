//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gui/Controls.h>
#include <Nemesis/Core/Math.h>
#include <Nemesis/Core/Memory.h>
#include <math.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace gui
{
	/// Wcs

	static void Wcs_Nice_Flt( wstr_t str, size_t count, int max_zeros )
	{
		if (max_zeros < 0)
			return;

		int dot = -1;
		int last_zero = -1;
		int last_non_zero = -1;
		for ( int i = 0; i < count; ++i )
		{
			// store dot position
			if (str[i] == L'.')
			{
				dot = i;
				continue;
			}

			// skip chars before dot
			if (dot < 0)
				continue;

			if (str[i] != L'0')
				last_non_zero = i;
			else
				last_zero = i;
		}

		if (dot < 0)
			return;	// no fractional digits

		if (last_non_zero < 0)
		{
			// all fractional digits are zeros
			// keep two of them and discard the rest
			// but beware of the case where 
			// there are fewer than two zeros
			const int num_zeros = ((int)count-1)-dot;
			if (num_zeros < max_zeros)
				return;

			// if there're to strip all fractional digits
			// we should strip the dot as well 
			const int num_keep = max_zeros ? (max_zeros + 1) : 0;	
			const int end = dot + num_keep;
			str[end] = L'\0';
			return;
		}

		// there are non-zero digits in the fractional part
		if (last_zero < last_non_zero)
			return; // but the last digit was not a zero

		// tim all trailing zeros
		str[last_non_zero+1] = L'\0';
	}

	static void Wcs_Fmt_Flt( wstr_t str, size_t count, float v, int max_zeros )
	{
		const int num = Wcs_Fmt( str, count, L"%f", v );
		Wcs_Nice_Flt( str, num, max_zeros );
	}

	static void Wcs_Fmt_Flt( wstr_t str, size_t count, float v )
	{
		Wcs_Fmt_Flt( str, count, v, 0 );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Rect

	static Rect_s Rect_CapHeight( const Rect_s& r, float height )
	{ return Rect_s { r.x, r.y, r.w, NeMin( r.h, height ) }; }

	static Rect_s Rect_ClampSize( const Rect_s& old_r, const Rect_s& new_r, const Vec2_s& min )
	{
		// determine underflow for width and height
		const float dw = NeMin( 0.0f, new_r.w - min.x );
		const float dh = NeMin( 0.0f, new_r.h - min.y );
		if ((dw == 0.0f) && (dh == 0.0f))
			return new_r;
		const float dx = (old_r.x != new_r.x) ? dw : 0.0f;
		const float dy = (old_r.y != new_r.y) ? dh : 0.0f;
		const Rect_s adj_r = 
		{ new_r.x + dx	// keep left edge fixed
		, new_r.y + dy	// keep top  edge fixed
		, new_r.w - dw	// fix underflow in width 
		, new_r.h - dh	// fix underflow in height
		};
		return adj_r;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Graphics

	void Graphics_DrawArrow( Graphics_t g, const Rect_s& r, Arrow::Dir dir, uint32_t argb )
	{
		Vec2_s a,b,c;
		switch ( dir )
		{
		default:
		case Arrow::Left:
			a = Vec2_s { r.x	  , r.y + 0.5f * r.h };	// l
			b = Vec2_s { r.x + r.w, r.y				 };	// t
			c = Vec2_s { r.x + r.w, r.y + r.h		 };	// b 
			break;
		case Arrow::Right:
			a = Vec2_s { r.x + r.w, r.y + 0.5f * r.h };	// r
			b = Vec2_s { r.x	  , r.y + r.h		 };	// b 
			c = Vec2_s { r.x	  , r.y				 };	// t
			break;
		case Arrow::Up:
			a = Vec2_s { r.x + 0.5f * r.w, r.y		 };	// t
			b = Vec2_s { r.x			 , r.y + r.h };	// l
			c = Vec2_s { r.x + r.w		 , r.y + r.h };	// r 
			break;
		case Arrow::Down:
			a = Vec2_s { r.x + 0.5f * r.w, r.y + r.h };	// b
			b = Vec2_s { r.x 			 , r.y		 };	// l
			c = Vec2_s { r.x + r.w		 , r.y		 };	// r
			break;
		}
		Graphics_FillTriangle( g, a, b, c, argb );
	}

	void Graphics_DrawBox( Graphics_t g, Theme_t theme, const Rect_s& r, Visual::Enum v, CtrlState::Enum s )
	{
		const Visual_s& vis = theme->Visual[ v ];
		Graphics_DrawBox( g, r, vis.Front[ s ], vis.Back[ s ] );
	}

	void Graphics_DrawRadio( Graphics_t g, Theme_t theme, const Rect_s& r, Visual::Enum v, CtrlState::Enum s )
	{
		const Visual_s& vis = theme->Visual[ v ];
		Graphics_DrawRadio( g, r, vis.Front[ s ], vis.Back[ s ] );
	}

	void Graphics_DrawString( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s )
	{
		const Visual_s& vis = theme->Visual[ v ];
		Graphics_DrawString( g, r, t, theme->Font, f, vis.Text[ s ] );
	}

	void Graphics_DrawBox( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m )
	{
		const Rect_s text_rect = Rect_Margin( r, theme->Metric[ m ] );
		Graphics_DrawBox	( g, theme, r			   , v, s );
		Graphics_DrawString	( g, theme, text_rect, t, f, v, s );
	}

	void Graphics_DrawRadio( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m )
	{
		const Rect_s text_rect = Rect_Margin( r, theme->Metric[ m ] );
		Graphics_DrawRadio	( g, theme, r			   , v, s );
		Graphics_DrawString	( g, theme, text_rect, t, f, v, s );
	}

	void Context_DrawArrow( Context_t dc, const Rect_s& r, Arrow::Dir dir, uint32_t argb )
	{
		Graphics_t g = Context_GetGraphics( dc );
		return Graphics_DrawArrow( g, r, dir, argb );
	}

	void Context_DrawArrow( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s, Arrow::Dir dir )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Visual_s& vis = theme->Visual[ v ];
		Context_DrawArrow( dc, r, dir, vis.Front[ s ] );
	}

	void Context_DrawRadio( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s )
	{
		Graphics_DrawRadio
			( Context_GetGraphics( dc )
			, Context_GetTheme( dc )
			, r
			, v
			, s
			);
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Ctrl

	CtrlState::Enum Ctrl_GetState( Context_t dc, Id_t id )
	{
		if (Context_IsPushed( dc, id ))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ))
			return CtrlState::Hot;
		return CtrlState::Normal;
	}

	CtrlState::Enum Ctrl_GetState( Context_t dc, Id_t id, int index, int hot, int pushed )
	{
		if (Context_IsPushed( dc, id ) && (index == pushed))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ) && (index == hot))
			return CtrlState::Hot;
		return CtrlState::Normal;
	}

	bool Ctrl_IsAnyDragging( Context_t dc, const Id_t* id, int count )
	{
		const Id_t drag_id = Context_GetDrag( dc ).Id;
		for ( int i = 0; i < count; ++i )
			if (id[i] == drag_id)
				return true;
		return false;
	}

	void Ctrl_DrawBox( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s )
	{
		Graphics_DrawBox
			( Context_GetGraphics( dc )
			, Context_GetTheme( dc )
			, r
			, v
			, s
			);
	}

	void Ctrl_DrawText( Context_t dc, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m )
	{
		Graphics_DrawBox
			( Context_GetGraphics( dc )
			, Context_GetTheme( dc )
			, r
			, t
			, f
			, v
			, s
			, m
			);
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Alignment

	TextFormat::Mask_t Alignment_ToTextFormat( Alignment::Horz horz )
	{
		return (horz == Alignment::Center) ? TextFormat::Center 
			 : (horz == Alignment::Right)  ? TextFormat::Right
			 :								 TextFormat::Left
			 ;
	}

	TextFormat::Mask_t Alignment_ToTextFormat( Alignment::Vert vert )
	{
		return (vert == Alignment::Middle) ? TextFormat::Middle 
			 : (vert == Alignment::Bottom) ? TextFormat::Bottom
			 :								 TextFormat::Top
			 ;
	}
	TextFormat::Mask_t Alignment_ToTextFormat( Alignment::Horz horz, Alignment::Vert vert )
	{
		return Alignment_ToTextFormat( horz )
			 | Alignment_ToTextFormat( vert );
	}

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// SortOrder

	SortOrder::Enum SortOrder_Next( SortOrder::Enum order )
	{
		return (order == SortOrder::Ascending) ? SortOrder::Descending : SortOrder::Ascending;
	}

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextList 

	float TextList_CalcMaxWidth( Context_t dc, const TextList_s& list )
	{
		return TextList_CalcMaxWidth( dc, Context_GetFont( dc ), list );
	}

	float TextList_CalcMaxWidth( Context_t dc, const cstr_t* items, int count )
	{
		return TextList_CalcMaxWidth( dc, Context_GetFont( dc ), items, count );
	}

	float TextList_CalcMaxWidth( Context_t dc, const wcstr_t* items, int count )
	{
		return TextList_CalcMaxWidth( dc, Context_GetFont( dc ), items, count );
	}

	float TextList_CalcMaxWidth( Context_t dc, Font_t font, const TextList_s& list )
	{
		return TextList_CalcMaxSize( dc, font, list ).x;
	}

	float TextList_CalcMaxWidth( Context_t dc, Font_t font, const cstr_t* items, int count )
	{
		return TextList_CalcMaxSize( dc, font, items, count ).x;
	}

	float TextList_CalcMaxWidth( Context_t dc, Font_t font, const wcstr_t* items, int count )
	{
		return TextList_CalcMaxSize( dc, font, items, count ).x;
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, const TextList_s& list )
	{
		return TextList_CalcMaxSize( dc, Context_GetFont( dc ), list );
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, const cstr_t* items, int count )
	{
		return TextList_CalcMaxSize( dc, Context_GetFont( dc ), items, count );
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, const wcstr_t* items, int count )
	{
		return TextList_CalcMaxSize( dc, Context_GetFont( dc ), items, count );
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, Font_t font, const TextList_s& list )
	{
		return list.Utf8 
			? TextList_CalcMaxSize( dc, font, list.Utf8, list.NumItems )
			: TextList_CalcMaxSize( dc, font, list.Utf16, list.NumItems );
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, Font_t font, const cstr_t* items, int count )
	{
		Graphics_t g = Context_GetGraphics( dc );
		float max_w = 0.0f;
		float line_h = 0.0f;
		if (items)
		{
			float item_w = 0.0f;
			for ( int i = 0; i < count; ++i )
			{
				item_w = Graphics_MeasureString( g, items[i], -1, font ).w;
				max_w = NeMax( max_w, item_w );
			}
			if (count)
				line_h = Graphics_GetLineHeight( g, font );
		}
		return Vec2_s { max_w, line_h };
	}

	Vec2_s TextList_CalcMaxSize( Context_t dc, Font_t font, const wcstr_t* items, int count )
	{
		Graphics_t g = Context_GetGraphics( dc );
		float max_w = 0.0f;
		float line_h = 0.0f;
		if (items)
		{
			float item_w = 0.0f;
			for ( int i = 0; i < count; ++i )
			{
				item_w = Graphics_MeasureString( g, items[i], -1, font ).w;
				max_w = NeMax( max_w, item_w );
			}
			if (count)
				line_h = Graphics_GetLineHeight( g, font );
		}
		return Vec2_s { max_w, line_h };
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Label

	Vec2_s Label_Do( Context_t dc, const Vec2_s& p, const Text_s& t )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		return Graphics_DrawString( g, p, t, theme->Font, theme->Visual[ Visual::Label ].Text[ CtrlState::Normal ] );
	}

	Vec2_s Label_FmtV( Context_t dc, const Vec2_s& p, cstr_t fmt, va_list args )
	{
		return Label_Do( dc, p, Context_FormatStringV( dc, fmt, args ) );
	}

	Vec2_s Label_FmtV( Context_t dc, const Vec2_s& p, wcstr_t fmt, va_list args )
	{
		return Label_Do( dc, p, Context_FormatStringV( dc, fmt, args ) );
	}

	Vec2_s Label_Fmt( Context_t dc, const Vec2_s& p, cstr_t fmt, ... )
	{
		Vec2_s out;
		va_list args;
		va_start( args, fmt );
		out = Label_FmtV( dc, p, fmt, args );
		va_end( args );
		return out;
	}

	Vec2_s Label_Fmt( Context_t dc, const Vec2_s& p, wcstr_t fmt, ... )
	{
		Vec2_s out;
		va_list args;
		va_start( args, fmt );
		out = Label_FmtV( dc, p, fmt, args );
		va_end( args );
		return out;
	}

	void Label_Do( Context_t dc, const Rect_s& r, const Text_s& t )
	{
		Label_Do( dc, r, TextFormat::Center | TextFormat::Middle, t );
	}

	void Label_Do( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, const Text_s& t )
	{
		Label_Do( dc, r, s, Context_GetTheme( dc )->Visual[ Visual::Label ].Text[ CtrlState::Normal ], t );
	}

	void Label_Do( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, uint32_t color, const Text_s& t )
	{
		if (Context_Cull( dc, r ))
			return;
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		Graphics_DrawString( g, r, t, theme->Font, s, color );
	}

	void Label_FmtV( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, cstr_t fmt, va_list args )
	{
		Label_Do( dc, r, s, Context_FormatStringV( dc, fmt, args ) );
	}

	void Label_FmtV( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, wcstr_t fmt, va_list args )
	{
		Label_Do( dc, r, s, Context_FormatStringV( dc, fmt, args ) );
	}

	void Label_Fmt( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, cstr_t fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		Label_FmtV( dc, r, s, fmt, args );
		va_end( args );
	}

	void Label_Fmt( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, wcstr_t fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		Label_FmtV( dc, r, s, fmt, args );
		va_end( args );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Button

	float Button_CalcHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight(g, theme->Font);
		const float margin_height = theme->Metric[ Metric::Button_Margin ].y;
		return line_height + 2.0f * margin_height;
	}

	Vec2_s Button_CalcSize( Context_t dc, const Text_s& text )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s bounds = Graphics_MeasureString( g, text, theme->Font );

		return Rect_Size( bounds ) 
			+ theme->Metric[ Metric::Button_Margin ]
			+ theme->Metric[ Metric::Button_Margin ]
			;
	}

	bool Button_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, CtrlState::Enum s )
	{
		if (Context_Cull( dc, r ))
			return false;
		Ctrl_DrawText( dc, r, text, TextFormat::Center | TextFormat::Middle, Visual::Button, s, Metric::Button_Margin );
		return true;
	}

	bool Button_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, ButtonStyle::Mask_t s )
	{
		return Button_Draw( dc, id, r, text, NeHasFlag(s, ButtonStyle::Disabled) ? CtrlState::Disabled : Ctrl_GetState( dc, id ) );
	}

	bool Button_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text )
	{
		return Button_Draw( dc, id, r, text, Ctrl_GetState( dc, id ) );
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode, int index, int& hot, int& pushed )
	{
		if (!Context_IsPushed( dc, id ))
			pushed = -1;

		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_ctrl = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );

		if (!on_ctrl)
			return action;

		Context_SetHot( dc, id );
		if (pushed < 0)
			hot = index;

		if (Context_IsHot( dc, id ) && (hot == index))
		{
			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetPushed( dc, id );
				pushed = index;
			}
		}

		if (Context_IsPushed( dc, id ) && (pushed == index))
		{
			if (Context_IsHot( dc, id ))
			{
				const bool trigger = ((mode == ButtonClick::Press)		 &&  mouse.Button[MB::Left].WentDown)
								  || ((mode == ButtonClick::Release)	 &&  mouse.Button[MB::Left].WentUp	)
								  || ((mode == ButtonClick::DoubleClick) && (mouse.Button[MB::Left].NumClicks == 2));
				if (trigger)
					action = ButtonAction::Clicked;
			}
		}
		return action;	
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode )
	{
		return Button_Mouse( dc, id, r, mode, MouseButton::Left );
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode, MouseButton::Enum button )
	{
		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_ctrl = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );

		if (!on_ctrl)
			return action;

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[button].WentDown)
				Context_SetPushed( dc, id );

			if (Context_IsPushed( dc, id ))
			{
				const bool trigger = ((mode == ButtonClick::Press)		   &&  mouse.Button[button].WentDown)
									|| ((mode == ButtonClick::Release)	   &&  mouse.Button[button].WentUp	)
									|| ((mode == ButtonClick::DoubleClick) && (mouse.Button[button].NumClicks == 2));
				if (trigger)
					action = ButtonAction::Clicked;
			}
		}
		return action;
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r, ButtonStyle::Mask_t s )
	{
		if (NeHasFlag(s, ButtonStyle::Disabled))
			return ButtonAction::None;
		return Button_Mouse( dc, id, r, ButtonClick::Release );
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r )
	{
		return Button_Mouse( dc, id, r, ButtonClick::Release );
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, Id_t id, const Rect_s& r, int index, int& hot, int& pushed )
	{
		return Button_Mouse( dc, id, r, ButtonClick::Release, index, hot, pushed );
	}

	ButtonAction::Enum Button_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text )
	{
		return Button_Do( dc, id, r, text, ButtonStyle::None );
	}

	ButtonAction::Enum Button_Do( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text )
	{
		return Button_Do( dc, id, p, text, ButtonStyle::None );
	}

	ButtonAction::Enum Button_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, ButtonStyle::Mask_t s )
	{
		if (!Button_Draw( dc, id, r, text, s ))
			return ButtonAction::None;
		return Button_Mouse( dc, id, r, s );
	}

	ButtonAction::Enum Button_Do( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, ButtonStyle::Mask_t s )
	{
		const Vec2_s size = Button_CalcSize( dc, text );
		return Button_Do( dc, id, Rect_Ctor( p, size ), text, s );
	}

	Button_s Button_Instance( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, ButtonStyle::Mask_t s )
	{
		const Vec2_s size = Button_CalcSize( dc, text );
		const Button_s instance = { id, Rect_Ctor( p, size ), text, s };
		return instance;
	}

	Button_s Button_Instance( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text )
	{
		return Button_Instance( dc, id, p, text, ButtonStyle::None );
	}

	bool Button_Draw( Context_t dc, const Button_s& instance )
	{
		return Button_Draw( dc, instance.Id, instance.Rect, instance.Text, instance.Style );
	}

	ButtonAction::Enum Button_Mouse( Context_t dc, const Button_s& instance )
	{
		return Button_Mouse( dc, instance.Id, instance.Rect, instance.Style );
	}

	ButtonAction::Enum Button_Do( Context_t dc, const Button_s& instance )
	{
		if (!Button_Draw( dc, instance ))
			return ButtonAction::None;
		return Button_Mouse( dc, instance );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// CheckBox

	Vec2_s CheckBox_CalcSize( Context_t dc, const Text_s& text )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Rect_s text_bounds = Graphics_MeasureString( g, text, theme->Font );
		const Vec2_s text_size = Rect_Size( text_bounds );

		Vec2_s client_size = text_size;
		client_size.y = NeMax( client_size.y, theme->Metric[ Metric::CheckBox_Box_Size ].y );	// at least as high as the box, but higher if font is larger
		client_size.x += theme->Metric[ Metric::CheckBox_Box_Size ].x;

		return client_size
			+ theme->Metric[ Metric::CheckBox_Margin ]
			+ theme->Metric[ Metric::CheckBox_Margin ]
			;
	}

	Rect_s CheckBox_CalcBoxRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		
		const float client_h	= r.h - (2.0f * theme->Metric[ Metric::CheckBox_Margin	 ].y);
		const float box_h		=				theme->Metric[ Metric::CheckBox_Box_Size ].y;
		const float off_center	= 0.5f * (client_h - box_h);

		return Rect_s
			{ r.x
			, r.y + theme->Metric[ Metric::CheckBox_Margin ].y	+ off_center
			,		theme->Metric[ Metric::CheckBox_Box_Size ].x
			,		theme->Metric[ Metric::CheckBox_Box_Size ].y
			};
	}

	Rect_s CheckBox_CalcCheckRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s box_rect = CheckBox_CalcBoxRect( dc ,r );
		return Rect_Margin( box_rect, theme->Metric[ Metric::CheckBox_Check_Margin ] );
	}

	Rect_s CheckBox_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s client_rect = Rect_Margin( r, theme->Metric[ Metric::CheckBox_Margin ] );
		return Rect_s
			{ client_rect.x + theme->Metric[ Metric::CheckBox_Box_Size ].x
			, client_rect.y
			, client_rect.w - theme->Metric[ Metric::CheckBox_Box_Size ].x
			, client_rect.h
			};
	}

	bool CheckBox_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool checked )
	{
		if (Context_Cull( dc, r ))
			return false;

		// state
		const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, id );

		// box
		const Rect_s box_rect = CheckBox_CalcBoxRect( dc, r );
		Ctrl_DrawBox( dc, box_rect, Visual::CheckBox_Box, ctrl_state );
		if (checked)
		{
			// check mark
			const Rect_s check_rect = CheckBox_CalcCheckRect( dc, r );
			Ctrl_DrawBox( dc, check_rect, Visual::CheckBox_Check, ctrl_state );
		}

		// label
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s label_rect	= CheckBox_CalcLabelRect( dc, r );
		Label_Do( dc, label_rect, TextFormat::Middle, theme->Visual[Visual::CheckBox_Box].Text[ctrl_state], text );
		return true;
	}

	ButtonAction::Enum CheckBox_Mouse( Context_t dc, Id_t id, const Rect_s& r, bool& checked )
	{
		const Rect_s box_rect = CheckBox_CalcBoxRect( dc, r );
		const ButtonAction::Enum action = Button_Mouse( dc, id, box_rect );
		if (action)
			checked = !checked;
		return action;
	}

	ButtonAction::Enum CheckBox_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool& checked )
	{
		if (!CheckBox_Draw( dc, id, r, text, checked ))
			return ButtonAction::None;
		return CheckBox_Mouse( dc, id, r, checked );
	}

	ButtonAction::Enum CheckBox_DoButton( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool& checked )
	{
		const CtrlState::Enum ctrl_state = checked ? CtrlState::Pushed : Ctrl_GetState( dc, id );
		if (!Button_Draw( dc, id, r, text, ctrl_state ))
			return ButtonAction::None;
		const ButtonAction::Enum action = Button_Mouse( dc, id, r );
		if (action)
			checked = !checked;
		return action;
	}

	Vec2_s CheckBox_Do( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool& checked )
	{
		const Vec2_s size = CheckBox_CalcSize( dc, text );
		CheckBox_Do( dc, id, Rect_Ctor( p, size ), text, checked );
		return size;
	}

	Vec2_s CheckBox_DoButton( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool& checked )
	{
		const Vec2_s size = Button_CalcSize( dc, text );
		CheckBox_DoButton( dc, id, Rect_Ctor( p, size ), text, checked );
		return size;
	}

	CheckBox_s CheckBox_Instance( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool checked )
	{
		const Vec2_s size = CheckBox_CalcSize( dc, text );
		const CheckBox_s instance = { id, Rect_Ctor( p, size ), text, checked };
		return instance;
	}

	bool CheckBox_Draw( Context_t dc, const CheckBox_s& instance )
	{
		return CheckBox_Draw( dc, instance.Id, instance.Rect, instance.Text, instance.Checked );
	}

	ButtonAction::Enum CheckBox_Mouse( Context_t dc, CheckBox_s& instance )
	{
		return CheckBox_Mouse( dc, instance.Id, instance.Rect, instance.Checked );
	}

	ButtonAction::Enum CheckBox_Do( Context_t dc, CheckBox_s& instance )
	{
		return CheckBox_Do( dc, instance.Id, instance.Rect, instance.Text, instance.Checked );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// RadioButton

	Vec2_s RadioButton_CalcSize( Context_t dc, const Text_s& text )
	{
		return CheckBox_CalcSize( dc, text );
	}

	bool RadioButton_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool checked )
	{
		if (Context_Cull( dc, r ))
			return false;

		// state
		const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, id );

		// box
		const Rect_s box_rect = CheckBox_CalcBoxRect( dc, r );
		Context_DrawRadio( dc, box_rect, Visual::CheckBox_Box, ctrl_state );
		if (checked)
		{
			// check mark
			const Rect_s check_rect = CheckBox_CalcCheckRect( dc, r );
			Context_DrawRadio( dc, check_rect, Visual::CheckBox_Check, ctrl_state );
		}

		// label
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s label_rect	= CheckBox_CalcLabelRect( dc, r );
		Label_Do( dc, label_rect, TextFormat::Middle, theme->Visual[Visual::CheckBox_Box].Text[ctrl_state], text );
		return true;
	}

	ButtonAction::Enum RadioButton_Mouse( Context_t dc, Id_t id, const Rect_s& r, int idx, int& sel )
	{
		bool checked = (idx == sel);
		const ButtonAction::Enum action = CheckBox_Mouse( dc, id, r, checked );
		if (checked)
			sel = idx;
		return action;
	}

	ButtonAction::Enum RadioButton_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, int idx, int& sel )
	{
		if (!RadioButton_Draw( dc, id, r, text, idx == sel ))
			return ButtonAction::None;
		return RadioButton_Mouse( dc, id, r, idx, sel );
	}

	ButtonAction::Enum RadioButton_DoButton( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, int idx, int& sel )
	{
		const CtrlState::Enum ctrl_state = (idx == sel) ? CtrlState::Pushed : Ctrl_GetState( dc, id );
		if (!Button_Draw( dc, id, r, text, ctrl_state ))
			return ButtonAction::None;
		const ButtonAction::Enum action = Button_Mouse( dc, id, r );
		if (action)
			sel = idx;
		return action;
	}

	Vec2_s RadioButton_Do( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx, int& sel )
	{
		const Vec2_s size = RadioButton_CalcSize( dc, text );
		RadioButton_Do( dc, id, Rect_Ctor( p, size ), text, idx, sel );
		return size;
	}
						
	Vec2_s RadioButton_DoButton( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx, int& sel )
	{
		const Vec2_s size = Button_CalcSize( dc, text );
		RadioButton_DoButton( dc, id, Rect_Ctor( p, size ), text, idx, sel );
		return size;
	}

	RadioButton_s RadioButton_Instance( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx )
	{
		const Vec2_s size = RadioButton_CalcSize( dc, text );
		const RadioButton_s instance = { id, Rect_Ctor( p, size ), text, idx };
		return instance;
	}

	bool RadioButton_Draw( Context_t dc, const RadioButton_s& instance, int sel )
	{
		return RadioButton_Draw( dc, instance.Id, instance.Rect, instance.Text, (instance.Index == sel) );
	}

	ButtonAction::Enum RadioButton_Mouse( Context_t dc, RadioButton_s& instance, int& sel )
	{
		return RadioButton_Mouse( dc, instance.Id, instance.Rect, instance.Index, sel );
	}

	ButtonAction::Enum RadioButton_Do( Context_t dc, RadioButton_s& instance, int& sel )
	{
		return RadioButton_Do( dc, instance.Id, instance.Rect, instance.Text, instance.Index, sel );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextEditBuffer

	void TextEditBuffer_Initialize( TextEditBuffer_s& buffer, wchar_t* text, int num_chars, int caret, int sel )
	{
		const int len = Wcs_Len( text, num_chars );
		buffer.Text = text;
		buffer.Capacity = num_chars;
		buffer.Length = len;
		buffer.CaretPos = NeClamp( caret, 0, len );
		buffer.SelectPos = NeClamp( sel, 0, len );
	}

	bool TextEditBuffer_HasSelection( const TextEditBuffer_s& buffer )
	{ 
		return (buffer.SelectPos != buffer.CaretPos); 
	}

	void TextEditBuffer_SetCaret( TextEditBuffer_s& buffer, int pos, bool select )
	{
		const int caret_pos = NeClamp( pos, 0, buffer.Length );
		buffer.CaretPos = caret_pos;
		buffer.SelectPos = select ? buffer.SelectPos : caret_pos;
	}

	void TextEditBuffer_DeleteSelection( TextEditBuffer_s& buffer )
	{
		const int p0 = NeMin( buffer.CaretPos, buffer.SelectPos );
		const int p1 = NeMax( buffer.CaretPos, buffer.SelectPos );
		const int num_del = p1-p0;
		if (!num_del)
			return;
		const int num_move = buffer.Length - p1;
		Mem_Mov( buffer.Text + p0, buffer.Text + p1, num_move * sizeof(buffer.Text[0]) );
		buffer.Length -= num_del;
		buffer.SelectPos = buffer.CaretPos = p0;
		buffer.Text[ buffer.Length ] = L'\0';
	}

	void TextEditBuffer_Delete( TextEditBuffer_s& buffer )
	{
		if (TextEditBuffer_HasSelection( buffer ))
		{
			TextEditBuffer_DeleteSelection( buffer );
			return;
		}
		if (buffer.CaretPos >= buffer.Length)
			return;
		Mem_Mov( buffer.Text + buffer.CaretPos, buffer.Text + 1 + buffer.CaretPos, (buffer.Length - buffer.CaretPos) * sizeof(buffer.Text[0]) );
		--buffer.Length;
		buffer.SelectPos = buffer.CaretPos;
	}

	void TextEditBuffer_Remove( TextEditBuffer_s& buffer )
	{
		if (TextEditBuffer_HasSelection( buffer ))
		{
			TextEditBuffer_DeleteSelection( buffer );
			return;
		}
		if (buffer.CaretPos <= 0)
			return;
		Mem_Mov( buffer.Text + buffer.CaretPos - 1, buffer.Text + buffer.CaretPos, (buffer.Length + 1 - buffer.CaretPos) * sizeof(buffer.Text[0]) );
		--buffer.Length;
		--buffer.CaretPos;
		buffer.SelectPos = buffer.CaretPos;
	}

	void TextEditBuffer_Insert( TextEditBuffer_s& buffer, const wchar_t* t, int count )
	{
		TextEditBuffer_DeleteSelection( buffer );

		const int remain = buffer.Capacity - buffer.Length - 1;
		const int num_insert = NeMin( count, remain );
		Mem_Mov( buffer.Text + buffer.CaretPos + num_insert, buffer.Text + buffer.CaretPos, (1 + buffer.Length - buffer.CaretPos) * sizeof(buffer.Text[0]) );
		Mem_Cpy( buffer.Text + buffer.CaretPos, remain, t, num_insert );
		buffer.Length += num_insert;
		buffer.CaretPos += num_insert;
		buffer.SelectPos = buffer.CaretPos;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextEdit

	float TextEdit_CalcHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::TextEdit_Margin ].y;
	}

	float TextEdit_CalcWidth( Context_t dc, float w )
	{
		Theme_t theme = Context_GetTheme( dc );
		return w + 2.0f * theme->Metric[ Metric::TextEdit_Margin ].x;
	}

	Vec2_s TextEdit_CalcSize( Context_t dc, float w )
	{
		return Vec2_s
			{ TextEdit_CalcWidth ( dc, w )
			, TextEdit_CalcHeight( dc ) 
			};
	}

	Rect_s TextEdit_CalcTextRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[ Metric::TextEdit_Margin ] );
	}

	CtrlState::Enum TextEdit_GetCtrlState( Context_t dc, Id_t id, TextEditStyle::Mask_t style )
	{
		if (NeHasFlag( style, TextEditStyle::ReadOnly ))
			return CtrlState::Disabled;
		if (Context_HasFocus( dc, id ))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ))
			return CtrlState::Hot;
		return CtrlState::Normal;
	}

	static bool TextEdit_HasAlphaSelect( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		const uint32_t sel_color = theme->Visual[Visual::TextEdit_Selection].Back[CtrlState::Pushed];
		return (sel_color & 0xff000000) != 0xff000000;
	}

	static void TextEdit_DrawSelection( Context_t dc, const Rect_s& text_rect, const Text_s& text, float scroll, int caret, int select )
	{
		const bool has_sel = (caret != select);
		if (!has_sel)
			return;
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float x0 = Graphics_CaretToCoord( g, text, theme->Font, caret  );
		const float x1 = Graphics_CaretToCoord( g, text, theme->Font, select );
		const Rect_s select_rect = { NeMin(x0, x1) - scroll, 0.0f, fabsf(x1-x0), text_rect.h };
		Ctrl_DrawBox( dc, select_rect, Visual::TextEdit_Selection, CtrlState::Pushed );
	}

	static void TextEdit_DrawText( Context_t dc, const Rect_s& text_rect, CtrlState::Enum ctrl_state, const Text_s& text, float scroll )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s scroll_rect = Rect_s { - scroll, 0.0f, text_rect.w + scroll, text_rect.h };
		Graphics_DrawString( g, theme, scroll_rect, text, TextFormat::Middle, Visual::TextEdit, ctrl_state );
	}

	static void TextEdit_DrawCaret( Context_t dc, Id_t id, const Rect_s& text_rect, const Text_s& text, float scroll, int caret, int select )
	{
		const bool has_sel = (caret != select);
		if (has_sel)
			return;
		const bool has_focus = Context_HasFocus( dc, id );
		if (!has_focus)
			return;
		if (!Context_GetCaret( dc ).Show)
			return;
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s scroll_rect = Rect_s { - scroll, 0.0f, text_rect.w + scroll, text_rect.h };
		const float caret_x = Graphics_CaretToCoord( g, text, theme->Font, caret );
		const Rect_s caret_rect = Rect_s { scroll_rect.x + caret_x, scroll_rect.y, 1.0f, scroll_rect.h };
		Graphics_DrawRect( g, caret_rect, theme->Visual[ Visual::TextEdit_Caret ].Text[ CtrlState::Normal ] );
	}

	bool TextEdit_Draw( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, CtrlState::Enum ctrl_state, const Text_s& text, float scroll, int caret, int select )
	{
		if (Context_Cull( dc, r ))
			return false;

		// ctrl
		const bool has_focus = Context_HasFocus( dc, id );
		const bool is_writable = !NeHasFlag( style, TextEditStyle::ReadOnly );
		Ctrl_DrawBox( dc, r, Visual::TextEdit, ctrl_state );

		// client
		const Rect_s text_rect = TextEdit_CalcTextRect( dc, r );
		Context_BeginChild( dc, text_rect );
		{
			Theme_t theme = Context_GetTheme( dc );
			if (TextEdit_HasAlphaSelect( dc ))
			{
				TextEdit_DrawText	  ( dc, text_rect, ctrl_state, text, scroll );
				TextEdit_DrawSelection( dc, text_rect, text, scroll, caret, select );
			}
			else
			{
				TextEdit_DrawSelection( dc, text_rect, text, scroll, caret, select );
				TextEdit_DrawText	  ( dc, text_rect, ctrl_state, text, scroll );
			}
			if (!NeHasFlag( style, TextEditStyle::NoCaret))
				TextEdit_DrawCaret( dc, id, text_rect, text, scroll, caret, select );
		}
		Context_EndChild( dc );
		return true;
	}

	bool TextEdit_Draw( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, const Text_s& text, float scroll, int caret, int select )
	{
		const CtrlState::Enum ctrl_state = TextEdit_GetCtrlState( dc, id, style );
		return TextEdit_Draw( dc, id, r, style, ctrl_state, text, scroll, caret, select );
	}

	bool TextEdit_Draw( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, cstr_t text, int num_chars, float scroll, int caret, int select )
	{
		return TextEdit_Draw( dc, id, r, style, Text_s( text, num_chars ), scroll, caret, select );
	}

	bool TextEdit_Draw( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wcstr_t text, int num_chars, float scroll, int caret, int select )
	{
		return TextEdit_Draw( dc, id, r, style, Text_s( text, num_chars ), scroll, caret, select );
	}

	void TextEdit_Scroll( Context_t dc, const Rect_s& r, wchar_t* text, int num_chars, int caret, float& scroll )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s text_rect = TextEdit_CalcTextRect( dc, r );
		const float max_w = text_rect.w - 2.0f;
		const float x = Graphics_CaretToCoord( g, text, theme->Font, caret );
		if ((x - scroll) > max_w)
			scroll = x - max_w;
		if ((x - scroll) < 0.0f)
			scroll = x;
	}

	void TextEdit_SetCoord( Context_t dc, const Rect_s& r, const Vec2_s& p, bool do_select, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		TextEditBuffer_s buffer;
		TextEditBuffer_Initialize( buffer, text, num_chars, caret, select );

		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );

		const Rect_s text_rect = TextEdit_CalcTextRect( dc, r );
		const int hit_caret = Graphics_CoordToCaret( g, text, theme->Font, p.x - text_rect.x + scroll );

		TextEditBuffer_SetCaret( buffer, hit_caret, do_select );
		caret  = buffer.CaretPos;
		select = buffer.SelectPos;

		TextEdit_Scroll( dc, r, text, num_chars, caret, scroll );
	}

	void TextEdit_Mouse( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_ctrl = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );

		if (Context_IsDragging( dc, id ))
		{
			if (mouse.Button[MB::Left].IsPressed)
				TextEdit_SetCoord( dc, r, mouse_pos, true, text, num_chars, scroll, caret, select );
		}

		if (!on_ctrl)
			return;

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[MB::Left].WentDown)
				Context_SetPushed( dc, id );
		}

		if (Context_IsPushed( dc, id ))
		{
			if (Context_IsHot( dc, id ))
			{
				if (mouse.Button[MB::Left].WentDown)
				{
					const bool shift_pressed = Context_GetKeyboard( dc ).Key[ VK::Shift ].IsPressed;
					TextEdit_SetCoord( dc, r, mouse_pos, shift_pressed, text, num_chars, scroll, caret, select );
					Context_BeginDragOnce( dc, id, mouse_pos );
				}

				else if (mouse.Button[MB::Left].NumClicks == 2)
				{
					// @todo: select word
				}
			}
		}
	}

	void TextEdit_Keyboard( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		if (!Context_HasFocus( dc, id ))
			return;

		TextEditBuffer_s buffer;
		TextEditBuffer_Initialize( buffer, text, num_chars, caret, select );

		const KeyboardState_s& kb = Context_GetKeyboard( dc );

		const bool is_writable = !NeHasFlag( style, TextEditStyle::ReadOnly );
		const bool is_shift_pressed = kb.Key[ VK::Shift ].IsPressed;

		// navigation
		if ( kb.Key[ VK::Home ].AutoRepeat )
		{
			TextEditBuffer_SetCaret( buffer, 0, is_shift_pressed );
		}
		else if ( kb.Key[ VK::End ].AutoRepeat )
		{
			TextEditBuffer_SetCaret( buffer, buffer.Length, is_shift_pressed );
		}
		else if ( kb.Key[ VK::Left ].AutoRepeat )
		{
			TextEditBuffer_SetCaret( buffer, buffer.CaretPos - 1, is_shift_pressed );
		}
		else if ( kb.Key[ VK::Right ].AutoRepeat )
		{
			TextEditBuffer_SetCaret( buffer, buffer.CaretPos + 1, is_shift_pressed );
		}

		// editing
		if (is_writable)
		{
			if ( kb.Key[ VK::Back ].AutoRepeat )
			{
				TextEditBuffer_Remove( buffer );
			}
			else if ( kb.Key[ VK::Delete ].AutoRepeat )
			{
				TextEditBuffer_Delete( buffer );
			}
		}

		// handle text input
		if (is_writable && kb.Char[0])
			TextEditBuffer_Insert( buffer, kb.Char, Wcs_Len( kb.Char ) );

		// reset caret blinking iff it moved
		if (caret != buffer.CaretPos)
			Context_SetFocus( dc, id );

		// update result
		caret = buffer.CaretPos;
		select = buffer.SelectPos;

		// keep the caret position in view
		TextEdit_Scroll( dc, r, text, num_chars, caret, scroll );
}

	void TextEdit_Input( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		TextEdit_Mouse	 ( dc, id, r, style, text, num_chars, scroll, caret, select );
		TextEdit_Keyboard( dc, id, r, style, text, num_chars, scroll, caret, select );
	}

	void TextEdit_Do( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		if (!TextEdit_Draw( dc, id, r, style, text, num_chars, scroll, caret, select ))
			return;
		TextEdit_Input( dc, id, r, style, text, num_chars, scroll, caret, select );
	}

	Vec2_s TextEdit_Do( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select )
	{
		const Vec2_s size = TextEdit_CalcSize( dc, w );
		TextEdit_Do( dc, id, Rect_Ctor( p, size ), style, text, num_chars, scroll, caret, select );
		return size;
	}

	void TextEdit_Do( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, TextEditData_s& data, TextEditState_s& state )
	{
		TextEdit_Do( dc, id, r, style, data.Text, data.NumChars, state.Scroll, state.Caret, state.Select );
	}

	Vec2_s TextEdit_Do( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, TextEditData_s& data, TextEditState_s& state )
	{
		return TextEdit_Do( dc, id, p, w, style, data.Text, data.NumChars, state.Scroll, state.Caret, state.Select );
	}

	void TextEdit_Do( Context_t dc, TextEdit_s& instance )
	{
		return TextEdit_Do( dc, instance.Id, instance.Rect, instance.Style, instance.Data, instance.State );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextEdit Extensions

	TextEditData_s TextEdit_Do( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wcstr_t v )
	{
		// persistent state
		typedef wchar_t wstr_256_t[256];
		TextEditState_s& focus_state = Context_CacheIntern< TextEditState_s >( dc, NE_UI_ID );
		wstr_256_t&		 focus_text  = Context_CacheIntern< wstr_256_t >	 ( dc, NE_UI_ID );

		// volatile state
		TextEditState_s& edit_state = Context_CacheIntern< TextEditState_s > ( dc, NE_UI_ID );
		wstr_256_t&	     edit_text  = Context_CacheIntern< wstr_256_t >		 ( dc, NE_UI_ID );
		TextEditData_s   edit_data  = { edit_text, NeCountOf( edit_text ) };

		// update
		if (Context_HasFocus( dc, id ))
		{
			edit_state = focus_state;
			Wcs_Cpy( edit_text, NeCountOf(edit_text), focus_text );
		}
		else
		{
			NeZero( edit_state );
			Wcs_Fmt( edit_text, NeCountOf( edit_text ), L"%s", v );
		}

		// edit
		TextEdit_Do( dc, id, r, style, edit_data, edit_state );

		// update
		if (Context_HasFocus( dc, id ))
		{
			focus_state = edit_state;
			Wcs_Cpy( focus_text, NeCountOf(focus_text), edit_text );
		}

		return edit_data;
	}

	void TextEdit_DoInt( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, int& v )
	{
		wchar_t edit_text[64] = L"";
		Wcs_Fmt( edit_text, NeCountOf( edit_text ), L"%d", v );
		TextEditData_s edit_data = TextEdit_Do( dc, id, r, style, edit_text );
		if (Context_HasFocus( dc, id ))
		{
			if (Context_GetKeyboard( dc ).Key[ VK::Return ].WentDown)
				v = Wcs_ToInt( edit_data.Text );
		}
	}

	void TextEdit_DoInt( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, float& v )
	{
		wchar_t edit_text[64] = L"";
		Wcs_Fmt( edit_text, NeCountOf( edit_text ), L"%d", (int)v );
		TextEditData_s edit_data = TextEdit_Do( dc, id, r, style, edit_text );
		if (Context_HasFocus( dc, id ))
		{
			if (Context_GetKeyboard( dc ).Key[ VK::Return ].WentDown)
				v = (float)Wcs_ToInt( edit_data.Text );
		}
	}

	void TextEdit_DoFloat( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, float& v )
	{
		wchar_t edit_text[64] = L"";
		Wcs_Fmt_Flt( edit_text, NeCountOf( edit_text ), v );
		TextEditData_s edit_data = TextEdit_Do( dc, id, r, style, edit_text );
		if (Context_HasFocus( dc, id ))
		{
			if (Context_GetKeyboard( dc ).Key[ VK::Return ].WentDown)
				v = Wcs_ToFloat( edit_data.Text );
		}
	}

	void TextEdit_DoString( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wstr_t v, int l )
	{
		TextEditData_s edit_data = TextEdit_Do( dc, id, r, style, v );
		if (Context_HasFocus( dc, id ))
		{
			if (Context_GetKeyboard( dc ).Key[ VK::Return ].WentDown)
				Wcs_Cpy( v, l, edit_data.Text );
		}
	}

	Vec2_s TextEdit_DoInt( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, int& v )
	{
		const Vec2_s size = TextEdit_CalcSize( dc, w );
		TextEdit_DoInt( dc, id, Rect_Ctor( p, size ), style, v );
		return size;
	}

	Vec2_s TextEdit_DoInt( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, float& v )
	{
		const Vec2_s size = TextEdit_CalcSize( dc, w );
		TextEdit_DoInt( dc, id, Rect_Ctor( p, size ), style, v );
		return size;
	}

	Vec2_s TextEdit_DoFloat( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, float& v )
	{
		const Vec2_s size = TextEdit_CalcSize( dc, w );
		TextEdit_DoFloat( dc, id, Rect_Ctor( p, size ), style, v );
		return size;
	}

	Vec2_s TextEdit_DoString( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, wstr_t v, int l )
	{
		const Vec2_s size = TextEdit_CalcSize( dc, w );
		TextEdit_DoString( dc, id, Rect_Ctor( p, size ), v, l );
		return size;
	}

	void TextEdit_DoInt( Context_t dc, Id_t id, const Rect_s& r, int& v )
	{
		return TextEdit_DoInt( dc, id, r, TextEditStyle::None, v );
	}

	void TextEdit_DoInt( Context_t dc, Id_t id, const Rect_s& r, float& v )
	{
		return TextEdit_DoInt( dc, id, r, TextEditStyle::None, v );
	}

	void TextEdit_DoFloat( Context_t dc, Id_t id, const Rect_s& r, float& v )
	{
		return TextEdit_DoFloat( dc, id, r, TextEditStyle::None, v );
	}

	void TextEdit_DoString( Context_t dc, Id_t id, const Rect_s& r, wstr_t v, int l )
	{
		return TextEdit_DoString( dc, id, r, TextEditStyle::None, v, l );
	}

	Vec2_s TextEdit_DoInt( Context_t dc, Id_t id, const Vec2_s& p, float w, int& v )
	{
		return TextEdit_DoInt( dc, id, p, w, TextEditStyle::None, v );
	}

	Vec2_s TextEdit_DoInt( Context_t dc, Id_t id, const Vec2_s& p, float w, float& v )
	{
		return TextEdit_DoInt( dc, id, p, w, TextEditStyle::None, v );
	}

	Vec2_s TextEdit_DoFloat( Context_t dc, Id_t id, const Vec2_s& p, float w, float& v )
	{
		return TextEdit_DoFloat( dc, id, p, w, TextEditStyle::None, v );
	}

	Vec2_s TextEdit_DoString( Context_t dc, Id_t id, const Vec2_s& p, float w, wstr_t v, int l )
	{
		return TextEdit_DoString( dc, id, p, w, TextEditStyle::None, v, l );
	}


} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ProgressBar

	Vec2_s ProgressBar_CalcSize( Context_t dc, float w )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Vec2_s { w, theme->Metric[ Metric::Progress_Track_Size ].y };
	}

	Rect_s ProgressBar_CalcBarRect( Context_t dc, const Rect_s& r, float min, float max, float pos )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s track_rect = Rect_Margin( r, theme->Metric[ Metric::Progress_Bar_Margin ] );
		const float alpha = NeClamp( (pos-min)/(max-min), 0.0f, 1.0f );
		return Rect_s { track_rect.x, track_rect.y, track_rect.w * alpha, track_rect.h };
	}

	bool ProgressBar_Draw( Context_t dc, const Rect_s& r, float min, float max, float pos )
	{
		if (Context_Cull( dc, r ))
			return false;
		const Rect_s bar_rect = ProgressBar_CalcBarRect( dc, r, min, max, pos );
		Ctrl_DrawBox( dc, r		  , Visual::Progress_Track, CtrlState::Normal );
		Ctrl_DrawBox( dc, bar_rect, Visual::Progress_Bar  , CtrlState::Hot	  );
		return true;
	}

	void ProgressBar_Do( Context_t dc, const Rect_s& r, float min, float max, float pos )
	{
		ProgressBar_Draw( dc, r, min, max, pos );
	}

	Vec2_s ProgressBar_Do( Context_t dc, const Vec2_s& p, float w, float min, float max, float pos )
	{
		const Vec2_s size = ProgressBar_CalcSize( dc, w );
		ProgressBar_Do( dc, Rect_Ctor( p, size ), min, max, pos );
		return size;
	}

	ProgressBar_s ProgressBar_Instance( Context_t dc, const Vec2_s& p, float w, float min, float max, float pos )
	{
		const Vec2_s size = ProgressBar_CalcSize( dc, w );
		const ProgressBar_s instance = { Rect_Ctor( p, size ), min, max, pos };
		return instance;
	}

	bool ProgressBar_Draw( Context_t dc, const ProgressBar_s& instance )
	{
		return ProgressBar_Draw( dc, instance.Rect, instance.Min, instance.Max, instance.Pos );
	}

	void ProgressBar_Do( Context_t dc, const ProgressBar_s& instance )
	{
		return ProgressBar_Do( dc, instance.Rect, instance.Min, instance.Max, instance.Pos );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Slider

	CtrlState::Enum Slider_GetThumbState( Context_t dc, Id_t id, const Rect_s& thumb_rect )
	{
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_thumb_rect = Rect_Contains( thumb_rect, mouse_pos );
		if (Context_IsPushed( dc, id ))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ) && on_thumb_rect)
			return CtrlState::Hot;
		return CtrlState::Normal;
	}

	Vec2_s Slider_CalcSize( Context_t dc, Orientation::Enum dir, float track )
	{
		Theme_t theme = Context_GetTheme( dc );
		const bool horz = (dir == Orientation::Horz);
		return Vec2_s { horz ? track : 0.0f, horz ? 0.0f : track } + theme->Metric[ Metric::Slider_Thumb_Size ];
	}

	Rect_s Slider_CalcTrackRect( Context_t dc, Orientation::Enum dir, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& thumb_size = theme->Metric[ Metric::Slider_Thumb_Size ];
		const Vec2_s& track_size = theme->Metric[ Metric::Slider_Track_Size ];
		return (dir == Orientation::Horz) 
			? Rect_s { r.x + 0.5f * thumb_size.x, r.y + 0.5f * (r.h - track_size.y), r.w - thumb_size.x, track_size.y }
			: Rect_s { r.x + 0.5f * (r.w - track_size.x), r.y + 0.5f * thumb_size.y, track_size.x, r.h - thumb_size.y };
	}

	Rect_s Slider_CalcThumbRect( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float pos )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s track_rect = Slider_CalcTrackRect( dc, dir, r );

		const float dist  = pos-min;
		const float range = max-min;

		const bool horz = (dir == Orientation::Horz);
		const float client_size = (horz ? track_rect.w : track_rect.h);
		const float thumb_coord = client_size * (range ? NeClamp(dist/range, 0.0f, 1.0f) : 0.0f);

		const Vec2_s& thumb_size = theme->Metric[ Metric::Slider_Thumb_Size ];

		return horz
			? Rect_s { r.x + thumb_coord, r.y, thumb_size.x, r.h }
			: Rect_s { r.x, r.y + thumb_coord, r.w, thumb_size.y };
	}

	float Slider_CoordToPos( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, const Vec2_s& coord )
	{
		const Rect_s track_rect = Slider_CalcTrackRect( dc, dir, r );

		const bool horz = (dir == Orientation::Horz);
		const float client_size = (horz ? track_rect.w : track_rect.h);
		const float thumb_coord  = horz ? (coord.x - r.x) : (coord.y - r.y);

		const float range = max-min;
		const float dist = client_size ? (thumb_coord * range / client_size) : 0.0f;

		return NeClamp( min + dist, min, max );
	}

	bool Slider_Draw( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float pos )
	{
		if (Context_Cull( dc, r ))
			return false;

		// track
		const Rect_s track_rect = Slider_CalcTrackRect( dc, dir, r );
		Ctrl_DrawBox( dc, track_rect, Visual::Slider_Track, CtrlState::Normal ); 

		// thumb
		const Rect_s thumb_rect = Slider_CalcThumbRect( dc, dir, r, min, max, pos );
		const CtrlState::Enum ctrl_state = Slider_GetThumbState( dc, id, thumb_rect );
		Ctrl_DrawBox( dc, thumb_rect, Visual::Slider_Thumb, ctrl_state );
		return true;
	}

	void Slider_Mouse( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float& pos )
	{
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_ctrl = Rect_Contains( r, mouse_pos );

		// tracking
		if (Context_IsDragging( dc, id ))
		{
			const Vec2_s coord = mouse_pos - Context_GetDrag( dc ).Pos;
			pos = Slider_CoordToPos( dc, dir, r, min, max, coord );
		}

		if (!on_ctrl)
			return;

		// track
		if (Button_Mouse( dc, id, r, ButtonClick::Press ))
		{
			const Rect_s track_rect = Slider_CalcTrackRect( dc, dir, r );
			const Vec2_s offset = Rect_Pos( track_rect ) - Rect_Pos( r );
			pos = Slider_CoordToPos( dc, dir, r, min, max, mouse_pos - offset );
		}

		// thumb
		{
			const Rect_s thumb_rect = Slider_CalcThumbRect( dc, dir, r, min, max, pos );
			const Vec2_s drag_pos = mouse_pos - Rect_Pos( thumb_rect );
			if (Button_Mouse( dc, id, thumb_rect, ButtonClick::Press ))
				Context_BeginDragOnce( dc, id, drag_pos );
		}
	}

	void Slider_Do( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float& pos )
	{
		if (!Slider_Draw( dc, dir, id, r, min, max, pos ))
			return;
		return Slider_Mouse( dc, dir, id, r, min, max, pos );
	}

	Vec2_s Slider_Do( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos )
	{
		const Vec2_s size = Slider_CalcSize( dc, dir, track );
		Slider_Do( dc, dir, id, Rect_Ctor( p, size ), min, max, pos );
		return size;
	}

	void Slider_DoH( Context_t dc, Id_t id, const Rect_s& r, int min, int max, int& pos )
	{
		float v = (float)pos;
		Slider_Do( dc, Orientation::Horz, id, r, (float)min, (float)max, v );
		pos = (int)(roundf(v));
	}

	void Slider_DoV( Context_t dc, Id_t id, const Rect_s& r, int min, int max, int& pos )
	{
		float v = (float)pos;
		Slider_Do( dc, Orientation::Vert, id, r, (float)min, (float)max, v );
		pos = (int)(roundf(v));
	}

	void Slider_DoH( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float& pos )
	{
		return Slider_Do( dc, Orientation::Horz, id, r, min, max, pos );
	}

	void Slider_DoV( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float& pos )
	{
		return Slider_Do( dc, Orientation::Vert, id, r, min, max, pos );
	}

	Vec2_s Slider_DoH( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos )
	{
		return Slider_Do( dc, Orientation::Horz, id, p, track, min, max, pos );
	}

	Vec2_s Slider_DoV( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos )
	{
		return Slider_Do( dc, Orientation::Vert, id, p, track, min, max, pos );
	}

	Slider_s Slider_Instance( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float pos )
	{
		const Vec2_s size = Slider_CalcSize( dc, dir, track );
		const Slider_s instance = { id, Rect_Ctor( p, size ), min, max, pos };
		return instance;
	}

	bool Slider_Draw( Context_t dc, Orientation::Enum dir, const Slider_s& instance )
	{
		return Slider_Draw( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Pos );
	}

	void Slider_Mouse( Context_t dc, Orientation::Enum dir, Slider_s& instance )
	{
		return Slider_Mouse( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Pos );
	}

	void Slider_Do( Context_t dc, Orientation::Enum dir, Slider_s& instance )
	{
		return Slider_Do( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Pos );
	}

	void Slider_DoH( Context_t dc, Slider_s& instance )
	{
		return Slider_Do( dc, Orientation::Horz, instance );
	}

	void Slider_DoV( Context_t dc, Slider_s& instance )
	{
		return Slider_Do( dc, Orientation::Vert, instance );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollRange

	void ScrollRange_Normalize( float& min, float& max, float& line, float& page, float& pos )
	{
		if (min > max)
			NeSwap( min, max );
		page = NeClamp( page, min, max  );
		line = NeClamp( line, min, page );
		pos  = NeClamp( pos , min, max  );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollBar

	/// Range: Max-Min		(distance from min to max)
	/// Limit: Range-Page	(distance from min to start of last page)
	/// End:   Max-Page		(start of last page) 

	CtrlState::Enum ScrollBar_GetThumbState( Context_t dc, Id_t id, const Rect_s& thumb_rect )
	{
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_thumb_rect = Rect_Contains( thumb_rect, mouse_pos );
		if (Context_IsPushed( dc, id ))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ) && on_thumb_rect)
			return CtrlState::Hot;
		return CtrlState::Normal;
	}

	Id_t ScrollBar_GetArrowId( Id_t id, int index )
	{ 
		return Id_Cat( id, index );
	}

	Vec2_s ScrollBar_CalcSize( Context_t dc, Orientation::Enum dir, float track )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& arrow_size = theme->Metric[ Metric::ScrollBar_Arrow_Size ];
		return (dir == Orientation::Horz)
			? Vec2_s { arrow_size.x + track + arrow_size.x, arrow_size.y }
			: Vec2_s { arrow_size.x, arrow_size.y + track + arrow_size.y };
	}

	Rect_s ScrollBar_CalcTrackRect( Context_t dc, Orientation::Enum dir, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& arrow_size = theme->Metric[ Metric::ScrollBar_Arrow_Size ];
		return (dir == Orientation::Horz)
			? Rect_Margin( r, arrow_size.x, 0.0f, arrow_size.x, 0.0f )
			: Rect_Margin( r, 0.0f, arrow_size.y, 0.0f, arrow_size.y );
	}

	Rect_s ScrollBar_CalcArrowRect( Context_t dc, Orientation::Enum dir, const Rect_s& r, int index )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& arrow_size = theme->Metric[ Metric::ScrollBar_Arrow_Size ];
		return (dir == Orientation::Horz)
			? Rect_s { r.x + (index ? (r.w - arrow_size.x) : 0.0f), r.y, arrow_size.x, r.h }
			: Rect_s { r.x, r.y + (index ? (r.h - arrow_size.y) : 0.0f), r.w, arrow_size.y };
	}

	Rect_s ScrollBar_CalcThumbRect( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float page, float pos )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s track_rect = ScrollBar_CalcTrackRect( dc, dir, r );
		const Rect_s client_rect = Rect_Margin( track_rect, theme->Metric[ Metric::ScrollBar_Thumb_Margin ] );

		const float dist	= pos-min;
		const float range	= max-min;
		const float limit	= range-page;

		const bool horz = (dir == Orientation::Horz);
		const float client_size = (horz ? client_rect.w : client_rect.h);

		const float thumb_size  = client_size * (range ? NeClamp(page/range, 0.0f, 1.0f) : 1.0f);
		const float remain_size = client_size - thumb_size;
		const float thumb_coord = remain_size * (limit ? NeClamp(dist/limit, 0.0f, 1.0f) : 0.0f);

		return (dir == Orientation::Horz)
			? Rect_s { client_rect.x + thumb_coord, client_rect.y, thumb_size, client_rect.h }
			: Rect_s { client_rect.x, client_rect.y + thumb_coord, client_rect.w, thumb_size };
	}

	float ScrollBar_CoordToPos( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float page, const Vec2_s& coord )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s track_rect = ScrollBar_CalcTrackRect( dc, dir, r );
		const Rect_s client_rect = Rect_Margin( track_rect, theme->Metric[ Metric::ScrollBar_Thumb_Margin ] );

		const float end = max-page;
		const float range = max-min;
		const float limit = range-page;

		const bool horz = (dir == Orientation::Horz);
		const float client_size = (horz ? client_rect.w : client_rect.h);

		const float thumb_size  = client_size * (range ? NeClamp(page/range, 0.0f, 1.0f) : 1.0f);
		const float remain_size = client_size - thumb_size;
		const float thumb_coord  = horz ? (coord.x - client_rect.x) : (coord.y - client_rect.y);

		const float dist = remain_size ? (thumb_coord * limit / remain_size) : 0.0f;
		const float pos  = dist + min;

		return NeClamp( pos, min, end );
	}

	bool ScrollBar_Draw( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float page, float pos )
	{
		if (Context_Cull( dc, r ))
			return false;

		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& arrow_margin = theme->Metric[ Metric::ScrollBar_Arrow_Margin ];

		const Rect_s track_rect = ScrollBar_CalcTrackRect( dc, dir, r );
		const Rect_s thumb_rect = ScrollBar_CalcThumbRect( dc, dir, r, min, max, page, pos );
		const Rect_s arrow_rect_0 = ScrollBar_CalcArrowRect( dc, dir, r, 0 );
		const Rect_s arrow_rect_1 = ScrollBar_CalcArrowRect( dc, dir, r, 1 );

		const CtrlState::Enum ctrl_state = ScrollBar_GetThumbState( dc, id, thumb_rect );
		const CtrlState::Enum arrow_state_0 = Ctrl_GetState( dc, ScrollBar_GetArrowId( id, 0 ) );
		const CtrlState::Enum arrow_state_1 = Ctrl_GetState( dc, ScrollBar_GetArrowId( id, 1 ) );

		const bool is_flat_style = true;

		if (is_flat_style)
		{
			Ctrl_DrawBox( dc, r, Visual::ScrollBar_Track, CtrlState::Normal );
		}
		else
		{
			Ctrl_DrawBox( dc, track_rect  , Visual::ScrollBar_Track, CtrlState::Normal );
			Ctrl_DrawBox( dc, arrow_rect_0, Visual::ScrollBar_Track, CtrlState::Normal );
			Ctrl_DrawBox( dc, arrow_rect_1, Visual::ScrollBar_Track, CtrlState::Normal );
		}
		Ctrl_DrawBox( dc, thumb_rect, Visual::ScrollBar_Thumb, ctrl_state );
		Context_DrawArrow( dc, Rect_Margin( arrow_rect_0, arrow_margin ), Visual::ScrollBar_Arrow, arrow_state_0, (dir == Orientation::Horz) ? Arrow::Left : Arrow::Up   );
		Context_DrawArrow( dc, Rect_Margin( arrow_rect_1, arrow_margin ), Visual::ScrollBar_Arrow, arrow_state_1, (dir == Orientation::Horz) ? Arrow::Right: Arrow::Down );
		return true;
	}

	void ScrollBar_Mouse( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_ctrl = Rect_Contains( r, mouse_pos );

		// tracking
		if (Context_IsDragging( dc, id ))
		{
			const Vec2_s coord = mouse_pos - Context_GetDrag( dc ).Pos;
			pos = ScrollBar_CoordToPos( dc, dir, r, min, max, page, coord );
		}

		if (!on_ctrl)
			return;

		// @todo: repeat for arrow/track clicks

		// @note #1: we do the track first so that the thumb will start dragging
		//			 if it happens to jump under the cursor due to a click on the track

		// track
		if (Button_Mouse( dc, id, r, ButtonClick::Press ))
		{
			const Rect_s thumb_rect = ScrollBar_CalcThumbRect( dc, dir, r, min, max, page, pos );
			const bool on_thumb = Rect_Contains( thumb_rect, mouse_pos );
			if (!on_thumb)
			{
				const bool horz = (dir == Orientation::Horz);
				const float mouse_coord = horz ? mouse_pos.x : mouse_pos.y;
				const float thumb_coord = horz ? thumb_rect.x : thumb_rect.y;
				const float step = (mouse_coord < thumb_coord) ? -page : page;
				const float end  = max-page;
				pos = NeClamp( pos + step, min, end );
			}
		}

		// thumb
		{
			// @note #2: thumb rect has to be recalculated (see note #1)
			const Rect_s thumb_rect = ScrollBar_CalcThumbRect( dc, dir, r, min, max, page, pos );
			const Vec2_s drag_pos = mouse_pos - Rect_Pos( thumb_rect );
			if (Button_Mouse( dc, id, thumb_rect, ButtonClick::Press ))
				Context_BeginDragOnce( dc, id, drag_pos );
		}

		// arrows
		const Rect_s arrow_rect_0 = ScrollBar_CalcArrowRect( dc, dir, r, 0 );
		if (Button_Mouse( dc, ScrollBar_GetArrowId( id, 0 ), arrow_rect_0, ButtonClick::Press ))
		{
			const float end = max-page;
			pos = NeClamp( pos - line, min, end );
		}

		const Rect_s arrow_rect_1 = ScrollBar_CalcArrowRect( dc, dir, r, 1 );
		if (Button_Mouse( dc, ScrollBar_GetArrowId( id, 1 ), arrow_rect_1, ButtonClick::Press ))
		{
			const float end = max-page;
			pos = NeClamp( pos + line, min, end );
		}
	}

	void ScrollBar_Keyboard( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		if (!Context_HasFocus( dc, id ))
			return;

		// mouse wheel
		const MouseState_s& mouse = Context_GetMouse( dc );
		if (mouse.Wheel)
		{
			const float end = max-page;
			const float step = line ? line : Context_GetLineHeight( dc );
			const float wheel_lines = Context_GetTheme( dc )->Metric[ Metric::ScrollBar_Wheel_Lines ].x;
			pos = NeClamp( pos - mouse.Wheel * wheel_lines * step, min, end );
			((MouseState_s&)mouse).Wheel = 0;
		}

		// keyboard
		const KeyboardState_s& kb = Context_GetKeyboard( dc );
	}

	void ScrollBar_Input( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		ScrollBar_Mouse	  ( dc, dir, id, r, min, max, line, page, pos );
		ScrollBar_Keyboard( dc, dir, id, r, min, max, line, page, pos );
	}

	void ScrollBar_Do( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		ScrollRange_Normalize( min, max, line, page, pos );
		if (!ScrollBar_Draw( dc, dir, id, r, min, max, page, pos ))
			return;
		return ScrollBar_Input( dc, dir, id, r, min, max, line, page, pos );
	}

	Vec2_s ScrollBar_Do( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos )
	{
		const Vec2_s size = ScrollBar_CalcSize( dc, dir, track );
		ScrollBar_Do( dc, dir, id, Rect_Ctor( p, size ), min, max, line, page, pos );
		return size;
	}

	void ScrollBar_DoH( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		return ScrollBar_Do( dc, Orientation::Horz, id, r, min, max, line, page, pos );
	}

	void ScrollBar_DoV( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos )
	{
		return ScrollBar_Do( dc, Orientation::Vert, id, r, min, max, line, page, pos );
	}

	Vec2_s ScrollBar_DoH( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos )
	{
		return ScrollBar_Do( dc, Orientation::Horz, id, p, track, min, max, line, page, pos );
	}

	Vec2_s ScrollBar_DoV( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos )
	{
		return ScrollBar_Do( dc, Orientation::Vert, id, p, track, min, max, line, page, pos );
	}

	ScrollBar_s	ScrollBar_Instance( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float pos )
	{
		const Vec2_s size = ScrollBar_CalcSize( dc, dir, track );
		const ScrollBar_s instance = { id, Rect_Ctor( p, size ), min, max, line, page, pos };
		return instance;
	}

	bool ScrollBar_Draw( Context_t dc, Orientation::Enum dir, const ScrollBar_s& instance )
	{
		return ScrollBar_Draw( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Page, instance.Pos );
	}

	void ScrollBar_Mouse( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance )
	{
		return ScrollBar_Mouse( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Line, instance.Page, instance.Pos );
	}

	void ScrollBar_Keyboard( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance )
	{
		return ScrollBar_Keyboard( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Line, instance.Page, instance.Pos );
	}

	void ScrollBar_Input( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance )
	{
		return ScrollBar_Input( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Line, instance.Page, instance.Pos );
	}

	void ScrollBar_Do( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance )
	{
		return ScrollBar_Do( dc, dir, instance.Id, instance.Rect, instance.Min, instance.Max, instance.Line, instance.Page, instance.Pos );
	}

	void ScrollBar_DoH( Context_t dc, ScrollBar_s& instance )
	{
		return ScrollBar_Do( dc, Orientation::Horz, instance );
	}

	void ScrollBar_DoV( Context_t dc, ScrollBar_s& instance )
	{
		return ScrollBar_Do( dc, Orientation::Vert, instance );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Expander

	CtrlState::Enum Expander_GetState( Context_t dc, const Expander_s& instance )
	{
		if (Context_IsHot( dc, instance.Id ))
			return CtrlState::Hot;
		if (instance.Expanded)
			return CtrlState::Pushed;
		return CtrlState::Normal;
	}

	float Expander_CalcHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::Expander_Margin ].y;
	}

	Rect_s Expander_CalcClientRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[ Metric::Expander_Margin ] );
	}

	Rect_s Expander_CalcButtonRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s client_rect = Expander_CalcClientRect( dc, r );
		return Rect_s
			{ client_rect.x
			, client_rect.y
			, theme->Metric[ Metric::Expander_Button_Size ].x
			, client_rect.h 
			};
	}

	Rect_s Expander_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s client_rect = Expander_CalcClientRect( dc, r );
		const float x0 = theme->Metric[ Metric::Expander_Button_Size ].x + theme->Metric[ Metric::Expander_Spacing ].x;
		return Rect_s
			{ client_rect.x + x0
			, client_rect.y 
			, client_rect.w - x0
			, client_rect.h
			};
	}

	bool Expander_Draw( Context_t dc, Expander_s& instance )
	{
		Graphics_t g = Context_GetGraphics( dc );
		if (Context_Cull( dc, instance.Rect ))
			return false;

		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& button_margin = theme->Metric[ Metric::Expander_Button_Margin ];

		// ctrl
		const CtrlState::Enum ctrl_state = Expander_GetState( dc, instance );
		Ctrl_DrawBox( dc, instance.Rect, Visual::Expander, ctrl_state );

		// button
		const Rect_s button_rect = Expander_CalcButtonRect( dc, instance.Rect );
		Context_DrawArrow( dc, Rect_Margin( button_rect, button_margin ), Visual::Expander_Arrow, ctrl_state, instance.Expanded ? Arrow::Down : Arrow::Right );

		// text
		const Rect_s label_rect = Expander_CalcLabelRect( dc, instance.Rect );
		Label_Do( dc, label_rect, TextFormat::Middle, instance.Text );
		return true;
	}

	void Expander_Mouse( Context_t dc, Expander_s& instance )
	{
		const Rect_s button_rect = Expander_CalcButtonRect( dc, instance.Rect  );

		const bool activated = (Button_Mouse( dc, instance.Id, button_rect  , ButtonClick::Press	   ) == ButtonAction::Clicked)
							|| (Button_Mouse( dc, instance.Id, instance.Rect, ButtonClick::DoubleClick ) == ButtonAction::Clicked);

		if (activated)
			instance.Expanded = !instance.Expanded;
	}

	void Expander_Do( Context_t dc, Expander_s& instance )
	{
		if (!Expander_Draw( dc, instance ))
			return;
		Expander_Mouse( dc, instance );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Caption

	float Caption_CalcHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::Caption_Margin ].y;
	}

	bool Caption_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, CtrlState::Enum s )
	{
		if (Context_Cull( dc, r ))
			return false;
		Ctrl_DrawText( dc, r, t, TextFormat::Middle, Visual::Caption, s, Metric::Caption_Margin );
		return true;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// SizeGrip

	SizeGripAction::Enum SizeGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, Vec2_s& size )
	{
		return SizeGrip_Mouse( dc, id, r, MouseButton::Left, size );
	}

	SizeGripAction::Enum SizeGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, MouseButton::Enum button, Vec2_s& size )
	{
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		if (Button_Mouse( dc, id, r, ButtonClick::Press, button ))
		{
			Context_BeginDragOnce( dc, id, size - mouse_pos, button );
			return SizeGripAction::Pushed;
		}

		if (Context_IsDragging( dc, id ))
		{
			const DragInfo_s& drag = Context_GetDrag( dc );
			if (drag.Button == button)
			{
				size = drag.Pos + mouse_pos;
				return SizeGripAction::Dragged;
			}
		}

		return SizeGripAction::None;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Window

	Id_t Window_GetSizeGripId( Id_t id )
	{
		return Id_Cat( id, 1 );
	}

	CtrlState::Enum Window_GetCaptionState( Context_t dc, Id_t id )
	{
		const Id_t grip_id = Window_GetSizeGripId( id );
		if (Context_IsHot( dc, grip_id ))
			return CtrlState::Hot;
		if (Context_IsWnd( dc, id ))
			return CtrlState::Pushed;
		return Ctrl_GetState( dc, id );
	}

	Rect_s Window_CalcCaptionRect( Context_t dc, const Rect_s& r )
	{
		return Rect_CapHeight( r, Caption_CalcHeight( dc ) );
	}

	Rect_s Window_CalcChildRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& client_margin = theme->Metric[ Metric::Window_Client_Margin ];
		return Rect_Margin
			( r
			, client_margin.x
			, client_margin.y + Caption_CalcHeight( dc )
			, client_margin.x
			, client_margin.y
			);
	}

	Vec2_s Window_CalcSize( Context_t dc, const Vec2_s& client )
	{
		Theme_t theme = Context_GetTheme( dc );
		return client 
				+ theme->Metric[ Metric::Window_Client_Margin ] 
				+ theme->Metric[ Metric::Window_Client_Margin ] 
				+ Vec2_s { 0.0f, Caption_CalcHeight( dc ) };
	}

	void Window_Draw( Context_t dc, Window_s& instance )
	{
		if (!instance.Collapsed)
		{
			const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, instance.Id );
			Ctrl_DrawBox( dc, instance.Rect, Visual::Window, ctrl_state );
		}
		const Rect_s caption_rect = Window_CalcCaptionRect( dc, instance.Rect );
		const CtrlState::Enum caption_state = Window_GetCaptionState( dc, instance.Id );
		Caption_Draw( dc, instance.Id, caption_rect, instance.Text, caption_state );
	}

	void Window_Mouse_Caption( Context_t dc, Window_s& instance )
	{
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );

		const Rect_s caption_rect = Window_CalcCaptionRect( dc, instance.Rect );
		const bool on_caption = Rect_Contains( caption_rect, mouse_pos );

		if (on_caption)
		{
			Context_SetHot( dc, instance.Id );

			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetWnd( dc, instance.Id );
				Context_SetPushed( dc, instance.Id );
			}

			if (mouse.Button[MB::Left].NumClicks == 2)
				instance.Collapsed = !instance.Collapsed;
			else
				Context_BeginDragOnce( dc, instance.Id, Vec2_s { caption_rect.x - mouse_pos.x, caption_rect.y - mouse_pos.y } );
		}

		else if (!instance.Collapsed)
		{
			const bool on_window = Rect_Contains( instance.Rect, mouse_pos );

			if (on_window)
			{
				Context_SetHot( dc, instance.Id );

				if (mouse.Button[MB::Left].WentDown)
				{
					Context_SetWnd( dc, instance.Id );
					Context_SetPushed( dc, instance.Id );
				}
			}
		}

		if (Context_IsDragging( dc, instance.Id ))
		{
			const DragInfo_s& drag = Context_GetDrag( dc );
			instance.Rect.x = mouse_pos.x + drag.Pos.x;
			instance.Rect.y = mouse_pos.y + drag.Pos.y;
		}
	}

	void Window_Mouse_SizeGrip( Context_t dc, Window_s& instance )
	{
		if (instance.Collapsed)
			return;

		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s grip_size = { 16.0f, 16.0f };
		const Rect_s grip_rect = Rect_s
			{ instance.Rect.x + instance.Rect.w - (0.5f * grip_size.x)
			, instance.Rect.y + instance.Rect.h - (0.5f * grip_size.y)
			, grip_size.x
			, grip_size.y
			};
		const Id_t grip_id = Window_GetSizeGripId( instance.Id );
		Vec2_s wnd_size = Rect_Size( instance.Rect );
		SizeGrip_Mouse( dc, grip_id, grip_rect, wnd_size );
		instance.Rect.w = NeMax( wnd_size.x, instance.MinSize.x ? instance.MinSize.x : theme->Metric[ Metric::Window_Min_Size ].x );
		instance.Rect.h = NeMax( wnd_size.y, instance.MinSize.y ? instance.MinSize.y : theme->Metric[ Metric::Window_Min_Size ].y );

		Graphics_t g = Context_GetGraphics( dc );
		if (Context_IsHot( dc, grip_id ))
			Graphics_DrawRect( g, grip_rect, theme->Visual[ Visual::Window ].Front[ CtrlState::Hot ] );
	}

	void Window_Mouse( Context_t dc, Window_s& instance )
	{
		Window_Mouse_Caption( dc, instance );
		Window_Mouse_SizeGrip( dc, instance );
	}

	void Window_Begin( Context_t dc, Window_s& instance )
	{
		const Rect_s child_rect = Window_CalcChildRect( dc, instance.Rect );
		Window_Draw( dc, instance );
		Window_Mouse( dc, instance );
		Context_BeginChild( dc, instance.Collapsed ? Rect_s() : child_rect );
	}

	void Window_End( Context_t dc )
	{
		Context_EndChild( dc );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollPanel

	Id_t ScrollPanel_GetHorzId( Id_t id )
	{
		return Id_Cat( id, __LINE__ );
	}

	Id_t ScrollPanel_GetVertId( Id_t id )
	{
		return Id_Cat( id, __LINE__ );
	}

	Vec2_s ScrollPanel_CalcNcSize( Context_t dc, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset, bool* show_sb_h, bool* show_sb_v )
	{
		// @todo: theme
		const Vec2_s margin = { 2.0f, 2.0f };

		// @note: doing two passes catches the case where showing one scrollbar
		//		  shrinks the child area enough to make the ther one appear
		bool has_h_scroll;
		bool has_v_scroll;
		Vec2_s nc_size = {};
		for ( int i = 0; i < 2; ++i )
		{
			has_h_scroll = ((size.x + nc_size.x) > r.w);
			has_v_scroll = ((size.y + nc_size.y) > r.h);
			nc_size = Vec2_s
				{ has_v_scroll ? ScrollBar_CalcSize( dc, Orientation::Vert, 0.0f ).x + margin.x : 0.0f
				, has_h_scroll ? ScrollBar_CalcSize( dc, Orientation::Horz, 0.0f ).y + margin.y : 0.0f
				};
		}
		if (show_sb_h)
			*show_sb_h = has_h_scroll;
		if (show_sb_v)
			*show_sb_v = has_v_scroll;
		return nc_size;
	}

	void ScrollPanel_Begin( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset )
	{
		// mouse
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		if (Rect_Contains( r, mouse_pos ))
			Context_SetHot( dc, id );
		if (mouse.Button[0].WentDown)
		{
			if (Context_IsHot( dc, id ))
				Context_SetPushed( dc, id );
		}

		// children
		const Vec2_s nc_size = ScrollPanel_CalcNcSize( dc, r, size, offset, nullptr, nullptr );
		const Rect_s remain_rect = Rect_Margin( r, 0.0f, 0.0f, nc_size.x, nc_size.y );
		Context_BeginChild( dc, remain_rect );
	}

	void ScrollPanel_End( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& step, Vec2_s& offset )
	{
		// children
		Context_EndChild( dc );

		// scroll bars
		bool has_h_scroll;
		bool has_v_scroll;
		const Vec2_s nc_size = ScrollPanel_CalcNcSize( dc, r, size, offset, &has_h_scroll, &has_v_scroll );

		if (!(has_h_scroll || has_v_scroll))
		{
			offset = Vec2_s {};
			return;
		}

		const float sb_v_w = ScrollBar_CalcSize( dc, Orientation::Vert, 0.0f ).x;
		const float sb_h_h = ScrollBar_CalcSize( dc, Orientation::Horz, 0.0f ).y;

		//	@note:
		//		We use a unique scroll bar id for both drawing and mouse input handling.
		//		For keyboard handling however, we use the view's own id.
		//		This makes the keyboard handler test whether the view has focus, thus
		//		essentially forwarding keyboard messages from the view to the scrollbar.

		if (has_v_scroll)
		{
			const Id_t sb_v_id		= ScrollPanel_GetVertId( id );
			const float sb_v_h		= r.h - (has_h_scroll ? sb_h_h : 0.0f);
			const Rect_s sb_v_rect	= Rect_s { r.x + r.w - sb_v_w, r.y, sb_v_w, sb_v_h };
			ScrollBar_Draw		( dc, Orientation::Vert, sb_v_id, sb_v_rect, 0.0f, size.y, r.h - nc_size.y, offset.y );
			ScrollBar_Mouse		( dc, Orientation::Vert, sb_v_id, sb_v_rect, 0.0f, size.y, step.y, r.h - nc_size.y, offset.y );
			ScrollBar_Keyboard	( dc, Orientation::Vert, id, sb_v_rect, 0.0f, size.y, step.y, r.h - nc_size.y, offset.y );
			if (Context_HasFocus( dc, sb_v_id ))
				Context_SetFocus( dc, id );
		}
		else
		{
			offset.y = 0.0f;
		}

		if (has_h_scroll)
		{
			const Id_t sb_h_id		= ScrollPanel_GetHorzId( id );
			const float sb_h_w		= r.w - (has_v_scroll ? sb_v_w : 0.0f);
			const Rect_s sb_h_rect	= Rect_s { r.x, r.y + r.h - sb_h_h, sb_h_w, sb_h_h };
			ScrollBar_Draw		( dc, Orientation::Horz, sb_h_id, sb_h_rect, 0.0f, size.x, r.w - nc_size.x, offset.x );
			ScrollBar_Mouse		( dc, Orientation::Horz, sb_h_id, sb_h_rect, 0.0f, size.x, step.x, r.w - nc_size.x, offset.x );
			ScrollBar_Keyboard	( dc, Orientation::Horz, id, sb_h_rect, 0.0f, size.x, step.x, r.w - nc_size.x, offset.x );
			if (Context_HasFocus( dc, sb_h_id ))
				Context_SetFocus( dc, id );
		}
		else
		{
			offset.x = 0.0f;
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollView

	void ScrollView_Begin( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset )
	{
		ScrollPanel_Begin( dc, id, r, size, offset );
		Context_BeginView( dc, Vec2_Floor( offset ) );
	}

	void ScrollView_End( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& step, Vec2_s& offset )
	{
		Context_EndView( dc );
		ScrollPanel_End( dc, id, r, size, step, offset );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// CollapseView

	void CollapseView_Begin( Context_t dc, Id_t id, float y, const Text_s& t, bool& expanded )
	{
		const float ctrl_h		 = Expander_CalcHeight( dc );
		const Rect_s child_rect  = Context_GetChild( dc );
		const Vec2_s view_offset = Context_GetView( dc );

		const Rect_s ctrl_rect	 = Rect_s { 0.0f, y		    , child_rect.w + view_offset.x, ctrl_h									    };
		const Rect_s remain_rect = Rect_s { 0.0f, y + ctrl_h, child_rect.w + view_offset.x, child_rect.h - (y + ctrl_h) + view_offset.y };

		Expander_s instance = { id, ctrl_rect, t, expanded };
		Expander_Do( dc, instance );
		expanded = instance.Expanded;

		Context_BeginChild( dc, expanded ? remain_rect : Rect_s() );
	}

	void CollapseView_End( Context_t dc )
	{
		Context_EndChild( dc );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// CollapseList

	Vec2_s CollapseList_Do( Context_t dc, Id_t id, CollapseItem_s* item, int count, Vec2_s* last_size )
	{
		Vec2_s item_size = {};
		Vec2_s total_size = {};
		for ( int i = 0; i < count; ++i )
		{
			CollapseView_Begin( dc, Id_Cat( id, i ), total_size.y, item[i].Text, item[i].Expanded );
			{
				if (item[i].Expanded)
					item_size = item[i].Do( dc );
				else
					item_size = Vec2_s {};
				item_size.y += Expander_CalcHeight( dc );
				total_size.y += item_size.y;
				total_size.x = NeMax( total_size.x, item_size.x );
			}
			CollapseView_End( dc );
		}
		if (last_size)
			*last_size = item_size;
		return total_size;
	}

	Vec2_s CollapseList_Do( Context_t dc, Id_t id, CollapseItem_s* item, int count )
	{
		return CollapseList_Do( dc, id, item, count, nullptr );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ListGrid

	float ListGrid_CalcRowY( float row_h, float view_y, int row )
	{
		return row_h * row - view_y;
	}

	int ListGrid_HitTestY( float row_h, const Rect_s& r, float view_y, float y )
	{
		if ((y < r.y) || y > (r.y+r.h))
			return -1;
		const float dist = (y + view_y) - r.y;
		const float row = dist / row_h;
		return (int)row;
	}


	void ListGrid_CalcRangeY( float row_h, const Rect_s& r, float view_y, int& first, int& count, float& offset_y )
	{
		const float skip_lines = view_y	/ row_h;					// how many lines preceed the view rect
		const float skip_full  = floorf( skip_lines );				// full lines preceeding the view
		const int	first_line = (int)skip_full;					// first (partially) visible line

		// @note: 
		//	We first expand the client rect at the top to include a potentially partial leading line
		//	We then compute the fraction of lines that fit within the expanded client rect
		//	We then round that fraction up to the next integer in order to include a potentially partial trailing line

		const float first_line_y   = first_line * row_h - view_y;	// coord of first partial line in client coords 
		const float visible_height = r.h - first_line_y;			// height from start of partial line to end of view
		const float visible_lines  = visible_height / row_h;		// fraction of lines in visible range
		const int	num_visible	   = (int)ceilf( visible_lines );	// number of (partially) visible lines in visible range

		first	 = first_line;
		count	 = num_visible;
		offset_y = first_line_y;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// ListHeader

	Rect_s ListHeaderItem_CalcIconRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s client_rect = Expander_CalcClientRect( dc, r );
		const float icon_w = theme->Metric[ Metric::ListHeader_Item_Icon_Size ].x;
		if (client_rect.w <= icon_w)
			return Rect_s();
		return Rect_s
			{ client_rect.x + client_rect.w - icon_w
			, client_rect.y
			, icon_w
			, client_rect.h 
			};
	}

	Rect_s ListHeaderItem_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s icon_size = Vec2_s { theme->Metric[ Metric::ListHeader_Item_Icon_Size ].x, 0.0f };
		const Vec2_s margin = theme->Metric[ Metric::ListHeader_Item_Margin ] + icon_size;
		return Rect_Margin( r, margin );
	}

	Rect_s ListHeaderItem_CalcDividerRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& splitter_size = theme->Metric[ Metric::ListHeader_Item_Divider_Size ];
		return Rect_s { r.x + r.w - 0.5f * splitter_size.x, r.y, splitter_size.x, r.h };
	}

	bool ListHeaderItem_Draw( Context_t dc, Id_t id, const Rect_s& r, const ListHeaderItem_s& item, int index, int hot, int pushed )
	{
		if (Context_Cull( dc, r ))
			return false;

		// item
		const CtrlState::Enum item_state = Ctrl_GetState( dc, id, index, hot, pushed );
		Ctrl_DrawBox( dc, r, Visual::ListHeader_Item, item_state );

		// text
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s text_rect = ListHeaderItem_CalcLabelRect( dc, r );
		const TextFormat::Mask_t text_fmt = Alignment_ToTextFormat( item.Alignment, Alignment::Middle );
		Graphics_DrawString( g, theme, text_rect, item.Text, text_fmt, Visual::ListHeader_Item, item_state );

		// icon
		if (item.SortOrder)
		{
			const Rect_s icon_rect = ListHeaderItem_CalcIconRect( dc, r );
			const Vec2_s icon_margin = theme->Metric[ Metric::ListHeader_Item_Icon_Margin ];
			Context_DrawArrow( dc, Rect_Margin( icon_rect, icon_margin ), Visual::ScrollBar_Arrow, CtrlState::Normal, (item.SortOrder == SortOrder::Ascending) ? Arrow::Up : Arrow::Down );
		}
		return true;
	}

	void ListHeaderItem_Mouse( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s& item, int index, int& hot, int& pushed )
	{
		if (Context_IsDragging( dc, id ) && (pushed == index) )
		{
			Theme_t theme = Context_GetTheme( dc );
			const Vec2_s mouse_pos = Context_GetMousePos( dc );
			const Vec2_s item_width = mouse_pos - Context_GetDrag( dc ).Pos;
			item.Width = NeMax( item_width.x, theme->Metric[ Metric::ListHeader_Item_Min_Size ].x );
			return;
		}

		const Rect_s divider_rect = ListHeaderItem_CalcDividerRect( dc, r );
		if (Button_Mouse( dc, id, divider_rect, ButtonClick::Press, index, hot, pushed ))
		{
			const Vec2_s mouse_pos = Context_GetMousePos( dc );
			const Vec2_s item_width = Vec2_s { item.Width, 0.0f };
			const Vec2_s drag_pos = mouse_pos - item_width;
			Context_BeginDragOnce( dc, id, drag_pos );
		}
		else
		{
			if (Button_Mouse( dc, id, r, index, hot, pushed ))
				item.SortOrder = SortOrder_Next( item.SortOrder );
		}
	}

	void ListHeaderItem_Do( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s& item, int index, int& hot, int& pushed )
	{
		if (!ListHeaderItem_Draw( dc, id, r, item, index, hot, pushed ))
			return;
		ListHeaderItem_Mouse( dc, id, r, item, index, hot, pushed );
	}

	float ListHeader_CalcWidth( const ListHeaderItem_s* item, int count )
	{
		float w = 0.0f;
		for ( int i = 0; i < count; ++i )
			w += item[i].Width;
		return w;
	}

	Vec2_s ListHeader_CalcSize( Context_t dc, float w )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		return Vec2_s { w, Graphics_GetLineHeight( g, theme->Font ) }
			 + 2.0f * theme->Metric[ Metric::ListHeader_Item_Margin ];
	}

	void ListHeader_Do( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s* item, int count, ListHeaderState_s& s )
	{
		NeGuiScopedChild( dc, r );

		const Rect_s child_rect = Context_GetChild( dc );
		const float child_x0 = child_rect.x;
		const float child_x1 = child_rect.x + child_rect.w;

		Rect_s item_rect = child_rect - Vec2_s { s.View, 0.0f };
		for ( int i = 0; i < count; ++i )
		{
			// resize item
			item_rect.w = item[i].Width;

			// cull left
			if ((item_rect.x + item_rect.w) >= child_x0)
			{
				ListHeaderItem_Do( dc, id, item_rect, item[i], i, s.Hot, s.Pushed );
			}

			// advance item
			item_rect.x += item_rect.w;

			// cull right
			if (item_rect.x > child_x1)
				break;
		}

		if (item_rect.x < child_x1)
		{
			ListHeaderItem_s fill_item = {};
			item_rect.w = child_x1 - item_rect.x;
			ListHeaderItem_Draw( dc, id, item_rect, fill_item, count, -1, -1 );
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// ListColumn

	ListColumnEvent_s ListColumnEvent_None()
	{
		const ListColumnEvent_s ev_none = {};
		return ev_none; 
	}

	float ListColumn_CalcItemHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::ListColumn_Item_Margin ].y;
	}

	Rect_s ListColumn_CalcItemRect( Context_t dc, const Rect_s& r, const Vec2_s& view, int index )
	{
		const float item_height = ListColumn_CalcItemHeight( dc );
		const float item_y = ListGrid_CalcRowY( item_height, view.y, index );
		return Rect_s { r.x, r.y + item_y, r.w, item_height };
	}

	int ListColumn_CalcNumPerPage( Context_t dc, const Rect_s& r )
	{
		const float item_height	= ListColumn_CalcItemHeight( dc );
		return (int)(r.h / item_height);
	}

	int ListColumn_CalcFirstFullyVisible( Context_t dc, const Rect_s& r, const Vec2_s& view, int num_items )
	{
		const float item_height	= ListColumn_CalcItemHeight( dc );
		const float skip		= view.y / item_height;
		return (int)ceilf(skip);
	}

	int ListColumn_CalcLastFullyVisible( Context_t dc, const Rect_s& r, const Vec2_s& view, int num_items )
	{
		const float rect_bottom = r.y + r.h;
		const float item_height	= ListColumn_CalcItemHeight( dc );
		const int	last_item	= NeMax( 0, num_items-1 );
		const int   hit_item	= ListGrid_HitTestY( item_height, r, view.y, rect_bottom );
		const float hit_bottom	= (1+hit_item) * item_height - view.y;
		const int	item		= (hit_bottom > rect_bottom) ? hit_item-1 : hit_item;
		return NeClamp( item, 0, last_item );
	}

	void ListColumn_SetCaret( Context_t dc, const Rect_s& r, const ListColumn_s& item, int num_items, int caret, ListColumnState_s& s )
	{
		const int first = 0;
		const int last  = NeMax( first, num_items-1 );
		const int sel   = NeClamp( caret, first, last );
		const float item_height	= ListColumn_CalcItemHeight( dc );
		const Rect_s item_rect = Rect_s { r.x, r.y + sel * item_height, r.w, item_height } - s.View;
		const float off_t =  item_rect.y			  - r.y;
		const float off_b = (item_rect.y+item_rect.h) - (r.y+r.h);
		if (off_t < 0.0f)
			s.View.y += off_t;
		else if (off_b > 0.0f)
			s.View.y += off_b;
		s.Sel = sel;
	}

	int	ListColumn_HitTest( Context_t dc, const Rect_s& r, const Vec2_s& view, int num_items, const Vec2_s& coord )
	{
		if (!Rect_Contains( r, coord ))
			return -1;
		const float row_height = ListColumn_CalcItemHeight( dc );
		const int hit_row = ListGrid_HitTestY( row_height, r, view.y, coord.y );
		return (hit_row < num_items) ? hit_row : -1;
	}

	void ListColumn_DrawSelection( Context_t dc, Id_t id, const Rect_s& r, const ListColumnState_s& s )
	{
		// @note: 
		//	prefer selected over hot state 
		//	it's less distracting visually 
		//	and also accounts for themes that do not want hot highlights
		const bool is_sel = (s.Sel >= 0);
		const bool is_hot = (s.Hot >= 0) && Context_IsHot( dc, id );
		if (is_sel)
		{
			const bool has_focus = Context_HasFocus( dc, id );
			const Rect_s sel_rect = ListColumn_CalcItemRect( dc, r, s.View, s.Sel );
			if (Rect_Intersects( r, sel_rect ))
				Ctrl_DrawBox( dc, sel_rect, Visual::ListColumn_Selection, has_focus ? CtrlState::Pushed : CtrlState::Normal ); 
		}
		if (is_hot && (s.Sel != s.Hot))
		{
			const Rect_s hot_rect = ListColumn_CalcItemRect( dc, r, s.View, s.Hot );
			if (Rect_Intersects( r, hot_rect ))
				Ctrl_DrawBox( dc, hot_rect, Visual::ListColumn_Selection, CtrlState::Hot ); 
		}
	}

	bool ListColumn_DrawItems( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, const ListColumnState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;

		if (num_items < 1)
			return false;

		// init text
		TextList_s text;
		text.Utf8 = item.Utf8;
		text.Utf16 = item.Utf16;
		text.NumItems = num_items;

		// init item rect
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		const Vec2_s item_margin = theme->Metric[ Metric::ListColumn_Item_Margin ];
		const TextFormat::Mask_t text_fmt = Alignment_ToTextFormat( item.Alignment, Alignment::Middle );
		Rect_s item_rect = { r.x, r.y, r.w, line_height + 2.0f * item_margin.y };

		// calc visible range
		int first_visible = 0;
		int max_num_visible = 0;
		float offset_y = 0.0f;
		ListGrid_CalcRangeY( item_rect.h, r, s.View.y, first_visible, max_num_visible, offset_y );
		const int visible_end = NeMin( first_visible + max_num_visible, num_items );

		// apply draw offset
		item_rect.x -= s.View.x;
		item_rect.y += offset_y;

		// draw visible range
		Text_s item_text;
		Rect_s text_rect;
		CtrlState::Enum item_state = CtrlState::Normal;
		for ( int i = first_visible; i < visible_end; ++i )
		{
			item_state =  (i == s.Hot) ? CtrlState::Hot
					   : ((i == s.Sel) ? CtrlState::Pushed
					   :			     CtrlState::Normal);
			text_rect = Rect_Margin( item_rect, item_margin );
			item_text = item.Callback.DoItem ? item.Callback.DoItem( item.Callback.Context, dc, item_rect, i, col, item_state ) : text[ i ];
			Graphics_DrawString( g, theme, text_rect, item_text, text_fmt, Visual::ListColumn_Item, item_state );
			item_rect.y += item_rect.h;
		}
		return true;
	}

	bool ListColumn_Draw( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, const ListColumnState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;
		ListColumn_DrawSelection( dc, id, r, s );
		ListColumn_DrawItems( dc, id, r, item, num_items, col, s );
		return true;
	}

	ListColumnEvent_s ListColumn_Mouse( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s )
	{
		ListColumnEvent_s ev = ListColumnEvent_None();

		// cull
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		if (!Rect_Contains( r, mouse_pos ))
		{
			s.Hot = -1;
			return ev;
		}

		// hit test
		const int hit_item = ListColumn_HitTest( dc, r, s.View, num_items, mouse_pos );

		// state
		Context_SetHot( dc, id );
		s.Hot = hit_item;

		if (Context_IsHot( dc, id ) )
		{
			if (mouse.Button[ MB::Left ].WentDown)
			{
				Context_SetFocus( dc, id );
				s.Sel = hit_item;
			}
			if (mouse.Button[ MB::Left ].NumClicks == 2)
			{
				ev.Activated = (s.Sel == hit_item);
			}
		}

		// event
		return ev;
	}

	ListColumnEvent_s ListColumn_Keyboard( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s )
	{
		ListColumnEvent_s ev = ListColumnEvent_None();

		if (!Context_HasFocus( dc, id ))
			return ev;
		if (num_items <= 0)
			return ev;

		const int idx_first	    = 0;
		const int idx_last	   = num_items-1;
		const int num_per_page = ListColumn_CalcNumPerPage( dc, r );

		const KeyboardState_s& kb = Context_GetKeyboard( dc );
		const bool is_ctrl_pressed  = kb.Key[ VK::Control ].IsPressed;
		const bool is_shift_pressed = kb.Key[ VK::Shift   ].IsPressed;

		// navigation
		int sel = s.Sel;
		if ( kb.Key[ VK::Home ].AutoRepeat )
		{
			sel = idx_first;
		}
		else if ( kb.Key[ VK::End ].AutoRepeat )
		{
			sel = idx_last;
		}
		else if ( kb.Key[ VK::Up ].AutoRepeat )
		{
			--sel;
		}
		else if ( kb.Key[ VK::Down ].AutoRepeat )
		{
			++sel;
		}
		else if ( kb.Key[ VK::PageUp ].AutoRepeat )
		{
			const int to = ListColumn_CalcFirstFullyVisible( dc, r, s.View, num_items );
			if (sel != to)
				sel = to;
			else
				sel -= num_per_page;
		}
		else if ( kb.Key[ VK::PageDown ].AutoRepeat )
		{
			const int to = ListColumn_CalcLastFullyVisible( dc, r, s.View, num_items );
			if (sel != to)
				sel = to;
			else
				sel += num_per_page;
		}
		else if ( kb.Key[ VK::Return ].WentDown )
		{
			ev.Activated = (sel >= 0);
		}

		if (sel != s.Sel)
			sel = NeClamp( sel, idx_first, idx_last );

		if (sel != s.Sel)
			ListColumn_SetCaret( dc, r, item, num_items, sel, s );

		return ev;
	}

	ListColumnEvent_s ListColumn_Input( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s )
	{
		const ListColumnEvent_s ev_m = ListColumn_Mouse	  ( dc, id, r, item, num_items, col, s );
		const ListColumnEvent_s ev_k = ListColumn_Keyboard( dc, id, r, item, num_items, col, s );
		const ListColumnEvent_s ev = { ev_m.Activated || ev_k.Activated };
		return ev;
	}

	ListColumnEvent_s ListColumn_Do( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s )
	{
		if (!ListColumn_Draw( dc, id, r, item, num_items, col, s ))
			return ListColumnEvent_None();
		return ListColumn_Input( dc, id, r, item, num_items, col, s );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// List

	ListEvent_s ListEvent_None()
	{
		const ListEvent_s ev_none = {};
		return ev_none;
	}

	ListEvent_s List_Do( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s* header, const ListColumn_s* column, int num_columns, int num_rows, ListStyle::Mask_t style, ListState_s& s )
	{
		ListEvent_s ev = ListEvent_None();

		// back
		Ctrl_DrawBox( dc, r, Visual::List, Ctrl_GetState( dc, id ) );
		NeGuiScopedChild( dc, r );

		// header
		Vec2_s header_size = {};
		if (!NeHasFlag( style, ListStyle::NoHeader))
		{
			ListHeaderState_s header_state = { s.HotHdr, s.SelHdr, s.View.x };
			header_size = ListHeader_CalcSize( dc, r.w );

			const Rect_s header_rect = { 0.0f, 0.0f, r.w, header_size.y };
			ListHeader_Do( dc, Id_Cat( id, 0 ), header_rect, header, num_columns, header_state );
			s.HotHdr = header_state.Hot;
			s.SelHdr = header_state.Pushed;
		}

		// list
		{
			const Rect_s list_rect = { 0.0f, header_size.y, r.w, r.h - header_size.y };
			const float row_height = ListColumn_CalcItemHeight( dc );

			//	@note: 
			//		Using the list's id saves us the trouble of having to forward keyboard
			//		input to the scroll panel.

			const Id_t view_id = id; 
			const Vec2_s view_size = { ListHeader_CalcWidth( header, num_columns ), row_height * num_rows };
			const Vec2_s view_step = { view_size.x / num_columns, row_height };
			ScrollPanel_Begin( dc, view_id, list_rect, view_size, s.View );
			{
				// init
				Rect_s			  column_rect_sel  = Context_GetChild( dc );
				ListColumnState_s column_state_sel = { s.HotRow, s.SelRow, s.View };

				// selection
				Rect_s			  column_rect   = column_rect_sel;
				ListColumnState_s column_state  = column_state_sel;
				ListColumn_DrawSelection( dc, id, column_rect, column_state );

				// horizontal lines
				if (NeHasFlag( style, ListStyle::HorzLines ))
				{
					int first;
					int count;
					float offset_y;
					ListGrid_CalcRangeY( row_height, column_rect, s.View.y, first, count, offset_y );

					Theme_t theme = Context_GetTheme( dc );
					Graphics_t g = Context_GetGraphics( dc );
					const Visual_s& vis = theme->Visual[ Visual::List_Grid ];

					float y = offset_y;
					for ( int i = 0; i < count; ++i, y += row_height )
					{
						const Vec2_s bl = { column_rect.x				 , y + row_height };
						const Vec2_s br = { column_rect.x + column_rect.w, y + row_height };
						Graphics_DrawLine( g, bl, br, vis.Front[ CtrlState::Normal ] );
					}
				}

				//	@note:
				//		scroll the columns themselves instead
				//		of the scrolling the items within 
				//		each column
				column_rect.x -= column_state.View.x;
				column_state.View.x = 0.0f;

				// columns
				for ( int i = 0; i < num_columns; ++i )
				{
					// init column
					column_rect.w = header[i].Width;

					// vertical lines
					if (NeHasFlag( style, ListStyle::VertLines ))
					{
						Theme_t theme = Context_GetTheme( dc );
						Graphics_t g = Context_GetGraphics( dc );
						const Visual_s& vis = theme->Visual[ Visual::List_Grid ];
						const Vec2_s tr = { column_rect.x + column_rect.w, column_rect.y				 };
						const Vec2_s br = { column_rect.x + column_rect.w, column_rect.y + column_rect.h };
						Graphics_DrawLine( g, tr, br, vis.Front[ CtrlState::Normal ] );
					}

					// draw column
					ListColumn_DrawItems( dc, id, column_rect, column[i], num_rows, i, column_state );

					// next column
					column_rect.x += column_rect.w;
				}

				// input
				const ListColumnEvent_s ev_col = ListColumn_Input( dc, id, column_rect_sel, column[0], num_rows, 0, column_state_sel );
				s.HotRow = column_state_sel.Hot;
				s.SelRow = column_state_sel.Sel;
				s.View   = column_state_sel.View;
				ev.Activated = ev_col.Activated;

				// @todo: own input (left/right scroll, auto-size, etc..)
			}
			ScrollPanel_End( dc, view_id, list_rect, view_size, view_step, s.View );
		}
		return ev;
	}

	ListEvent_s List_Do( Context_t dc, List_s& instance )
	{
		return List_Do
			( dc, instance.Id, instance.Rect
			, instance.Data.Header, instance.Data.Column, instance.Data.NumColumns, instance.Data.NumRows
			, instance.Style
			, instance.State 
			);
	}

	void List_ShowRow( Context_t dc, List_s& list, int index )
	{
		if (index < 0)
			return;
		if (index >= list.Data.NumRows)
			return;

		const Rect_s r = Context_GetChild( dc );
		const Vec2_s header_size = ListHeader_CalcSize( dc, 0.0f );
		const Rect_s list_rect = { 0.0f, header_size.y, r.w, r.h - header_size.y };
		const float row_height = ListColumn_CalcItemHeight( dc );
		const int num_vs = (int)(list_rect.h / row_height);

		const float item_ls_y0 = (index+0) * row_height;
		const float item_ls_y1 = (index+1) * row_height;
		const float item_vs_y0 = item_ls_y0 - list.State.View.y;
		const float item_vs_y1 = item_ls_y1 - list.State.View.y;
		if (item_vs_y0 < 0.0f)
		{
			list.State.View.y = item_ls_y0;
			return;
		}
		if (item_vs_y1 > list_rect.h)
		{
			list.State.View.y = (index - (num_vs-1)) * row_height;
			return;
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Tree

	float Tree_CalcItemHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::Tree_Item_Margin ].y;
	}

	float Tree_CalcItemWidth( Context_t dc, const Text_s& t, int level, TreeStyle::Mask_t style )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Rect_s text_bounds = Graphics_MeasureString( g, t, theme->Font );

		const float indent = theme->Metric[ Metric::Tree_Item_Indent ].x;
		const float icon_w = theme->Metric[ Metric::Tree_Item_Icon_Size ].x;
		const float space  = theme->Metric[ Metric::Tree_Item_Text_Indent ].x;

		return theme->Metric[ Metric::Tree_Item_Margin ].x
			 + level * indent
			 + icon_w
			 + (NeHasFlag( style, TreeStyle::HasState ) ? icon_w : 0.0f)
			 + (NeHasFlag( style, TreeStyle::HasIcon  ) ? icon_w : 0.0f)
			 + space
			 + text_bounds.w
			 + theme->Metric[ Metric::Tree_Item_Margin ].x
			 ;
	}

	Rect_s Tree_CalcLevelRect( Context_t dc, const Rect_s& r, int level )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float indent = theme->Metric[ Metric::Tree_Item_Indent ].x;
		const Rect_s level_rect = Rect_Margin( r, level * indent, 0.0f, 0.0f, 0.0f );
		return level_rect;
	}

	Rect_s Tree_CalcInnerRect( Context_t dc, const Rect_s& r, int level )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s level_rect = Tree_CalcLevelRect( dc, r, level );
		const Rect_s inner_rect = Rect_Margin( level_rect, theme->Metric[ Metric::Tree_Item_Margin ] );
		return inner_rect;
	}

	Rect_s Tree_CalcExpanderRect( Context_t dc, const Rect_s& r, int level )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float icon_w = theme->Metric[ Metric::Tree_Item_Icon_Size ].x;
		const Rect_s inner_rect = Tree_CalcInnerRect( dc, r, level );
		const Rect_s expander_rect = Rect_s { inner_rect.x, inner_rect.y, icon_w, inner_rect.h };
		return expander_rect;
	}

	Rect_s Tree_CalcStateRect( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float icon_w = theme->Metric[ Metric::Tree_Item_Icon_Size ].x;
		const Rect_s inner_rect = Tree_CalcInnerRect( dc, r, level );
		const Rect_s state_rect = Rect_s { inner_rect.x + icon_w, inner_rect.y, icon_w, inner_rect.h };
		return state_rect;
	}

	Rect_s Tree_CalcIconRect( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float icon_w = theme->Metric[ Metric::Tree_Item_Icon_Size ].x;
		const float lead_in = icon_w
							+ (NeHasFlag( style, TreeStyle::HasState ) ? icon_w : 0.0f);
		const Rect_s inner_rect = Tree_CalcInnerRect( dc, r, level );
		const Rect_s state_rect = Rect_s { inner_rect.x + lead_in, inner_rect.y, icon_w, inner_rect.h };
		return state_rect;
	}

	Rect_s Tree_CalcLabelRect( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float indent_x = theme->Metric[ Metric::Tree_Item_Text_Indent ].x;
		const float icon_w   = theme->Metric[ Metric::Tree_Item_Icon_Size   ].x;
		const float lead_in  = indent_x + icon_w
							 + (NeHasFlag( style, TreeStyle::HasState ) ? icon_w : 0.0f)
							 + (NeHasFlag( style, TreeStyle::HasIcon  ) ? icon_w : 0.0f);
		const Rect_s inner_rect = Tree_CalcInnerRect( dc, r, level );
		const Rect_s text_rect = Rect_Margin( inner_rect, lead_in, 0.0f, 0.0f, 0.0f );
		return text_rect;
	}

	void Tree_Begin( Context_t dc, Id_t id, const Rect_s& r )
	{
		Ctrl_DrawBox( dc, r, Visual::Tree, Ctrl_GetState( dc, id ) );
		Context_BeginChild( dc, r );
	}

	bool Tree_Item_Draw( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, const TreeItemState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;

		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& icon_margin = theme->Metric[ Metric::Tree_Item_Icon_Margin ];
		const CtrlState::Enum ctrl_state = s.Hot ? CtrlState::Hot : CtrlState::Normal;
		const bool has_focus = Context_HasFocus( dc, id );

		// selection
		if (s.Selected)
		{
			Ctrl_DrawBox( dc, r, Visual::Tree_Selection, has_focus ? CtrlState::Pushed : CtrlState::Normal ); 
		}
		else if (s.Hot)
		{
			Ctrl_DrawBox( dc, r, Visual::Tree_Selection, CtrlState::Hot ); 
		}

		// expander
		if (item.HasChildren)
		{
			const Rect_s expander_rect = Tree_CalcExpanderRect( dc, r, level );
			Context_DrawArrow( dc, Rect_Margin( expander_rect, icon_margin ), Visual::Tree_Item_Arrow, ctrl_state, item.Expanded ? Arrow::Down : Arrow::Right );
		}

		// label
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s text_rect = Tree_CalcLabelRect( dc, r, level, style );
		Graphics_DrawString( g, theme, text_rect, item.Text, TextFormat::Middle, Visual::Tree_Item, ctrl_state );
		return true;
	}

	void Tree_Item_Mouse( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s )
	{
		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_item = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );

		s.Hot = on_item;

		if (!on_item)
			return;

		bool selected = false;
		bool activated = false;

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetPushed( dc, id );
				selected = true;
			}
		}

		if (Context_IsPushed( dc, id ))
		{
			if (Context_IsHot( dc, id ))
			{
				const Rect_s level_rect		= Tree_CalcLevelRect   ( dc, r, level );
				const Rect_s expander_rect	= Tree_CalcExpanderRect( dc, r, level );

				if (Rect_Contains( expander_rect, mouse_pos ))
				{
					activated = mouse.Button[MB::Left].WentDown;
				}
				else if (Rect_Contains( level_rect, mouse_pos ))
				{
					activated = (mouse.Button[MB::Left].NumClicks == 2);
				}
			}
		}

		s.Selected  = selected;
		s.Activated = activated;
	}

	void Tree_Item_Do( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s )
	{
		// @note: 
		//	we evaluate hot state before draw 
		//	because it won't persist 
		{
			const MouseState_s& mouse = Context_GetMouse( dc );
			const Vec2_s mouse_pos = Context_GetMousePos( dc );
			const Rect_s clip_rect = Context_GetClipRect( dc );
			s.Hot = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );
		}

		if (!Tree_Item_Draw( dc, id, r, item, level, style, s ))
			return;
		Tree_Item_Mouse( dc, id, r, item, level, style, s );
	}

	Vec2_s Tree_Item_Do( Context_t dc, Id_t id, const Vec2_s& p, float w, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s )
	{
		const Vec2_s size = 
		{ Tree_CalcItemWidth( dc, item.Text, level, style )
		, Tree_CalcItemHeight( dc )
		};
		Tree_Item_Do( dc, id, Rect_s { p.x, p.y, w, size.y }, item, level, style, s );
		return size;
	}

	void Tree_End( Context_t dc )
	{
		Context_EndChild( dc );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Virtual Tree

	void Tree_DoItem( Context_t dc, Id_t tree_id, Vec2_s& item_pos, float w, Vec2_s& tree_size, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s )
	{
		const Vec2_s item_size = Tree_Item_Do( dc, tree_id, item_pos, w, item, level, style, s );
		item_pos.y += item_size.y;
		tree_size.y = item_pos.y;
		tree_size.x = NeMax( tree_size.x, item_size.x );
	}

	void Tree_DoChildren( Context_t dc, Id_t id, Vec2_s& pos, float w, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeNode_t node, int level, TreeState_s& s )
	{
		for ( TreeNode_t child = data.Callback.GetFirstChild( data.Callback.Context, node )
			; child != data.None
			; child = data.Callback.GetNextSibling( data.Callback.Context, node, child )
			)
		{
			Tree_DoSubTree( dc, id, pos, w, data, style, size, child, level, s );
		}
	}

	void Tree_DoSubTree( Context_t dc, Id_t id, Vec2_s& pos, float w, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeNode_t node, int level, TreeState_s& s )
	{
		TreeItem_s		item  = {};
		TreeItemState_s state = {};

		item = data.Callback.GetTreeItem( data.Callback.Context, node );

		state.Selected = (s.Selected == node);

		Tree_DoItem( dc, id, pos, w, size, item, level, style, state );

		if (state.Activated)
			s.Activated = node;
		if (state.Selected)
			s.Selected = node;
		if (state.Hot)
			s.Hot = node;

		if (!item.HasChildren)
			return;
		if (!item.Expanded)
			return;

		Tree_DoChildren( dc, id, pos, w, data, style, size, node, level+1, s );
	}

	void Tree_Do( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeState_s& s )
	{
		s.Activated = data.None;
		s.Hot		= data.None;

		Vec2_s pos = {};
		Tree_DoChildren( dc, id, pos, r.w, data, style, size, data.Root, 0, s );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// TreeView

	void TreeView_Begin( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset )
	{
		Tree_Begin( dc, id, r );
		ScrollView_Begin( dc, id, Rect_s { 0.0f, 0.0f, r.w, r.h }, size, offset );
	}

	void TreeView_End( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, Vec2_s& offset )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s step = { theme->Metric[ Metric::Tree_Item_Icon_Size ].x, Tree_CalcItemHeight( dc ) };
		ScrollView_End( dc, id, Rect_s { 0.0f, 0.0f, r.w, r.h }, size, step, offset );
		Tree_End( dc );
	}

	void TreeView_Do( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, Vec2_s& offset, TreeState_s& s )
	{
		TreeView_Begin( dc, id, r, size, offset );
			Tree_Do( dc, id, r, data, style, size, s );
		TreeView_End( dc, id, r, size, offset );
	}

	void TreeView_Do( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, TreeViewState_s& s )
	{
		TreeView_Do( dc, id, r, data, style, s.View.Size, s.View.Offset, s.Tree );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Tab Item

	Vec2_s TabItem_CalcIndent( Context_t dc, TabStyle::Mask_t style )
	{
		const Theme_t theme = Context_GetTheme( dc );
		const bool is_page = NeHasFlag( style, TabStyle::Frame );
		return theme->Metric[ is_page ? Metric::Frame_Tab_Item_Indent : Metric::Tab_Item_Indent ];
	}

	Vec2_s TabItem_CalcMargin( Context_t dc, TabStyle::Mask_t style )
	{
		const Theme_t theme = Context_GetTheme( dc );
		const bool is_page = NeHasFlag( style, TabStyle::Frame );
		return theme->Metric[ is_page ? Metric::Frame_Tab_Item_Margin : Metric::Tab_Item_Margin ];
	}

	float TabItem_CalcEnlarge( Context_t dc, TabStyle::Mask_t style )
	{
		const Theme_t theme = Context_GetTheme( dc );
		const bool is_page = NeHasFlag( style, TabStyle::Frame );
		const bool is_enlarge = NeHasFlag( style, TabStyle::Enlarge );
		if (!is_enlarge)
			return 0.0f;
		return theme->Metric[ is_page ? Metric::Frame_Tab_Item_Enlarge : Metric::Tab_Item_Enlarge ].y;
	}

	float TabItem_CalcHeight( Context_t dc, TabStyle::Mask_t style )
	{
		const Theme_t theme = Context_GetTheme( dc );
		const bool is_page = NeHasFlag( style, TabStyle::Frame );
		const bool is_large = NeHasFlag( style, TabStyle::Enlarge );
		const float line_height = Context_GetLineHeight( dc );
		const Vec2_s margin = TabItem_CalcMargin( dc, style );
		const float enlarge = TabItem_CalcEnlarge( dc, style );
		return line_height + 2.0f * margin.y + enlarge;
	}

	float TabItem_CalcWidth( Context_t dc, TabStyle::Mask_t style, const Text_s& t )
	{
		const Theme_t theme = Context_GetTheme( dc );
		Font_t font = Context_GetFont( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Vec2_s margin = TabItem_CalcMargin( dc, style );
		const float item_w = margin.x + Graphics_MeasureString( g, t, font ).w + margin.x;
		return item_w;
	}

	void TabItem_CalcWidths( Context_t dc, const Rect_s& r, TabStyle::Mask_t style, const Text_s* text, float* width, int num_items )
	{
		Theme_t theme = Context_GetTheme( dc );
		Font_t font = Context_GetFont( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float margin = TabItem_CalcMargin( dc, style ).x;

		float w = 0.0f;
		for ( int i = 0; i < num_items; ++i )
		{
			const float item_w = margin + Graphics_MeasureString( g, text[i], font ).w + margin;
			width[i] = item_w;
			w += item_w;
		}
		if (w <= r.w)
			return;

		const float item_w = floorf(r.w / num_items);
		for ( int i = 0; i < num_items; ++i )
			width[i] = item_w;
	}

	Rect_s TabItem_CalcLabelRect( Context_t dc, const Rect_s& r, TabStyle::Mask_t style )
	{
		const Theme_t theme = Context_GetTheme( dc );
		const Vec2_s margin = TabItem_CalcMargin( dc, style );
		const float enlarge = TabItem_CalcEnlarge( dc, style );
		return Rect_Inflate( Rect_Margin( r, margin ), 0.0f, 0.0f, 0.0f, -enlarge );
	}

	bool TabItem_Draw( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t style, const Text_s& text, bool selected )
	{
		if (Context_Cull( dc, r ))
			return false;

		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const bool on_item = Context_IsHot( dc, id );
		const CtrlState::Enum s 
			= selected 
			? CtrlState::Pushed 
			: (on_item ? CtrlState::Hot : CtrlState::Normal);

		const bool is_page = NeHasFlag( style, TabStyle::Frame );
		const Visual_s& vis = theme->Visual[ is_page ? Visual::Frame_Tab_Item : Visual::Tab_Item ];

		// background
		Graphics_FillRect( g, r, vis.Back[ s ] );

		// label
		const Rect_s text_rect = TabItem_CalcLabelRect( dc, r, style );
		Graphics_DrawString( g, text_rect, text, theme->Font, TextFormat::Left | TextFormat::Middle, vis.Text[ s ] );

		if (!selected)
			return true;

		// raised egde
		Vec2_s pos = Rect_Pos( r );
		Vec2_s pos1 = pos  + Vec2_s { 0.0f, r.h - 2.0f };
		Vec2_s pos2 = pos1 + Vec2_s { r.w , 0.0f };
		Vec2_s pos3 = pos2; pos3.y = pos.y;
		Vec2_s pos4 = pos2; pos4.y += 1;
		Graphics_DrawLine( g, pos , pos1, vis.Front[ s ] );
		Graphics_DrawLine( g, pos1, pos2, vis.Front[ s ] );
		Graphics_DrawLine( g, pos3, pos4, vis.Front[ s ] );
		return true;
	}

	void TabItem_Mouse( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t s, bool& selected )
	{
		if (Button_Mouse( dc, id, r, ButtonClick::Press ))
			selected = true;
	}

	void TabItem_Do( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t s, const Text_s& t, bool& selected )
	{
		if (!TabItem_Draw( dc, id, r, s, t, selected ))
			return;
		TabItem_Mouse( dc, id, r, s, selected );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// PopUpMenu

	float Menu_PopUp_CalcItemHeight( Context_t dc, const Text_s& text )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const bool is_separator = (text.NumChars == 0);
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		const float icon_height = theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size ].y;
		const float sepa_height = theme->Metric[ Metric::Menu_PopUp_Separator_Size ].y;
		const float cell_height = is_separator ? sepa_height : NeMax( line_height, icon_height );
		return cell_height + 2.0f * theme->Metric[ Metric::Menu_PopUp_Item_Margin ].y;
	}

	float Menu_PopUp_CalcItemWidth( Context_t dc, const Text_s& text )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Rect_s bounds = Graphics_MeasureString( g, text, theme->Font );

		return theme->Metric[ Metric::Menu_PopUp_Item_Margin		].x // left margin
			 + theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size		].x	// icon
			 + theme->Metric[ Metric::Menu_PopUp_Item_Text_Indent	].x	// text indent
			 + bounds.w													// text
																		// hot key
			 + theme->Metric[ Metric::Menu_PopUp_Item_Expander_Size	].x	// expander
			 + theme->Metric[ Metric::Menu_PopUp_Item_Margin		].x // right margin
			 ;
	}

	Vec2_s Menu_PopUp_CalcItemSize( Context_t dc, const Text_s& text )
	{
		return Vec2_s
			{ Menu_PopUp_CalcItemWidth ( dc, text )
			, Menu_PopUp_CalcItemHeight( dc, text )
			};
	}

	Vec2_s Menu_PopUp_CalcMenuSize( Context_t dc, const MenuItemList_s& list )
	{
		Vec2_s item_size = {};
		Vec2_s menu_size = {};
		for ( int i = 0; i < list.NumItems; ++i )
		{
			item_size = Menu_PopUp_CalcItemSize( dc, list.Item[i].Text );
			menu_size.x = NeMax( menu_size.x, item_size.x );
			menu_size.y += item_size.y;
		}
		return menu_size;
	}

	Rect_s Menu_PopUp_CalcInnerRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[ Metric::Menu_PopUp_Item_Margin ] );
	}

	Rect_s Menu_PopUp_CalcIconRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s inner_rect = Menu_PopUp_CalcInnerRect( dc, r );
		const float icon_width = theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size ].x;
		return Rect_s { inner_rect.x, inner_rect.y, icon_width, inner_rect.h };
	}

	Rect_s Menu_PopUp_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s inner_rect = Menu_PopUp_CalcInnerRect( dc, r );
		const float icon_width		= theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size		].x;
		const float indent_width	= theme->Metric[ Metric::Menu_PopUp_Item_Text_Indent	].x;
		const float expander_width  = theme->Metric[ Metric::Menu_PopUp_Item_Expander_Size	].x;
		return Rect_s
			{ inner_rect.x + icon_width
			, inner_rect.y
			, inner_rect.w - icon_width - indent_width - expander_width
			, inner_rect.h 
			};
	}

	Rect_s Menu_PopUp_CalcHotKeyRect( Context_t dc, const Rect_s& r )
	{
		return Rect_s();
	}

	Rect_s Menu_PopUp_CalcExpanderRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s inner_rect = Menu_PopUp_CalcInnerRect( dc, r );
		const float icon_width = theme->Metric[ Metric::Menu_PopUp_Item_Expander_Size ].x;
		return Rect_s { inner_rect.x + inner_rect.w - icon_width, inner_rect.y, icon_width, inner_rect.h };
	}

	Rect_s Menu_PopUp_CalcSeparatorRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Rect_s inner_rect = Menu_PopUp_CalcInnerRect( dc, r );
		const float icon_width  = theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size ].x;
		const float sepa_height = theme->Metric[ Metric::Menu_PopUp_Separator_Size ].y;
		return Rect_s { inner_rect.x + icon_width, inner_rect.y, inner_rect.w - icon_width, sepa_height };
	}

	bool Menu_PopUp_Item_Draw( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, const MenuItemState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;

		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const bool is_hot = Context_IsHot( dc, id ) && s.Hot;
		const bool has_bkgnd = is_hot || !NeHasFlag( style, MenuItemStyle::NoBkgnd );
		const CtrlState::Enum item_state = is_hot ? CtrlState::Hot : CtrlState::Normal;
		if (has_bkgnd)
			Ctrl_DrawBox( dc, r, Visual::Menu_PopUp_Item, item_state );

		const bool is_separator = (item.Text.NumChars == 0);
		if (is_separator)
		{
			const Rect_s separator_rect = Menu_PopUp_CalcSeparatorRect( dc, r );
			Ctrl_DrawBox( dc, separator_rect, Visual::Menu_PopUp_Separator, item_state );
			return true;
		}

		const Rect_s label_rect = Menu_PopUp_CalcLabelRect( dc, r );
		Graphics_DrawString( g, theme, label_rect, item.Text, TextFormat::Middle, Visual::Menu_PopUp_Item, item_state );

		if (item.Child.NumItems)
		{
			const Rect_s expander_rect = Menu_PopUp_CalcExpanderRect( dc, r );
			const float expander_margin = theme->Metric[ Metric::Menu_PopUp_Item_Expander_Margin ].x;
			Context_DrawArrow( dc, Rect_Margin( expander_rect, expander_margin ), Visual::Menu_PopUp_Item_Arrow, item_state, Arrow::Right );
		}
		return true;
	}
	
	void Menu_PopUp_Item_Mouse( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s )
	{
		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool is_item = (item.Text.NumChars != 0);
		const bool on_item = is_item ? Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos ) : false;

		s.Hot = on_item;

		if (!on_item)
			return;

		bool activated = false;

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetPushed( dc, id );
				activated = true;
			}
		}

		s.Activated = activated;
	}

	void Menu_PopUp_Item_Do( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s )
	{
		if (!Menu_PopUp_Item_Draw( dc, id, r, style, item, s ))
			return;
		Menu_PopUp_Item_Mouse( dc, id, r, style, item, s );
	}

	void Menu_DoPopUp( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, int level, MenuState_s& s )
	{
		if (level >= NeCountOf(s.Expanded))
			return;
		if ((!list.Item) || (!list.NumItems))
			return;

		// track expanded child
		Rect_s expanded_rect = {};
		const MenuItem_s* expanded_item = s.Expanded[level];

		// draw background
		Ctrl_DrawBox( dc, r, Visual::Menu_PopUp, CtrlState::Normal );

		// draw icon inlay
		Theme_t theme = Context_GetTheme( dc );
		const float icon_w = theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size ].x;
		const Rect_s icon_rect = { r.x + 1.0f, r.y + 1.0f, icon_w, r.h - 2.0f };
		Ctrl_DrawBox( dc, icon_rect, Visual::Menu_PopUp_Icon, CtrlState::Normal );

		// do items
		MenuItemState_s item_state = {};
		Rect_s item_rect = { r.x, r.y, r.w, 0.0f };
		for ( int i = 0; i < list.NumItems; ++i )
		{
			item_state.Activated = false;
			item_state.Hot = (s.Hot == list.Item + i);
			item_rect.h = Menu_PopUp_CalcItemHeight( dc, list.Item[i].Text );
			Menu_PopUp_Item_Do( dc, id, item_rect, MenuItemStyle::NoBkgnd, list.Item[i], item_state );

			if (item_state.Hot)
			{
				s.Hot = list.Item+i;
			}
			if (item_state.Activated)
			{
				if (list.Item[i].Child.NumItems)
				{
					s.Expanded[level] = list.Item + i;
				}
				else
				{
					s.Activated = list.Item + i;
					NeZero(s.Expanded);
				}
			}

			if (expanded_item == list.Item + i)
				expanded_rect = item_rect;

			item_rect.y += item_rect.h;
		}
	
		// do expanded child
		if (expanded_item)
		{
			const Vec2_s pos = { expanded_rect.x + expanded_rect.w - 4.0f, expanded_rect.y + 0.25f * expanded_rect.h };
			Menu_DoPopUp( dc, id, pos, expanded_item->Child, level+1, s );
		}
	}

	Vec2_s Menu_DoPopUp( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, int level, MenuState_s& s )
	{
		const Vec2_s size = Menu_PopUp_CalcMenuSize( dc, list );
		Menu_DoPopUp( dc, id, Rect_Ctor( p, size ), list, level, s );
		return size;
	}

	void Menu_DoPopUp( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, MenuState_s& s, bool& done )
	{
		s.Activated = nullptr;
		{
			NeGuiScopedModal( dc );
			Menu_DoPopUp( dc, id, r, list, 0, s );
		}
		done = (s.Activated != nullptr);
		const MouseState_s& mouse = Context_GetMouse( dc );
		if (mouse.Button[MB::Left].WentDown)
		{
			if (!Context_IsHot( dc, id ))
			{
				NeZero(s);
				done = true;
			}
		}
	}

	Vec2_s Menu_DoPopUp( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, MenuState_s& s, bool& done )
	{
		const Vec2_s size = Menu_PopUp_CalcMenuSize( dc, list );
		Menu_DoPopUp( dc, id, Rect_Ctor( p, size ), list, s, done );
		return size;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// MainMenu

	float Menu_Main_CalcItemHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float line_height = Graphics_GetLineHeight( g, theme->Font );
		return line_height + 2.0f * theme->Metric[ Metric::Menu_Main_Item_Margin ].y;
	}

	float Menu_Main_CalcItemWidth( Context_t dc, const Text_s& text )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Rect_s bounds = Graphics_MeasureString( g, text, theme->Font );

		return theme->Metric[ Metric::Menu_Main_Item_Margin	].x // left margin
			 + bounds.w											// text
			 + theme->Metric[ Metric::Menu_Main_Item_Margin	].x // right margin
			 ;
	}

	Vec2_s Menu_Main_CalcItemSize( Context_t dc, const Text_s& text )
	{
		return Vec2_s
			{ Menu_Main_CalcItemWidth ( dc, text )
			, Menu_Main_CalcItemHeight( dc )
			};
	}

	Vec2_s Menu_Main_CalcMenuSize( Context_t dc, const MenuItemList_s& list )
	{
		Vec2_s size = { 0.0f, Menu_Main_CalcItemHeight( dc ) };
		for ( int i = 0; i < list.NumItems; ++i )
			size.x += Menu_Main_CalcItemWidth( dc, list.Item[i].Text );
		return size;
	}

	Rect_s Menu_Main_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[ Metric::Menu_Main_Item_Margin ] );
	}

	bool Menu_Main_Item_Draw( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, const MenuItemState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const bool is_hot = Context_IsHot( dc, id ) && s.Hot;
		const bool has_bkgnd = is_hot || !NeHasFlag( style, MenuItemStyle::NoBkgnd );
		const CtrlState::Enum item_state 
			= is_hot	  ? CtrlState::Hot 
		//	: s.Expanded  ? CtrlState::Pushed 
						  : CtrlState::Normal;
		if (has_bkgnd)
			Ctrl_DrawBox( dc, r, Visual::Menu_Main_Item, item_state );
		const Rect_s label_rect = Menu_Main_CalcLabelRect( dc, r );
		Graphics_DrawString( g, theme, label_rect, item.Text, TextFormat::Middle, Visual::Menu_Main_Item, item_state );
		return true;
	}

	void Menu_Main_Item_Mouse( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s )
	{
		ButtonAction::Enum action = ButtonAction::None;
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_item = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );

		s.Hot = on_item;

		if (!on_item)
			return;

		bool activated = false;

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[MB::Left].WentDown)
			{
				Context_SetPushed( dc, id );
				activated = true;
			}
		}

		s.Activated = activated;
	}

	void Menu_Main_Item_Do( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s )
	{
		if (!Menu_Main_Item_Draw( dc, id, r, style, item, s ))
			return;
		Menu_Main_Item_Mouse( dc, id, r, style, item, s );
	}

	void Menu_DoMain( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, MenuState_s& s )
	{
		// draw background
		Ctrl_DrawBox( dc, r, Visual::Menu_Main, CtrlState::Normal );

		// reset outputs
		s.Activated = nullptr;

		// track expanded item
		Rect_s expanded_rect = {};
		const MenuItem_s* expanded_item = s.Expanded[0];

		// main menu
		const MenuItem_s* item = nullptr;
		MenuItemState_s item_state = {};
		Rect_s item_rect = { r.x, r.y, 0.0f, r.h }; 
		for ( int i = 0; i < list.NumItems; ++i )
		{
			// current item
			item = list.Item + i;

			// do item
			item_state.Activated = false;
			item_state.Hot = (s.Hot == item);
			item_rect.w = Menu_Main_CalcItemWidth( dc, item->Text );
			Menu_Main_Item_Do( dc, id, item_rect, MenuItemStyle::NoBkgnd, *item, item_state );
			if (item_state.Hot)
			{
				s.Hot = item;
				if (expanded_item)
				{
					NeZero(s.Expanded);
					s.Expanded[0] = s.Hot;
				}
			}
			if (item_state.Activated)
			{
				if (item != expanded_item)
					s.Expanded[0] = item;
				else
					NeZero(s);
			}

			// find expanded item's rect
			if (item == expanded_item)
				expanded_rect = item_rect;

			// advance rect
			item_rect.x += item_rect.w;
		}

		// pop up
		if (expanded_item)
		{
			// do sub-menu pop-up
			{
				NeGuiScopedModal( dc );
				const Vec2_s popup_pos = { expanded_rect.x, expanded_rect.y + expanded_rect.h };
				Menu_DoPopUp( dc, id, popup_pos, expanded_item->Child, 1, s );
			}

			// handle collapse
			bool done = (s.Activated != nullptr);
			const MouseState_s& mouse = Context_GetMouse( dc );
			if (mouse.Button[MB::Left].WentDown)
			{
				if (!Context_IsHot( dc, id ))
					NeZero(s);
			}
		}
	}

	Vec2_s Menu_DoMain( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, MenuState_s& s )
	{
		const Vec2_s size = Menu_Main_CalcMenuSize( dc, list );
		Menu_DoMain( dc, id, Rect_Ctor( p, size ), list, s );
		return size;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// ContextMenu

	struct ContextMenu_s
	{
		Id_t		Id;
		bool		Show;
		Vec2_s		Pos;
		MenuState_s State;
	};

	const MenuItem_s* ContextMenu_Do( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list )
	{
		ContextMenu_s& context_menu = Context_CacheIntern< ContextMenu_s >( dc, NE_UI_ID );

		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const Rect_s hit_rect = Rect_Intersect( clip_rect, r );

		if (Rect_Contains( hit_rect, mouse_pos ))
		{
			if (mouse.Button[MB::Right].WentDown)
			{
				context_menu.Id = id;
				context_menu.Show = true;
				context_menu.Pos = mouse_pos;
				NeZero( context_menu.State );
			}
		}

		if (!context_menu.Show)
			return nullptr;
		if (context_menu.Id != id)
			return nullptr;

		bool done = false;
		Menu_DoPopUp( dc, id, context_menu.Pos, list, context_menu.State, done );
		if (!done)
			return nullptr;
		const MenuItem_s* activated_item = context_menu.State.Activated;
		NeZero(context_menu);
		return activated_item;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// DropDownItem

	Vec2_s DropDownItem_CalcSize( Context_t dc, float text_w )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float line_h = Context_GetLineHeight( dc );
		const float margin_x = theme->Metric[ Metric::DropDown_Item_Margin ].x;
		const float margin_y = theme->Metric[ Metric::DropDown_Item_Margin ].y;
		return Vec2_s 
			{ margin_x + text_w + margin_x
			, margin_y + line_h + margin_y
			};
	}

	Vec2_s DropDownItem_CalcSize( Context_t dc, const Text_s& t )
	{
		Font_t font = Context_GetFont( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const float text_w = Graphics_MeasureString( g, t, font ).w;
		return DropDownItem_CalcSize( dc, text_w );
	}

	Rect_s DropDownItem_CalcLabelRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[ Metric::DropDown_Item_Margin ] );
	}

	bool DropDownItem_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t )
	{
		if (Context_Cull( dc, r ))
			return false;

		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const bool is_hot = Context_IsHot( dc, id );
		const CtrlState::Enum item_state = is_hot ? CtrlState::Hot : CtrlState::Normal;
		Ctrl_DrawBox( dc, r, Visual::DropDown_Item, item_state );

		const Rect_s label_rect = DropDownItem_CalcLabelRect( dc, r );
		Graphics_DrawString( g, theme, label_rect, t, TextFormat::Middle, Visual::DropDown_Item, item_state );
		return true;
	}

	bool DropDownItem_Mouse( Context_t dc, Id_t id, const Rect_s& r )
	{
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_ctrl = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );
		if (!on_ctrl)
			return false;

		// @note: 
		//	drop downs behave differently from buttons
		//	in that they don't claim pushed state
		//	but instead react on the mouse button
		//	being released while being hot

		Context_SetHot( dc, id );

		if (Context_IsHot( dc, id ))
		{
			if (mouse.Button[MB::Left].WentUp)
			{
				Context_SetPushed( dc, id );
				return true;
			}
		}

		return false;
	}

	bool DropDownItem_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t )
	{
		if (!DropDownItem_Draw( dc, id, r, t ))
			return false;
		return DropDownItem_Mouse( dc, id, r );
	}

	/// DropDownList

	Vec2_s DropDownList_CalcSize( Context_t dc, float text_w, int n )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s item_size = DropDownItem_CalcSize( dc, text_w );
		const Vec2_s list_border = theme->Metric[Metric::DropDown_Border];
		return Vec2_s 
			{ list_border.x + item_size.x	  + list_border.x
			, list_border.y + item_size.y * n + list_border.y };
	}

	Rect_s DropDownList_CalcInnerRect( Context_t dc, const Rect_s& r )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Rect_Margin( r, theme->Metric[Metric::DropDown_Border] );
	}

	template < typename T >
	static bool DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const T* t, int n, int& sel )
	{
		// list
		Graphics_t g = Context_GetGraphics( dc );
		const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, id );
		Ctrl_DrawBox( dc, r, Visual::DropDown, ctrl_state );

		// items
		int active = -1;
		const Vec2_s item_size = DropDownItem_CalcSize( dc, 0.0f );
		const Rect_s inner_rect = DropDownList_CalcInnerRect( dc, r );
		for ( int i = 0; i < n; ++i )
		{
			const Id_t item_id = Id_Cat( id, i );
			const Rect_s item_rect = 
				{ inner_rect.x
				, inner_rect.y + i * item_size.y
				, inner_rect.w
				, item_size.y 
				};
			if (DropDownItem_Do( dc, item_id, item_rect /*, no_bkgnd*/, t[i] ))
				active = i;
		}
		if (active >= 0)
		{
			sel = active;
			return true;
		}
		return false;
	}

	bool DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel )
	{
		return DropDownList_Do<cstr_t>( dc, id, r, t, n, sel );
	}

	bool DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel )
	{
		return DropDownList_Do<wcstr_t>( dc, id, r, t, n, sel );
	}

	bool DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& data, int& sel )
	{
		return data.Utf8
			? DropDownList_Do( dc, id, r, data.Utf8, data.NumItems, sel )
			: DropDownList_Do( dc, id, r, data.Utf16, data.NumItems, sel )
			;
	}

	bool DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel )
	{
		NeGuiScopedModal( dc );
		return DropDownList_Do( dc, id, r, t, n, sel );
	}

	bool DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel )
	{
		NeGuiScopedModal( dc );
		return DropDownList_Do( dc, id, r, t, n, sel );
	}

	bool DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& data, int& sel )
	{
		NeGuiScopedModal( dc );
		return DropDownList_Do( dc, id, r, data, sel );
	}

	/// DropDownBox

	Vec2_s DropDownBox_CalcSize( Context_t dc, float text_w )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float box_w = theme->Metric[Metric::DropDown_Box_Button_Size].x;
		return TextEdit_CalcSize( dc, text_w + box_w );
	}

	Vec2_s DropDownBox_CalcSize( Context_t dc, const Text_s& text )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Font_t font = Context_GetFont( dc );
		const float text_w = Graphics_MeasureString( g, text, font ).w; 
		return DropDownBox_CalcSize( dc, text_w );
	}

	static CtrlState::Enum DropDownBox_GetCtrlState( Context_t dc, Id_t id, bool expanded )
	{
		return expanded ? CtrlState::Pushed : Ctrl_GetState( dc, id );
	}

	static void DropDownBox_DrawBox( Context_t dc, Id_t id, const Rect_s& r, CtrlState::Enum ctrl_state )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s button_size = theme->Metric[Metric::DropDown_Box_Button_Size];
		const Rect_s box_rect = { r.x + r.w - button_size.x, r.y, button_size.x, r.h };
		switch (ctrl_state)
		{
		case CtrlState::Pushed:
		case CtrlState::Hot:
			Ctrl_DrawBox( dc, box_rect, Visual::DropDown_Box, ctrl_state );
			break;
		default:
			break;
		}
		const Vec2_s margin = theme->Metric[Metric::DropDown_Box_Button_Margin];
		const float icon_h = box_rect.h - margin.y - margin.y;
		const float icon_w = box_rect.w - margin.x - margin.x;
		const float icon_y = floorf(0.5f * (box_rect.h - icon_h));
		const float icon_x = margin.x;
		const Rect_s icon_rect = 
		{ box_rect.x + icon_x
		, box_rect.y + icon_y
		, icon_w
		, icon_h
		};
		const uint32_t icon_color = theme->Visual[Visual::DropDown_Box].Text[ctrl_state];
		Context_DrawArrow( dc, icon_rect, Arrow::Down, icon_color );
	}

	Rect_s DropDownBox_CalcListRect( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& t )
	{
		// calculate desktop space coordinate for the caret position
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s screen_rect = Context_GetDektop( dc );
		const Vec2_s child_to_screen = Graphics_GetTransform( g );
		const Vec2_s child_pos = Rect_Pos( r );
		const Vec2_s screen_pos = child_pos + child_to_screen;

		// calculate list size
		const float text_max_w = TextList_CalcMaxWidth( dc, t );
		const Vec2_s min_size = DropDownList_CalcSize( dc, text_max_w, t.NumItems );
		const Vec2_s list_size = { NeMax( min_size.x, r.w ), min_size.y };

		// select between drop-down (default) and pull-up
		//	depending on whether the list will fit onto
		//	the screen when dropped down
		const bool drop_down = (screen_pos.y + list_size.y) < screen_rect.h;
		const Rect_s list_rect = drop_down 
			? Rect_s { child_pos.x, child_pos.y + r.h		 , list_size.x, list_size.y }
			: Rect_s { child_pos.x, child_pos.y - list_size.y, list_size.x, list_size.y }
			;

		return list_rect;
	}

	bool DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel, bool& expanded )
	{
		return DropDownBox_Do( dc, id, r, TextList_s( t, n ), sel, expanded );
	}

	bool DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel, bool& expanded )
	{
		return DropDownBox_Do( dc, id, r, TextList_s( t, n ), sel, expanded );
	}

	bool DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& t, int& sel, bool& expanded )
	{
		CtrlState::Enum ctrl_state = DropDownBox_GetCtrlState( dc, id, expanded );
		if (TextEdit_Draw( dc, id, r, TextEditStyle::NoCaret, ctrl_state, t[sel], 0.0f, 0, 0 ))
			DropDownBox_DrawBox( dc, id, r, ctrl_state );
		if (Button_Mouse( dc, id, r ))
			expanded = !expanded;
		if (!expanded)
			return false;
		const Rect_s list_rect = DropDownBox_CalcListRect( dc, id, r, t );
		const bool action = DropDownList_DoModal( dc, id, list_rect, t, sel );
		if (action)
			expanded = false;
		if (expanded)
		{
			// if the left mouse button went down 
			// and neither the box *nor* the button
			// was hit, close the list
			const Mouse_s& mouse = Context_GetMouse( dc );
			if (mouse.Button[MB::Left].WentDown)
			{
				const Vec2_s mouse_pos = Context_GetMousePos( dc );
				const bool on_box = Rect_Contains( r, mouse_pos );
				const bool on_list = Rect_Contains( list_rect, mouse_pos );
				if (!on_box && !on_list)
					expanded = false;
			}
		}
		return action;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// MessageBox

	void MessageBox_Do( Context_t dc, Id_t id, const Text_s& caption, const Text_s& text, bool& done )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Rect_s text_bounds	= Graphics_MeasureString( g, text	, theme->Font );
		const Rect_s caption_bounds = Graphics_MeasureString( g, caption, theme->Font );
		const Vec2_s text_size = { NeMax( text_bounds.w, caption_bounds.w ), text_bounds.h };
		{
			NeGuiScopedModal( dc );

			const Vec2_s transform = Graphics_GetTransform( g );
			const Rect_s screen_rect = Context_GetDektop( dc );
			const Vec2_s client_size = { text_size.x + 16.0f, text_size.y + 16.0f };
			const Vec2_s wnd_min_size  = { 300.0f, 100.0f };
			const Vec2_s wnd_calc_size = Window_CalcSize( dc, client_size );
			const Vec2_s wnd_size = Vec2_Max( wnd_min_size, wnd_calc_size );
			const Rect_s wnd_rect = 
				{ -transform.x + floorf( 0.5f * ( screen_rect.w - wnd_size.x ) )
				, -transform.y + floorf( 0.5f * ( screen_rect.h - wnd_size.y ) )
				, wnd_size.x
				, wnd_size.y 
				};

			Window_s wnd = { id, wnd_rect, caption, wnd_size, false };
			Window_Begin( dc, wnd );
			{
				Graphics_DrawString( g, theme, Context_GetChild( dc ), text, TextFormat::Center | TextFormat::Middle, Visual::Window, CtrlState::Normal );
			}
			Window_End( dc );

			done = Button_Mouse( dc, id, wnd_rect ) == ButtonAction::Clicked;
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// ScrollWindow

	Rect_s ScrollWindow_Begin( Context_t dc, Window_s& wnd, ScrollInfo_s& scroll )
	{
		Window_Begin( dc, wnd );
		const Rect_s view_rect = Context_GetChild( dc );
		if (!wnd.Collapsed)
			ScrollView_Begin( dc, wnd.Id, view_rect, scroll.Size, scroll.Offset );
		return view_rect;
	}

	void ScrollWindow_End( Context_t dc, Window_s& wnd, ScrollInfo_s& scroll, const Rect_s& view )
	{
		if (!wnd.Collapsed)
			ScrollView_End( dc, wnd.Id, view, scroll.Size, Vec2_s {}, scroll.Offset );
		Window_End( dc );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Desktop

	static Rect_s ScrollWindow_Begin( Context_t dc, DesktopWindow_s& wnd )
	{
		Window_Begin( dc, wnd.Wnd );
		const Rect_s view_rect = Context_GetChild( dc );
		if (!wnd.Wnd.Collapsed)
			ScrollView_Begin( dc, wnd.Wnd.Id, view_rect, wnd.Scroll.Size, wnd.Scroll.Offset );
		return view_rect;
	}

	static void ScrollWindow_End( Context_t dc, DesktopWindow_s& wnd, const Rect_s& view_rect )
	{
		ScrollWindow_End( dc, wnd.Wnd, wnd.Scroll, view_rect );
	}

	void Desktop_DoWindow( Context_t dc, DesktopWindow_s& wnd )
	{
		const Rect_s view_rect = ScrollWindow_Begin( dc, wnd );
		if (!wnd.Wnd.Collapsed)
			wnd.Scroll.Size = wnd.View.Do( dc, wnd.View.User );
		ScrollWindow_End( dc, wnd, view_rect );
	}

	void Desktop_Do( Context_t dc, DesktopWindow_s* item, int count )
	{
		for ( int i = 0; i < count; ++i )
			Desktop_DoWindow( dc, item[i] );
	}

} } 

//======================================================================================
// Revision 1.0
//======================================================================================

///=====================================================================================
/// @todo:
///			
///		- Texture != Bitmap/Icon/Image 
///			introduce a new type (note: glyph similarities)
///			
///		- TextEdit
///			mouse-to-caret, mouse selection
///			
///		- Tree
///			first child/next sibling api
///			scrolling
///			improve skipping of scrolled range (for each list calculate first visible item index)
///			
///		- Dead input
///			Windows still claim HOT for their client arena even if collapsed
///
///=====================================================================================

//======================================================================================
namespace nemesis { namespace gui 
{
	///
	///	Generic
	///
	namespace
	{
		static inline Rect_s Rect_Margin( const Rect_s& r, float margin_x, float margin_y )
		{ return Rect_s { r.x + margin_x, r.y + margin_y, r.w - 2.0f * margin_x, r.h - 2.0f * margin_y }; }

		static inline Rect_s Rect_Margin( const Rect_s& r, float margin )
		{ return Rect_Margin( r, margin, margin ); }
	}

	namespace
	{
		static bool Key_Hit( const KeyInfo_s& key )
		{
			return /*key.WentDown || key.IsRepeated*/ key.AutoRepeat;
		}
	}

	namespace
	{
		static CtrlState::Enum Input_GetState( Context_t dc, Id_t id )
		{
			if (Context_IsPushed( dc, id ))
				return CtrlState::Pushed;
			if (Context_IsHot( dc, id ))
				return CtrlState::Hot;
			return CtrlState::Normal;
		}
	}

	namespace
	{
		void Graphics_DrawVisual( Graphics_t g, const Rect_s& r, const Theme_t theme, Visual::Enum element, CtrlState::Enum state )
		{
			const Visual_s& v = theme->Visual[ element ];
			Graphics_DrawBox( g, r, v.Back[ state ], v.Front[ state ] );
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	struct Button
	{
		struct ClickMode
		{
			enum Enum
			{ Release
			, Press
			};
		};

		struct Action
		{
			enum Enum
			{ None
			, Clicked
			};
		};

		struct Instance_s
		{
			Id_t				Id;
			Rect_s			Rect;
			const wchar_t*	Text;
		};

		static Action::Enum Input( Context_t dc, Id_t id, const Rect_s& r );
		static Action::Enum Input( Context_t dc, Id_t id, const Rect_s& r, ClickMode::Enum mode );
		static bool			Draw ( Context_t dc, Id_t id, const Rect_s& r, const wchar_t* t, int l );
		static Action::Enum Do	 ( Context_t dc, Id_t id, const Rect_s& r, const wchar_t* t, int l );
		static Action::Enum Do   ( Context_t dc, const Instance_s& instance );
	};

	///
	/// Button
	///
	Button::Action::Enum Button::Input( Context_t dc, Id_t id, const Rect_s& r )
	{
		return Input( dc, id, r, ClickMode::Release );
	}

	Button::Action::Enum Button::Input( Context_t dc, Id_t id, const Rect_s& r, ClickMode::Enum mode )
	{
		// transform
		Graphics_t g = Context_GetGraphics( dc );

		// setup
		Action::Enum action = Action::None;
		const Mouse_s& mouse = Context_GetMouse( dc );
		const Vec2_s local_mouse_pos = Context_GetMousePos( dc );

		// @todo: 
		//	do we need to take the clip rect into consideration?

		// hot
		if ( Rect_Contains( r, local_mouse_pos ) )
			Context_SetHot( dc, id );

		// active
		if ( Context_IsHot( dc, id ) )
		{
			if ( mouse.Button[0].WentDown )
				Context_SetPushed( dc, id );
		}

		// action
		if ( Context_IsPushed( dc, id ) )
		{
			// click
			if ( Context_IsHot( dc, id ) )
			{
				const bool trigger =   (( mode == Button::ClickMode::Press   ) && mouse.Button[0].WentDown)
									|| (( mode == Button::ClickMode::Release ) && mouse.Button[0].WentUp  )
									;
				if ( trigger )
				{
					action = Action::Clicked;
				}
			}
		}
		return action;
	}

	static void Graphics_DrawBox( Graphics_t g, const Rect_s& r, const Visual_s* v, CtrlState::Enum s )
	{
		Graphics_DrawBox( g, r, v->Back[s], v->Front[s] );
	}

	bool Button::Draw( Context_t dc, Id_t id, const Rect_s& r, const wchar_t* t, int l )
	{
		// cull
		Graphics_t g = Context_GetGraphics( dc );
		if (Context_Cull( dc, r ))
			return false;

		// state
		const Mouse_s& mouse = Context_GetMouse( dc );
		const bool is_active = Context_IsPushed( dc, id );
		const bool is_hot = Context_IsHot( dc, id );
		const CtrlState::Enum state = is_active ? CtrlState::Pushed : (is_hot ? CtrlState::Hot : CtrlState::Normal);

		// visual
		const Theme_t theme = Context_GetTheme( dc );
		const Visual_s* button_visual = &theme->Visual[ Visual::Button ];

		// draw
		Graphics_DrawBox	( g, r, button_visual, state );
		Graphics_DrawString	( g, r, t, l, theme->Font, TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap, button_visual->Text[ state ] );
		return true;
	}

	Button::Action::Enum Button::Do( Context_t dc, Id_t id, const Rect_s& r, const wchar_t* t, int l )
	{
		return Draw( dc, id, r, t, l ) 
			? Input( dc, id, r )
			: Action::None
			;
	}

	Button::Action::Enum Button::Do( Context_t dc, const Instance_s& instance )
	{
		return Do( dc, instance.Id, instance.Rect, instance.Text, -1 );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ToolBar

	float ToolBar_CalcHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme(dc);
		const float button_h = Button_CalcHeight( dc );
		const float margin = theme->Metric[Metric::ToolBar_Margin].y;
		return button_h + 2.0f * margin;
	}

	ToolBarEvent_s ToolBar_Do( Context_t dc, Id_t id, const Rect_s& r, const ToolBarItem_s* item, int num_items )
	{
		ToolBarEvent_s ev = {};

		if (Context_Cull( dc, r ))
			return ev;

		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		Graphics_DrawBox( g, r, &theme->Visual[ Visual::ToolBar ], CtrlState::Normal ); 

		const float end = r.x + r.w;
		const Vec2_s& spacing = theme->Metric[ Metric::ToolBar_Spacing ];
		Rect_s caret = Rect_Margin( r, theme->Metric[ Metric::ToolBar_Margin ] );
		for ( int i = 0; i < num_items; ++i )
		{
			switch( item[i].Type )
			{
			case ToolBarItem::Seperator:
				caret.x += spacing.x;
				caret.w = 1.0f;
				Graphics_DrawRect( g, caret, theme->Visual[ Visual::ToolBar ].Back[ CtrlState::Normal ] );
				caret.x += spacing.x;
				break;
			case ToolBarItem::Button:
				{
					const Id_t button_id = Id_Cat( id, i );
					caret.w = Button_CalcSize( dc, item[i].Text ).x;
					if (Button_Do( dc, button_id, caret, item[i].Text ))
						ev.Clicked = item[i].Id;
				}
				break;
			default:
				caret.w = 0.0f;
				break;
			}

			caret.x += caret.w + spacing.x;
			if (caret.x > end)
				break;
		}

		return ev;
	}

	ToolBarEvent_s ToolBar_Do( Context_t dc, const ToolBar_s& instance )
	{
		return ToolBar_Do( dc, instance.Id, instance.Rect, instance.Item, instance.NumItems );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Property

	void Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, bool& v )
	{
		if (Context_Cull( dc, r ))
			return;
		const Rect_s label_rect = { r.x			 , r.y,		  label_w, r.h };
		const Rect_s value_rect = { r.x + label_w, r.y, r.w - label_w, r.h };
		const Vec2_s margin		= { 2.0f, 2.0f };
		Label_Do   ( dc,	 Rect_Margin( label_rect, margin ), TextFormat::Middle, label );
		CheckBox_Do( dc, id, Rect_Margin( value_rect, margin ), v ? "True" : "False", v );
	}

	void Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, int32_t& v )
	{
		if (Context_Cull( dc, r ))
			return;
		const Rect_s label_rect = { r.x			 , r.y,		  label_w, r.h };
		const Rect_s value_rect = { r.x + label_w, r.y, r.w - label_w, r.h };
		const Vec2_s margin		= { 2.0f, 2.0f };
		Label_Do	  ( dc,		Rect_Margin( label_rect, margin ), TextFormat::Middle, label );
		TextEdit_DoInt( dc, id, Rect_Margin( value_rect, margin ), v );
	}

	void Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, uint32_t& v )
	{
		if (Context_Cull( dc, r ))
			return;
		const Rect_s label_rect = { r.x			 , r.y,		  label_w, r.h };
		const Rect_s value_rect = { r.x + label_w, r.y, r.w - label_w, r.h };
		const Vec2_s margin		= { 2.0f, 2.0f };
		Label_Do	  ( dc,		Rect_Margin( label_rect, margin ), TextFormat::Middle, label );
		TextEdit_DoInt( dc, id, Rect_Margin( value_rect, margin ), (int32_t&)v );
	}

	void Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, float& v )
	{
		if (Context_Cull( dc, r ))
			return;
		const Rect_s label_rect = { r.x			 , r.y,		  label_w, r.h };
		const Rect_s value_rect = { r.x + label_w, r.y, r.w - label_w, r.h };
		const Vec2_s margin		= { 2.0f, 2.0f };
		Label_Do		( dc,	  Rect_Margin( label_rect, margin ), TextFormat::Middle, label );
		TextEdit_DoFloat( dc, id, Rect_Margin( value_rect, margin ), v );
	}

	void Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, float& v, float min, float max )
	{
		if (Context_Cull( dc, r ))
			return;
		const Id_t	 slider_id	 = Id_Cat( id, __LINE__ );
		const float  slider_w	 = 0.5f * (r.w - label_w);
		const Rect_s label_rect  = { r.x					 , r.y, label_w , r.h };
		const Rect_s slider_rect = { r.x + label_w			 , r.y, slider_w, r.h };
		const Rect_s value_rect  = { r.x + label_w + slider_w, r.y, slider_w, r.h };
		const Vec2_s margin		 = { 2.0f, 2.0f };
		Label_Do		( dc,			 Rect_Margin( label_rect , margin ), TextFormat::Middle, label );
		Slider_DoH		( dc, slider_id, Rect_Margin( slider_rect, margin ), min, max, v );
		TextEdit_DoFloat( dc, id	   , Rect_Margin( value_rect , margin ), v );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Splitter

	float Splitter_CalcSize( Context_t dc, Orientation::Enum dir )
	{
		Theme_t theme = Context_GetTheme( dc );
		const Vec2_s& size = theme->Metric[Metric::Splitter_Size];
		return dir == Orientation::Horz ? size.y : size.x;
	}

	Rect_s Splitter_CalcRect( Context_t dc, const Rect_s& r, Orientation::Enum dir, float distance )
	{
		float size = Splitter_CalcSize( dc, dir );
		return dir == Orientation::Horz
			? Rect_s { r.x, distance, r.w, size }
			: Rect_s { distance, r.y, size, r.h };
	}

	bool Splitter_Draw( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float distance )
	{
		if (Context_Cull( dc, r ))
			return false;
		const Rect_s rect = Splitter_CalcRect( dc, r, dir, distance );
		const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, id );
		Ctrl_DrawBox( dc, rect, Visual::Splitter, ctrl_state );
		return true;
	}

	void Splitter_Mouse( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float panel1_min_size, float panel2_min_size, float& distance )
	{
		const Rect_s splitter_rect = Splitter_CalcRect( dc, r, dir, distance );

		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const bool on_ctrl = Rect_Contains( splitter_rect, mouse_pos );

		if (Context_IsDragging( dc, id ))
		{
			const bool horz = (dir == Orientation::Horz);
			const Vec2_s coord = mouse_pos - Context_GetDrag( dc ).Pos;
			distance = horz 
				? NeClamp( coord.y, r.y + panel1_min_size, r.y + r.h - panel2_min_size - splitter_rect.h ) 
				: NeClamp( coord.x, r.x + panel1_min_size, r.x + r.w - panel2_min_size - splitter_rect.w );
		}

		if (!on_ctrl)
			return;

		const Vec2_s drag_pos = mouse_pos - Rect_Pos( splitter_rect );
		if (Button_Mouse( dc, id, splitter_rect, ButtonClick::Press ))
			Context_BeginDragOnce( dc, id, drag_pos );
	}

	void Splitter_Do( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float panel1_min_size, float panel2_min_size, float& distance )
	{
		if (!Splitter_Draw( dc, id, r, dir, distance ))
			return;
		Splitter_Mouse( dc, id, r, dir, panel1_min_size, panel2_min_size, distance );
	}

	void Splitter_DoV( Context_t dc, Id_t id, const Rect_s& r, float panel1_min_size, float panel2_min_size, float& distance )
	{
		Splitter_Do( dc, id, r, Orientation::Vert, panel1_min_size, panel2_min_size, distance );
	}
	
	void Splitter_DoH( Context_t dc, Id_t id, const Rect_s& r, float panel1_min_size, float panel2_min_size, float& distance )
	{
		Splitter_Do( dc, id, r, Orientation::Horz, panel1_min_size, panel2_min_size, distance );
	}
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Border

	void FrameBorder_GetPartIds( Context_t dc, Id_t id, Id_t* p )
	{
		p[ FrameBorderPart::L  ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::T  ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::R  ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::B  ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::TL ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::TR ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::BL ] = NE_UI_ID_CHILD( id );
		p[ FrameBorderPart::BR ] = NE_UI_ID_CHILD( id );
	}

	void FrameBorder_CalcPartRects( Context_t dc, const Rect_s& r, const Vec2_s& margin, Rect_s* p )
	{
		p[ FrameBorderPart::L  ] = Rect_s { r.x							, r.y + margin.y			 , margin.x				, r.h - 2.0f * margin.y };
		p[ FrameBorderPart::T  ] = Rect_s { r.x + margin.x				, r.y						 , r.w - 2.0f * margin.x, margin.y				};
		p[ FrameBorderPart::R  ] = Rect_s { Rect_Right( r ) - margin.x	, r.y + margin.y			 , margin.x				, r.h - 2.0f * margin.y };
		p[ FrameBorderPart::B  ] = Rect_s { r.x + margin.x				, Rect_Bottom( r ) - margin.y, r.w - 2.0f * margin.x, margin.y				};
		p[ FrameBorderPart::TL ] = Rect_s { r.x							, r.y						 , margin.x				, margin.y				};
		p[ FrameBorderPart::TR ] = Rect_s { Rect_Right( r ) - margin.x	, r.y						 , margin.x				, margin.y				};
		p[ FrameBorderPart::BL ] = Rect_s { r.x							, Rect_Bottom( r ) - margin.y, margin.x				, margin.y				};
		p[ FrameBorderPart::BR ] = Rect_s { Rect_Right( r ) - margin.x	, Rect_Bottom( r ) - margin.y, margin.x				, margin.y				};
	}

	Vec2_s FrameBorder_CalcThickness( Context_t dc, FrameBorderStyle::Mask_t s )
	{
		Theme_t theme = Context_GetTheme( dc );
		return NeHasFlag( s, FrameBorderStyle::Thin ) 
			? theme->Metric[ Metric::Frame_Border_Thin_Size ]
			: theme->Metric[ Metric::Frame_Border_Size ]
			;
	}

	bool FrameBorder_Draw( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& margin )
	{
		if (Context_Cull( dc, r ))
			return false;

		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Visual_s& vis = theme->Visual[ Visual::Frame_Border ];

		Id_t   hot_id    = Context_GetHot	( dc );
		Id_t   pushed_id = Context_GetPushed( dc );
		Id_t   part_id[ FrameBorderPart::COUNT ];
		Rect_s part_r [ FrameBorderPart::COUNT ];
		FrameBorder_GetPartIds	( dc, id, part_id );
		FrameBorder_CalcPartRects( dc, r, margin, part_r );

		const bool hot_edge[4] = 
		{ (hot_id == part_id[ FrameBorderPart::L ]) || (hot_id == part_id[ FrameBorderPart::TL ]) || (hot_id == part_id[ FrameBorderPart::BL ])
		, (hot_id == part_id[ FrameBorderPart::T ]) || (hot_id == part_id[ FrameBorderPart::TL ]) || (hot_id == part_id[ FrameBorderPart::TR ])
		, (hot_id == part_id[ FrameBorderPart::R ]) || (hot_id == part_id[ FrameBorderPart::TR ]) || (hot_id == part_id[ FrameBorderPart::BR ])
		, (hot_id == part_id[ FrameBorderPart::B ]) || (hot_id == part_id[ FrameBorderPart::BL ]) || (hot_id == part_id[ FrameBorderPart::BR ])
		};

		const bool pushed_edge[4] = 
		{ (pushed_id == part_id[ FrameBorderPart::L ]) || (pushed_id == part_id[ FrameBorderPart::TL ]) || (pushed_id == part_id[ FrameBorderPart::BL ])
		, (pushed_id == part_id[ FrameBorderPart::T ]) || (pushed_id == part_id[ FrameBorderPart::TL ]) || (pushed_id == part_id[ FrameBorderPart::TR ])
		, (pushed_id == part_id[ FrameBorderPart::R ]) || (pushed_id == part_id[ FrameBorderPart::TR ]) || (pushed_id == part_id[ FrameBorderPart::BR ])
		, (pushed_id == part_id[ FrameBorderPart::B ]) || (pushed_id == part_id[ FrameBorderPart::BL ]) || (pushed_id == part_id[ FrameBorderPart::BR ])
		};

		for ( int i = 0; i < NeCountOf(hot_edge); ++i )
		{
			const CtrlState::Enum ctrl_state 
				= pushed_edge[i] ? CtrlState::Pushed
				: hot_edge	 [i] ? CtrlState::Hot
				:				   CtrlState::Normal
				;
			Graphics_DrawBorder( g, part_r[i], vis.Back[ ctrl_state ], margin );
		}

		for ( int i = NeCountOf(hot_edge); i < FrameBorderPart::COUNT; ++i )
		{
			const CtrlState::Enum ctrl_state = Ctrl_GetState( dc, part_id[i] );
			Graphics_DrawBorder( g, part_r[i], vis.Back[ ctrl_state ], margin );
		}

		return true;
	}

	Rect_s FrameBorder_Mouse( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& margin, const Vec2_s& min )
	{
		const Vec2_s grip_size = Vec2_s { 4.0f, 4.0f };
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s inner_rect = Rect_Margin ( r, margin );
		const Rect_s grip_rect  = Rect_Inflate( r, grip_size );
		const bool on_grip	= Rect_Contains( grip_rect , mouse_pos );
		const bool on_inner = Rect_Contains( inner_rect, mouse_pos );
		const bool on_frame = on_grip && !on_inner;

		Id_t part_id[ FrameBorderPart::COUNT ];
		FrameBorder_GetPartIds( dc, id, part_id );

		const bool any_dragging = Ctrl_IsAnyDragging( dc, part_id, NeCountOf( part_id ) );
		if (!(on_frame || any_dragging))
			return r;

		Graphics_t g = Context_GetGraphics( dc );
		Rect_s part_r[ FrameBorderPart::COUNT ];
		FrameBorder_CalcPartRects( dc, r, margin, part_r );

		const Vec2_s size = Rect_Size( r );

		Rect_s wnd = r;
		Vec2_s tl = Rect_Pos( r );
		Vec2_s tr = { tl.x + size.x, tl.y };
		Vec2_s bl = { tl.x, tl.y + size.y };
		Vec2_s br = { tl.x + size.x, tl.y + size.y };

		if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::TL], Rect_Inflate( part_r[FrameBorderPart::TL], grip_size ), tl ) == SizeGripAction::Dragged)
		{
			const float offset_x = r.x - tl.x;
			const float offset_y = r.y - tl.y;
			wnd.x -= offset_x;
			wnd.w += offset_x;
			wnd.y -= offset_y;
			wnd.h += offset_y;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::TR], Rect_Inflate( part_r[FrameBorderPart::TR], grip_size ), tr ) == SizeGripAction::Dragged)
		{
			const float offset_x = tr.x - Rect_Right( r );
			const float offset_y = r.y - tr.y;
			wnd.w += offset_x;
			wnd.y -= offset_y;
			wnd.h += offset_y;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::BL], Rect_Inflate( part_r[FrameBorderPart::BL], grip_size ), bl ) == SizeGripAction::Dragged)
		{
			const float offset_x = r.x - bl.x;
			const float offset_y = bl.y - Rect_Bottom( r );
			wnd.h += offset_y;
			wnd.x -= offset_x;
			wnd.w += offset_x;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::BR], Rect_Inflate( part_r[FrameBorderPart::BR], grip_size ), br ) == SizeGripAction::Dragged)
		{
			const float offset_x = br.x - Rect_Right ( r );
			const float offset_y = br.y - Rect_Bottom( r );
			wnd.w += offset_x;
			wnd.h += offset_y;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::L], part_r[FrameBorderPart::L], tl ) == SizeGripAction::Dragged)
		{
			const float offset_x = r.x - tl.x;
			wnd.x -= offset_x;
			wnd.w += offset_x;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::R], part_r[FrameBorderPart::R], br ) == SizeGripAction::Dragged)
		{
			const float offset_x = br.x - Rect_Right( r );
			wnd.w += offset_x;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::T], part_r[FrameBorderPart::T], tl ) == SizeGripAction::Dragged)
		{
			const float offset_y = r.y - tl.y;
			wnd.y -= offset_y;
			wnd.h += offset_y;
		}
		else if (SizeGrip_Mouse( dc, part_id[FrameBorderPart::B], part_r[FrameBorderPart::B], br ) == SizeGripAction::Dragged)
		{
			const float offset_y = br.y - Rect_Bottom( r );
			wnd.h += offset_y;
		}
		return Rect_ClampSize( r, wnd, min );
	}

	Rect_s FrameBorder_Do( Context_t dc, Id_t id, const Rect_s& r, FrameBorderStyle::Mask_t s, const Vec2_s& min )
	{
		const Vec2_s margin = FrameBorder_CalcThickness( dc, s );
		FrameBorder_Draw( dc, id, r, margin );
		if (NeHasFlag( s, FrameBorderStyle::NoSize))
			return r;
		return FrameBorder_Mouse( dc, id, r, margin, min );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Caption

	CtrlState::Enum FrameCaption_GetCtrlState( Context_t dc, Id_t id )
	{
		if (Context_IsWnd( dc, id ))
			return CtrlState::Pushed;
		if (Context_IsHot( dc, id ))
			return CtrlState::Hot;
		return Ctrl_GetState( dc, id );
	}

	float FrameCaption_CalcHeight( Context_t dc )
	{
		return Caption_CalcHeight( dc );
	}

	bool FrameCaption_Draw( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t )
	{
		const CtrlState::Enum state = FrameCaption_GetCtrlState( dc, id );
		return Caption_Draw( dc, id, r, t, state );
	}

	SizeGripAction::Enum FrameCaption_Mouse( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& threshold, Vec2_s& drag_begin, Rect_s& wnd )
	{
		Vec2_s pos = Rect_Pos( wnd );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const SizeGripAction::Enum action = SizeGrip_Mouse( dc, id, r, pos );
		switch( action )
		{
		case SizeGripAction::Pushed:
			Context_SetWnd( dc, id );
			drag_begin = mouse_pos;
			break;

		case SizeGripAction::Dragged:
			{
				// don't report as dragging until we've surpassed the threshold
				const Vec2_s drag_offset = drag_begin - mouse_pos;
				const bool is_drag = (fabsf( drag_offset.x ) > threshold.x) 
								  || (fabsf( drag_offset.y ) > threshold.y);
				if (!is_drag)
					return SizeGripAction::None;

				wnd.x = pos.x;
				wnd.y = pos.y;
				return action;
			}
			break;

		default:
			break;
		}
		return action;
	}

	SizeGripAction::Enum FrameCaption_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, const Vec2_s& threshold, Vec2_s& drag_begin, Rect_s& wnd )
	{
			   FrameCaption_Draw ( dc, id, r, t );
		return FrameCaption_Mouse( dc, id, r, threshold, drag_begin, wnd );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Wnd

	Vec2_s FrameWnd_CalcDragThreshold( Context_t dc, FrameWndStyle::Mask_t s )
	{
		if (!NeHasFlag( s, FrameWndStyle::TearOff ))
			return Vec2_s {};
		Theme_t theme = Context_GetTheme( dc );
		return theme->Metric[ Metric::TearOff_Threshold ];
	}

	Rect_s FrameWnd_CalcInnerRect( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s )
	{
		if (NeHasFlag( s, FrameWndStyle::NoBorder ))
			return r;
		const FrameBorderStyle::Mask_t border = NeHasFlag( s, FrameWndStyle::ThinBorder ) 
			? FrameBorderStyle::Thin 
			: FrameBorderStyle::None;
		const Vec2_s w = FrameBorder_CalcThickness( dc, border );
		return Rect_Margin( r, w );
	}

	Rect_s FrameWnd_CalcCaptionRect( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s )
	{
		const bool   no_caption	  = NeHasFlag( s, FrameWndStyle::NoCaption );
		const float  caption_h	  = no_caption ? 0 : FrameCaption_CalcHeight( dc );
		const Rect_s inner_rect	  = FrameWnd_CalcInnerRect( dc, r, s );
		const Rect_s caption_rect = { inner_rect.x, inner_rect.y, inner_rect.w, caption_h };
		return caption_rect;
	}

	Rect_s FrameWnd_CalcClientRect( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s )
	{
		const bool	 no_caption	 = NeHasFlag( s, FrameWndStyle::NoCaption );
		const float  caption_h	 = no_caption ? 0 : FrameCaption_CalcHeight( dc );
		const Rect_s inner_rect	 = FrameWnd_CalcInnerRect( dc, r, s );
		const Rect_s client_rect = { inner_rect.x, inner_rect.y + caption_h, inner_rect.w, inner_rect.h - caption_h };
		return client_rect;
	}

	FrameWndResult_s FrameWnd_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, FrameWndStyle::Mask_t s, const Vec2_s& min, Vec2_s& drag_begin )
	{
		// caption
		FrameWndResult_s caption_result = { r };
		const Rect_s caption_rect = FrameWnd_CalcCaptionRect( dc, r, s );
		const Vec2_s drag_threshold = FrameWnd_CalcDragThreshold( dc, s );
		caption_result.Drag = FrameCaption_Do( dc, id, caption_rect, t, drag_threshold, drag_begin, caption_result.Rect );
		if (NeHasFlag( s, FrameWndStyle::NoBorder ))
			return caption_result;

		// border
		FrameWndResult_s border_result = { r };
		const FrameBorderStyle::Mask_t border_style 
			= (NeHasFlag( s, FrameWndStyle::NoSize )	 ? FrameBorderStyle::NoSize	: 0)
			| (NeHasFlag( s, FrameWndStyle::ThinBorder ) ? FrameBorderStyle::Thin	: 0);
		border_result.Rect = FrameBorder_Do( dc, id, r, border_style, min );

		return (caption_result.Drag != SizeGripAction::None) 
				? caption_result
				: border_result;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Dock Indicator

	void DockIndicator_CalcTargetRect( Context_t dc, const Rect_s& r, DockSide::Flag side, Rect_s* wnd, Rect_s* host )
	{
		switch ( side )
		{
		case DockSide::Center:
			{
				if ( wnd )
					*wnd = r;

				if ( host )
					*host = r;
			}
			break;

		case DockSide::Top:
			Rect_SplitV( r, r.h / 2.0f, wnd, host );
			break;

		case DockSide::Left:
			Rect_SplitH( r, r.w / 2.0f, wnd, host );
			break;

		case DockSide::Right:
			Rect_SplitH( r, r.w / 2.0f, host, wnd );
			break;

		case DockSide::Bottom:
			Rect_SplitV( r, r.h / 2.0f, host, wnd );
			break;

		default:
			break;
		}
	}

	DockSide::Flag DockIndicator_Do( Context_t dc, const Rect_s& r )
	{
		NeGuiScopedModal( dc );

		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );

		const Vec2_s button_size  = theme->Metric[ Metric::DockIndicator_Button_Size	];
		const Vec2_s button_space = theme->Metric[ Metric::DockIndicator_Button_Spacing ];
		const Vec2_s center = Vec2_s { r.x + 0.5f * r.w, r.y + 0.5f * r.h };

		const Vec2_s pos_c = center - 0.5f * button_size;
		const Vec2_s pos_l = pos_c - Vec2_s { button_size.x + button_space.x, 0.0f };
		const Vec2_s pos_r = pos_c + Vec2_s { button_size.x + button_space.x, 0.0f };
		const Vec2_s pos_t = pos_c - Vec2_s { 0.0f, button_size.y + button_space.y };
		const Vec2_s pos_b = pos_c + Vec2_s { 0.0f, button_size.y + button_space.y };

		const Rect_s rc_c = Rect_Ctor( pos_c, button_size );
		const Rect_s rc_l = Rect_Ctor( pos_l, button_size );
		const Rect_s rc_r = Rect_Ctor( pos_r, button_size );
		const Rect_s rc_t = Rect_Ctor( pos_t, button_size );
		const Rect_s rc_b = Rect_Ctor( pos_b, button_size );

		const Vec2_s& mouse_pos = Context_GetMousePos( dc );
		const bool on_c = Rect_Contains( rc_c, mouse_pos );
		const bool on_l = Rect_Contains( rc_l, mouse_pos );
		const bool on_r = Rect_Contains( rc_r, mouse_pos );
		const bool on_t = Rect_Contains( rc_t, mouse_pos );
		const bool on_b = Rect_Contains( rc_b, mouse_pos );

		DockSide::Flag side = DockSide::None;
		if ( on_c )
			side = DockSide::Center;
		else if ( on_l )
			side = DockSide::Left;
		else if ( on_r )
			side = DockSide::Right;
		else if ( on_t )
			side = DockSide::Top;
		else if ( on_b )
			side = DockSide::Bottom;

		Rect_s target_rect;
		DockIndicator_CalcTargetRect( dc, r, side, &target_rect, nullptr );

		// target
		const Visual_s& vis_dst = theme->Visual[ Visual::DockIndicator_Target ];
		const uint32_t front_target = vis_dst.Front[ CtrlState::Hot ];
		const uint32_t back_target  = vis_dst.Back [ CtrlState::Hot ];
		Graphics_DrawBox( g, target_rect, front_target, back_target );

		// indicator
		const Visual_s& vis_ind = theme->Visual[ Visual::DockIndicator ];
		const uint32_t front_hot  = vis_ind.Front[ CtrlState::Hot ];
		const uint32_t back_hot	  = vis_ind.Back [ CtrlState::Hot ];
		const uint32_t front_cold = vis_ind.Front[ CtrlState::Normal ];
		const uint32_t back_cold  = vis_ind.Back [ CtrlState::Normal ];
		Graphics_DrawBox( g, rc_c, on_c ? front_hot : front_cold, on_c ? back_hot : back_cold );
		Graphics_DrawBox( g, rc_l, on_l ? front_hot : front_cold, on_l ? back_hot : back_cold );
		Graphics_DrawBox( g, rc_r, on_r ? front_hot : front_cold, on_r ? back_hot : back_cold );
		Graphics_DrawBox( g, rc_t, on_t ? front_hot : front_cold, on_t ? back_hot : back_cold );
		Graphics_DrawBox( g, rc_b, on_b ? front_hot : front_cold, on_b ? back_hot : back_cold );

		const MouseState_s& mouse = Context_GetMouse( dc );
		return mouse.Button[0].WentUp ? side : DockSide::None;
	}

} }
