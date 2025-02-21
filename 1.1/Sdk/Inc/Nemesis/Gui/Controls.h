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
#include "Types.h"

//======================================================================================
namespace nemesis { namespace gui
{
	/// Draw

	struct Arrow
	{
		enum Dir
		{ Left
		, Up
		, Right
		, Down
		};
	};

	void NE_API Graphics_DrawArrow( Graphics_t g, const Rect_s& r, Arrow::Dir dir, uint32_t argb );

	void NE_API Graphics_DrawBox( Graphics_t g, Theme_t theme, const Rect_s& r, Visual::Enum v, CtrlState::Enum s );
	void NE_API Graphics_DrawRadio( Graphics_t g, Theme_t theme, const Rect_s& r, Visual::Enum v, CtrlState::Enum s );
	void NE_API Graphics_DrawString( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s );
	void NE_API Graphics_DrawBox( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m );
	void NE_API Graphics_DrawRadio( Graphics_t g, Theme_t theme, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m );

	void NE_API Context_DrawArrow( Context_t dc, const Rect_s& r, Arrow::Dir dir, uint32_t argb );
	void NE_API Context_DrawArrow( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s, Arrow::Dir dir );
	void NE_API Context_DrawRadio( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Ctrl

	CtrlState::Enum NE_API Ctrl_GetState( Context_t dc, Id_t id );
	CtrlState::Enum NE_API Ctrl_GetState( Context_t dc, Id_t id, int index, int hot, int pushed );

	bool NE_API Ctrl_IsAnyDragging( Context_t dc, const Id_t* id, int count );

	void NE_API Ctrl_DrawBox( Context_t dc, const Rect_s& r, Visual::Enum v, CtrlState::Enum s );
	void NE_API Ctrl_DrawText( Context_t dc, const Rect_s& r, const Text_s& t, TextFormat::Mask_t f, Visual::Enum v, CtrlState::Enum s, Metric::Enum m );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	// Alignment

	struct Orientation
	{ 
		enum Enum
		{ Horz
		, Vert
		};
	};

	struct Alignment
	{
		enum Horz
		{ Left
		, Center
		, Right
		};

		enum Vert
		{ Top
		, Middle
		, Bottom
		};
	};

	TextFormat::Mask_t NE_API Alignment_ToTextFormat( Alignment::Horz horz );
	TextFormat::Mask_t NE_API Alignment_ToTextFormat( Alignment::Vert vert );
	TextFormat::Mask_t NE_API Alignment_ToTextFormat( Alignment::Horz horz, Alignment::Vert vert );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	// Sort Order

	struct SortOrder
	{
		enum Enum
		{ None
		, Ascending
		, Descending
		};
	};

	SortOrder::Enum NE_API SortOrder_Next( SortOrder::Enum order );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextList 

	float NE_API TextList_CalcMaxWidth( Context_t dc, const TextList_s& list );
	float NE_API TextList_CalcMaxWidth( Context_t dc, const cstr_t* items, int count );
	float NE_API TextList_CalcMaxWidth( Context_t dc, const wcstr_t* items, int count );
	float NE_API TextList_CalcMaxWidth( Context_t dc, Font_t font, const TextList_s& list );
	float NE_API TextList_CalcMaxWidth( Context_t dc, Font_t font, const cstr_t* items, int count );
	float NE_API TextList_CalcMaxWidth( Context_t dc, Font_t font, const wcstr_t* items, int count );

	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, const TextList_s& list );
	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, const cstr_t* items, int count );
	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, const wcstr_t* items, int count );
	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, Font_t font, const TextList_s& list );
	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, Font_t font, const cstr_t* items, int count );
	Vec2_s NE_API TextList_CalcMaxSize( Context_t dc, Font_t font, const wcstr_t* items, int count );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Label

	Vec2_s	NE_API Label_Do  ( Context_t dc, const Vec2_s& p, const Text_s& t );
	Vec2_s	NE_API Label_FmtV( Context_t dc, const Vec2_s& p, cstr_t fmt, va_list args );
	Vec2_s	NE_API Label_FmtV( Context_t dc, const Vec2_s& p, wcstr_t fmt, va_list args );
	Vec2_s	NE_API Label_Fmt ( Context_t dc, const Vec2_s& p, cstr_t fmt, ... );
	Vec2_s	NE_API Label_Fmt ( Context_t dc, const Vec2_s& p, wcstr_t fmt, ... );

	void	NE_API Label_Do( Context_t dc, const Rect_s& r, const Text_s& t );
	void	NE_API Label_Do( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, const Text_s& t );
	void	NE_API Label_Do( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, uint32_t color, const Text_s& t );

	void	NE_API Label_FmtV( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, cstr_t fmt, va_list args );
	void	NE_API Label_FmtV( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, wcstr_t fmt, va_list args );
	void	NE_API Label_Fmt ( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, cstr_t fmt, ... );
	void	NE_API Label_Fmt ( Context_t dc, const Rect_s& r, TextFormat::Mask_t s, wcstr_t fmt, ... );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Button

	struct ButtonStyle
	{
		enum Flag
		{ None		= 0
		, Disabled	= 1<<0
		};
		typedef uint32_t Mask_t;
	};

	struct Button_s
	{
		Id_t				Id;
		Rect_s				Rect;
		Text_s				Text;
		ButtonStyle::Mask_t Style;
	};

	struct ButtonClick
	{
		enum Enum
		{ Press
		, Release
		, DoubleClick
		};
	};

	struct ButtonAction
	{
		enum Enum
		{ None
		, Clicked
		};
	};

	float				NE_API Button_CalcHeight( Context_t dc );
	Vec2_s				NE_API Button_CalcSize	( Context_t dc, const Text_s& text );
	bool				NE_API Button_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, CtrlState::Enum s );
	bool				NE_API Button_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, ButtonStyle::Mask_t s );
	bool				NE_API Button_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode, MouseButton::Enum button );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ButtonClick::Enum mode, int index, int& hot, int& pushed );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ButtonStyle::Mask_t s );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, Id_t id, const Rect_s& r, int index, int& hot, int& pushed );
	ButtonAction::Enum	NE_API Button_Do		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text );
	ButtonAction::Enum	NE_API Button_Do		( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text );
	ButtonAction::Enum	NE_API Button_Do		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, ButtonStyle::Mask_t s );
	ButtonAction::Enum	NE_API Button_Do		( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, ButtonStyle::Mask_t s );

	Button_s			NE_API Button_Instance	( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, ButtonStyle::Mask_t s );
	Button_s			NE_API Button_Instance	( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text );
	bool				NE_API Button_Draw		( Context_t dc, const Button_s& instance );
	ButtonAction::Enum	NE_API Button_Mouse		( Context_t dc, const Button_s& instance );
	ButtonAction::Enum	NE_API Button_Do		( Context_t dc, const Button_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// CheckBox

	struct CheckBox_s
	{
		Id_t	Id;
		Rect_s	Rect;
		Text_s	Text;
		bool	Checked;
	};

	Vec2_s				NE_API CheckBox_CalcSize( Context_t dc, const Text_s& text );
	bool				NE_API CheckBox_Draw	( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool checked );
	ButtonAction::Enum	NE_API CheckBox_Mouse	( Context_t dc, Id_t id, const Rect_s& r, bool& checked );
	ButtonAction::Enum	NE_API CheckBox_Do		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool& checked );
	ButtonAction::Enum	NE_API CheckBox_DoButton( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool& checked );
	Vec2_s				NE_API CheckBox_Do		( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool& checked );
	Vec2_s				NE_API CheckBox_DoButton( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool& checked );

	CheckBox_s			NE_API CheckBox_Instance( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, bool checked );
	bool				NE_API CheckBox_Draw	( Context_t dc, const CheckBox_s& instance );
	ButtonAction::Enum	NE_API CheckBox_Mouse	( Context_t dc, CheckBox_s& instance );
	ButtonAction::Enum	NE_API CheckBox_Do		( Context_t dc, CheckBox_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// RadioButton

	struct RadioButton_s
	{
		Id_t	Id;
		Rect_s	Rect;
		Text_s	Text;
		int		Index;
	};

	Vec2_s				NE_API RadioButton_CalcSize	( Context_t dc, const Text_s& text );
	bool				NE_API RadioButton_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, bool checked );
	ButtonAction::Enum	NE_API RadioButton_Mouse	( Context_t dc, Id_t id, const Rect_s& r, int idx, int& sel );
	ButtonAction::Enum	NE_API RadioButton_Do		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, int idx, int& sel );
	ButtonAction::Enum	NE_API RadioButton_DoButton	( Context_t dc, Id_t id, const Rect_s& r, const Text_s& text, int idx, int& sel );
	Vec2_s				NE_API RadioButton_Do		( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx, int& sel );
	Vec2_s				NE_API RadioButton_DoButton	( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx, int& sel );
							   
	RadioButton_s		NE_API RadioButton_Instance	( Context_t dc, Id_t id, const Vec2_s& p, const Text_s& text, int idx );
	bool				NE_API RadioButton_Draw		( Context_t dc, const RadioButton_s& instance, int sel );
	ButtonAction::Enum	NE_API RadioButton_Mouse	( Context_t dc, RadioButton_s& instance, int& sel );
	ButtonAction::Enum	NE_API RadioButton_Do		( Context_t dc, RadioButton_s& instance, int& sel );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextEditBuffer

	struct TextEditBuffer_s
	{
		wchar_t* Text;
		int Capacity;
		int Length;
		int CaretPos;
		int SelectPos;
	};

	void NE_API TextEditBuffer_Initialize		( TextEditBuffer_s& buffer, wchar_t* text, int num_chars, int caret, int sel );
	bool NE_API TextEditBuffer_HasSelection		( const TextEditBuffer_s& buffer );
	void NE_API TextEditBuffer_SetCaret			( TextEditBuffer_s& buffer, int pos, bool select );
	void NE_API TextEditBuffer_DeleteSelection	( TextEditBuffer_s& buffer );
	void NE_API TextEditBuffer_Delete			( TextEditBuffer_s& buffer );
	void NE_API TextEditBuffer_Remove			( TextEditBuffer_s& buffer );
	void NE_API TextEditBuffer_Insert			( TextEditBuffer_s& buffer, const wchar_t* t, int count );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// TextEdit

	struct TextEditStyle
	{
		enum Flag
		{ None		= 0
		, ReadOnly	= 1<<0
		, NoCaret	= 1<<1
		};
		typedef uint32_t Mask_t;
	};

	struct TextEditData_s
	{
		wchar_t* Text;
		int NumChars;
	};

	struct TextEditState_s
	{
		int Caret;
		int Select;
		float Scroll;
	};

	struct TextEdit_s
	{
		Id_t					Id;
		Rect_s					Rect;
		TextEditData_s			Data;
		TextEditState_s			State;
		TextEditStyle::Mask_t	Style;
	};

	float	NE_API TextEdit_CalcHeight	( Context_t dc );
	float	NE_API TextEdit_CalcWidth	( Context_t dc, float w );
	Vec2_s	NE_API TextEdit_CalcSize	( Context_t dc, float w );
	Rect_s	NE_API TextEdit_CalcTextRect( Context_t dc, const Rect_s& r );
	bool	NE_API TextEdit_Draw		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, CtrlState::Enum ctrl_state, const Text_s& text, float scroll, int caret, int select );
	bool	NE_API TextEdit_Draw		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, const Text_s& text, float scroll, int caret, int select );
	bool	NE_API TextEdit_Draw		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, cstr_t text, int num_chars, float scroll, int caret, int select );
	bool	NE_API TextEdit_Draw		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wcstr_t text, int num_chars, float scroll, int caret, int select );
	void	NE_API TextEdit_Mouse		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select );
	void	NE_API TextEdit_Keyboard	( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select );
	void	NE_API TextEdit_Input		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select );
	void	NE_API TextEdit_Do			( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select );
	Vec2_s	NE_API TextEdit_Do			( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, wchar_t* text, int num_chars, float& scroll, int& caret, int& select );
	void	NE_API TextEdit_Do			( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, TextEditData_s& data, TextEditState_s& state );
	Vec2_s	NE_API TextEdit_Do			( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, TextEditData_s& data, TextEditState_s& state );
	void	NE_API TextEdit_Do			( Context_t dc, TextEdit_s& instance );

	void	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, int&   v );
	void	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, float& v );
	void	NE_API TextEdit_DoFloat		( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, float& v );
	void	NE_API TextEdit_DoString	( Context_t dc, Id_t id, const Rect_s& r, TextEditStyle::Mask_t style, wstr_t v, int l );
	Vec2_s	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, int&   v );
	Vec2_s	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, float& v );
	Vec2_s	NE_API TextEdit_DoFloat		( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, float& v );
	Vec2_s	NE_API TextEdit_DoString	( Context_t dc, Id_t id, const Vec2_s& p, float w, TextEditStyle::Mask_t style, wstr_t v, int l );

	void	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Rect_s& r, int&   v );
	void	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Rect_s& r, float& v );
	void	NE_API TextEdit_DoFloat		( Context_t dc, Id_t id, const Rect_s& r, float& v );
	void	NE_API TextEdit_DoString	( Context_t dc, Id_t id, const Rect_s& r, wstr_t v, int l );
	Vec2_s	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Vec2_s& p, float w, int&   v );
	Vec2_s	NE_API TextEdit_DoInt		( Context_t dc, Id_t id, const Vec2_s& p, float w, float& v );
	Vec2_s	NE_API TextEdit_DoFloat		( Context_t dc, Id_t id, const Vec2_s& p, float w, float& v );
	Vec2_s	NE_API TextEdit_DoString	( Context_t dc, Id_t id, const Vec2_s& p, float w, wstr_t v, int l );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ProgressBar

	struct ProgressBar_s
	{
		Rect_s	Rect;
		float	Min;
		float	Max;
		float	Pos;
	};

	Vec2_s			NE_API ProgressBar_CalcSize		( Context_t dc, float w );
	Rect_s			NE_API ProgressBar_CalcBarRect	( Context_t dc, const Rect_s& r, float min, float max, float pos );
	bool			NE_API ProgressBar_Draw			( Context_t dc, const Rect_s& r, float min, float max, float pos );
	void			NE_API ProgressBar_Do			( Context_t dc, const Rect_s& r, float min, float max, float pos );
	Vec2_s			NE_API ProgressBar_Do			( Context_t dc, const Vec2_s& p, float w, float min, float max, float pos );

	ProgressBar_s	NE_API ProgressBar_Instance		( Context_t dc, const Vec2_s& p, float w, float min, float max, float pos );
	bool			NE_API ProgressBar_Draw			( Context_t dc, const ProgressBar_s& instance );
	void			NE_API ProgressBar_Do			( Context_t dc, const ProgressBar_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Slider

	struct Slider_s
	{
		Id_t	Id;
		Rect_s	Rect;
		float	Min;
		float	Max;
		float	Pos;
	};

	Vec2_s		NE_API Slider_CalcSize		( Context_t dc, Orientation::Enum dir, float track );
	Rect_s		NE_API Slider_CalcTrackRect	( Context_t dc, Orientation::Enum dir, const Rect_s& r );
	Rect_s		NE_API Slider_CalcThumbRect	( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float pos );
	float		NE_API Slider_CoordToPos	( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, const Vec2_s& coord );

	bool		NE_API Slider_Draw			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float pos );
	void		NE_API Slider_Mouse			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float& pos );
	void		NE_API Slider_Do			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float& pos );
	Vec2_s		NE_API Slider_Do			( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos );
	void		NE_API Slider_DoH			( Context_t dc, Id_t id, const Rect_s& r, int min, int max, int& pos );
	void		NE_API Slider_DoV			( Context_t dc, Id_t id, const Rect_s& r, int min, int max, int& pos );
	void		NE_API Slider_DoH			( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float& pos );
	void		NE_API Slider_DoV			( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float& pos );
	Vec2_s		NE_API Slider_DoH			( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos );
	Vec2_s		NE_API Slider_DoV			( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float& pos );

	Slider_s	NE_API Slider_Instance		( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float pos );
	bool		NE_API Slider_Draw			( Context_t dc, Orientation::Enum dir, const Slider_s& instance );
	void		NE_API Slider_Mouse			( Context_t dc, Orientation::Enum dir, Slider_s& instance );
	void		NE_API Slider_Do			( Context_t dc, Orientation::Enum dir, Slider_s& instance );
	void		NE_API Slider_DoH			( Context_t dc, Slider_s& instance );
	void		NE_API Slider_DoV			( Context_t dc, Slider_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollRange

	void NE_API ScrollRange_Normalize( float& min, float& max, float& line, float& page, float& pos );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollBar

	struct ScrollBar_s
	{
		Id_t	Id;
		Rect_s	Rect;
		float	Min;
		float	Max;
		float	Line;
		float	Page;
		float	Pos;
	};

	Id_t		NE_API ScrollBar_GetArrowId		( Id_t id, int index );

	Vec2_s		NE_API ScrollBar_CalcSize		( Context_t dc, Orientation::Enum dir, float track );
	Rect_s		NE_API ScrollBar_CalcTrackRect	( Context_t dc, Orientation::Enum dir, const Rect_s& r );
	Rect_s		NE_API ScrollBar_CalcArrowRect	( Context_t dc, Orientation::Enum dir, const Rect_s& r, int index );
	Rect_s		NE_API ScrollBar_CalcThumbRect	( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float page, float pos );
	float		NE_API ScrollBar_CoordToPos		( Context_t dc, Orientation::Enum dir, const Rect_s& r, float min, float max, float page, const Vec2_s& coord );

	bool		NE_API ScrollBar_Draw			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float page, float pos );
	void		NE_API ScrollBar_Mouse			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	void		NE_API ScrollBar_Keyboard		( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	void		NE_API ScrollBar_Input			( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	void		NE_API ScrollBar_Do				( Context_t dc, Orientation::Enum dir, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	Vec2_s		NE_API ScrollBar_Do				( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos );
	void		NE_API ScrollBar_DoH			( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	void		NE_API ScrollBar_DoV			( Context_t dc, Id_t id, const Rect_s& r, float min, float max, float line, float page, float& pos );
	Vec2_s		NE_API ScrollBar_DoH			( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos );
	Vec2_s		NE_API ScrollBar_DoV			( Context_t dc, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float& pos );

	ScrollBar_s	NE_API ScrollBar_Instance		( Context_t dc, Orientation::Enum dir, Id_t id, const Vec2_s& p, float track, float min, float max, float line, float page, float pos );
	bool		NE_API ScrollBar_Draw			( Context_t dc, Orientation::Enum dir, const ScrollBar_s& instance );
	void		NE_API ScrollBar_Mouse			( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance );
	void		NE_API ScrollBar_Keyboard		( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance );
	void		NE_API ScrollBar_Input			( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance );
	void		NE_API ScrollBar_Do				( Context_t dc, Orientation::Enum dir, ScrollBar_s& instance );
	void		NE_API ScrollBar_DoH			( Context_t dc, ScrollBar_s& instance );
	void		NE_API ScrollBar_DoV			( Context_t dc, ScrollBar_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Expander

	struct Expander_s
	{
		Id_t	Id;
		Rect_s	Rect;
		Text_s	Text;
		bool	Expanded;
	};

	float	NE_API Expander_CalcHeight	( Context_t dc );
	bool	NE_API Expander_Draw		( Context_t dc, const Expander_s& instance );
	void	NE_API Expander_Mouse		( Context_t dc, Expander_s& instance );
	void	NE_API Expander_Do			( Context_t dc, Expander_s& instance );

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// Caption

	float NE_API Caption_CalcHeight	( Context_t dc );
	bool  NE_API Caption_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, CtrlState::Enum s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// SizeGrip

	struct SizeGripAction
	{
		enum Enum
		{ None
		, Pushed
		, Dragged
		};
	};

	SizeGripAction::Enum NE_API SizeGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, Vec2_s& size );
	SizeGripAction::Enum NE_API SizeGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, MouseButton::Enum button, Vec2_s& size );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Window

	struct Window_s
	{
		Id_t	Id;
		Rect_s	Rect;
		Text_s	Text;
		Vec2_s	MinSize;
		bool	Collapsed;
	};

	Vec2_s	NE_API Window_CalcSize	( Context_t dc, const Vec2_s& client );
	void	NE_API Window_Draw		( Context_t dc, Window_s& instance );
	void	NE_API Window_Mouse		( Context_t dc, Window_s& instance );
	void	NE_API Window_Begin		( Context_t dc, Window_s& instance );
	void	NE_API Window_End		( Context_t dc );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollPanel

	struct ScrollState_s
	{
		Vec2_s Size;
		Vec2_s Offset;
	};

	void NE_API ScrollPanel_Begin( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset );
	void NE_API ScrollPanel_End  ( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& step, Vec2_s& offset );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollView

	void NE_API ScrollView_Begin( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset );
	void NE_API ScrollView_End	( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& step, Vec2_s& offset );

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// CollpaseView

	void NE_API CollapseView_Begin( Context_t dc, Id_t id, float y, const Text_s& t, bool& expanded );
	void NE_API CollapseView_End  ( Context_t dc );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// CollpaseList

	struct CollapseItem_s
	{
		Vec2_s (NE_CALLBK *Do)( Context_t dc );
		Text_s Text;
		bool Expanded;
	};

	Vec2_s NE_API CollapseList_Do( Context_t dc, Id_t id, CollapseItem_s* item, int count, Vec2_s* item_size );
	Vec2_s NE_API CollapseList_Do( Context_t dc, Id_t id, CollapseItem_s* item, int count );

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// ListGrid

	float NE_API ListGrid_CalcRowY	( float row_h, float view_y, int row );
	int	  NE_API ListGrid_HitTestY	( float row_h, const Rect_s& r, float view_y, float y );
	void  NE_API ListGrid_CalcRangeY( float row_h, const Rect_s& r, float view_y, int& first, int& count, float& offset_y );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ListHeader

	struct ListHeaderItem_s
	{
		Text_s			Text;
		float			Width;
		Alignment::Horz Alignment;
		SortOrder::Enum	SortOrder;
	};

	struct ListHeaderState_s
	{
		int		Hot;
		int		Pushed;
		float	View;
	};

	Rect_s	NE_API ListHeaderItem_CalcIconRect	 ( Context_t dc, const Rect_s& r );
	Rect_s	NE_API ListHeaderItem_CalcLabelRect	 ( Context_t dc, const Rect_s& r );
	Rect_s	NE_API ListHeaderItem_CalcDividerRect( Context_t dc, const Rect_s& r );

	bool	NE_API ListHeaderItem_Draw	( Context_t dc, Id_t id, const Rect_s& r, const ListHeaderItem_s& item, int index, int hot, int pushed );
	void	NE_API ListHeaderItem_Mouse	( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s& item, int index, int& hot, int& pushed );
	void	NE_API ListHeaderItem_Do	( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s& item, int index, int& hot, int& pushed );

	float	NE_API ListHeader_CalcWidth	( const ListHeaderItem_s* item, int count );
	Vec2_s	NE_API ListHeader_CalcSize	( Context_t dc, float w );
	void	NE_API ListHeader_Do		( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s* item, int count, ListHeaderState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ListColumn

	struct ListColumnCallback_s
	{
		Text_s (NE_CALLBK *DoItem)( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state );
		void* Context;
	};

	struct ListColumn_s
	{
		cstr_t*				 Utf8;
		wcstr_t*			 Utf16;
		ListColumnCallback_s Callback;
		Alignment::Horz		 Alignment;
	};

	struct ListColumnState_s
	{
		int		Hot;
		int		Sel;
		Vec2_s	View;
	};

	struct ListColumnEvent_s
	{
		bool Activated;
	};

	float				NE_API ListColumn_CalcItemHeight( Context_t dc );
	Rect_s				NE_API ListColumn_CalcItemRect	( Context_t dc, const Rect_s& r, const Vec2_s& view, int index );
	int					NE_API ListColumn_HitTest		( Context_t dc, const Rect_s& r, const Vec2_s& view, int num_items, const Vec2_s& coord );
	void				NE_API ListColumn_DrawSelection	( Context_t dc, Id_t id, const Rect_s& r, const ListColumnState_s& s );
	bool				NE_API ListColumn_DrawItems		( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, const ListColumnState_s& s );
	bool				NE_API ListColumn_Draw			( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, const ListColumnState_s& s );
	ListColumnEvent_s	NE_API ListColumn_Mouse			( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s );
	ListColumnEvent_s	NE_API ListColumn_Keyboard		( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s );
	ListColumnEvent_s	NE_API ListColumn_Input			( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s );
	ListColumnEvent_s	NE_API ListColumn_Do			( Context_t dc, Id_t id, const Rect_s& r, const ListColumn_s& item, int num_items, int col, ListColumnState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// List

	struct ListStyle
	{
		enum Flag
		{ None		= 0
		, NoHeader	= 1<<0
		, HorzLines	= 1<<1
		, VertLines	= 1<<2
		};
		typedef uint32_t Mask_t;
	};

	struct ListData_s
	{
		ListHeaderItem_s*   Header;
		const ListColumn_s* Column;
		int					NumColumns;
		int					NumRows;
	};

	struct ListState_s
	{
		int		HotHdr;
		int		HotCol;
		int		HotRow;
		int		SelHdr;
		int		SelCol;
		int		SelRow;
		Vec2_s	View;
	};

	struct ListEvent_s
	{
		bool Activated;
	};

	struct List_s
	{
		Id_t			  Id;
		Rect_s			  Rect;
		ListData_s		  Data;
		ListState_s		  State;
		ListStyle::Mask_t Style;
	};

	ListEvent_s NE_API List_Do( Context_t dc, Id_t id, const Rect_s& r, ListHeaderItem_s* header, const ListColumn_s* column, int num_columns, int num_rows, ListStyle::Mask_t style, ListState_s& s );
	ListEvent_s NE_API List_Do( Context_t dc, List_s& instance );

	void NE_API List_ShowRow( Context_t dc, List_s& list, int index );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Tree

	struct TreeStyle
	{
		enum Flag
		{ None		= 0
		, HasIcon	= 1<<0
		, HasState	= 1<<1
		};
		typedef uint32_t Mask_t;
	};

	struct TreeItem_s
	{
		Text_s	Text;
		bool	HasChildren;
		bool	Expanded;
	};

	struct TreeItemState_s
	{
		bool Hot;
		bool Selected;
		bool Activated;
	};

	float	NE_API Tree_CalcItemHeight	( Context_t dc );
	float	NE_API Tree_CalcItemWidth	( Context_t dc, const Text_s& t, int level, TreeStyle::Mask_t style );
	Rect_s	NE_API Tree_CalcLevelRect	( Context_t dc, const Rect_s& r, int level );
	Rect_s	NE_API Tree_CalcInnerRect	( Context_t dc, const Rect_s& r, int level );
	Rect_s	NE_API Tree_CalcExpanderRect( Context_t dc, const Rect_s& r, int level );
	Rect_s	NE_API Tree_CalcStateRect	( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style );
	Rect_s	NE_API Tree_CalcIconRect	( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style );
	Rect_s	NE_API Tree_CalcLabelRect	( Context_t dc, const Rect_s& r, int level, TreeStyle::Mask_t style );
	void	NE_API Tree_Begin			( Context_t dc, Id_t id, const Rect_s& r );
	bool	NE_API Tree_Item_Draw		( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, const TreeItemState_s& s );
	void	NE_API Tree_Item_Mouse		( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s );
	void	NE_API Tree_Item_Do			( Context_t dc, Id_t id, const Rect_s& r, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s );
	Vec2_s	NE_API Tree_Item_Do			( Context_t dc, Id_t id, const Vec2_s& p, float w, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s );
	void	NE_API Tree_End				( Context_t dc );

	/// Virtual Tree

	typedef void* TreeNode_t;

	struct TreeCallback_s
	{
		TreeItem_s (NE_CALLBK *GetTreeItem)	  ( void* context, TreeNode_t node );
		TreeNode_t (NE_CALLBK *GetFirstChild) ( void* context, TreeNode_t parent );
		TreeNode_t (NE_CALLBK *GetNextSibling)( void* context, TreeNode_t parent, TreeNode_t child );
		void* Context;
	};

	struct TreeData_s
	{
		TreeCallback_s	Callback;
		TreeNode_t		Root;
		TreeNode_t		None;
	};

	struct TreeState_s
	{
		TreeNode_t Hot;
		TreeNode_t Selected;
		TreeNode_t Activated;
	};

	void NE_API Tree_DoItem		( Context_t dc, Id_t tree_id, Vec2_s& item_pos, float w, Vec2_s& tree_size, const TreeItem_s& item, int level, TreeStyle::Mask_t style, TreeItemState_s& s );
	void NE_API Tree_DoChildren	( Context_t dc, Id_t id, Vec2_s& pos, float w, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeNode_t node, int level, TreeState_s& s );
	void NE_API Tree_DoSubTree	( Context_t dc, Id_t id, Vec2_s& pos, float w, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeNode_t node, int level, TreeState_s& s );
	void NE_API Tree_Do			( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, TreeState_s& s );

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// TreeView

	struct TreeViewState_s
	{
		TreeState_s   Tree;
		ScrollState_s View;
	};

	void NE_API TreeView_Begin	( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, const Vec2_s& offset );
	void NE_API TreeView_End	( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& size, Vec2_s& offset );
	void NE_API TreeView_Do		( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, Vec2_s& size, Vec2_s& offset, TreeState_s& s );
	void NE_API TreeView_Do		( Context_t dc, Id_t id, const Rect_s& r, const TreeData_s& data, TreeStyle::Mask_t style, TreeViewState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Tab

	struct TabStyle
	{
		enum Flag
		{ None		= 0
		, Frame		= 1<<1
		, Enlarge	= 1<<2
		};
		typedef uint32_t Mask_t;
	};

	Vec2_s NE_API TabItem_CalcIndent	( Context_t dc, TabStyle::Mask_t style );
	Vec2_s NE_API TabItem_CalcMargin	( Context_t dc, TabStyle::Mask_t style );
	float  NE_API TabItem_CalcEnlarge	( Context_t dc, TabStyle::Mask_t style );
	float  NE_API TabItem_CalcHeight	( Context_t dc, TabStyle::Mask_t style );
	float  NE_API TabItem_CalcWidth		( Context_t dc, TabStyle::Mask_t style, const Text_s& t );
	void   NE_API TabItem_CalcWidths	( Context_t dc, const Rect_s& r, TabStyle::Mask_t style, const Text_s* text, float* width, int num_items );
	Rect_s NE_API TabItem_CalcLabelRect	( Context_t dc, const Rect_s& r, TabStyle::Mask_t style );

	bool NE_API TabItem_Draw	( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t style, const Text_s& text, bool selected );
	void NE_API TabItem_Mouse	( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t style, bool& selected );
	void NE_API TabItem_Do		( Context_t dc, Id_t id, const Rect_s& r, TabStyle::Mask_t style, const Text_s& text, bool& selected );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Menu

	struct MenuItemStyle
	{
		enum Flag
		{ None		= 0
		, NoBkgnd	= 1<<0
		};
		typedef uint32_t Mask_t;
	};

	struct MenuItem_s;

	struct MenuItemList_s
	{
		const MenuItem_s* Item;
		int NumItems;
	};

	struct MenuItem_s
	{
		Text_s			Text;
		Text_s			HotKey;
		MenuItemList_s	Child;
		void*			UserData;
	};

	struct MenuItemState_s
	{
		bool Hot;
		bool Activated;
	};

	struct MenuState_s
	{
		enum { MAX_DEPTH = 16 };
		const MenuItem_s* Hot;
		const MenuItem_s* Activated;
		const MenuItem_s* Expanded[ MAX_DEPTH ];
	};

	float	NE_API Menu_PopUp_CalcItemHeight	( Context_t dc, const Text_s& text );
	float	NE_API Menu_PopUp_CalcItemWidth		( Context_t dc, const Text_s& text );
	Vec2_s	NE_API Menu_PopUp_CalcItemSize		( Context_t dc, const Text_s& text );
	Vec2_s	NE_API Menu_PopUp_CalcMenuSize		( Context_t dc, const MenuItemList_s& list );
	Rect_s	NE_API Menu_PopUp_CalcInnerRect		( Context_t dc, const Rect_s& r );
	Rect_s	NE_API Menu_PopUp_CalcIconRect		( Context_t dc, const Rect_s& r );
	Rect_s	NE_API Menu_PopUp_CalcLabelRect		( Context_t dc, const Rect_s& r );
	Rect_s	NE_API Menu_PopUp_CalcHotKeyRect	( Context_t dc, const Rect_s& r );
	Rect_s	NE_API Menu_PopUp_CalcExpanderRect	( Context_t dc, const Rect_s& r );
	Rect_s	NE_API Menu_PopUp_CalcSeparatorRect	( Context_t dc, const Rect_s& r );
	bool	NE_API Menu_PopUp_Item_Draw			( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, const MenuItemState_s& s );
	void	NE_API Menu_PopUp_Item_Mouse		( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s );
	void	NE_API Menu_PopUp_Item_Do			( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s );
	void	NE_API Menu_DoPopUp					( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, int level, MenuState_s& s );
	Vec2_s	NE_API Menu_DoPopUp					( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, int level, MenuState_s& s );
	void	NE_API Menu_DoPopUp					( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, MenuState_s& s, bool& done );
	Vec2_s	NE_API Menu_DoPopUp					( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, MenuState_s& s, bool& done );

	float	NE_API Menu_Main_CalcItemHeight		( Context_t dc );
	float	NE_API Menu_Main_CalcItemWidth		( Context_t dc, const Text_s& text );
	Vec2_s	NE_API Menu_Main_CalcItemSize		( Context_t dc, const Text_s& text );
	Vec2_s	NE_API Menu_Main_CalcMenuSize		( Context_t dc, const MenuItemList_s& list );
	Rect_s	NE_API Menu_Main_CalcLabelRect		( Context_t dc, const Rect_s& r );
	bool	NE_API Menu_Main_Item_Draw			( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, const MenuItemState_s& s );
	void	NE_API Menu_Main_Item_Mouse			( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s );
	void	NE_API Menu_Main_Item_Do			( Context_t dc, Id_t id, const Rect_s& r, MenuItemStyle::Mask_t style, const MenuItem_s& item, MenuItemState_s& s );
	void	NE_API Menu_DoMain					( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list, MenuState_s& s );
	Vec2_s	NE_API Menu_DoMain					( Context_t dc, Id_t id, const Vec2_s& p, const MenuItemList_s& list, MenuState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ContextMenu

	const MenuItem_s* NE_API ContextMenu_Do( Context_t dc, Id_t id, const Rect_s& r, const MenuItemList_s& list );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// DropDown

	Vec2_s NE_API DropDownItem_CalcSize		( Context_t dc, float text_w );
	Vec2_s NE_API DropDownItem_CalcSize		( Context_t dc, const Text_s& text );
	Rect_s NE_API DropDownItem_CalcLabelRect( Context_t dc, const Rect_s& r );

	bool NE_API DropDownItem_Mouse	( Context_t dc, Id_t id, const Rect_s& r );
	bool NE_API DropDownItem_Draw	( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t );
	bool NE_API DropDownItem_Do		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t );

	/// DropDownList

	Vec2_s NE_API DropDownList_CalcSize		( Context_t dc, float text_w, int n );
	Rect_s NE_API DropDownList_CalcInnerRect( Context_t dc, const Rect_s& r );

	bool NE_API DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel );
	bool NE_API DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel );
	bool NE_API DropDownList_Do( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& data, int& sel );

	bool NE_API DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel );
	bool NE_API DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel );
	bool NE_API DropDownList_DoModal( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& data, int& sel );

	/// DropDownBox

	Vec2_s NE_API DropDownBox_CalcSize( Context_t dc, float text_w );
	Vec2_s NE_API DropDownBox_CalcSize( Context_t dc, const Text_s& text );
	Rect_s NE_API DropDownBox_CalcListRect( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& t );

	bool NE_API DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const cstr_t* t, int n, int& sel, bool& expanded );
	bool NE_API DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const wcstr_t* t, int n, int& sel, bool& expanded );
	bool NE_API DropDownBox_Do( Context_t dc, Id_t id, const Rect_s& r, const TextList_s& data, int& sel, bool& expanded );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// MessageBox

	void NE_API MessageBox_Do( Context_t dc, Id_t id, const Text_s& caption, const Text_s& text, bool& done );
	
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScrollWindow

	Rect_s	NE_API ScrollWindow_Begin( Context_t dc, Window_s& wnd, ScrollInfo_s& scroll );
	void	NE_API ScrollWindow_End	 ( Context_t dc, Window_s& wnd, ScrollInfo_s& scroll, const Rect_s& view );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Desktop

	struct UserView_s
	{
		Vec2_s (NE_CALLBK *Do)( Context_t dc, void* user );
		void* User;
	};

	struct DesktopWindow_s
	{
		Window_s	 Wnd;
		UserView_s	 View;
		ScrollInfo_s Scroll;
	};

	void    NE_API Desktop_DoWindow   ( Context_t dc, DesktopWindow_s& wnd );
	void	NE_API Desktop_Do		  ( Context_t dc, DesktopWindow_s* item, int count );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ToolBar

	struct ToolBarItem
	{
		enum Enum
		{ Seperator
		, Button
		};
	};

	struct ToolBarItem_s
	{
		Id_t			  Id;
		ToolBarItem::Enum Type;
		Text_s			  Text;
	};

	struct ToolBarEvent_s
	{
		Id_t Clicked;
	};

	struct ToolBar_s
	{
		Id_t				 Id;
		Rect_s				 Rect;
		const ToolBarItem_s* Item;
		int					 NumItems;
	};

	float		   NE_API ToolBar_CalcHeight( Context_t dc );
	ToolBarEvent_s NE_API ToolBar_Do		( Context_t dc, Id_t id, const Rect_s& r, const ToolBarItem_s* item, int num_items );
	ToolBarEvent_s NE_API ToolBar_Do		( Context_t dc, const ToolBar_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Property

	struct PropertyStyle
	{
		enum Flag
		{ None
		, ReadOnly
		};
		typedef uint32_t Mask_t;
	};

	void NE_API Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, bool&		v );
	void NE_API Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, uint32_t&	v );
	void NE_API Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, int32_t&	v );
	void NE_API Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, float&	v );
	void NE_API Property_Do( Context_t dc, Id_t id, const Rect_s& r, float label_w, const Text_s& label, PropertyStyle::Mask_t style, float&	v, float min, float max );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Splitter

	struct SplitterState_s
	{
		Orientation::Enum	Direction;
		float				Distance;
		float				Panel1MinSize;
		float				Panel2MinSize;
	};

	float	NE_API Splitter_CalcSize	( Context_t dc, Orientation::Enum dir );
	Rect_s	NE_API Splitter_CalcRect	( Context_t dc, const Rect_s& r, Orientation::Enum dir, float distance );
	bool	NE_API Splitter_Draw		( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float distance );
	void	NE_API Splitter_Mouse		( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float panel1_min_size, float panel2_min_size, float& distance );
	void	NE_API Splitter_Do			( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float panel1_min_size, float panel2_min_size, float& distance );
	void	NE_API Splitter_DoV			( Context_t dc, Id_t id, const Rect_s& r, float panel1_min_size, float panel2_min_size, float& distance );
	void	NE_API Splitter_DoH			( Context_t dc, Id_t id, const Rect_s& r, float panel1_min_size, float panel2_min_size, float& distance );
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Border

	struct FrameBorderPart
	{
		enum Enum
		{ L
		, T
		, R
		, B
		, TL
		, TR
		, BL
		, BR
		, COUNT
		};
	};

	struct FrameBorderStyle
	{
		enum Flag
		{ None	 = 0
		, NoSize = 1
		, Thin	 = 2
		};

		typedef uint32_t Mask_t;
	};

	void	NE_API FrameBorder_GetPartIds	( Context_t dc, Id_t id, Id_t* p );
	void	NE_API FrameBorder_CalcPartRects( Context_t dc, const Rect_s& r, const Vec2_s& margin, Rect_s* p );
	Vec2_s	NE_API FrameBorder_CalcThickness( Context_t dc, FrameBorderStyle::Mask_t s );
	Rect_s	NE_API FrameBorder_Mouse		( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& margin, const Vec2_s& min );
	Rect_s	NE_API FrameBorder_Do			( Context_t dc, Id_t id, const Rect_s& r, FrameBorderStyle::Mask_t s, const Vec2_s& min );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Caption

	float				 NE_API FrameCaption_CalcHeight	( Context_t dc );
	bool				 NE_API FrameCaption_Draw		( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t );
	SizeGripAction::Enum NE_API FrameCaption_Mouse		( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& threshold, Vec2_s& drag_begin, Rect_s& wnd );
	SizeGripAction::Enum NE_API FrameCaption_Do			( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, const Vec2_s& threshold, Vec2_s& drag_begin, Rect_s& wnd );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Window

	struct FrameWndStyle
	{
		enum Flag
		{ None		 = 0
		, NoSize	 = 1<<0
		, NoBorder	 = 1<<1
		, NoCaption	 = 1<<2
		, ThinBorder = 1<<3
		, TearOff	 = 1<<4
		};
		typedef uint32_t Mask_t;
	};

	struct FrameWndResult_s
	{
		Rect_s				 Rect;
		SizeGripAction::Enum Drag;
	};

	Vec2_s NE_API FrameWnd_CalcDragThreshold( Context_t dc, FrameWndStyle::Mask_t s );
	Rect_s NE_API FrameWnd_CalcInnerRect	( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s );
	Rect_s NE_API FrameWnd_CalcCaptionRect	( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s );
	Rect_s NE_API FrameWnd_CalcClientRect	( Context_t dc, const Rect_s& r, FrameWndStyle::Mask_t s );

	FrameWndResult_s NE_API FrameWnd_Do( Context_t dc, Id_t id, const Rect_s& r, const Text_s& t, FrameWndStyle::Mask_t s, const Vec2_s& min, Vec2_s& drag_begin );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Dock Indicator

	struct DockSide
	{
		enum Flag
		{ None   = 0x00
		, Center = 0x01
		, Left	 = 0x02
		, Right	 = 0x04
		, Top	 = 0x08
		, Bottom = 0x10
		};

		typedef uint32_t Mask_t;
	};

	void NE_API DockIndicator_CalcTargetRect( Context_t dc, const Rect_s& r, DockSide::Flag side, Rect_s* wnd, Rect_s* host );

	DockSide::Flag NE_API DockIndicator_Do( Context_t dc, const Rect_s& r );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Scoped

	class ScopedChild_c
	{
	public:
		 ScopedChild_c( Context_t dc, const Rect_s& r ) : DC( dc ) { Context_BeginChild( DC, r ); }
		~ScopedChild_c()										   { Context_EndChild  ( DC );	  }
	private:
		Context_t DC;
	};

	class ScopedModal_c
	{
	public:
		 ScopedModal_c( Context_t dc ) : DC( dc ) { Context_BeginModal( DC ); }
		~ScopedModal_c()						  { Context_EndModal  ( DC ); }
	private:
		Context_t DC;
	};

	class ScopedFont_c
	{
	public:
		 ScopedFont_c( Context_t dc, wcstr_t name, float size ) 
			 : DC( dc )
		 {
			 Font_t font = FontCache_CreateFont( System_GetFontCache(), name, size, FontStyle::Default );
			 Theme_t theme = Context_GetTheme( DC );
			 OldFont = theme->Font;
			 theme->Font = font;
		 }

		 ScopedFont_c( Context_t dc, Font_t font ) 
			: DC( dc ) 
		 { 
			 Theme_t theme = Context_GetTheme( DC );
			 OldFont = theme->Font;
			 theme->Font = font;
		 }

		~ScopedFont_c() 
		{ 
			Context_GetTheme( DC )->Font = OldFont;
		}

	private:
		Context_t DC;
		Font_t OldFont;
	};

	#define NeGuiScopedModal( dc )		ScopedModal_c NeUnique(scoped_modal)( dc )
	#define NeGuiScopedChild( dc, r )	ScopedChild_c NeUnique(scoped_child)( dc, r )
	#define NeGuiScopedFont(  dc, ... ) ScopedFont_c  NeUnique(scoped_font) ( dc, __VA_ARGS__ )

} }
