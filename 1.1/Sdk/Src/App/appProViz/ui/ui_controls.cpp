//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"

//======================================================================================
namespace nemesis { namespace gui { namespace layout
{
	/// Initializes layout information using the given metrics.
	void Begin( Layout& l, const Rect_s& bounds, const Vec2_s& spacing, Direction::Enum direction, const Vec2_s& offset )
	{
		l.Bounds = bounds;
		l.Spacing = spacing;
		l.Horizontal = (direction == Direction::Horizontal);
		l.CaretInfo_s = Vec2_s { bounds.x, bounds.y } + offset;
	}

	/// Initializes layout information using the given metrics.
	void Begin( Layout& l, const Rect_s& bounds, const Vec2_s& spacing, Direction::Enum direction )
	{ Begin( l, bounds, spacing, direction, Vec2_s {} ); }

	/// Calculates item bounds and visibility for an item with given dimensions and
	/// advances the caret according to the layout's spacing.
	Item Append( Layout& l, float w, float h )
	{
		Item r = {};
		if (l.Horizontal)
		{
			r.Bounds.x = l.CaretInfo_s.x;
			r.Bounds.y = l.CaretInfo_s.y;
			r.Bounds.w = w;
			r.Bounds.h = h;
			l.CaretInfo_s.x += l.Spacing.x + w;
			r.Visible = r.Bounds.x < Rect_Right( l.Bounds );
		}
		else
		{
			r.Bounds.x = l.CaretInfo_s.x;
			r.Bounds.y = l.CaretInfo_s.y;
			r.Bounds.w = w;
			r.Bounds.h = h;
			l.CaretInfo_s.y += l.Spacing.y + h;
			r.Visible = r.Bounds.y < Rect_Bottom( l.Bounds );
		}
		return r;
	}

	/// Calculates the remainder if the given layout.
	Item Remainder( Layout& l )
	{
		Item r;
		r.Bounds.x = l.CaretInfo_s.x;
		r.Bounds.y = l.CaretInfo_s.y;
		r.Bounds.w  = Rect_Right( l.Bounds ) - r.Bounds.x;
		r.Bounds.h = Rect_Bottom( l.Bounds ) - r.Bounds.y;
		r.Visible = (r.Bounds.w > 0.0f) && (r.Bounds.h > 0.0f);
		return r;
	}


} } }

//======================================================================================
namespace nemesis { namespace gui { namespace label
{
	static const TextFormat::Mask_t TEXT_FORMAT = TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap;

