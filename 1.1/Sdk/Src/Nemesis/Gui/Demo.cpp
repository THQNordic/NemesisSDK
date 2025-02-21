//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gui/Demo.h>
#include <Nemesis/Gui/System.h>
#include <Nemesis/Gui/Controls.h>
#include <math.h>

//======================================================================================
namespace nemesis { namespace gui
{
	static float Header_CalcHeight( Context_t dc )
	{
		return 2.0f + Context_GetLineHeight( dc ) + 2.0f;
	}

	static void Header_Do( Context_t dc, const Rect_s& r, cstr_t text )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Theme_t theme = Context_GetTheme( dc );
		const uint32_t text_col = theme->Visual[Visual::TextEdit].Text[CtrlState::Normal];
		const uint32_t frame_bk = theme->Visual[Visual::TextEdit].Front[CtrlState::Normal];
		const uint32_t frame_fn = theme->Visual[Visual::Expander].Front[CtrlState::Normal];
		Graphics_FillRect( g, r, frame_bk );
		Graphics_DrawRect( g, r, frame_fn );
		Label_Do( dc, r, TextFormat::Center | TextFormat::Middle, text_col, text );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	namespace 
	{
		static system::StopWatch_c theTimer;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static void Context_DrawChildRect( Context_t dc, uint32_t argb )
	{
		Graphics_DrawRect( Context_GetGraphics( dc ), Context_GetChild( dc ), argb );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	struct Column_s
	{
		Vec2_s Caret;
		Vec2_s Indent;
		Vec2_s Spacing;
		Vec2_s Limit;
	};

	void		Column_Init		( Column_s& col );
	void		Column_Add		( Column_s& col, const Vec2_s& size );
	void		Column_AddLine	( Column_s& col, const Vec2_s& size );
	void		Column_EndLine	( Column_s& col );
	void		Column_SpaceX	( Column_s& col );
	void		Column_SpaceY	( Column_s& col );
	Vec2_s		Column_End		( const Column_s* col, int count );
	Vec2_s		Column_End		( const Column_s& col );
	Column_s	Column_Next		( const Column_s& col );

	void Column_Init( Column_s& col )
	{
		col.Indent  = Vec2_s { 4.0f, 4.0f };
		col.Spacing = Vec2_s { 4.0f, 4.0f };
		col.Limit   = Vec2_s {};
		col.Caret   = col.Indent;
	}

	void Column_Add( Column_s& col, const Vec2_s& size )
	{
		col.Caret.x += size.x;
		col.Limit.x = NeMax( col.Limit.x, col.Caret.x );
		col.Limit.y = NeMax( col.Limit.y, col.Caret.y + size.y );
	}

	void Column_AddLine( Column_s& col, const Vec2_s& size )
	{
		Column_Add( col, size );
		Column_EndLine( col );
	}

	void Column_EndLine( Column_s& col )
	{
		col.Caret.x = col.Indent.x;
		col.Caret.y = col.Limit.y;
	}

	void Column_SpaceX( Column_s& col )
	{
		Column_Add( col, Vec2_s { col.Spacing.x, 0.0f } );
	}

	void Column_SpaceY( Column_s& col )
	{
		Column_AddLine( col, col.Spacing );
	}

	Vec2_s Column_End( const Column_s& col )
	{
		return col.Limit + col.Spacing;
	}

	Vec2_s Column_End( const Column_s* col, int count )
	{
		Vec2_s size = {};
		for ( int i = 0; i < count; ++i )
			size = Vec2_Max( size, Column_End( col[i] ) );
		return size;
	}

	Column_s Column_Next( Column_s& col )
	{
		Column_s next;
		Column_Init( next ); 
		next.Caret.x += col.Limit.x;
		next.Indent.x += col.Limit.x;
		return next;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	void Column_DoHeader( Column_s& col, Context_t dc, const Rect_s& r, cstr_t text )
	{
		const Vec2_s size = { r.w - col.Caret.x - col.Indent.x, Header_CalcHeight( dc ) };
		const Rect_s header_rect = Rect_Ctor( col.Caret, size );
		Header_Do( dc, header_rect, text );
		Column_AddLine( col, size );
		Column_SpaceY( col );
		Column_SpaceY( col );
	}

	void Column_DoHeader( Column_s& col, Context_t dc, cstr_t text )
	{
		return Column_DoHeader( col, dc, Context_GetClipRect( dc ), text );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static Vec2_s View_Graphics_Rects_Fixed( Context_t dc, const Vec2_s& caret )
	{
		Graphics_t g = Context_GetGraphics( dc );

		float sx = 2.0f;
		float sy = 2.0f;
		float w = 10.0f;
		float x = caret.x;
		float y = caret.y;
		float max_x = 0.0f;

		Rect_s rect;
		for ( int i = 0; i <= 10; ++i )
		{
			rect = Rect_s { x + (sx + w)*i, y, w, (float)i };
			Graphics_DrawRect( g, rect, 0xffff0000 );
			max_x = NeMax( max_x, rect.x + rect.w );
		}
		y += 12.0f;

		for ( int i = 0; i <= 10; ++i )
		{
			rect = Rect_s { x + (sx + w)*i, y, w, (float)i };
			Graphics_FillRect( g, rect, 0xffffff00 );
			max_x = NeMax( max_x, rect.x + rect.w );
		}
		y += 12.0f;

		for ( int i = 0; i <= 10; ++i )
		{
			rect = Rect_s { x + (sx + w)*i, y, w, (float)i };
			Graphics_DrawRect( g, rect, 0xffff0000 );
			Graphics_FillRect( g, rect, 0x77ffff00 );
			max_x = NeMax( max_x, rect.x + rect.w );
		}
		y += 12.0f;

		for ( int i = 0; i <= 10; ++i )
		{
			rect = Rect_s { x + (sx + w)*i, y, (float)i, w };
			Graphics_FillRect( g, rect, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffffff );
			max_x = NeMax( max_x, rect.x + rect.w );
		}
		y += 12.0f;

		return Vec2_s { max_x, y-caret.y };
	}

	static Vec2_s View_Graphics_Rect_Var( Context_t dc, const Vec2_s& caret, const Vec2_s& size )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s rect = Rect_Ctor( caret, size );
		Graphics_DrawRect( g, rect, 0xffff0000 );
		Graphics_FillRect( g, rect, 0x77ffff00 );
		return size;
	}

	static void Slider_IntFloat( Column_s& pos, Context_t dc, Id_t id, const Text_s& text, float w, float min, float max, float& sel, bool& do_floor )
	{
		Column_Add( pos, Label_Do( dc, pos.Caret, text ) );										 Column_SpaceX( pos ); 
		Column_Add( pos, Slider_DoH( dc, Id_Cat( id, __LINE__ ), pos.Caret, w, min, max, sel ) ); Column_SpaceX( pos ); 
		Column_Add( pos, Label_Fmt( dc, pos.Caret, "%.2f", sel ) );								 Column_SpaceX( pos ); Column_SpaceX( pos );
		//Column_Add( pos, CheckBox_Do( dc, Id_Gen( id, __LINE__ ), pos.Caret - Vec2_s { 0.0f, 5.0f ), "Integer", do_floor ) ); 
		Column_EndLine( pos ); 
		Column_SpaceY( pos );
		if (do_floor)
			sel = floorf( sel );
	}

	static void View_Graphics_Rects( Context_t dc, Column_s& pos )
	{
		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Rectangles:" ) );
		Column_SpaceY( pos );
		{
			Column_AddLine( pos, View_Graphics_Rects_Fixed( dc, pos.Caret ) );
			Column_SpaceY( pos );
		}

		{
			const  Vec2_s max_size = { 50.0f, 50.0f };
			static Vec2_s sel_size = max_size;
			static bool do_floor_x = true;
			static bool do_floor_y = true;

			Slider_IntFloat( pos, dc, NE_UI_ID, "Width: ", 150.0f, 0.0f, max_size.x, sel_size.x, do_floor_x );
			Slider_IntFloat( pos, dc, NE_UI_ID, "Height:", 150.0f, 0.0f, max_size.y, sel_size.y, do_floor_y );
			Column_SpaceY( pos );

			pos.Caret.x = pos.Indent.x + 10.0f;
			Column_Add( pos, View_Graphics_Rect_Var( dc, pos.Caret, sel_size ) );
			pos.Caret.x = pos.Indent.x + 10.0f + max_size.x + 10.0f;

			const float sin_dt = sinf( NE_PI * theTimer.ElapsedSec() );
			const float sin_dt_norm = 0.5f * (sin_dt + 1.0f);
			const Vec2_s anim_size = Vec2_s { sin_dt_norm * max_size.x, sin_dt_norm * max_size.y };
			View_Graphics_Rect_Var( dc, pos.Caret, anim_size );
			Column_Add( pos, max_size );
			Column_EndLine( pos );
			Column_SpaceY( pos );
		}
	}

	static Vec2_s View_Graphics_Circles_Fixed( Context_t dc, const Vec2_s& caret )
	{
		Graphics_t g = Context_GetGraphics( dc );

		float sx = 2.0f;
		float w = 20.0f;
		float cx = 10.0f;
		float cy = 10.0f;
		float x = caret.x;
		float y = caret.y;

		for ( int i = 0; i <= 10; ++i )
			Graphics_DrawCircle( g, Vec2_s { x + (sx + w)*i + cx, y + cy }, (float)i, 0xffff0000 );
		y += 22.0f;

		for ( int i = 0; i <= 10; ++i )
			Graphics_FillCircle( g, Vec2_s { x + (sx + w)*i + cx, y + cy }, (float)i, 0xffffff00 );
		y += 22.0f;

		for ( int i = 0; i <= 10; ++i )
		{
			Graphics_DrawCircle( g, Vec2_s { x + (sx + w)*i + cx, y + cy }, (float)i, 0xffff0000 );
			Graphics_FillCircle( g, Vec2_s { x + (sx + w)*i + cx, y + cy }, (float)i, 0x7fffff00 );
		}
		y += 22.0f;

		return Vec2_s { 0.0f, y-caret.y };
	}

	static Vec2_s View_Graphics_Circle_Var( Context_t dc, const Vec2_s& caret, const float radius )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const float d = 2.0f * radius;
		const Vec2_s center = { caret.x + radius, caret.y + radius };
		Graphics_DrawCircle( g, center, radius, 0xffff0000 );
		Graphics_FillCircle( g, center, radius, 0x7fffff00 );
		return Vec2_s { d, d };
	}

	static void View_Graphics_Circles( Context_t dc, Column_s& pos )
	{
		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Circles:" ) );
		Column_SpaceY( pos );
		{
			Column_AddLine( pos, View_Graphics_Circles_Fixed( dc, pos.Caret ) );
			Column_SpaceY( pos );
		}

		{
			const  float max_radius = 25.0f;
			static float sel_radius = max_radius;
			static bool do_floor = true;

			Slider_IntFloat( pos, dc, NE_UI_ID, "Radius: ", 150.0f, 0.0f, max_radius, sel_radius, do_floor );
			Column_SpaceY( pos );

			pos.Caret.x = pos.Indent.x + 10.0f;
			Column_Add( pos, View_Graphics_Circle_Var( dc, pos.Caret, sel_radius ) );
			pos.Caret.x = pos.Indent.x + 10.0f + ( 2.0f * max_radius ) + 10.0f;

			const float sin_dt = sinf( NE_PI * theTimer.ElapsedSec() );
			const float sin_dt_norm = 0.5f * (sin_dt + 1.0f);
			const Vec2_s anim_size = Vec2_s { sin_dt_norm * max_radius, sin_dt_norm * max_radius };
			View_Graphics_Circle_Var( dc, pos.Caret, anim_size.x );
			Column_Add( pos, Vec2_s { 0.0f, 2.0f * max_radius } );
			Column_EndLine( pos );
			Column_SpaceY( pos );
		}
	}

	static Vec2_s View_Graphics_Lines_Fixed( Context_t dc, const Vec2_s& caret )
	{
		Graphics_t g = Context_GetGraphics( dc );

		float sx = 2.0f;
		float sy = 2.0f;
		float w = 20.0f;
		float x = caret.x;
		float y = caret.y;
		float max_x = 0.0f;

		Rect_s rect;
		for ( int i = 0; i <= 10; ++i )
		{
			const float d = 2.0f * i;
			rect = Rect_s { x + (sx + w)*i, y, d, d };

			const float l = rect.x;
			const float t = rect.y;
			const float r = rect.x + rect.w - 1.0f;
			const float b = rect.y + rect.h - 1.0f;
			const Vec2_s lt = { l, t };
			const Vec2_s rt = { r, t };
			const Vec2_s lb = { l, b };
			const Vec2_s rb = { r, b };
			Graphics_DrawLine( g, lt, rt, Color::Yellow );
			Graphics_DrawLine( g, lb, rb, Color::Yellow );
			Graphics_DrawLine( g, lt, lb, Color::Yellow );
			Graphics_DrawLine( g, rt, rb, Color::Yellow );
			Graphics_DrawLine( g, lb, rt, Color::Yellow );
			Graphics_DrawLine( g, lt, rb, Color::Yellow );
			max_x = NeMax( max_x, rect.x + rect.w );
		}
		y += 12.0f;

		return Vec2_s { max_x, y-caret.y };
	}

	static void View_Graphics_Lines( Context_t dc, Column_s& pos )
	{
		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Lines:" ) );
		Column_SpaceY( pos );
		{
			Column_AddLine( pos, View_Graphics_Lines_Fixed( dc, pos.Caret ) );
			Column_SpaceY( pos );
		}
	}

	static Vec2_s View_Graphics( Context_t dc )
	{
		Column_s pos[3];
		{
			Column_Init( pos[0] );
			View_Graphics_Rects( dc, pos[0] );
		}
		{
			pos[1] = Column_Next( pos[0] ); 
			View_Graphics_Circles( dc, pos[1] );
		}
		{
			pos[2] = Column_Next( pos[1] ); 
			View_Graphics_Lines( dc, pos[2] );
		}
		return Column_End( pos, NeCountOf(pos) );
	}

	//

	static void View_Font_Texture( Context_t dc, Column_s& col, Font_t f )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Renderer_t client = System_GetRenderer();
		FontCache_t atlas = System_GetFontCache();

		Result_t hr;

		FontInfo_s fi = {};
		hr = FontCache_GetFontInfo( atlas, f, fi );
		if (!fi.Texture)
			return;

		TextureInfo_s ti = {}; 
		hr = client->Api->GetTextureInfo( client, fi.Texture, ti );

		Theme_t theme = Context_GetTheme( dc );
		const uint32_t label_color = theme->Visual[Visual::Label].Text[CtrlState::Normal];

		float tw = 1.0f*ti.Width;
		float th = 1.0f*ti.Height;
		const Rect_s r = { col.Caret.x, col.Caret.y, tw, th };
		const Vec2_s uv[2] = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
		Graphics_SetTexture( g, fi.Texture, fi.WhiteUV );
		Graphics_FillRect( g, r, uv[0], uv[1], label_color );
		Graphics_DrawRect( g, r, Color::Goldenrod );
		Column_Add( col, Vec2_s { tw, th } );
	}

	static Vec2_s View_Font( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		Graphics_t g = Context_GetGraphics( dc );
		Renderer_t client = System_GetRenderer();
		FontCache_t atlas = System_GetFontCache();

		Result_t hr = NE_OK;
		Font_t fid[] = 
		{ FontCache_CreateGdiFont( atlas, L"Consolas", 12.0, FontStyle::Default ) 
		, FontCache_CreateStbFont( atlas, L"Consolas", 12.0, FontStyle::Default ) 
		, FontCache_CreateGdiFont( atlas, L"Tahoma"  , 12.0, FontStyle::Default )
		, FontCache_CreateStbFont( atlas, L"Tahoma"  , 12.0, FontStyle::Default )
		, FontCache_CreateGdiFont( atlas, L"Verdana" , 12.0, FontStyle::Default )
		, FontCache_CreateStbFont( atlas, L"Verdana" , 12.0, FontStyle::Default )
		, FontCache_CreateGdiFont( atlas, L"Segoe UI", 12.0, FontStyle::Default )
		, FontCache_CreateStbFont( atlas, L"Segoe UI", 12.0, FontStyle::Default )
		};

		FontInfo_s fi;
		for ( int i = 0; i < NeCountOf(fid); ++i )
			hr = FontCache_PreloadChars( atlas, fid[i], 0x01, 0xff, fi );

		for ( int i = 0; i < NeCountOf(fid); ++i )
		{
			View_Font_Texture( dc, pos, fid[i] );
			Column_SpaceX( pos );
		}
		Column_EndLine( pos );

		const uint32_t label_color = Context_GetTheme( dc )->Visual[Visual::Label].Text[CtrlState::Normal];
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Consolas, -12, Gdi] The quick brown fox jumps over the lazy dog \x000c"	, fid[0], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Consolas, -12, Stb] The quick brown fox jumps over the lazy dog \x000c"	, fid[1], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Tahoma, -12, Gdi] The quick brown fox jumps over the lazy dog \x000c"	, fid[2], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Tahoma, -12, Stb] The quick brown fox jumps over the lazy dog \x000c"	, fid[3], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Verdana, -12, Gdi] The quick brown fox jumps over the lazy dog \x000c"	, fid[4], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Verdana, -12, Stb] The quick brown fox jumps over the lazy dog \x000c"	, fid[5], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Segoe UI, -12, Gdi] The quick brown fox jumps over the lazy dog \x000c"	, fid[6], label_color ) );
		Column_AddLine( pos, Graphics_DrawString( g, pos.Caret, L"[Segoe UI, -12, Stb] The quick brown fox jumps over the lazy dog \x000c"	, fid[7], label_color ) );
		return Column_End( pos );
	}

	//

	static Vec2_s View_Labels( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Simple utf-8 string" ) );
		Column_AddLine( pos, Label_Do( dc, pos.Caret, L"Simple utf-16 string" ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Formatted utf-8 string: %.2f", theTimer.ElapsedSec() ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, L"Formatted utf-16 string: %.2f", theTimer.ElapsedSec() ) );
		{
			ScratchBuffer_t scratch = Context_GetScratch( dc );
			const Text_s text = ScratchBuffer_Format( scratch, L"Pre-formatted utf-16 string: %.2f", theTimer.ElapsedSec() );
			Column_AddLine( pos, Label_Do( dc, pos.Caret, text ) );
		}

		const float sin_dt = sinf( NE_PI * theTimer.ElapsedSec() );
		const float sin_dt_norm = 0.5f * (sin_dt + 1.0f);
		{
			const cstr_t text = "Delimited utf-8 string";
			const float sin_count = sin_dt_norm * (1+Str_Len(text));
			Column_AddLine( pos, Label_Do( dc, pos.Caret, Text_s( text, (int)sin_count ) ) );
		}
		{
			const wcstr_t text = L"Delimited utf-16 string";
			const float sin_count = sin_dt_norm * (1+Wcs_Len(text));
			Column_AddLine( pos, Label_Do( dc, pos.Caret, Text_s( text, (int)sin_count ) ) );
		}

		Graphics_t g = Context_GetGraphics( dc );
		{
			Column_AddLine( pos, pos.Spacing );
			Rect_s r = Rect_Ctor( pos.Caret, Vec2_s { 100.0f, 30.0f } );
			Label_Do( dc, r, TextFormat::Center | TextFormat::Middle, "Aligned text" );
			Graphics_DrawRect( g, Rect_Inflate( r, 1.0f ), Color::Black );
			Column_AddLine( pos, Rect_Size( r ) );
			Column_AddLine( pos, pos.Indent );
		}
		{
			Column_SpaceY( pos );

			// text edit
			static wchar_t buffer[256] = L"This is a text edit control...";
			static TextEdit_s edit = 
			{ NE_UI_ID
			, Rect_Ctor( Vec2_s {}, TextEdit_CalcSize( dc, 100.0f ) )
			, { buffer, NeCountOf(buffer) }
			, {}
			, TextEditStyle::None
			};

			edit.Rect.x = pos.Caret.x;
			edit.Rect.y = pos.Caret.y;

			TextEdit_Do( dc, edit );
			Column_Add( pos, Rect_Size( edit.Rect ) );

			// flags
			Column_SpaceX( pos );
			bool is_read_only = NeHasFlag( edit.Style, TextEditStyle::ReadOnly );
			Column_Add( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "ReadOnly", is_read_only ) );
			NeSetFlag( edit.Style, TextEditStyle::ReadOnly, is_read_only );
			Column_EndLine( pos );
			Column_SpaceY( pos );

			// scroll bar
			Font_t font = Context_GetFont( dc );
			const Rect_s bounds = Graphics_MeasureString( g, buffer, font );

			const Rect_s text_rect = TextEdit_CalcTextRect( dc, edit.Rect );
			const Rect_s scroll_rect = { pos.Caret.x, pos.Caret.y, edit.Rect.w, ScrollBar_CalcSize( dc, Orientation::Horz, 0.0f ).y } ;
			ScrollBar_DoH( dc, NE_UI_ID, scroll_rect, 0.0f, bounds.w, 1.0f, text_rect.w, edit.State.Scroll );
			Column_AddLine( pos, Rect_Size( scroll_rect ) );
			Column_SpaceY( pos );
		}

		// @todo: fix mult-line alignment
		/*
		{
			Rect_s r = Rect_s( pos.Caret, Vec2_s { 100.0f, 50.0f ) );
			Label_Do( dc, r, TextFormat::Bottom | TextFormat::Right, "Aligned\nmulti-line\ntext" );
			Graphics_DrawRect( g, Rect_Inflate( r, 1.0f ), Color::Black );
			Column_AddLine( pos, Rect_Size( r ) );
		}
		*/
		return Column_End( pos );
	}

	//

	static void View_Buttons_Simple( Context_t dc, Column_s& pos )
	{
		const cstr_t label[2] = { "Click me!", "Again!" };

		const Vec2_s btn_size[2] = { Button_CalcSize( dc, label[0] ), Button_CalcSize( dc, label[1] ) };
		const Vec2_s max_size = Vec2_s { NeMax( btn_size[0].x, btn_size[1].x ), btn_size[0].y };

		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Simple Buttons:" ) );
		Column_SpaceY( pos );

		enum { NUM_BUTTONS = 10 };
		{
			static cstr_t state[ NUM_BUTTONS ] = {};
			if (!state[0])
			{
				for ( int i = 0; i < NeCountOf(state); ++i )
					state[i] = label[0];
			}

			for ( int i = 0; i < NeCountOf(state); ++i )
			{
				if (Button_Do( dc, NE_UI_ID_CHILD( i ), pos.Caret, state[i] ))
					state[i] = (state[i] == label[0]) ? label[1] : label[0];
				Column_Add( pos, max_size + pos.Spacing );
			}
			Column_EndLine( pos );
		}
		Column_SpaceY( pos );
		{
			static cstr_t state[NUM_BUTTONS] = {};
			if (!state[0])
			{
				for ( int i = 0; i < NeCountOf(state); ++i )
					state[i] = label[0];
			}

			for ( int i = 0; i < NeCountOf(state); ++i )
			{
				if (Button_Do( dc, NE_UI_ID_CHILD( i ), Rect_Ctor( pos.Caret, max_size ), state[i] ))
					state[i] = (state[i] == label[0]) ? label[1] : label[0];
				Column_Add( pos, max_size + pos.Spacing );
			}
			Column_EndLine( pos );
		}
		Column_SpaceY( pos );
	}

	static void View_Buttons_CheckBox( Context_t dc, Column_s& pos )
	{
		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Check Boxes:" ) );
		Column_SpaceY( pos );
		{
			static bool checked[3] = {};
			for ( int i = 0; i < NeCountOf(checked); ++i )
			{
				Column_AddLine( pos, CheckBox_Do( dc, NE_UI_ID_CHILD( i ), pos.Caret, "Check me!", checked[i] ) );
				Column_SpaceY( pos );
			}
		}
		{
			static bool checked[3] = {};
			for ( int i = 0; i < NeCountOf(checked); ++i )
			{
				Column_AddLine( pos, CheckBox_DoButton( dc, NE_UI_ID_CHILD( i ), pos.Caret, "Check me!", checked[i] ) );
				Column_SpaceY( pos );
			}
		}
		Column_SpaceY( pos );
	}

	static void View_Buttons_Radio( Context_t dc, Column_s& pos )
	{
		Column_AddLine( pos, Label_Do( dc, pos.Caret, "Radio Buttons:" ) );
		Column_SpaceY( pos );
		ScratchBuffer_t scratch = Context_GetScratch( dc );
		{
			static int sel = -1;
			for ( int i = 0; i < 3; ++i )
			{
				Column_AddLine( pos, RadioButton_Do( dc, NE_UI_ID_CHILD( i ), pos.Caret, ScratchBuffer_Format( scratch, "Option %d", 1+i ), i, sel ) );
				Column_SpaceY( pos );
			}
		}
		{
			static int sel = -1;
			for ( int i = 0; i < 3; ++i )
			{
				Column_AddLine( pos, RadioButton_DoButton( dc, NE_UI_ID_CHILD( i ), pos.Caret, ScratchBuffer_Format( scratch, "Option %d", 1+i ), i, sel ) );
				Column_SpaceY( pos );
			}
		}
		Column_SpaceY( pos );
	}

	static Vec2_s View_Buttons( Context_t dc )
	{
		Column_s pos[3];
		Column_Init( pos[0] );
		View_Buttons_Simple( dc, pos[0] );

		Column_Init( pos[1] );
		pos[1].Caret.y += Column_End( pos[0] ).y;
		View_Buttons_CheckBox( dc, pos[1] );

		pos[2] = Column_Next( pos[1] );
		pos[2].Indent.x += 10.0f;
		pos[2].Caret.x = pos[2].Indent.x;
		pos[2].Caret.y += Column_End( pos[0] ).y;
		View_Buttons_Radio( dc, pos[2] );

		return Column_End( pos, 2 );
	}

	//

	static Vec2_s View_Ranges( Context_t dc )
	{
		const float scroll_min  = -50.0f;
		const float scroll_max  =  50.0f;
		const float scroll_page =  25.0f;
		const float scroll_line =   5.0f;

		enum { NUM_COL = 2 };
		static float progress	[NUM_COL] = {};
		static float slider		[NUM_COL] = {};
		static float scroll_bar	[NUM_COL] = { scroll_min, scroll_max - scroll_page };

		const float sin_dt = sinf( theTimer.ElapsedSec() );
		const float sin_dt_norm = 0.5f * (sin_dt + 1.0f);
		progress  [0] = sin_dt;
		slider	  [0] = sin_dt;
		scroll_bar[0] = scroll_min + (scroll_max-scroll_min-scroll_page) * sin_dt_norm;
		progress  [1] = slider[1];

		Vec2_s limit;
		Column_s pos;
		Column_Init( pos );

		Graphics_t g = Context_GetGraphics( dc );
		const float grid_h = floorf( 1.5f * Context_GetLineHeight( dc ) );

		for ( int i = 0; i < NUM_COL; ++i )
		{
			// labels
			Column_SpaceY( pos );
			Column_AddLine( pos, Vec2_s { Label_Do( dc, pos.Caret, "ScrollBar:"	).x, grid_h } ); 
			Column_AddLine( pos, Vec2_s { Label_Do( dc, pos.Caret, "Slider:"	).x, grid_h } ); 
			Column_AddLine( pos, Vec2_s { Label_Do( dc, pos.Caret, "Progress:"	).x, grid_h } ); 

			limit = pos.Limit;
			pos = Column_Next( pos );
			pos.Caret.x  += 20.0f;
			pos.Indent.x += 20.0f;

			// controls
			Column_SpaceY( pos );
			Column_AddLine( pos, Vec2_s { ScrollBar_DoH	( dc, Id_Cat( NE_UI_ID, 2*i+1 ), pos.Caret, 100.0f, scroll_min, scroll_max, scroll_line, scroll_page, scroll_bar[i] ).x, grid_h } );
			Column_AddLine( pos, Vec2_s { Slider_DoH	( dc, Id_Cat( NE_UI_ID, 2*i+0 ), pos.Caret, 100.0f, -1.0f, 1.0f										, slider	[i] ).x, grid_h } );
			Column_AddLine( pos, Vec2_s { ProgressBar_Do( dc						   , pos.Caret, 100.0f, -1.0f, 1.0f										, progress	[i] ).x, grid_h } );

			Column_SpaceX( pos );
			Column_SpaceY( pos );
			limit = pos.Limit;
			pos = Column_Next( pos );
			pos.Caret.x  += 20.0f;
			pos.Indent.x += 20.0f;
		}

		return limit;
	}

	//

	static Vec2_s View_List( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		Graphics_t g = Context_GetGraphics( dc );

		// data
		static ListHeaderItem_s lhi[] = 
		{ { "Column 0", 150.0f, Alignment::Center, SortOrder::None		 } 
		, { "Column 1", 150.0f, Alignment::Right , SortOrder::Ascending  } 
		, { "Column 2", 150.0f, Alignment::Left  , SortOrder::Descending } 
		, { "Column 3", 150.0f, Alignment::Center, SortOrder::Descending } 
		, { "Column 4", 150.0f, Alignment::Left  , SortOrder::Ascending  } 
		, { "Column 5", 150.0f, Alignment::Right , SortOrder::None		 } 
		};

		// header
		{
			// header item
			{
				// label
				Column_DoHeader( pos, dc, "List Header Item" );

				// item
				static int hot_item	= 0;
				static int pushed_item = 0;
				const Vec2_s header_size = ListHeader_CalcSize( dc, 0.0f );
				for ( int i = 0; i < NeCountOf(lhi); ++i )
				{
					const Rect_s item_rect = { pos.Caret.x, pos.Caret.y, lhi[i].Width, header_size.y };
					ListHeaderItem_Do( dc, NE_UI_ID, item_rect, lhi[i], i, hot_item, pushed_item );
					Column_AddLine( pos, Rect_Size( item_rect ) );
					Column_SpaceY( pos );
				}
				Column_SpaceY( pos );
			}

			// header
			{ 
				// label
				Column_DoHeader( pos, dc, "List Header" );

				static ListHeaderState_s header_state = {};

				const float line_height = Context_GetLineHeight( dc );
				const Vec2_s header_size = ListHeader_CalcSize( dc, 400.0f );
				const Rect_s header_rect = Rect_Ctor( pos.Caret, header_size );
				ListHeader_Do( dc, NE_UI_ID, header_rect, lhi, NeCountOf( lhi ), header_state );
				Column_AddLine( pos, header_size );
				Column_SpaceY( pos );

				// scroll bar
				const float total_width = ListHeader_CalcWidth( lhi, NeCountOf( lhi ) );
				const Vec2_s sb_size = Vec2_s { header_size.x, ScrollBar_CalcSize( dc, Orientation::Horz, 100.0f ).y };
				const Rect_s sb_rect = Rect_Ctor( pos.Caret, sb_size );

				const float sb_min = 0.0f;
				const float sb_max = total_width;
				const float sb_page = header_rect.w;
				const float sb_line = floorf( total_width ) / NeCountOf( lhi );
				ScrollBar_DoH( dc, NE_UI_ID, sb_rect, sb_min, sb_max, sb_line, sb_page, header_state.View );
				Column_AddLine( pos, sb_size );
				Column_SpaceY( pos );
			}
		}

		// column
		{
			// label
			Column_DoHeader( pos, dc, "List Column" );

			// column
			const float column_min_height =  10.0f;
			const float column_max_height = 200.0f;
			static float column_height	  = 200.0f;

			static cstr_t column_label_utf8[] = 
			{ "Item 0", "Item 1", "Item 2", "Item 3", "Item 4"
			, "Item 5", "Item 6", "Item 7", "Item 8", "Item 9"
			};

			static wcstr_t column_label_utf16[ NeCountOf(column_label_utf8) ] = 
			{ L"Wide 0" //L"\x2605"
			, L"Wide 1"
			, L"Wide 2"
			, L"Wide 3"
			, L"Wide 4"
			, L"Wide 5"
			, L"Wide 6"
			, L"Wide 7"
			, L"Wide 8"
			, L"Wide 9"
			};

			const int num_rows = (int)NeMin( NeCountOf( column_label_utf8 ), NeCountOf( column_label_utf16 ) );

			struct MyCallback
			{
				static Text_s Do( void*, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
				{
					ProgressBar_Draw( dc, Rect_Margin( r, 2.0f ), 0.0f, 10.0f, 1.0f * row ); 
					ScratchBuffer_t scratch = Context_GetScratch( dc );
					return ScratchBuffer_Format( scratch, "Callback %d", row );
				}
			};

			static ListColumn_s lc[] = 
			{ { column_label_utf8 , nullptr, {}, lhi[0].Alignment }
			, { nullptr, column_label_utf16, {}, lhi[1].Alignment }
			, { nullptr, nullptr, { MyCallback::Do, nullptr }, lhi[2].Alignment }
			};

			static ListColumnState_s lcs[ NeCountOf( lc ) ] = {};
			for ( int i = 0; i < NeCountOf( lc ); ++i )
			{
				const Rect_s column_rect = Rect_s { pos.Caret.x, pos.Caret.y, lhi[i].Width, column_height };
				ListColumn_Do( dc, NE_UI_ID, column_rect, lc[i], num_rows, i, lcs[i] );
				Graphics_DrawRect( g, column_rect, Color::PaleGoldenrod );
				Column_Add( pos, Vec2_s { column_rect.w + 10.0f, column_max_height } );
			}
			Column_EndLine( pos );
			Column_SpaceY( pos );

			// slider
			const Vec2_s slider_size = Slider_CalcSize( dc, Orientation::Horz, 0.0f );
			const Rect_s slider_rect = { pos.Caret.x, pos.Caret.y, pos.Limit.x - 10.0f - pos.Caret.x, slider_size.y };
			Slider_Do( dc, Orientation::Horz, NE_UI_ID, slider_rect, column_min_height, column_max_height, column_height );
			Column_Add( pos, Rect_Size( slider_rect ) );
			Column_EndLine( pos );
			Column_SpaceY( pos );
		}

		// list
		{
			// label
			Column_DoHeader( pos, dc, "List" );

			// style boxes
			static bool no_header = false;
			static bool horz_lines = false;
			static bool vert_lines = false;
			Column_Add( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "No Header", no_header ) );		Column_SpaceX( pos );
			Column_Add( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "Horz Lines", horz_lines ) );	Column_SpaceX( pos );
			Column_Add( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "Vert Lines", vert_lines ) );	Column_SpaceX( pos );
			Column_EndLine( pos );
			Column_SpaceY( pos );

			// data
			struct MyCallback
			{
				static Text_s DoText( void*, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
				{
					ScratchBuffer_t scratch = Context_GetScratch( dc );
					return ScratchBuffer_Format( scratch, "Item %d", row );
				}

				static Text_s DoProg( void*, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
				{
					ProgressBar_Draw( dc, Rect_Margin( r, 2.0f ), 0.0f, 100.0f, 1.0f * row );
					return Text_s();
				}
			};
			
			static ListColumn_s lc[ NeCountOf(lhi) ] =
			{ { nullptr, nullptr, { MyCallback::DoText, nullptr }, lhi[0].Alignment }
			, { nullptr, nullptr, { MyCallback::DoText, nullptr }, lhi[1].Alignment }
			, { nullptr, nullptr, { MyCallback::DoProg, nullptr }, lhi[2].Alignment }
			, { nullptr, nullptr, { MyCallback::DoText, nullptr }, lhi[3].Alignment }
			, { nullptr, nullptr, { MyCallback::DoText, nullptr }, lhi[4].Alignment }
			, { nullptr, nullptr, { MyCallback::DoText, nullptr }, lhi[5].Alignment }
			};

			// list
			const Id_t list_id = NE_UI_ID;
			ListState_s& ls = Context_CacheIntern< ListState_s >( dc, list_id );
			const Rect_s child_rect = Context_GetChild( dc );
			const Rect_s list_rect = { 10.0f, pos.Caret.y, child_rect.w - 20.0f, 400.0f };
			const ListStyle::Mask_t list_style = ListStyle::None
							| (no_header  ? ListStyle::NoHeader  : ListStyle::None)
							| (horz_lines ? ListStyle::HorzLines : ListStyle::None)
							| (vert_lines ? ListStyle::VertLines : ListStyle::None)
							;
			const ListEvent_s ev = List_Do( dc, list_id, list_rect, lhi, lc, NeCountOf(lhi), 100, list_style, ls );
			Column_Add( pos, Rect_Size( list_rect ) );
			Column_EndLine( pos );
			Column_SpaceY( pos );

			static int msg_box_idx = -1;
			if (ev.Activated)
				msg_box_idx = ls.SelRow;

			if (msg_box_idx >= 0)
			{
				bool done = false;
				MessageBox_Do( dc, NE_UI_ID, "List Event", Context_FormatString( dc, "Activated: %d", msg_box_idx ), done );
				if (done)
					msg_box_idx = -1;
			}
				
		}
		Column_AddLine( pos, Vec2_s { 0.0f, 10.0f } );

		return pos.Limit;
	}

	//

	static Vec2_s View_Tree( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		const Rect_s child_rect = Context_GetChild( dc );

		// tree item
		{
			// label
			Column_DoHeader( pos, dc, "Tree Item" );

			const Id_t tree_id = NE_UI_ID;
			static TreeItem_s item[] = 
			{ { "Root"	 , true  , true  }
			, { "Item 0" , true  , true  }
			, { "Item 1" , false , false }
			, { "Item 2" , true  , true  }
			, { "Item 3" , false , false }
			, { "Item 4" , true  , false }
			};
			static int level[ NeCountOf( item) ] = 
			{ 0
			, 1
			, 2
			, 1
			, 2
			, 1
			};

			for ( int i = 0; i < NeCountOf( item ); ++i )
			{
				TreeItemState_s state = {};
				Vec2_s size = Tree_Item_Do( dc, tree_id, pos.Caret, child_rect.w - pos.Caret.x, item[i], level[i], TreeStyle::None, state );
				if (state.Activated)
					item[i].Expanded = !item[i].Expanded;
				Column_AddLine( pos, size );
			}
			Column_SpaceY( pos );
		}

		// tree
		{
			// label
			Column_DoHeader( pos, dc, "Virtual Tree" );

			// data

			struct MyNode_s
			{
				Text_s	  Text;
				bool	  Expanded;
				MyNode_s* Child;
				int		  NumChildren;
			};

			static MyNode_s anim_node[] = { { "Anim 0" }, { "Anim 1" }, { "Anim 2" }, { "Anim 3" }, { "Anim 4" }, { "Anim 5" } };
			static MyNode_s mesh_node[] = { { "Mesh 0" }, { "Mesh 1" }, { "Mesh 2" }, { "Mesh 3" }, { "Mesh 4" }, { "Mesh 5" } };
			static MyNode_s skin_node[] = { { "Skin 0" }, { "Skin 1" }, { "Skin 2" }, { "Skin 3" }, { "Skin 4" }, { "Skin 5" } };
			static MyNode_s text_node[] = { { "Text 0" }, { "Text 1" }, { "Text 2" }, { "Text 3" }, { "Text 4" }, { "Text 5" } };

			static MyNode_s res_node[] = 
			{ { "Animations", false, anim_node, NeCountOf(anim_node) }
			, { "Meshes"	, false, mesh_node, NeCountOf(mesh_node) } 
			, { "Skins"		, false, skin_node, NeCountOf(skin_node) } 
			, { "Textures"	, false, text_node, NeCountOf(text_node) } 
			};

			static MyNode_s db_node = { "Database", false, res_node, NeCountOf(res_node) };

			struct MyCallback
			{
				static TreeItem_s GetTreeItem( void* context, TreeNode_t node )
				{
					MyNode_s* my_node = (MyNode_s*)node;
					const TreeItem_s item = { my_node->Text, my_node->NumChildren != 0, my_node->Expanded };
					return item;
				}

				static TreeNode_t GetFirstChild( void* context, TreeNode_t node )
				{
					return node ? ((MyNode_s*)node)->Child : (MyNode_s*)context;
				}

				static TreeNode_t GetNextSibling( void* context, TreeNode_t parent, TreeNode_t child )
				{
					if (parent && child)
					{
						MyNode_s* my_parent = (MyNode_s*)parent;
						MyNode_s* my_child  = (MyNode_s*)child;
						MyNode_s* my_end	= my_parent->Child + my_parent->NumChildren;
						NeAssert(child >= my_parent->Child);
						NeAssert(child < my_end);

						MyNode_s* my_next	= my_child+1;
						if (my_next < my_end)
							return my_next;
					}
					return nullptr;
				}
			};

			const TreeData_s tree_data = 
			{ { MyCallback::GetTreeItem, MyCallback::GetFirstChild, MyCallback::GetNextSibling, &db_node }
			, nullptr
			, nullptr
			};

			// tree view

			const Id_t tree_id = NE_UI_ID;
			const TreeStyle::Mask_t tree_style = TreeStyle::None;
			const Rect_s tree_rect = { 10.0f, pos.Caret.y, child_rect.w - pos.Caret.x - 20.0f, 200.0f };

			TreeViewState_s& tree_state = Context_CacheIntern<TreeViewState_s>( dc, tree_id );

			TreeView_Do( dc, tree_id, tree_rect, tree_data, tree_style, tree_state );

			if (tree_state.Tree.Activated)
			{
				MyNode_s* node = (MyNode_s*)tree_state.Tree.Activated;
				if (node->NumChildren)
					node->Expanded = !node->Expanded;
			}

			Column_Add( pos, Rect_Size( tree_rect ) );
			Column_EndLine( pos );
			Column_SpaceY( pos );
		}

		Column_AddLine( pos, Vec2_s { 0.0f, 10.0f } );
		return pos.Limit;
	}

	static Vec2_s View_Menu( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		const Rect_s child_rect = Context_GetChild( dc );

		static const MenuItem_s menu_file_new[] = 
		{ { "Project..." }
		, { "File..."    }
		};

		static const MenuItem_s menu_file[] = 
		{ { "New"		 , "Ctrl+N"			, { menu_file_new, NeCountOf(menu_file_new) }, nullptr }
		, { "Open..."	 , "Ctrl+O"			, { nullptr, 0 }, nullptr }
		, {}
		, { "Save"		 , "Ctrl+S"			, { nullptr, 0 }, nullptr }
		, { "Save As..." , ""				, { nullptr, 0 }, nullptr }
		, { "Save All"   , "Ctrl+Shift+S"	, { nullptr, 0 }, nullptr }
		, {}
		, { "Exit"		 , "Alt+F4"			, { nullptr, 0 }, nullptr }
		};

		static const MenuItem_s menu_edit[] = 
		{ { "Copy"		 , "Ctrl+C"			, { nullptr, 0 }, nullptr }
		, { "Cut"		 , "Ctrl+X"			, { nullptr, 0 }, nullptr }
		, { "Paste"		 , "Ctrl+V"			, { nullptr, 0 }, nullptr }
		, { "Delete"	 , "Del"			, { nullptr, 0 }, nullptr }
		, {}
		, { "Select All" , "Ctrl+A"			, { nullptr, 0 }, nullptr }
		};

		static const MenuItem_s menu_view[] = 
		{ { "Demo Window"	}
		, { "Debug Window"  }
		};

		static const MenuItem_s menu_help[] = 
		{ { "About..."	}
		};

		static const MenuItem_s menu_main[] = 
		{ { "File", "", { menu_file, NeCountOf(menu_file) }, nullptr }
		, { "Edit", "", { menu_edit, NeCountOf(menu_edit) }, nullptr }
		, { "View", "", { menu_view, NeCountOf(menu_view) }, nullptr }
		, { "Help", "", { menu_help, NeCountOf(menu_help) }, nullptr }
		};

		const MenuItemList_s menu_list_file = { menu_file, NeCountOf( menu_file ) };
		const MenuItemList_s menu_list_edit = { menu_edit, NeCountOf( menu_edit ) };
		const MenuItemList_s menu_list_main = { menu_main, NeCountOf( menu_main ) };

		// menu item
		{
			// label
			Column_DoHeader( pos, dc, "Menu Item" );

			// style
			static bool no_bkgnd = false;
			Column_SpaceY( pos );
			Column_AddLine( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "No Bkgnd", no_bkgnd ) );
			Column_SpaceY( pos );
			const MenuItemStyle::Mask_t item_style = no_bkgnd ? MenuItemStyle::NoBkgnd : MenuItemStyle::None;

			// menu items
			const Id_t menu_id = NE_UI_ID;

			const float item_h = Menu_PopUp_CalcItemHeight( dc, menu_main[0].Text );
			const Vec2_s menu_size = Menu_PopUp_CalcMenuSize( dc, menu_list_main );

			MenuState_s& menu_state = Context_CacheIntern< MenuState_s >( dc, menu_id );
			MenuItemState_s item_state = {};
			Rect_s item_rect = Rect_Ctor( pos.Caret, Vec2_s { menu_size.x, item_h } );
			for ( int i = 0; i < NeCountOf( menu_main ); ++i )
			{
				item_state.Hot = ((menu_main+i) == menu_state.Hot);
				item_state.Activated = false;
				Menu_PopUp_Item_Do( dc, menu_id, item_rect, item_style, menu_main[i], item_state );
				Column_AddLine( pos, Rect_Size( item_rect ) );
				Column_SpaceY( pos );
				if (item_state.Hot)
					menu_state.Hot = menu_main+i;
				if (item_state.Activated)
					menu_state.Expanded[0] = menu_main[i].Child.NumItems ? menu_main+i : nullptr;
				item_rect.x = pos.Caret.x;
				item_rect.y = pos.Caret.y;
			}

			Column_SpaceY( pos );
			Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Expanded: %s", (menu_state.Expanded[0] ? menu_state.Expanded[0]->Text.Utf8 : "") ) );
			Column_SpaceY( pos );
			Column_SpaceY( pos );
			Column_SpaceY( pos );
		}

		// main menu
		{
			// label
			Column_DoHeader( pos, dc, "Main Menu" );

			// main menu
			{
				const Id_t menu_id = NE_UI_ID;
				const Vec2_s menu_size = Menu_Main_CalcMenuSize( dc, menu_list_main );
				const Rect_s menu_rect = { pos.Caret.x, pos.Caret.y, NeMax( child_rect.w - pos.Caret.x - 2.0f, menu_size.x ), menu_size.y }; 
				Menu_DoMain( dc, menu_id, menu_rect, menu_list_main, Context_CacheIntern< MenuState_s >( dc, menu_id ) );
				Column_AddLine( pos, Rect_Size( menu_rect ) );
			}

			Column_SpaceY( pos );
			Column_SpaceY( pos );
		}

		// context menu
		{
			Graphics_t g = Context_GetGraphics( dc );

			// label
			Column_DoHeader( pos, dc, "Context Menu" );

			// context menu
			{
				const Rect_s label_rect = { pos.Caret.x, pos.Caret.y, 200.0f, 50.0f };
				Label_Do( dc, label_rect, TextFormat::Center | TextFormat::Middle, "Context Menu #1" );
				Column_AddLine( pos, Rect_Size( label_rect ) );
				Column_SpaceY( pos );
				Graphics_DrawRect( g, label_rect, Color::Black );
				const MenuItem_s* activated_item = ContextMenu_Do( dc, NE_UI_ID, label_rect, menu_list_file );

				static const MenuItem_s* msg_box_item = nullptr;
				if (activated_item)
					msg_box_item = activated_item;

				if (msg_box_item)
				{
					bool done = false;
					MessageBox_Do( dc, NE_UI_ID, "Context Menu #1", msg_box_item->Text, done );
					if (done)
						msg_box_item = nullptr;
				}
			}

			// context menu
			{
				const Rect_s label_rect = { pos.Caret.x, pos.Caret.y, 200.0f, 50.0f };
				Label_Do( dc, label_rect, TextFormat::Center | TextFormat::Middle, "Context Menu #2" );
				Column_AddLine( pos, Rect_Size( label_rect ) );
				Column_SpaceY( pos );
				Graphics_DrawRect( g, label_rect, Color::Black );
				const MenuItem_s* activated_item = ContextMenu_Do( dc, NE_UI_ID, label_rect, menu_list_main );

				static const MenuItem_s* msg_box_item = nullptr;
				if (activated_item)
					msg_box_item = activated_item;

				if (msg_box_item)
				{
					bool done = false;
					MessageBox_Do( dc, NE_UI_ID, "Context Menu #2", msg_box_item->Text, done );
					if (done)
						msg_box_item = nullptr;
				}
			}
		}

		Column_AddLine( pos, Vec2_s { 0.0f, 10.0f } );
		return pos.Limit;
	}

	static Vec2_s View_Tab( Context_t dc )
	{
		Column_s pos;
		Column_Init( pos );

		const Rect_s child_rect = Context_GetChild( dc );

		const cstr_t item_text[] =
		{ "Output"
		, "Properties"
		, "Solution Explorer"
		, "Symbol Find Results"
		};
		static int selected_item = 0;

		// tab item
		{
			// label
			Column_DoHeader( pos, dc, "Tab Item" );

			// style
			static bool page_style = false;
			Column_SpaceY( pos );
			Column_AddLine( pos, CheckBox_Do( dc, NE_UI_ID, pos.Caret, "Frame", page_style ) );
			Column_SpaceY( pos );
			const TabStyle::Mask_t tab_style = page_style ? TabStyle::Frame : TabStyle::None;

			// items
			for ( int i = 0; i < NeCountOf( item_text ); ++i )
			{
				bool item_selected = (selected_item == i);
				const float item_h = TabItem_CalcHeight( dc, tab_style );
				const float item_w = TabItem_CalcWidth( dc, tab_style, item_text[i] );
				const Rect_s item_rect = { pos.Caret.x, pos.Caret.y, item_w, item_h };
				TabItem_Do( dc, NE_UI_ID_CHILD(i), item_rect, tab_style, item_text[i], item_selected );
				Column_AddLine( pos, Rect_Size( item_rect ) );
				Column_SpaceY( pos );
				if (item_selected)
					selected_item = i;
			}
		}

		Column_AddLine( pos, Vec2_s { 0.0f, 10.0f } );
		return pos.Limit;
	}

	static Vec2_s View_Combo( Context_t dc )
	{
		const Id_t id = NE_UI_ID;
		const Rect_s child_rect = Context_GetChild( dc );

		const cstr_t items[] = 
		{ "First"
		, "Second"
		, "Third"
		};

		const float text_max_w = TextList_CalcMaxWidth( dc, items, NeCountOf( items ) ) + 8.0f;

		Column_s pos;
		Column_Init( pos );
		{
			const Id_t child_id = Id_Cat( id, "Items" );

			// label
			Column_DoHeader( pos, dc, "DropDown Item" );

			// items
			const Vec2_s size = DropDownItem_CalcSize( dc, child_rect.w - 2.0f * pos.Indent.x );
			for ( int i = 0; i < NeCountOf(items); ++i )
			{
				DropDownItem_Do( dc, Id_Cat( child_id, i ), { pos.Caret.x, pos.Caret.y, size.x, size.y }, items[i] );
				Column_AddLine( pos, size );
				Column_SpaceY( pos );
			}
			Column_SpaceY( pos );
		}
		{
			const Id_t child_id = Id_Cat( id, "List" );

			// label
			Column_DoHeader( pos, dc, "DropDown List" );

			// list
			static int sel = -1;
			const Vec2_s list_size = DropDownList_CalcSize( dc, text_max_w, NeCountOf(items) );
			const Rect_s list_rect = Rect_Ctor( pos.Caret, list_size );
			DropDownList_Do( dc, child_id, list_rect, items, NeCountOf(items), sel );
			Column_AddLine( pos, list_size );
			Column_SpaceY( pos );

			// selection
			Column_SpaceY( pos );
			Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Selected: %d [%s]", sel, (sel >= 0) ? items[sel] : nullptr ) );
			Column_SpaceY( pos );
		}
		{
			const Id_t child_id = Id_Cat( id, "Modal" );
			static int sel = -1;

			// label
			Column_DoHeader( pos, dc, "DropDownButton" );

			// button
			static bool expanded = false;
			Text_s button_label = (sel >= 0) ? items[sel] : "<Select>";
			const Vec2_s button_size = Button_CalcSize( dc, button_label );
			const Rect_s button_rect = Rect_Ctor( pos.Caret, Vec2_s { text_max_w + 20.0f, button_size.y } );
			const bool clicked = Button_Do( dc, Id_Cat( child_id, "Button" ), button_rect, button_label );
			Column_AddLine( pos, button_size );
			Column_SpaceY( pos );
			if (clicked)
				expanded = !expanded;
			if (expanded)
			{
				// calculate desktop space coordinate for the caret position
				Graphics_t g = Context_GetGraphics( dc );
				const Rect_s screen_rect = Context_GetDektop( dc );
				const Vec2_s child_to_screen = Graphics_GetTransform( g );
				const Vec2_s screen_pos = pos.Caret + child_to_screen;

				// calculate list size
				const Vec2_s list_size = DropDownList_CalcSize( dc, text_max_w, NeCountOf(items) );

				// select between drop-down (default) and pull-up
				//	depending on whether the list will fit onto
				//	the screen when dropped down
				const bool drop_down = (screen_pos.y + list_size.y) < screen_rect.h;
				const Rect_s list_rect = drop_down 
					? Rect_Ctor( pos.Caret, list_size ) 
					: Rect_s { pos.Caret.x, pos.Caret.y - list_size.y - button_size.y, list_size.x, list_size.y }
					;

				// do the modal list
				if (DropDownList_DoModal( dc, Id_Cat( child_id, "List" ), list_rect, items, NeCountOf(items), sel ))
					expanded = false;
			}
		}
		{
			const Id_t child_id = Id_Cat( id, "DropDown Box" );

			// label
			Column_DoHeader( pos, dc, "DropDownBox" );

			// box
			static int sel = -1;
			static bool expanded = false;
			const Id_t box_id = Id_Cat( child_id, "Box" );
			const Vec2_s box_size = DropDownBox_CalcSize( dc, text_max_w );
			const Rect_s box_rect = Rect_Ctor( pos.Caret, box_size );
			DropDownBox_Do( dc, box_id, box_rect, items, NeCountOf(items), sel, expanded );
		}

		return pos.Limit;
	}

	static CollapseItem_s Demo_View[] = 
	{ { View_Graphics	, "Graphics"	, false }
	, { View_Font		, "Font"		, false }
	, { View_Labels		, "Labels"		, false }
	, { View_Buttons	, "Buttons"		, false }
	, { View_Ranges		, "Ranges"		, false }
	, { View_List		, "List"		, false }
	, { View_Tree		, "Tree"		, false }
	, { View_Tab		, "Tab"			, false }
	, { View_Menu		, "Menu"		, false }
	, { View_Combo		, "Combo"		, false }
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static Vec2_s View_UI( Context_t dc )
	{
		NeGuiScopedFont( dc, L"Consolas", 12.0f );

		Column_s pos;
		Column_Init( pos );

		const DragInfo_s& drag = Context_GetDrag( dc );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Active:   0x%016x", Context_GetWnd   ( dc ) ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Hot:      0x%016x", Context_GetHot   ( dc ) ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Pushed:   0x%016x", Context_GetPushed( dc ) ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Focused:  0x%016x", Context_GetFocus ( dc ) ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Dragging: 0x%016x - Pos: %.0f, %.0f", drag.Id, drag.Pos.x, drag.Pos.y ) );
		Column_SpaceY( pos );

		const Rect_s child = Context_GetChild( dc );
		const Vec2_s view  = Context_GetView ( dc );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Child: %.0f, %.0f", child.w, child.h ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "View:  %.0f, %.0f", view.x, view.y ) );

		return Column_End( pos );
	}

	static Vec2_s View_Mouse( Context_t dc )
	{
		NeGuiScopedFont( dc, L"Consolas", 12.0f );

		Column_s pos;
		Column_Init( pos );

		cstr_t state;
		const MouseState_s& mouse = Context_GetMouse( dc );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Pos:   x=%4.0f, y=%4.0f", mouse.Pos.x, mouse.Pos.y ) );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Wheel: %d", mouse.Wheel ) );
		for ( int i = 0; i < MB::COUNT; ++i )
		{
			if (mouse.Button[i].NumClicks == 2)
				state = "Double Click";
			else if (mouse.Button[i].IsPressed)
				state = "Pressed";
			else
				state = "";
			Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Button %d: %s", i, state ) );
		}
		return Column_End( pos );
	}

	static Vec2_s View_Keyboard( Context_t dc )
	{
		NeGuiScopedFont( dc, L"Consolas", 12.0f );

		Column_s pos;
		Column_Init( pos );

		const KeyboardState_s& kb = Context_GetKeyboard( dc );
		Column_Add( pos, Label_Do( dc, pos.Caret, "Pressed: " ) );
		for ( int i = 0; i < VK::COUNT; ++i )
		{
			if ( kb.Key[i].IsPressed )
				Column_Add( pos, Label_Fmt( dc, pos.Caret, "0x%02x ", i ) );
		}
		Column_EndLine( pos );
		Column_AddLine( pos, Label_Fmt( dc, pos.Caret, "Text: %s", kb.Char ) );
		return Column_End( pos );
	}

	static CollapseItem_s Debug_View[] = 
	{ { View_UI			, "UI"		, true }
	, { View_Mouse		, "Mouse"	, true }
	, { View_Keyboard	, "Keyboard", true }
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static Vec2_s View_Theme_Font( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		return Vec2_s { 100.0f, 100.0f };
	}

	static Vec2_s View_Theme_Metrics( Context_t dc )
	{
		/*
		Theme_t theme = Context_GetTheme( dc );
		FontId_t font_id = Theme_GetFont( theme );
		const int num_metrics = Theme_GetMetricCount( theme );

		// setup
		const Vec2_s margin  = { 4.0f, 4.0f };
		const Vec2_s spacing = { 8.0f, 4.0f };

		Vec2_s limit = {};
		Vec2_s caret = { margin.x, margin.y };
		Vec2_s size  = {};

		// labels
		{
			Graphics_t g = Context_GetGraphics( dc );
			const float line_height = Graphics_GetLineHeight( g, font_id );
			const float edit_height = TextEdit_CalcHeight( dc );
			const Vec2_s offset = { 0.0f, ceilf( 0.5f * (line_height - edit_height) ) };

			for ( int i = 0; i < num_metrics; ++i )
			{
				size = Label_Do( dc, caret - offset, Theme_GetMetricName( theme, i ) );
				size.y = edit_height;
				limit = Vec2_Max( limit, caret + size + spacing );
				caret.y += size.y + spacing.y;
			}
		}

		// controls
		{
			// x
			caret.x = limit.x;
			caret.y = margin.y;
			for ( int i = 0; i < num_metrics; ++i )
			{
				size = TextEdit_DoInt( dc, NE_UI_ID_CHILD( i ), caret, 50.0f, Theme_GetMetric( theme, i ).x );
				limit = Vec2_Max( limit, caret + size + spacing );
				caret.y += size.y + spacing.y;
			}

			// y
			caret.x = limit.x;
			caret.y = margin.y;
			for ( int i = 0; i < num_metrics; ++i )
			{
				size = TextEdit_DoInt( dc, NE_UI_ID_CHILD( i ), caret, 50.0f, Theme_GetMetric( theme, i ).y );
				limit = Vec2_Max( limit, caret + size + spacing );
				caret.y += size.y + spacing.y;
			}
		}

		return limit + margin;
		*/
		return Vec2_s {};
	}

	static Vec2_s View_Theme_Visuals( Context_t dc )
	{
		/*
		Theme_t theme = Context_GetTheme( dc );
		FontId_t font_id = Theme_GetFont( theme );
		const int num_visual = Theme_GetVisualCount( theme );

		// setup
		const Vec2_s margin  = { 4.0f, 4.0f };
		const Vec2_s spacing = { 8.0f, 4.0f };

		Vec2_s limit = {};
		Vec2_s caret = { margin.x, margin.y };
		Vec2_s size  = {};

		// labels
		{
			Graphics_t g = Context_GetGraphics( dc );
			const float line_height = Graphics_GetLineHeight( g, font_id );
			const float edit_height = TextEdit_CalcHeight( dc );
			const Vec2_s offset = { 0.0f, ceilf( 0.5f * (line_height - edit_height) ) };

			for ( int i = 0; i < num_visual; ++i )
			{
				size = Label_Do( dc, caret - offset, Theme_GetVisualName( theme, i ) );
				size.y = edit_height;
				limit = Vec2_Max( limit, caret + size + spacing );
				caret.y += size.y + spacing.y;
			}
		}

		return limit + margin;
		*/
		return Vec2_s {};
	}

	static CollapseItem_s Theme_View[] = 
	{ { View_Theme_Metrics	, "Metrics"	, false }
	, { View_Theme_Visuals	, "Visuals"	, true  }
	, { View_Theme_Font		, "Font"	, false }
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Demo Views

	Vec2_s DemoView_DoCtrl( Context_t dc )
	{
		Vec2_s item_size = {};
		const Rect_s child_rect = Context_GetChild( dc );
		const Vec2_s list_size = CollapseList_Do( dc, NE_UI_ID, &Demo_View[0], NeCountOf( Demo_View ), &item_size );
		const Vec2_s max_size = list_size + Vec2_s { 0.0f, child_rect.h - item_size.y };
		return max_size;
	}

	Vec2_s DemoView_DoDebug( Context_t dc )
	{
		return CollapseList_Do( dc, NE_UI_ID, &Debug_View[0], NeCountOf( Debug_View ) );
	}

	Vec2_s DemoView_DoTheme( Context_t dc )
	{
		return CollapseList_Do( dc, NE_UI_ID, &Theme_View[0], NeCountOf( Theme_View ) );
	}

	static Vec2_s DemoView_DoCtrl( Context_t dc, void* user )
	{
		return DemoView_DoCtrl( dc );
	}

	static Vec2_s DemoView_DoDebug( Context_t dc, void* user )
	{
		return DemoView_DoDebug( dc );
	}

	static Vec2_s DemoView_DoTheme( Context_t dc, void* user )
	{
		return DemoView_DoTheme( dc );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Demo

	static DesktopWindow_s Demo_Wnd[] = 
	{
		{ { NE_UI_ID, Rect_s(), "Controls", false }, { DemoView_DoCtrl  }, {} }
	,	{ { NE_UI_ID, Rect_s(), "Debug"	  , false }, { DemoView_DoDebug }, {} }
	,	{ { NE_UI_ID, Rect_s(), "Theme"	  , false }, { DemoView_DoTheme }, {} }
	};

	void Demo_Initialize()
	{
		Context_t dc = System_GetContext();
		Demo_Wnd[0].Wnd.Rect = Rect_Ctor( Vec2_s { 420.0f,  10.0f }, Window_CalcSize( dc, Vec2_s { 800.0f, 600.0f } ) );
		Demo_Wnd[1].Wnd.Rect = Rect_Ctor( Vec2_s {  10.0f,  10.0f }, Window_CalcSize( dc, Vec2_s { 400.0f, 320.0f } ) );
		Demo_Wnd[2].Wnd.Rect = Rect_Ctor( Vec2_s {  10.0f, 360.0f }, Window_CalcSize( dc, Vec2_s { 400.0f, 250.0f } ) );
	}

	void Demo_Render()
	{
		Context_t dc = System_GetContext();
		Desktop_Do( dc, Demo_Wnd, NeCountOf(Demo_Wnd) );
	}

	void Demo_Shutdown()
	{
	}

} }

