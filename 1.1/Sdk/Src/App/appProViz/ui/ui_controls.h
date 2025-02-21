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
namespace nemesis { namespace gui 
{
	struct Layout
	{
		Rect_s Bounds;
		Vec2_s Spacing;
		bool Horizontal;

		Vec2_s CaretInfo_s;
	};

	namespace layout
	{
		struct Direction
		{
			enum Enum { Horizontal, Vertical };
		};

		struct Item
		{
			Rect_s Bounds;
			bool Visible;
		};

		void Begin( Layout& l, const Rect_s& bounds, const Vec2_s& spacing, Direction::Enum direction );
		void Begin( Layout& l, const Rect_s& bounds, const Vec2_s& spacing, Direction::Enum direction, const Vec2_s& offset );
		Item Append( Layout& l, float w, float h );
		Item Remainder( Layout& l );
	}

} }

//======================================================================================
namespace nemesis { namespace gui {	namespace label
{
	struct Visual_s
	{
		Font_t Font;
		Color_c TextColor;
	};

	void Draw( const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len );

} } }

//======================================================================================
namespace nemesis { namespace gui {	namespace popup
{
	struct Visual_s
	{
		label::Visual_s Label;
		Color_c BkgndColor;
	};

	void Draw( const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace button
{
	struct Action
	{
		enum Enum { None, Clicked };
	};

	struct State
	{
		enum Enum { Unchecked, Checked };
	};

	struct Visual_s
	{
		label::Visual_s Label;
		Color_c BkgndColor[2];
		Color_c CheckedColor;
	};

	Action::Enum Draw( Id_t id, const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len );
	Action::Enum Draw( Id_t id, const Rect_s& r, const Visual_s& v, const wchar_t* text, int text_len, State::Enum state );
	Action::Enum Input( Id_t id, const Rect_s& r );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace trackbar
{
	struct Action
	{
		enum Enum { None, Clicked, Dragged };
	};

	struct Result
	{
		Action::Enum Action;
		float TrackPos;
	};

	struct Item
	{
		float Pos;
		float Min;
		float Max;
	};

	struct Layout
	{
		enum Direction
		{
			Horizontal
		,	Vertical
		};
	};

	struct Visual_s
	{
		float			  ThumbSize;
		Layout::Direction Direction;
	};

	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item& item );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace scrollbar
{
	struct Action
	{
		enum Enum { None, Clicked, Dragged };
	};

	struct Result
	{
		Action::Enum Action;
		float TrackPos;
	};

	struct Layout
	{
		enum Direction
		{
			Horizontal
		,	Vertical
		};
	};

	struct Visual_s
	{
		Layout::Direction Direction;
	};

	struct Item
	{
		float Pos;
		float Page;
		float Min;
		float Max;
	};

	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item& item );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace tab
{
	struct Action
	{
		enum Enum { None, Clicked };
	};

	struct Result
	{
		Action::Enum Action;
		int Item;
	};

	struct Item
	{
		const wchar_t* Text;
	};

	struct Visual_s
	{
		label::Visual_s Label;
	};

	Rect_s CtrlToPage( const Rect_s& r );
	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const Item* items, int count, int active_item );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace toolbar
{
	struct Action
	{
		enum Enum { None, Clicked };
	};

	struct Result
	{
		Action::Enum Action;
		int Item;
	};

	struct Item
	{
		enum Enum { Label, Button, Delimiter };

		Id_t ID;
		Enum Type;
		const wchar_t* Text;
		bool Checked;
	};

	struct Visual_s
	{
		Color_c			Color;
		label::Visual_s	Label;
		button::Visual_s	Button;
	};

	Result Draw( const Rect_s& r, const Visual_s& v, const Item* items, int count, Rect_s* remainder );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace optionbar
{
	struct Item
	{
		enum Enum { Delimiter, Bool, Byte, Float };
		union Value
		{
			bool	Bool;
			uint8_t	Byte;
			float	Float;
		};

		Id_t Id;
		const wchar_t* Text;
		Enum Type;
		Value Pos;
		Value Min;
		Value Max;

		Item()
			: Id()
			, Text( nullptr )
			, Type( Item::Delimiter )
		{
			Pos.Byte = 0;
			Min.Byte = 0;
			Max.Byte = 0;
		}

		Item( Id_t id, const wchar_t* text, bool value )
			: Id( id )
			, Text( text )
			, Type( Item::Bool )
		{
			Pos.Bool = value;
			Min.Bool = false;
			Max.Bool = true;
		}

		Item( Id_t id, const wchar_t* text, uint8_t value, uint8_t min, uint8_t max )
			: Id( id )
			, Text( text )
			, Type( Item::Byte )
		{
			Pos.Byte = value;
			Min.Byte = min;
			Max.Byte = max;
		}

		Item( Id_t id, const wchar_t* text, float value, float min, float max )
			: Id( id )
			, Text( text )
			, Type( Item::Float )
		{
			Pos.Float = value;
			Min.Float = min;
			Max.Float = max;
		}
	};

	struct Visual_s
	{
		label::Visual_s Label;
		button::Visual_s Button;
	};

	void Draw( const Rect_s& r, const Visual_s& v, Item* items, int count );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace collapsebar
{
	struct Action
	{
		enum Enum { None, Clicked };
	};

	struct Result
	{
		Action::Enum Action;
	};

	struct Alignment
	{
		enum Enum
		{ Left
		, Right
		};
	};

	struct Item
	{
		Id_t ID;
		const wchar_t* Text;
		bool Collapsed;
		Alignment::Enum Side;
	};

	struct Visual_s
	{
		label::Visual_s Label;
		button::Visual_s Button;
	};

	Result Draw( const Rect_s& r, const Visual_s& v, const Item& item );

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace tree_ctrl
{
	static const float ITEM_MARGIN		= 2.0f;
	static const float BUTTON_SIZE		= 12.0f;
	static const float BUTTON_SPACING	= 2.0f;
	static const float LABEL_MARGIN		= 4.0f;
	static const float DEPTH_INDENT		= 18.0f;

	struct Visual_s
	{
		label::Visual_s Label;
		button::Visual_s Button;
	};

	struct Item
	{
		const wchar_t* Text;
		int TextLength;
		Id_t CollapseId;
		Id_t CheckId;
		int Indent;
		bool Collapsed;
		bool Checked;
	};

	struct ItemResult
	{
		bool CheckClicked;
		bool CollapseClicked;
	};

	float CalculateItemHeight( const Visual_s& v );
	ItemResult DrawItem( const Rect_s& r, const Visual_s& v, const Item& item );

} } }