	void Draw( const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len )
	{ DrawString( text, text_len, v.Font, TEXT_FORMAT, r, v.TextColor ); }

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace popup
{
	void Draw( const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len )
	{
		Rect_s bounds;
		MeasureString( text, text_len, v.Label.Font, TextFormat::Default, r, &bounds );

		Rect_s text_rect;
		text_rect.x = ( gui::GetMouse().Pos.x + bounds.w < r.w ) ? (gui::GetMouse().Pos.x + 16) : (gui::GetMouse().Pos.x - bounds.w);
		text_rect.y = ( gui::GetMouse().Pos.y + bounds.h < r.h ) ? (gui::GetMouse().Pos.y + 16) : (gui::GetMouse().Pos.y - bounds.h);
		text_rect.w = bounds.w;
		text_rect.h = bounds.h;

		Rect_s popup_rect = text_rect;
		popup_rect = Rect_Inflate( popup_rect, 8.0f, 4.0f );

		FillRectangle( popup_rect, v.BkgndColor );
		DrawBorder( popup_rect, Color::Black, Color::Black );
		DrawString( text, text_len, v.Label.Font, TextFormat::Middle, text_rect, v.Label.TextColor );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace button
{
	static const TextFormat::Mask_t TEXT_FORMAT = TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap;

	/// only draws the button face, without text
	static void Draw( const Rect_s& r, const Visual_s& v, float blend_alpha )
	{
		const Color_c light = Color_Blend( v.BkgndColor[0], v.CheckedColor, blend_alpha );
		const Color_c dark  = Color_Blend( v.BkgndColor[1], v.CheckedColor, blend_alpha );
		const Rect_s inner_rect { r.x + 2.0f, r.y + 2.0f, r.w - 5.0f, r.h - 5.0f };
		FillRectangle( r, light, dark, LinearGradient::Vertical );
		DrawRectangle( r, Color::Black );
		FillRectangle( inner_rect, dark, light, LinearGradient::Vertical );
	}

	Action::Enum Draw( Id_t id, const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len )
	{ return Draw( id, r, v, text, text_len, State::Unchecked ); }

	Action::Enum Draw( Id_t id, const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len, State::Enum state )
	{
		// render
		{
			const float alpha = (state == State::Unchecked) ? 0.0f : 0.5f;
			Draw( r, v, alpha );
			const Rect_s text_rect { r.x + 3.0f, r.y + 3.0f, r.w - 6.0f, r.h - 6.0f };
			DrawString( text, text_len, v.Label.Font, TEXT_FORMAT, text_rect, v.Label.TextColor );
		}

		// input
		return Input( id, r );
	}

	Action::Enum Input( Id_t id, const Rect_s& r )
	{
		Action::Enum action = Action::None;
		if (Rect_Contains( r, gui::GetMouse().Pos ))
			SetHot( id );

		if ( gui::IsActive( id ) )
		{
			if ( gui::GetMouse().Button[0].WentUp )
			{
				if ( gui::IsHot( id ) )
					action = Action::Clicked;
			}
		}
		else if ( gui::IsHot( id ) )
		{
			if ( gui::GetMouse().Button[0].WentDown )
				SetActive( id );
		}

		return action;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace trackbar
{
	static const float TRACK_SIZE = 5.0f;
	static const float TRACK_HALF = ceilf(TRACK_SIZE * 0.5f);

	static void DrawThumb( const Rect_s& r )
	{
		Rect_s part_rect = r;
		FillRectangle( part_rect, 0xfff0f0f0 );
		DrawBorder( part_rect, 0xffffffff, 0xff696969 );
		part_rect = Rect_Inflate( part_rect, -1.0f, -1.0f );
		DrawBorder( part_rect, 0xffe3e3e3, 0xffa0a0a0 );
	}

	static void DrawTrack( const Rect_s& r, Layout::Direction d )
	{
		Rect_s first_rect;
		Rect_s second_rect;
		switch ( d )
		{
		case Layout::Horizontal:
			first_rect  = Rect_s { r.x, r.y			    , r.w,		 TRACK_HALF };
			second_rect = Rect_s { r.x, r.y + TRACK_HALF, r.w, r.h - TRACK_HALF };
			break;
		case Layout::Vertical:
			first_rect  = Rect_s { r.x			   , r.y, 	    TRACK_HALF, r.h };
			second_rect = Rect_s { r.x + TRACK_HALF, r.y, r.w - TRACK_HALF, r.h };
			break;
		}

		FillRectangle( first_rect , 0xffacacb7 );
		FillRectangle( second_rect, 0xffececf7 );
		DrawRectangle( r, Color::Black );
	}

	//==================================================================================
	// public interface
	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item& item )
	{
		// clamp
		const float pos = NeClamp( item.Pos, item.Min, item.Max );

		// render
		float new_pos;
		{
			// layout
			const float margin = v.ThumbSize/2.0f;
			const float rel_pos = (pos-item.Min) / (item.Max-item.Min);

			Rect_s thumb_rect;
			Rect_s track_rect;
			Rect_s track_rect_1;
			Rect_s track_rect_2;

			if ( v.Direction == Layout::Horizontal )
			{
				const float center = r.y + r.h/2.0f;
				track_rect = Rect_s { r.x + margin, center - TRACK_HALF, r.w - v.ThumbSize, TRACK_SIZE };

				const float offset = rel_pos * track_rect.w;
				thumb_rect = Rect_s { r.x + offset, r.y, v.ThumbSize, r.h };

				track_rect_1.x	= r.x;
				track_rect_1.y	= r.y;
				track_rect_1.w	= offset;
				track_rect_1.h	= r.h;

				track_rect_2.x	= r.x + offset + v.ThumbSize;
				track_rect_2.y	= r.y;
				track_rect_2.w	= r.w - offset - v.ThumbSize;
				track_rect_2.h	= r.h;

				const float rel_mouse = (gui::GetMouse().Pos.x - track_rect.x) / (track_rect.w);
				const float track_pos = rel_mouse * (item.Max - item.Min) + item.Min;
				new_pos = NeClamp( track_pos, item.Min, item.Max );
			}
			else
			{
				const float center = r.x + r.w/2.0f;
				track_rect = Rect_s { center - TRACK_HALF, r.y + margin, TRACK_SIZE, r.h - v.ThumbSize };

				const float offset = rel_pos * track_rect.h;
				thumb_rect = Rect_s { r.x, r.y + offset, r.w, v.ThumbSize };

				track_rect_1.x		= r.x;
				track_rect_1.y		= r.y;
				track_rect_1.w	= r.w;
				track_rect_1.h	= offset;

				track_rect_2.x		= r.x;
				track_rect_2.y		= r.y      + offset + v.ThumbSize;
				track_rect_2.w	= r.w;
				track_rect_2.h	= r.h - offset - v.ThumbSize;

				const float rel_mouse = (gui::GetMouse().Pos.y - track_rect.y) / (track_rect.h);
				const float track_pos = rel_mouse * (item.Max - item.Min) + item.Min;
				new_pos = NeClamp( track_pos, item.Min, item.Max );
			}

			// draw
			DrawTrack( track_rect, v.Direction );
			DrawThumb( thumb_rect );
		}

		// input
		Result result = { Action::None, pos };
		{
			if (Rect_Contains( r, gui::GetMouse().Pos ))
				SetHot( id );

			if ( gui::IsActive( id ) )
			{
				result.Action = Action::Dragged;
				result.TrackPos = new_pos;
				return result;
			}
			
			if ( gui::IsHot( id ) )
			{
				if ( gui::GetMouse().Button[0].WentDown )
				{
					SetActive( id );
					result.Action = Action::Clicked;
				}
			}
		}
		return result;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace scrollbar
{
	static const Vec2_s TRACK_MARGIN	= Vec2_s { 2.0f, 2.0f };
	static const float THUMB_WIDTH_MIN	= 16.0f;
	static const float THUMB_MARGIN		= 1.0f;

	static Rect_s CtrlToTrack( const Rect_s r )
	{ return Rect_s { r.x + TRACK_MARGIN.x, r.y + TRACK_MARGIN.y, r.w - 2.0f * TRACK_MARGIN.x, r.h - 2.0f * TRACK_MARGIN.y }; }

	static void DrawThumb( const Rect_s& r )
	{ trackbar::DrawThumb( r ); }

	static bool IsBefore( Layout::Direction d, const Vec2_s& p, const Rect_s& r )
	{
		if ( d == Layout::Horizontal )
			return p.x < r.x;
		return p.y < r.y;
	}

	//==================================================================================
	// public interface
	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item& item )
	{
		const float pos = NeClamp( item.Pos, item.Min, item.Max - item.Page );
		const bool horz = (v.Direction == Layout::Horizontal);
		const Rect_s track_rect = CtrlToTrack( r );
		Rect_s thumb_rect;

		// render
		{
			// layout
			{
				const float track_size = horz ? track_rect.w : track_rect.h;

				const float offset = (pos - item.Min) / (item.Max - item.Min);
				const float ratio  = item.Page		  / (item.Max - item.Min);
				const float ratio_capped = NeMin( 1.0f, ratio );

				const float thumb_size = track_size * ratio_capped;
				const float thumb_size_capped = NeMax( THUMB_WIDTH_MIN, thumb_size );

				const float thumb_pos = offset * track_size - (thumb_size_capped - thumb_size);
				const float thumb_pos_capped = NeMax( 0.0f, thumb_pos );

				if ( horz ) thumb_rect = Rect_s { track_rect.x + thumb_pos_capped, track_rect.y, thumb_size_capped, track_rect.h };
				else		thumb_rect = Rect_s { track_rect.x, track_rect.y + thumb_pos_capped, track_rect.w, thumb_size_capped };
			}

			// draw
			DrawThumb( thumb_rect );
		}

		// input
		Result result = { Action::None, item.Pos };
		{
			if (Rect_Contains( r, gui::GetMouse().Pos ))
				SetHot( id );

			if ( gui::IsActive( id ) )
			{
				const DragInfo_s& drag = gui::GetDrag();
				if ( drag.Id == id )
				{
					const Vec2_s mouse_pos = gui::GetMouse().Pos + drag.Pos;

					float rel_mouse;
					if ( horz )
						rel_mouse = (mouse_pos.x - track_rect.x - thumb_rect.w) / (track_rect.w);
					else
						rel_mouse = (mouse_pos.y - track_rect.y - thumb_rect.h) / (track_rect.h);

					const float track_pos = rel_mouse * (item.Max - item.Min) + item.Min;
					result.Action = Action::Dragged;
					result.TrackPos = NeClamp( track_pos, item.Min, item.Max - item.Page );
					return result;
				}
			}
			
			if ( gui::IsHot( id ) )
			{
				if ( gui::GetMouse().Button[0].WentDown )
				{
					SetActive( id );
					result.Action = Action::Clicked;

					if ( Rect_Contains( thumb_rect, gui::GetMouse().Pos ) )
					{
						gui::BeginDrag( id, Vec2_s { Rect_Right( thumb_rect ) - gui::GetMouse().Pos.x, Rect_Bottom( thumb_rect ) - gui::GetMouse().Pos.y } );
					}
					else
					{
						const float offset = IsBefore( v.Direction, gui::GetMouse().Pos, thumb_rect ) ? -item.Page : item.Page;
						result.TrackPos = NeClamp( pos + offset, item.Min, item.Max - item.Page );
					}
				}
			}
		}

		return result;
	}
	
} } }

//======================================================================================
namespace nemesis { namespace gui { namespace tab
{
	static const float TAB_HEADER_INDENT = 25.0f;
	static const float TAB_HEADER_HEIGHT = 25.0f;
	static const float TAB_ITEM_EXPAND	 = 2.0f;
	static const Vec2_s TAB_LABEL_MARGIN = Vec2_s { 4.0f, 4.0f };
	static const Vec2_s TAB_FACE_MARGIN  = Vec2_s { 5.0f, 5.0f };
	static const TextFormat::Mask_t TEXT_FORMAT = TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap;

	static Rect_s TabCtrlToHeader( const Rect_s& r )
	{
		return Rect_s { r.x + TAB_HEADER_INDENT, r.y, r.w - TAB_HEADER_INDENT, TAB_HEADER_HEIGHT };
	}

	static Rect_s TabCtrlToPage( const Rect_s& r )
	{
		return Rect_s { r.x, r.y + TAB_HEADER_HEIGHT, r.w, r.h - TAB_HEADER_HEIGHT };
	}

	static Rect_s TabPageToFace( const Rect_s& r )
	{
		return Rect_s { r.x + TAB_FACE_MARGIN.x, r.y + TAB_FACE_MARGIN.x, r.w - 2.0f * TAB_FACE_MARGIN.x, r.h - 2.0f * TAB_FACE_MARGIN.x };
	}

	//==================================================================================
	// public interface
	Rect_s CtrlToPage( const Rect_s& r )
	{ return TabPageToFace( TabCtrlToPage( r ) ); }

	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item* items, int count, int active_item )
	{
		Result result = { Action::None, active_item };
		const Rect_s tab_ctrl_rect = r;

		// inactive buttons
		Rect_s text_bounds;
		Rect_s button_rect = TabCtrlToHeader( tab_ctrl_rect );
		Rect_s expanded_rect;
		Rect_s hit_rect;
		Id_t item_id = id;
		for ( int i = 0; i < count; ++i )
		{
			MeasureString( items[i].Text, -1, v.Label.Font, TEXT_FORMAT, tab_ctrl_rect, &text_bounds );
			button_rect.w = text_bounds.w + 2.0f * TAB_LABEL_MARGIN.x;
			{
				if ( i == active_item )
				{
					expanded_rect = button_rect;
					expanded_rect = Rect_Inflate( expanded_rect, TAB_ITEM_EXPAND, TAB_ITEM_EXPAND );
					hit_rect = expanded_rect;
				}
				else
				{
					FillRectangle( button_rect, 0xffdddddd, 0xff888888, LinearGradient::Vertical );
					DrawRectangle( button_rect, Color::Black );
					DrawString( items[i].Text, -1, v.Label.Font, TEXT_FORMAT, button_rect, v.Label.TextColor );
					hit_rect = button_rect;
				}

				// input
				item_id = Id_Cat( id, i );
				if (Rect_Contains( hit_rect, gui::GetMouse().Pos ))
					gui::SetHot( item_id );

				if (gui::IsActive(item_id))
				{
					if (gui::GetMouse().Button[0].WentUp)
					{
						if (gui::IsHot(item_id))
						{
							result.Action = Action::Clicked;
							result.Item = i;
						}
					}
				}
				if (gui::IsHot(item_id))
				{
					if (gui::GetMouse().Button[0].WentDown)
						SetActive( item_id );
				}
			}
			button_rect.x += button_rect.w - 1.0f;
		}

		// page 
		const Rect_s tab_page_rect = TabCtrlToPage( tab_ctrl_rect );
		const Rect_s tab_r_rect = TabPageToFace( tab_page_rect );
		DrawRectangle( tab_page_rect, Color::Black );

		// active button
		FillRectangle( expanded_rect, Color::WhiteSmoke );
		DrawRectangle( expanded_rect, Color::Black );
		DrawString( items[ active_item ].Text, -1, v.Label.Font, TEXT_FORMAT, expanded_rect, v.Label.TextColor );

		// page inlay
		FillRectangle( tab_page_rect, Color::WhiteSmoke );
		FillRectangle( tab_r_rect, Color::LightGray );
		return result;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace toolbar
{
	/// toolbar metrics
	static const float BUTTON_BORDER = 2.0f;
	static const float BUTTON_MARGIN = 8.0f;
	static const float BUTTON_SPACING = 8.0f;
	static const float TOOLBAR_BORDER = 2.0f;
	static const float TOOLBAR_MARGIN = 6.0f;

	/// Draws a single toolbar item
	void DrawItem( const Rect_s& r, const Visual_s& v, const Item& item, int index, Rect_s& item_rect, Result& result )
	{
		switch ( item.Type )
		{
		case Item::Label:
			{
				Rect_s text_rect;
				MeasureString( item.Text, -1, v.Label.Font, TextFormat::NoWrap, r, &text_rect );
				item_rect.w = text_rect.w + 2 * BUTTON_MARGIN;
				label::Draw( item_rect, v.Label, item.Text, -1 );
			}
			break;

		case Item::Button:
			{
				Rect_s text_rect;
				MeasureString( item.Text, -1, v.Button.Label.Font, TextFormat::NoWrap, r, &text_rect );
				item_rect.w = text_rect.w + 2 * BUTTON_MARGIN;
				if (button::Draw( item.ID, item_rect, v.Button, item.Text, -1, item.Checked ? button::State::Checked : button::State::Unchecked ))
				{
					result.Action = Action::Clicked;
					result.Item = index;
				}
			}
			break;

		case Item::Delimiter:
			{
				item_rect.x += BUTTON_SPACING;
				item_rect.w = 1.0f;
				DrawBorder( item_rect, Color::Black, 0xffaaaaaa );
			}
			break;

		default:
			break;
		}

		item_rect.x += item_rect.w + BUTTON_SPACING;
	}

	/// Draws all toolbar items
	void DrawItems( const Rect_s& r, const Visual_s& v, const Item* items, int count, Rect_s& item_rect, Result& result )
	{
		for ( int i = 0; i < count; ++i )
			DrawItem( r, v, items[i], i, item_rect, result );
	}

	//==================================================================================
	// public interface
	Result Draw( const Rect_s& r, const Visual_s& v, const Item* items, int count, Rect_s* remainder )
	{
		DrawBar( r, v.Color, TOOLBAR_BORDER );

		Rect_s item_rect = r;
		item_rect = Rect_Inflate( item_rect, -TOOLBAR_MARGIN, -TOOLBAR_MARGIN );

		Result result = { Action::None, -1 };
		DrawItems( r, v, items, count, item_rect, result );

		if (remainder)
			*remainder = Rect_s { item_rect.x + BUTTON_SPACING, item_rect.y, r.w - item_rect.x, item_rect.h };

		return result;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace optionbar
{
	//==================================================================================
	// internals
	const float ITEM_SPACING_Y	 = 6.0f;
	const float ITEM_SPACING_X	 = 4.0f;
	const float DELIMITER_HEIGHT = 8.0f;
	const float CONTROL_WIDTH	 = 150.0f;

	static float CalculateMaxLabelWidth( const Rect_s& r, const Visual_s& v, const Item* items, int count )
	{
		float max = 0.0f; 
		Rect_s bounds;
		for ( int i = 0; i < count; ++i )
		{
			const Item& item = items[i];
			if (!item.Text)
				continue;

			MeasureString( item.Text, -1, v.Label.Font, TextFormat::NoWrap, r, &bounds ); 
			if (bounds.w > max)
				max = bounds.w;
		}
		return max;
	}

	static int FormatValue( wchar_t* text, int size, const Item& item )
	{
		switch ( item.Type )
		{
		case Item::Bool:
			return swprintf_s( text, size, L"%S", item.Pos.Bool ? "on" : "off" );
		case Item::Byte:
			return swprintf_s( text, size, L"%u", (uint32_t)item.Pos.Byte );
		case Item::Float:
			return swprintf_s( text, size, L"%.2f", item.Pos.Float );
		}

		text[0] = 0;
		return 0;
	}

	static void DrawControl( Id_t id, const Rect_s& r, const Visual_s& v, Item& item, const wchar_t* text, int text_len )
	{
		switch ( item.Type )
		{
		case Item::Bool:
			{
				if (button::Draw( id, r, v.Button, text, text_len, item.Pos.Bool ? button::State::Checked : button::State::Unchecked ))
					item.Pos.Bool = !item.Pos.Bool;
			}
			break;

		case Item::Byte:
			{
				const trackbar::Visual_s tv = { r.h * 0.75f, trackbar::Layout::Horizontal };
				const trackbar::Item ti = { (float)item.Pos.Byte, (float)item.Min.Byte, (float)item.Max.Byte };
				const trackbar::Result tr = trackbar::Draw( id, r, tv, ti );
				if (tr.Action)
					item.Pos.Byte = (uint8_t)tr.TrackPos;
			}
			break;

		case Item::Float:
			{
				const trackbar::Visual_s tv = { r.h * 0.75f, trackbar::Layout::Horizontal };
				const trackbar::Item ti = { item.Pos.Float, item.Min.Float, item.Max.Float };
				const trackbar::Result tr = trackbar::Draw( id, r, tv, ti );
				if (tr.Action)
					item.Pos.Float = tr.TrackPos;
			}
			break;
		}
	}

	//==================================================================================
	// public interface
	void Draw( const Rect_s& r, const Visual_s& v, Item* items, int count )
	{
		const float max_label_width = CalculateMaxLabelWidth( r, v, items, count );
		const float line_height = GetLineHeight( v.Label.Font );
		const float item_spacing = line_height + ITEM_SPACING_Y;

		Rect_s part_rect;
		Rect_s item_rect = r;
		item_rect.h = line_height;

		TextBuffer& text = gui::GetTextBuffer();

		for ( int i = 0; i < count; ++i )
		{
			Item& item = items[i];

			// delimiter
			switch (item.Type)
			{
			case Item::Delimiter:
				item_rect.y += DELIMITER_HEIGHT;
				continue;
			}

			// format value text
			text.Length = FormatValue( text.Text, NeCountOf(text.Text), item );

			// draw label
			part_rect = item_rect;
			item_rect.y += item_spacing; 
			part_rect.w = max_label_width;
			DrawString( item.Text, -1, v.Label.Font, TextFormat::NoWrap, part_rect, v.Label.TextColor );
			
			// draw control
			part_rect.x += part_rect.w + ITEM_SPACING_X;
			part_rect.w = CONTROL_WIDTH;
			DrawControl( item.Id, part_rect, v, item, text.Text, text.Length );

			// draw value
			switch (item.Type)
			{
			case Item::Bool:
				continue;
			}
			part_rect.x += part_rect.w + ITEM_SPACING_X;
			part_rect.w = Rect_Right( r ) - part_rect.x;
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::NoWrap, part_rect, v.Label.TextColor );
		}
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace collapsebar
{
	//==================================================================================
	// internals
	static const float MARGIN		 = 4.0f;
	static const float SPACING		 = 6.0f;
	static const float BUTTON_WIDTH	 = 16.0f;
	static const float BUTTON_HEIGHT = 16.0f;

	Rect_s CtrlToInner( const Rect_s& r )
	{
		Rect_s result = r;
		result = Rect_Inflate( result, -MARGIN, -MARGIN );
		return result;
	}

	Rect_s CtrlToButton( const Rect_s& r, Alignment::Enum align )
	{
		Rect_s result = CtrlToInner( r );
		if ( align == Alignment::Right )
			result.x = Rect_Right( result ) - BUTTON_WIDTH;
		result.w  = BUTTON_WIDTH;
		result.h = BUTTON_HEIGHT;
		return result;
	}

	Rect_s CtrlToLabel( const Rect_s& r, Alignment::Enum align )
	{
		Rect_s result = CtrlToInner( r );
		if ( align == Alignment::Left )
			result.x += BUTTON_WIDTH + SPACING;
		result.w -= BUTTON_WIDTH + SPACING;
		result.h = BUTTON_HEIGHT;
		return result;
	}

	//==================================================================================
	// public interface
	Result Draw( const Rect_s& r, const Visual_s& v, const Item& item )
	{
		Result result = { Action::None };

		// render
		const TextFormat::Mask_t fmt = TextFormat::Middle | TextFormat::NoWrap | ((item.Side == Alignment::Right) ? TextFormat::Right : TextFormat::Left);
		const Rect_s label_rect = CtrlToLabel( r, item.Side );
		DrawString( item.Text, -1, v.Label.Font, fmt, label_rect, v.Label.TextColor );		

		const Rect_s button_rect = CtrlToButton( r, item.Side );
		if (button::Draw( item.ID, button_rect, v.Button, item.Collapsed ? L"+" : L"-", 1 ))
			result.Action = Action::Clicked;

		// input
		return result;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace tree_ctrl
{
	//==================================================================================
	// internals
	static Rect_s ItemToInner( const Rect_s& r )
	{
		return Rect_Inflate( r, -ITEM_MARGIN, -ITEM_MARGIN );
	}

	static Rect_s InnerToCollapse( const Rect_s& r )
	{
		return Rect_s { r.x, r.y, BUTTON_SIZE, BUTTON_SIZE };
	}

	static Rect_s InnerToCheck( const Rect_s& r )
	{
		return Rect_s { r.x + BUTTON_SIZE + BUTTON_SPACING, r.y, BUTTON_SIZE, BUTTON_SIZE };
	}

	static Rect_s InnerToLabel( const Rect_s& r )
	{
		return Rect_s
			{ r.x + 2.0f * (BUTTON_SIZE + BUTTON_SPACING) + LABEL_MARGIN
			, r.y
			, r.w - 2.0f * (BUTTON_SIZE - BUTTON_SPACING) - 2.0f * LABEL_MARGIN
			, r.h 
			};
	}

	//==================================================================================
	// public interface
	float CalculateItemHeight( const Visual_s& v )
	{
		const float line_height = GetLineHeight( v.Label.Font );
		return 2.0f * ITEM_MARGIN + NeMax( line_height, BUTTON_SIZE );
	}

	ItemResult DrawItem( const Rect_s& r, const Visual_s& v, const Item& item )
	{
		const Rect_s temp_rect = ItemToInner( r );
		Rect_s inner_rect = temp_rect;
		inner_rect.x	 += item.Indent * (BUTTON_SIZE + BUTTON_SPACING);
		inner_rect.w -= item.Indent * (BUTTON_SIZE + BUTTON_SPACING);

		ItemResult result = {};

		// label
		DrawString( item.Text, item.TextLength, v.Label.Font, TextFormat::NoWrap | TextFormat::Middle, InnerToLabel( inner_rect ), v.Label.TextColor );
		
		// collapse button
		if (item.CollapseId != 0)
		{
			const Rect_s node_rect = InnerToCollapse( inner_rect );

			if (button::Draw( item.CollapseId, InnerToCollapse( inner_rect ), v.Button, item.Collapsed ? L"+": L"-", -1 ))
				result.CollapseClicked = true;
		}

		// check button
		const wchar_t* txt = item.Checked ? L"x" /*L"\u2714"*/ : L"";
		if (button::Draw( item.CheckId, InnerToCheck( inner_rect ), v.Button, txt, -1 ))
			result.CheckClicked = true;

		return result;
	}

} } }
