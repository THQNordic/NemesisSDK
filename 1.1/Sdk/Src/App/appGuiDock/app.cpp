//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "app.h"
#include "gui_d3d11.h"
#include "dock_d3d11.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::system;
using namespace nemesis::graphics;
using namespace nemesis::gui;

//======================================================================================
static const cstr_t Visual_Str[] = 
{ "Label"
, "Button"
, "CheckBox.Box"
, "CheckBox.Check"
, "TextEdit"
, "TextEdit.Caret"
, "TextEdit.Selection"
, "Progress.Track"
, "Progress.Bar"
, "Slider.Track"
, "Slider.Thumb"
, "ScrollBar.Track"
, "ScrollBar.Thumb"
, "ScrollBar.Arrow"
, "ListHeader.Item"
, "ListColumn.Item"
, "ListColumn.Selection"
, "List"
, "List.Grid"
, "Tree"
, "Tree.Item"
, "Tree.Item.Arrow"
, "Tree.Selection"
, "Tab"
, "Tab.Item"
, "Menu.Main"
, "Menu.Main.Item"
, "Menu.PopUp"
, "Menu.PopUp.Icon"
, "Menu.PopUp.Item"
, "Menu.PopUp.Item.Arrow"
, "Menu.PopUp.Separator"
, "DropDown"
, "DropDown.Item"
, "DropDown.Box"
, "Caption"
, "Expander"
, "Expander.Arrow"
, "Window"
, "ToolBar"
, "Splitter"
, "Frame.Tab"
, "Frame.Tab.Item"
, "Frame.Border"
, "DockIndicator"
, "DockIndicator.Target"
};

NeStaticAssert(NeCountOf(Visual_Str) == Visual::COUNT);

static cstr_t Visual_ToStr( Visual::Enum kind )
{
	if ((kind >= 0) && (kind < Visual::COUNT))
		return Visual_Str[kind];
	return "<invalid>";
}

//--------------------------------------------------------------------------------------
static const cstr_t CtrlState_Str[] = 
{ "Normal"
, "Hot"
, "Pushed"
, "Disabled"
};

NeStaticAssert(NeCountOf(CtrlState_Str) == CtrlState::COUNT);

static cstr_t CtrlState_ToStr( CtrlState::Enum kind )
{
	if ((kind >= 0) && (kind < CtrlState::COUNT))
		return CtrlState_Str[kind];
	return "<invalid>";
}

//======================================================================================
typedef char Path_t[256];

//======================================================================================
// App - Private
//======================================================================================
struct PropState_s
{
	float LabelWidth;
};

struct App_s
{
	Allocator_t		Alloc;
	Gfx_d3d11_t		Gfx;
	Gui_d3d11_t		Gui;
	DockMgr_t		Dock;
	HWND			Wnd;
	bool			Focus;
	bool			Minimized;
	StopWatch_c		ElapsedTime;
	MouseState_s	Mouse;
	KeyboardState_s	Keyboard;

	Array<DockCtrl_t> Ctrls;
	DockCtrl_t Clicked;

	List_s ThemeList;
	Path_t ThemePath;
	Path_t ThemeName;

	PropState_s PropState;
};

static App_s App = {};

//--------------------------------------------------------------------------------------
static bool Shell_OpenTheme( HWND hWnd, Path_t& res_path, Path_t& file_name )
{
	OPENFILENAMEA ofn;
	NeZero(ofn);
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = file_name;
	ofn.nMaxFile = sizeof(file_name);
	ofn.lpstrFilter = "Theme Files (*.thm)\0*.thm\0Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = res_path;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;	
	if (!GetOpenFileNameA( &ofn ))
		return false;

	cstr_t pos = strrchr( file_name, '\\' );
	NeAssert(pos);

	Path_t path;
	Path_t file;
	ptrdiff_t at = pos-file_name;
	size_t len = strlen(file_name);
	size_t end = len-(at+1);
	strncpy_s( path, file_name			, at  );
	strncpy_s( file, file_name + at + 1	, end );
	path[at] = 0;
	file[end] = 0;
	Str_Cpy(res_path, path);
	Str_Cpy(file_name, file);
	return true;
}

static bool Shell_SaveTheme( HWND hWnd, Path_t& res_path, Path_t& file_name )
{
	OPENFILENAMEA ofn;
	NeZero(ofn);
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = file_name;
	ofn.nMaxFile = sizeof(file_name);
	ofn.lpstrFilter = "Theme Files (*.thm)\0*.thm\0Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = res_path;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameA( &ofn ))
		return false;

	cstr_t pos = strrchr( file_name, '\\' );
	NeAssert(pos);

	Path_t path;
	Path_t file;
	ptrdiff_t at = pos-file_name;
	size_t len = strlen(file_name);
	size_t end = len-(at+1);
	strncpy_s( path, file_name			, at  );
	strncpy_s( file, file_name + at + 1	, end );
	path[at] = 0;
	file[end] = 0;
	Str_Cpy(res_path, path);
	Str_Cpy(file_name, file);
	return true;
}

struct Buffer_s
{
	Allocator_t Alloc;
	ptr_t		Data;
	size_t		Size;
};

static void Buffer_Free( Buffer_s& buffer )
{
	Mem_Free( buffer.Alloc, buffer.Data );
}

static Buffer_s File_Read( cstr_t file_name, Allocator_t alloc )
{
	FILE* file = nullptr;
	fopen_s( &file, file_name, "rb" );
	if (!file)
		return {};
	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	if (size <= 0)
		return {};
	fseek( file, 0, SEEK_SET );
	Buffer_s buffer = { alloc, Mem_Alloc( alloc, size+1 ), (size_t)size };
	const size_t read = fread( buffer.Data, buffer.Size, 1, file );
	fclose( file );
	uint8_t* end = ((uint8_t*)buffer.Data) + buffer.Size;
	*end = 0;
	return buffer;
}

static bool File_Write( cstr_t file_name, cptr_t data, size_t size )
{
	FILE* file = nullptr;
	fopen_s( &file, file_name, "wb" );
	if (!file)
		return false;
	const size_t written = fwrite( data, size, 1, file );
	fclose( file );
	return written;
}

static int Visual_Find( cstr_t find )
{
	for ( int i = 0; i < Visual::COUNT; ++i )
	{
		cstr_t name = Visual_ToStr( (Visual::Enum)i );
		if (Str_EqNoCase( name, find ))
			return i;
	}
	return -1;
}

static bool Theme_Read( const JsonDoc_s& doc )
{
	const JsonNode_s* json_visuals = JsonNode_FindMember( doc.Head, "Visuals" );
	if (!json_visuals)
		return false;

	Context_t dc = System_GetContext();
	Theme_t theme = Context_GetTheme( dc );

	struct Parse
	{
		static bool Color( const JsonNode_s* json, cstr_t label , uint32_t& v )
		{
			cstr_t text = nullptr;
			JsonNode_Read_Member( json, label, &text );
			if (!text)
				return false;

			uint32_t a = 0;
			uint32_t r = 0;
			uint32_t g = 0;
			uint32_t b = 0;
			sscanf_s( text, "%02x %02x %02x %02x", &a, &r, &g, &b );

			v  = (a << 0);
			v |= (r << 8);
			v |= (g << 16);
			v |= (b << 24);
			return true;
		}
	};

	for ( const JsonNode_s* json = json_visuals->Member; json; json = json->Next )
	{
		int id = 0;
		cstr_t name = nullptr;
		JsonNode_Read_Member( json, "Id", &id );
		JsonNode_Read_Member( json, "Name", &name );

		cstr_t expected = Visual_ToStr( (Visual::Enum)id );
		if (!Str_EqNoCase(expected, name))
			id = Visual_Find( name );
		if (id < 0)
			continue;

		Visual_s& v = theme->Visual[id];

		Parse::Color( json, "Back.Normal"	, v.Back[CtrlState::Normal]		);
		Parse::Color( json, "Back.Hot"		, v.Back[CtrlState::Hot]		);
		Parse::Color( json, "Back.Pushed"	, v.Back[CtrlState::Pushed]		);
		Parse::Color( json, "Back.Disabled"	, v.Back[CtrlState::Disabled]	);

		Parse::Color( json, "Front.Normal"	, v.Front[CtrlState::Normal]	);
		Parse::Color( json, "Front.Hot"		, v.Front[CtrlState::Hot]		);
		Parse::Color( json, "Front.Pushed"	, v.Front[CtrlState::Pushed]	);
		Parse::Color( json, "Front.Disabled", v.Front[CtrlState::Disabled]	);

		Parse::Color( json, "Text.Normal"	, v.Text[CtrlState::Normal]		);
		Parse::Color( json, "Text.Hot"		, v.Text[CtrlState::Hot]		);
		Parse::Color( json, "Text.Pushed"	, v.Text[CtrlState::Pushed]		);
		Parse::Color( json, "Text.Disabled"	, v.Text[CtrlState::Disabled]	);

	}

	return true;
}

static void Theme_Write( JsonOut_s& json )
{
	Context_t dc = System_GetContext();
	Theme_t theme = Context_GetTheme( dc );

	Path_t temp = "";

	struct Fmt
	{
		static cstr_t Color( Path_t& temp, uint32_t v )
		{
			const uint32_t a = ((v >>  0) & 0xff);
			const uint32_t r = ((v >>  8) & 0xff);
			const uint32_t g = ((v >> 16) & 0xff);
			const uint32_t b = ((v >> 24) & 0xff);
			Str_Fmt( temp, "%02x %02x %02x %02x", a, r, g, b );
			return temp;
		}
	};

	struct Out
	{
		static void Color( JsonOut_s& json, cstr_t name, Path_t& temp, uint32_t v )
		{
			JsonOut_Field( json, name );
			JsonOut_Value( json, Fmt::Color( temp, v ) );
		}
	};

	JsonOut_Object( json );
	JsonOut_Array( json, "Visuals" );
	{
		for ( int i = 0; i < Visual::COUNT; ++i )
		{
			const Visual_s& v = theme->Visual[i];
			JsonOut_Object( json );

			JsonOut_Field( json, "Id" );
			JsonOut_Value( json, i );

			JsonOut_Field( json, "Name" );
			JsonOut_Value( json, Visual_ToStr( (Visual::Enum)i ) );

			Out::Color( json, "Back.Normal"		, temp, v.Back[CtrlState::Normal]	);
			Out::Color( json, "Back.Hot"		, temp, v.Back[CtrlState::Hot]		);
			Out::Color( json, "Back.Pushed"		, temp, v.Back[CtrlState::Pushed]	);
			Out::Color( json, "Back.Disabled"	, temp, v.Back[CtrlState::Disabled] );

			Out::Color( json, "Front.Normal"	, temp, v.Front[CtrlState::Normal]	 );
			Out::Color( json, "Front.Hot"		, temp, v.Front[CtrlState::Hot]		 );
			Out::Color( json, "Front.Pushed"	, temp, v.Front[CtrlState::Pushed]	 );
			Out::Color( json, "Front.Disabled"	, temp, v.Front[CtrlState::Disabled] );

			Out::Color( json, "Text.Normal"		, temp, v.Text[CtrlState::Normal]	);
			Out::Color( json, "Text.Hot"		, temp, v.Text[CtrlState::Hot]		);
			Out::Color( json, "Text.Pushed"		, temp, v.Text[CtrlState::Pushed]	);
			Out::Color( json, "Text.Disabled"	, temp, v.Text[CtrlState::Disabled] );

			JsonOut_End( json );
		}
	}
	JsonOut_End( json );
	JsonOut_End( json );
}

static bool App_HasTheme()
{
	return App.ThemeName[0];
}

static void App_OpenTheme( cstr_t path, cstr_t file )
{
	Path_t file_name = "";
	Str_Cpy( file_name, path );
	Str_Cat( file_name, "\\" );
	Str_Cat( file_name, file );

	Buffer_s buffer = File_Read( file_name, App.Alloc );
	if (!buffer.Size)
		return;

	Result_t hr;
	JsonDoc_s doc = {};
	JsonError_s err = {};
	JsonDoc_Init( doc, App.Alloc );
	hr = JsonDoc_Parse( doc, (cstr_t)buffer.Data, buffer.Size, err );
	if (NeSucceeded(hr))
	{
		if (Theme_Read( doc ))
		{
			Str_Cpy( App.ThemePath, path );
			Str_Cpy( App.ThemeName, file );
		}
	}

	JsonDoc_Clear( doc );
	Buffer_Free( buffer );
}

static void App_SaveTheme( cstr_t path, cstr_t file )
{
	Path_t file_name = "";
	Str_Cpy( file_name, path );
	Str_Cat( file_name, "\\" );
	Str_Cat( file_name, file );

	JsonOut_s json = {};
	JsonOut_Initialize( json, App.Alloc );
	Theme_Write( json );

	const bool ok = File_Write( file_name, json.Text.Data, json.Text.Count-1 );
	JsonOut_Shutdown( json );

	if (!ok)
		return;

	Str_Cpy( App.ThemePath, path );
	Str_Cpy( App.ThemeName, file );
}

static void App_OpenTheme()
{
	Path_t path = "";
	Path_t file = "";
	if(!Shell_OpenTheme( App.Wnd, path, file ))
		return;
	App_OpenTheme( path, file );
}

static void App_ReloadTheme()
{
	App_OpenTheme( App.ThemePath, App.ThemeName );
}

static void App_SaveTheme()
{
	App_SaveTheme( App.ThemePath, App.ThemeName );
}

static void App_SaveThemeAs()
{
	Path_t path = "";
	Path_t file = "";
	if(!Shell_SaveTheme( App.Wnd, path, file ))
		return;
	App_SaveTheme( path, file );
}

//--------------------------------------------------------------------------------------
static Rect_s App_GetClientRect()
{
	RECT rc;
	GetClientRect(App.Wnd, &rc);
	return Rect_s { 0.0f, 0.0f, float(rc.right-rc.left), float(rc.bottom-rc.top) };
}

static float DragInfo_CalcHeight( gui::Context_t dc )
{
	using namespace nemesis::gui;
	const float line_h = Context_GetLineHeight( dc );
	return 2.0f + (4.0f * line_h + 1.0f) + 2.0f;
}

static void DragInfo_Do( gui::Context_t dc, gui::Id_t id )
{
	using namespace nemesis::gui;
	const Rect_s r = Context_GetChild( dc );
	Ctrl_DrawBox( dc, r, Visual::List_Grid, CtrlState::Normal );
	DockCtrl_t drag_source = DockMgr_Dbg_GetDragSource( App.Dock );
	if (!drag_source)
		return;
	const Rect_s inner_rect = Rect_Margin( r, 2.0f );
	const float line_h = Context_GetLineHeight( dc );
	Vec2_s pos = { inner_rect.x, inner_rect.y };

	const Vec2_s drag_begin = DockMgr_Dbg_GetDragBegin( App.Dock );
	const Vec2_s drag_mouse = DockMgr_Dbg_GetDragMouse( App.Dock );
	DockCtrl_t drop_target = DockMgr_Dbg_GetDragTarget( App.Dock );
	cstr_t drag_source_text = DockCtrl_GetText( drag_source );
	cstr_t drop_target_text = drop_target ? DockCtrl_GetText( drop_target ) : "(none)";
	Label_Fmt( dc, pos, "Source: %s", drag_source_text );					pos.y += line_h + 1.0f;
	Label_Fmt( dc, pos, "Push: %.0f, %.0f", drag_begin.x, drag_begin.y );	pos.y += line_h + 1.0f;
	Label_Fmt( dc, pos, "Drag: %.0f, %.0f", drag_mouse.x, drag_mouse.y );	pos.y += line_h + 1.0f;
	Label_Fmt( dc, pos, "Target: %s", drop_target_text );					pos.y += line_h + 1.0f;
}

static void ControlList_Do( gui::Context_t dc, gui::Id_t id )
{
	using namespace nemesis::gui;
	const Rect_s r = Context_GetChild( dc );
	const Graphics_t g = Context_GetGraphics( dc );
	Graphics_FillRect( g, r, Color::LightGray );
	struct Callback
	{
		static Text_s NE_CALLBK DoIndex( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			return Context_FormatString( dc, "%d", row );
		}

		static Text_s NE_CALLBK DoKind( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			return DockCtrl_GetKindString( ctrl );
		}

		static Text_s NE_CALLBK DoName( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			return DockCtrl_GetText( ctrl );
		}

		static Text_s NE_CALLBK DoSide( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			return DockCtrl_GetSideString( ctrl );
		}

		static Text_s NE_CALLBK DoFloat( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			const Vec2_s fp = DockCtrl_Dbg_GetFloatPos( ctrl );
			return Context_FormatString( dc, "%.0f, %.0f", fp.x, fp.y );
		}

		static Text_s NE_CALLBK DoLocal( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			const Rect_s lr = DockCtrl_Dbg_GetLocalRect( ctrl );
			return Context_FormatString( dc, "%.0f, %.0f; %.0f, %.0f", lr.x, lr.y, lr.w, lr.h );
		}

		static Text_s NE_CALLBK DoNc( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			const Pad_s nc = DockCtrl_Dbg_GetNonClient( ctrl );
			return Context_FormatString( dc, "%.0f, %.0f, %.0f, %.0f", nc.l, nc.t, nc.r, nc.b );
		}

		static Text_s NE_CALLBK DoScreen( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DockCtrl_t ctrl = DockMgr_Dbg_GetCtrl( App.Dock, row );
			const Rect_s rc = DockCtrl_GetScreenRect( ctrl );
			return Context_FormatString( dc, "%.0f, %.0f; %.0f, %.0f", rc.x, rc.y, rc.w, rc.h );
		}
	};

	static ListHeaderItem_s list_headers[] = 
	{ { "Index"	,  80.0f, Alignment::Left, SortOrder::None }
	, { "Kind"	, 100.0f, Alignment::Left, SortOrder::None }
	, { "Name"	, 100.0f, Alignment::Left, SortOrder::None }
	, { "Float"	, 100.0f, Alignment::Left, SortOrder::None }
	, { "Local" , 100.0f, Alignment::Left, SortOrder::None }
	, { "Nc"	, 100.0f, Alignment::Left, SortOrder::None }
	, { "Screen", 100.0f, Alignment::Left, SortOrder::None }
	, { "Side"	, 100.0f, Alignment::Left, SortOrder::None }
	};

	static ListColumn_s list_columns[] = 
	{ { nullptr, nullptr, { Callback::DoIndex	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoKind	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoName	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoFloat	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoLocal	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoNc		, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoScreen	, nullptr }, Alignment::Left }
	, { nullptr, nullptr, { Callback::DoSide	, nullptr }, Alignment::Left }
	};

	ListState_s& list_state = Context_CacheIntern<ListState_s>( dc, id );
	const ListStyle::Mask_t list_style = ListStyle::HorzLines | ListStyle::VertLines;
	List_Do( dc, id, r, list_headers, list_columns, NeCountOf(list_columns), DockMgr_Dbg_GetNumCtrls( App.Dock ), list_style, list_state );
}

static void NE_CALLBK InfoCtrl_Do( DockCtrl_t ctrl, ptr_t user )
{
	using namespace nemesis::gui;
	const Id_t id = DockCtrl_GetId( ctrl );
	const Context_t dc = DockCtrl_GetDc( ctrl );
	const Rect_s r = Context_GetChild( dc );
	const float drag_h = DragInfo_CalcHeight( DockCtrl_GetDc( ctrl ) );
	const Rect_s ctrl_rect = { r.x, r.y, r.w, r.y + r.h - drag_h };
	const Rect_s drag_rect = { r.x, r.y + r.h - drag_h, r.w, drag_h };
	{
		NeGuiScopedChild(dc, ctrl_rect);
		ControlList_Do( dc, Id_Cat( id, "Controls" ) );
	}
	{
		NeGuiScopedChild(dc, drag_rect);
		DragInfo_Do( dc, Id_Cat( id, "Drag" ) );
	}
}

static void NE_CALLBK TestCtrl_Do( DockCtrl_t ctrl, ptr_t user )
{
	using namespace nemesis::gui;

	const Id_t id = DockCtrl_GetId( ctrl );
	const Context_t dc = DockCtrl_GetDc( ctrl );
	const Rect_s rc = Context_GetChild( dc );

	const Graphics_t g = Context_GetGraphics( dc );
	Graphics_FillRect( g, rc, Color::LightGray );
	Graphics_DrawRect( g, rc, Color::Goldenrod );

	const cstr_t text = DockCtrl_GetText( ctrl );
	const Vec2_s fp = DockCtrl_Dbg_GetFloatPos( ctrl );
	const Rect_s lr = DockCtrl_Dbg_GetLocalRect( ctrl );
	const Pad_s  nc = DockCtrl_Dbg_GetNonClient( ctrl );
	const Rect_s sr = DockCtrl_GetScreenRect( ctrl );
	const float  ss = DockCtrl_Dbg_GetSplitSize( ctrl );

	const cstr_t btn_text = text ? text : "Click me!";
	const Vec2_s btn_size = Button_CalcSize( dc, btn_text );
	const Rect_s btn_rect = { 10.0f, 10.0f, btn_size.x, btn_size.y };
	if (Button_Do( dc, id, btn_rect, btn_text ))
		App.Clicked = ctrl;

	const Theme_t theme = Context_GetTheme( dc );
	const float line_h = Context_GetLineHeight( dc );
	const uint32_t label_color = theme->Visual[Visual::Label].Text[CtrlState::Normal];
	theme->Visual[Visual::Label].Text[CtrlState::Normal] = Color::Black;
	Vec2_s caret = { btn_rect.x, btn_rect.y + btn_rect.h + 2.0f };
	Label_Fmt( dc, caret, "FP: %4.0f,%4.0f, SZ: %.2f"	, fp.x, fp.y, ss );			caret.y += line_h + 2.0f;
	Label_Fmt( dc, caret, "SR: %4.0f,%4.0f, %4.0f,%4.0f", sr.x, sr.y, sr.w, sr.h );	caret.y += line_h + 2.0f;
	Label_Fmt( dc, caret, "LR: %4.0f,%4.0f, %4.0f,%4.0f", lr.x, lr.y, lr.w, lr.h );	caret.y += line_h + 2.0f;
	Label_Fmt( dc, caret, "CR: %4.0f,%4.0f, %4.0f,%4.0f", rc.x, rc.y, rc.w, rc.h );	caret.y += line_h + 2.0f;
	Label_Fmt( dc, caret, "NC: %4.0f,%4.0f, %4.0f,%4.0f", nc.l, nc.t, nc.r, nc.b );	caret.y += line_h + 2.0f;
	theme->Visual[Visual::Label].Text[CtrlState::Normal] = label_color;

	const float max_w = rc.w - caret.x - btn_rect.x;
	ProgressBar_Do( dc, { caret.x, caret.y, max_w, line_h } , 0.0f, 1.0f, fmodf(App.ElapsedTime.ElapsedSec(), 1.0f)  );
}

static void ThemeList_Init( Context_t dc, Id_t id )
{
	if (App.ThemeList.Id == id)
		return;

	struct Local
	{
		enum Part
		{ Back
		, Front
		, Text
		, COUNT
		};

		static uint32_t GetColor( const Visual_s& v, Part part, CtrlState::Enum state ) 
		{
			switch (part)
			{
			case Back:	return v.Back [state];
			case Front:	return v.Front[state];
			case Text:	return v.Text [state];
			default:
				IMPLEMENT_ME;
				break;
			}
			return 0;
		}

		static void DoColor( Context_t dc, const Rect_s& r, Part part, int row )
		{
			Graphics_t g = Context_GetGraphics( dc );
			const Rect_s inner_rect = Rect_Margin( r, 2.0f );
			const float color_size = NeMin( inner_rect.w, inner_rect.h );

			Theme_t theme = Context_GetTheme( dc );
			const Visual::Enum	 visual_id = (Visual	::Enum)(row / CtrlState::COUNT);
			const CtrlState::Enum state_id = (CtrlState	::Enum)(row % CtrlState::COUNT);
			const Visual_s& visual = theme->Visual[visual_id];
			const uint32_t color_value = GetColor( visual, part, state_id );
			{
				// draw color indicator
				const Rect_s color_rect = { inner_rect.x, inner_rect.y, color_size, color_size };
				Graphics_FillRect( g, color_rect, color_value );
				Graphics_DrawRect( g, color_rect, Color::Black );
			}
			{
				// draw value label
				const float label_off_x = color_size + 2.0f;
				const Rect_s label_rect = 
					{ inner_rect.x + label_off_x
					, inner_rect.y
					, inner_rect.w - label_off_x
					, inner_rect.h 
					};
				const uint32_t c_a = ((color_value >>  0) & 0xff);
				const uint32_t c_r = ((color_value >>  8) & 0xff);
				const uint32_t c_g = ((color_value >> 16) & 0xff);
				const uint32_t c_b = ((color_value >> 24) & 0xff);
				Label_Fmt( dc, label_rect, TextFormat::Left | TextFormat::Middle, "ARGB: %02x %02x %02x %02x", c_a, c_r, c_g, c_b );
			}
		}

		static Text_s NE_CALLBK DoName( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			const int visual_id = (row / CtrlState::COUNT);
			return Visual_ToStr( (Visual::Enum)visual_id );
		}

		static Text_s NE_CALLBK DoState( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			const int state_id = (row % CtrlState::COUNT);
			return CtrlState_ToStr( (CtrlState::Enum)state_id );
		}

		static Text_s NE_CALLBK DoBack( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DoColor( dc, r, Part::Back, row );
			return "";
		}

		static Text_s NE_CALLBK DoFront( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DoColor( dc, r, Part::Front, row );
			return "";
		}

		static Text_s NE_CALLBK DoText( void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state )
		{
			DoColor( dc, r, Part::Text, row );
			return "";
		}
	};

	static ListHeaderItem_s header_items[] = 
	{ { "Visual"	, 150.0f, Alignment::Left, SortOrder::None }
	, { "State"		, 150.0f, Alignment::Left, SortOrder::None }
	, { "Background", 150.0f, Alignment::Left, SortOrder::None }
	, { "Foreground", 150.0f, Alignment::Left, SortOrder::None }
	, { "Text"		, 150.0f, Alignment::Left, SortOrder::None }
	};

	static ListColumn_s list_columns[] =
	{ { nullptr, nullptr, { Local::DoName	, &App }, header_items[0].Alignment }
	, { nullptr, nullptr, { Local::DoState	, &App }, header_items[1].Alignment }
	, { nullptr, nullptr, { Local::DoBack	, &App }, header_items[2].Alignment }
	, { nullptr, nullptr, { Local::DoFront	, &App }, header_items[3].Alignment }
	, { nullptr, nullptr, { Local::DoText	, &App }, header_items[4].Alignment }
	};

	NeStaticAssert(NeCountOf(header_items) == NeCountOf(list_columns));

	App.ThemeList.Id = NE_UI_ID;
	App.ThemeList.Data.Header = header_items;
	App.ThemeList.Data.NumColumns = NeCountOf(header_items);
	App.ThemeList.Data.Column = list_columns;
	App.ThemeList.Data.NumRows = Visual::COUNT * CtrlState::COUNT;
}

static void ThemeList_Do( Context_t dc, Id_t id, const Rect_s& r, List_s& list )
{
	ThemeList_Init( dc, id );
	list.Rect = r;
	List_Do( dc, list );
}

static bool ToolBar_DoItem( Context_t dc, Id_t id, Rect_s& caret, cstr_t label, bool enabled )
{
	caret.w = Button_CalcSize(dc, label).x;
	const ButtonStyle::Mask_t style = enabled 
		? ButtonStyle::None 
		: ButtonStyle::Disabled;
	const bool clicked = Button_Do(dc, id, caret, label, style );
	caret.x += caret.w;
	caret.x += 2.0f;
	return clicked;
}

static void ToolBar_DoSeparator( Context_t dc, Id_t id, Rect_s& caret )
{
	Theme_t theme = Context_GetTheme( dc );
	Graphics_t g = Context_GetGraphics( dc );
	const uint32_t c = theme->Visual[Visual::Button].Text[CtrlState::Disabled];
	const Rect_s r = Rect_s { caret.x, caret.y + 2.0f, 1.0f, caret.h - 4.0f };
	Graphics_FillRect(g, r, c);
	caret.x += 2.0f;
}

static void ThemeBar_Do( Context_t dc, Id_t id, const Rect_s& r )
{
	Ctrl_DrawBox( dc, r, Visual::Window, CtrlState::Normal );

	const float h = Button_CalcHeight(dc);
	const float x = 2.0f;
	Rect_s caret = { r.x + x, r.y, 0.0f, h };
	
	const bool save_enabled = App_HasTheme();
	Path_t save_text = "Save";
	if (save_enabled)
	{
		Str_Cat( save_text, " " );
		Str_Cat( save_text, App.ThemeName );
	}

	int i = 0;
	if (ToolBar_DoItem(dc, Id_Cat(id, i++), caret, "Open", true))
	{
		App_OpenTheme();
	}
	if (ToolBar_DoItem(dc, Id_Cat(id, i++), caret, "Reload", save_enabled))
	{
		App_ReloadTheme();
	}
	if (ToolBar_DoItem(dc, Id_Cat(id, i++), caret, save_text, save_enabled))
	{
		App_SaveTheme();
	}
	if (ToolBar_DoItem(dc, Id_Cat(id, i++), caret, "Save As...", true))
	{
		App_SaveThemeAs();
	}
}

static void NE_CALLBK ThemeCtrl_Do( DockCtrl_t ctrl, ptr_t user )
{
	Context_t dc = DockCtrl_GetDc( ctrl );
	const Id_t id = DockCtrl_GetId( ctrl );
	const Rect_s r = Context_GetChild( dc );
	Graphics_t g = Context_GetGraphics( dc );

	const float tool_h = Button_CalcHeight( dc );
	const Id_t tool_id = Id_Cat( id, "Toolbar" );
	const Rect2_s split_tool_list = Rect_SplitT( r, tool_h );
	ThemeBar_Do( dc, tool_id, split_tool_list.Rect[0] );

	const Id_t list_id = Id_Cat( id, "List" );
	ThemeList_Do( dc, list_id, split_tool_list.Rect[1], App.ThemeList );
}

static void NE_CALLBK Value_DoColor( Context_t dc, Id_t id, const Rect_s& r, uint32_t& v )
{
	int32_t col_a = (int)((v >>  0) & 0xff);
	int32_t col_r = (int)((v >>  8) & 0xff);
	int32_t col_g = (int)((v >> 16) & 0xff);
	int32_t col_b = (int)((v >> 24) & 0xff);
	const float w = r.w / 4.0f - 2.0f;
	Rect_s rc_a = { r.x + 0 * (w + 2.0f), r.y, w, r.h };
	Rect_s rc_r = { r.x + 1 * (w + 2.0f), r.y, w, r.h };
	Rect_s rc_g = { r.x + 2 * (w + 2.0f), r.y, w, r.h };
	Rect_s rc_b = { r.x + 3 * (w + 2.0f), r.y, w, r.h };
	TextEdit_DoInt( dc, Id_Cat( id, 'a' ), rc_a, col_a );
	TextEdit_DoInt( dc, Id_Cat( id, 'r' ), rc_r, col_r );
	TextEdit_DoInt( dc, Id_Cat( id, 'g' ), rc_g, col_g );
	TextEdit_DoInt( dc, Id_Cat( id, 'b' ), rc_b, col_b );
	v = (((uint32_t)col_a) <<  0)
	  | (((uint32_t)col_r) <<  8)
	  | (((uint32_t)col_g) << 16)
	  | (((uint32_t)col_b) << 24);
}

static void NE_CALLBK PropsCtrl_Do( DockCtrl_t ctrl, ptr_t user )
{
	Context_t dc = DockCtrl_GetDc( ctrl );
	const Id_t id = DockCtrl_GetId( ctrl );
	const Rect_s r = Context_GetChild( dc );
	Ctrl_DrawBox( dc, r, Visual::Window, Ctrl_GetState( dc, id ) );

	if (App.ThemeList.State.SelRow < 0)
		return;

	// setup constants
	const Vec2_s space = { 4.0f, 2.0f };
	const Pad_s pad = { 4.0f, 4.0f, 6.0f, 4.0f };
	const cstr_t labels[] = 
		{ "Background"
		, "Foreground"
		, "Text" 
		};

	// init state
	if (!App.PropState.LabelWidth)
		 App.PropState.LabelWidth = TextList_CalcMaxWidth( dc, labels, NeCountOf(labels) );

	// calculate metrics
	const float line_h = Context_GetLineHeight( dc );
	const float label_x = r.x + pad.l;
	const float label_y = r.y + pad.t;
	const float label_w = App.PropState.LabelWidth;
	const float value_x = label_x + label_w + pad.r + space.x + pad.l;
	const float value_w = r.w - value_x - pad.r;
	Rect_s label_rect = { label_x, label_y, label_w, line_h };
	Rect_s value_rect = { value_x, label_y, value_w, line_h };
	const TextFormat::Mask_t text_fmt = TextFormat::Left | TextFormat::Middle;

	// do properties
	Theme_t theme = Context_GetTheme( dc );
	const int row = App.ThemeList.State.SelRow;
	const Visual::Enum	 visual_id = (Visual	::Enum)(row / CtrlState::COUNT);
	const CtrlState::Enum state_id = (CtrlState	::Enum)(row % CtrlState::COUNT);
	Visual_s& visual = theme->Visual[visual_id];

	int prop_id = 0;
	const float value_h = TextEdit_CalcHeight( dc );
	label_rect.h = value_h;
	value_rect.h = value_h;

	Label_Do	 ( dc, label_rect, text_fmt, "Foreground" );
	Value_DoColor( dc, Id_Cat( id, prop_id++ ), value_rect, visual.Front[state_id] );
	label_rect.y += value_rect.h + pad.b + space.y;
	value_rect.y += value_rect.h + pad.b + space.y;

	Label_Do	 ( dc, label_rect, text_fmt, "Background" );
	Value_DoColor( dc, Id_Cat( id, prop_id++ ), value_rect, visual.Back[state_id] );
	label_rect.y += value_rect.h + pad.b + space.y;
	value_rect.y += value_rect.h + pad.b + space.y;

	Label_Do	 ( dc, label_rect, text_fmt, "Text" );
	Value_DoColor( dc, Id_Cat( id, prop_id++ ), value_rect, visual.Text[state_id] );
	label_rect.y += value_rect.h + pad.b + space.y;
	value_rect.y += value_rect.h + pad.b + space.y;
}

/*
static void App_TestCtrls_00()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Right	);			// (1|2)
}

static void App_TestCtrls_01()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Seventh", { 220, 220, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Eighth"	, { 240, 240, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Ninth"	, { 260, 260, 800, 600 }, { TestCtrl_Do } } ) );
//	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Tenth"	, { 400, 300, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Right	);			// (1|2)
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2], ne::DockSide::Left	);			// (1|3|2)
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3], ne::DockSide::Bottom );			// (1(|3/4)|2)
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[4], ne::DockSide::Top	);			// (1|(5/3/4)|2)
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5], ne::DockSide::Left	);			// (1|((6|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[6], ne::DockSide::Center	);			// (1|(((6+7)|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[7], ne::DockSide::Center	);			// (1|(((6+7+8)|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[8], ne::DockSide::Center	);			// (1|(((6+7+8+9)|5)/3/4)|2)
//	DockCtrl_Dock( App.Ctrls[9], App.Ctrls[0]->Parent, ne::DockSide::Center );	// (2+4+3+5+6+7+8+9+1+10)
}

static void App_TestCtrls_02()
{
	// dock any container center into control
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2]		 , ne::DockSide::Right	);	// (2|3)
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]->Parent, ne::DockSide::Center );	// (1+2+3)
}

static void App_TestCtrls_03()
{
	// dock l/r container l/r into control
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2]		 , ne::DockSide::Right );	// (2|3)
//	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]->Parent, ne::DockSide::Right );	// (1|2|3)
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]->Parent, ne::DockSide::Left  );	// (2|3|1)

	// dock t/b container l/r into control
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5]		 , ne::DockSide::Bottom	);	// (5/6)
//	DockCtrl_Dock( App.Ctrls[3], App.Ctrls[4]->Parent, ne::DockSide::Right  );	// (4|(5/6))
	DockCtrl_Dock( App.Ctrls[3], App.Ctrls[4]->Parent, ne::DockSide::Left  );	// ((5/6)|4)
}

static void App_TestCtrls_04()
{
	// dock t/b container t/b into control
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2]		 , ne::DockSide::Bottom );	// (2/3)
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]->Parent, ne::DockSide::Bottom );	// (1/2/3)
//	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]->Parent, ne::DockSide::Top    );	// (2/3/1)

	// dock l/r container t/b into control
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5]		 , ne::DockSide::Right	);	// (5|6)
	DockCtrl_Dock( App.Ctrls[3], App.Ctrls[4]->Parent, ne::DockSide::Bottom );	// (4/(5|6))
//	DockCtrl_Dock( App.Ctrls[3], App.Ctrls[4]->Parent, ne::DockSide::Top	);	// ((5|6)/4)
}

static void App_TestCtrls_05()
{
	// dock l/r container into l/r container
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0]			, App.Ctrls[1]			, ne::DockSide::Right );	// (1|2)
	DockCtrl_Dock( App.Ctrls[2]			, App.Ctrls[3]			, ne::DockSide::Right );	// (3|4)
	DockCtrl_Dock( App.Ctrls[0]->Parent	, App.Ctrls[2]->Parent	, ne::DockSide::Right );	// (1|2|3|4)
}

static void App_TestCtrls_06()
{
	// dock l/r container into l/r container
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 400, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0]			, App.Ctrls[1]			, ne::DockSide::Bottom );	// (1/2)
	DockCtrl_Dock( App.Ctrls[2]			, App.Ctrls[3]			, ne::DockSide::Bottom );	// (3/4)
	DockCtrl_Dock( App.Ctrls[0]->Parent	, App.Ctrls[2]->Parent	, ne::DockSide::Bottom );	// (1/2/3/4)
}

static void App_TestCtrls_07()
{
	// dock l/r container l/r of control in l/r container
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1]			, ne::DockSide::Right );	// (1/2)
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3]			, ne::DockSide::Right );	// (3/4)
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2]->Parent	, ne::DockSide::Right );	// (1/2/3/4)

	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 200, 200, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 220, 220, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Seventh", { 240, 240, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Eighth"	, { 260, 260, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5]			, ne::DockSide::Right );	// (5/6)
	DockCtrl_Dock( App.Ctrls[6], App.Ctrls[7]			, ne::DockSide::Right );	// (7/8)
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[6]->Parent	, ne::DockSide::Left  );	// (7/8/5/6)
}

static void App_TestCtrls_08()
{
	// dock ctrl l/r of tab
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 400, 300 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 400, 300 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Center );	// (1+2)
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2], ne::DockSide::Right );	// ((1+2)/3)
}

static void App_TestCtrls_09()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Seventh", { 220, 220, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Eighth"	, { 240, 240, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Ninth"	, { 260, 260, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Tenth"	, { 400, 300, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Right	);			// (1|2)
	DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2], ne::DockSide::Left	);			// (1|3|2)
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3], ne::DockSide::Bottom );			// (1(|3/4)|2)
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[4], ne::DockSide::Top	);			// (1|(5/3/4)|2)
	DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5], ne::DockSide::Left	);			// (1|((6|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[6], ne::DockSide::Center	);			// (1|(((6+7)|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[7], ne::DockSide::Center	);			// (1|(((6+7+8)|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[5], App.Ctrls[8], ne::DockSide::Center	);			// (1|(((6+7+8+9)|5)/3/4)|2)
	DockCtrl_Dock( App.Ctrls[9], App.Ctrls[0]->Parent, ne::DockSide::Center );	// (2+4+3+5+6+7+8+9+1+10)
}

static void App_TestCtrls_10()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Bottom	);	// [*\1]
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[1], ne::DockSide::Top		);	// [2\*\1]
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[2], ne::DockSide::Bottom	);	// [2\*\3\1]
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[3], ne::DockSide::Right	);	// 
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[4], ne::DockSide::Left	);	// 
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[5], ne::DockSide::Right	);	// 
}

static void App_TestCtrls_11()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
//	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Right	);	// [*|1]
//	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[1], ne::DockSide::Left	);	// [1|*|2]
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Bottom	);	// [*\1]
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[1], ne::DockSide::Top		);	// [1\*\2]
}

static void App_TestCtrls_12()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 200, 200 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Bottom	);
	DockCtrl_Dock( App.Ctrls[0]				  , App.Ctrls[1], ne::DockSide::Center	);
}

static void App_TestCtrls_13()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 200, 200 }, { TestCtrl_Do } } ) );
//	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Sixth"  , { 200, 200, 200, 200 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Top		);	
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[1], ne::DockSide::Left	);	
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[2], ne::DockSide::Bottom	);	
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[3], ne::DockSide::Right	);	
//	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[4], ne::DockSide::Center	);	
//	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[5], ne::DockSide::Center	);	
//	DockCtrl_Dock( App.Ctrls[4]				  , App.Ctrls[5], ne::DockSide::Center	);
}

static void App_TestSplit()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Left );
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3], ne::DockSide::Left );
}

static void App_TestTab()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 800, 600 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 800, 600 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Center );
	DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3], ne::DockSide::Right );
}
*/

static void App_TestMain()
{
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "First"  , { 100, 100, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Second" , { 120, 120, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Third"  , { 140, 140, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fourth" , { 160, 160, 200, 200 }, { TestCtrl_Do } } ) );
	App.Ctrls.Append( DockCtrl_Create( App.Dock, { "Fifth"  , { 180, 180, 200, 200 }, { TestCtrl_Do } } ) );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[0], ne::DockSide::Top		);
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[1], ne::DockSide::Left	);
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[2], ne::DockSide::Bottom	);
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), App.Ctrls[3], ne::DockSide::Right	);
}

static int App_Initialize( HWND hWnd, HINSTANCE hInstance, LPCSTR pszTitle )
{
	// core
	Allocator_t alloc = nullptr;
	ne::core::Initialize( alloc );

	// app
	App.Alloc = alloc;
	App.Wnd = hWnd;

	// gfx
	App.Gfx = Gfx_d3d11_Create( alloc, hWnd );
	if (!App.Gfx)
		return -1;

	// gui
	App.Gui = Gui_d3d11_Create( alloc, App.Gfx );
	if (!App.Gui)
		return -2;

	// theme
	{
		using namespace gui;
		Context_t dc = System_GetContext();
		Theme_t theme = Context_GetTheme( dc );
		Theme_ApplyPreset( theme, ThemePreset::Dark );
	}

	// dock
	const DockMgrSetup_s dock_mgr_setup = 
	{ alloc
	, hWnd
	, App_GetClientRect()
	, pszTitle
	, hInstance
	, Dock_d3d11_GetApi( App.Gfx )
	, DockOs_GetWin32()
	};
	App.Dock = DockMgr_Create( dock_mgr_setup );
	if (!App.Dock)
		return -3;

	DockCtrl_t info  = DockCtrl_Create( App.Dock, { "Manager"	, { 1000, 100, 800, 250 }, { InfoCtrl_Do  } } );
	DockCtrl_t theme = DockCtrl_Create( App.Dock, { "Theme"		, { 1000, 100, 800, 600 }, { ThemeCtrl_Do } } );
	DockCtrl_t props = DockCtrl_Create( App.Dock, { "Properties", { 1000, 100, 500, 600 }, { PropsCtrl_Do } } );

	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), info , ne::DockSide::Bottom );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), theme, ne::DockSide::Center );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), props, ne::DockSide::Right );

	// ctrls
//	App.Ctrls.Init( alloc );
//	App_TestCtrls_00();
//	App_TestCtrls_01();
//	App_TestCtrls_02();
//	App_TestCtrls_03();
//	App_TestCtrls_04();
//	App_TestCtrls_05();
//	App_TestCtrls_06();
//	App_TestCtrls_07();
//	App_TestCtrls_08();
//	App_TestCtrls_09();
//	App_TestCtrls_10();
//	App_TestCtrls_11();
//	App_TestCtrls_12();
//	App_TestCtrls_13();
//	App_TestMain();

	App.Clicked = nullptr;

	return 0;
}

static void App_DoClicked()
{
	DockCtrl_t clicked = App.Clicked;
	App.Clicked = nullptr;
	if (!clicked)
		return;
	if (!DockCtrl_IsFloating(clicked))
		return DockCtrl_Undock( clicked );
	for ( int i = 0; i < App.Ctrls.Count; ++i )
	{
		if (App.Ctrls[i] == clicked)
		{
			switch (i)
			{
			case 0:
				DockCtrl_Dock( App.Ctrls[1], App.Ctrls[0], ne::DockSide::Left);
				break;
			case 1:
				DockCtrl_Dock( App.Ctrls[0], App.Ctrls[1], ne::DockSide::Right);
				break;
			case 2:
				DockCtrl_Dock( App.Ctrls[1], App.Ctrls[2], ne::DockSide::Left );
				break;
			case 3:
				DockCtrl_Dock( App.Ctrls[2], App.Ctrls[3], ne::DockSide::Bottom );
				break;
			case 4:
				DockCtrl_Dock( App.Ctrls[2], App.Ctrls[4], ne::DockSide::Top );
				break;
			case 5:
				DockCtrl_Dock( App.Ctrls[4], App.Ctrls[5], ne::DockSide::Left );
				break;
			case 6:
				DockCtrl_Dock( App.Ctrls[5], App.Ctrls[6], ne::DockSide::Center	);
				break;
			case 7:
				DockCtrl_Dock( App.Ctrls[5], App.Ctrls[7], ne::DockSide::Center	);
				break;
			case 8:
				DockCtrl_Dock( App.Ctrls[5], App.Ctrls[8], ne::DockSide::Center	);
				break;
			}
			break;
		}
	}
}

static void App_Shutdown()
{
	for ( int i = 0; i < App.Ctrls.Count; ++i )
		DockCtrl_Destroy( App.Ctrls[i] );
	App.Ctrls.Clear();
	DockMgr_Release( App.Dock );
	Gui_d3d11_Release( App.Gui );
	Gfx_d3d11_Release( App.Gfx );
	ne::core::Shutdown();
}

static void App_NextFrame()
{
	if (!App.Gfx)
		return;
	if (App.Minimized)
		return;
	Mouse_Poll( App.Mouse, App.Wnd );
	Keyboard_Poll( App.Keyboard );
	DockMgr_Do( App.Dock, App_GetClientRect(), App.Mouse, App.Keyboard );
	App_DoClicked();
}

static int App_Loop( HACCEL hAccel )
{
	MSG msg = {};

	while ( msg.message != WM_QUIT )
	{
		if (!PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ))
		{
			App_NextFrame();
		}
		else
		{
			if (!hAccel || !TranslateAccelerator(msg.hwnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
}

static void App_SetFocus( bool focus )
{
	App.Focus = focus;
}

static void App_Resize()
{
	App.Minimized = false;
	if (!App.Gfx)
		return;
	Gfx_d3d11_Resize( App.Gfx );
}

static void App_Minimized()
{
	App.Minimized = true;
}

//======================================================================================
// App - Public
//======================================================================================

int App_Run( HWND hWnd, HINSTANCE hInstance, HACCEL hAccel, LPCSTR pszTitle )
{
	int hr;
	hr = App_Initialize( hWnd, hInstance, pszTitle );
	if (!hr)
		 hr = App_Loop( hAccel );
	App_Shutdown();
	return hr;
}

bool App_Msg( HWND wnd, UINT msg, WPARAM w, LPARAM l, LRESULT* r )
{
	ne::Input_Notify( wnd, msg, w, l );

	switch ( msg )
	{
	case WM_SETFOCUS:
		App_SetFocus( true );
		break;

	case WM_KILLFOCUS:
		App_SetFocus( false );
		break;

	case WM_SIZE:
		switch (w)
		{
		default:
			App_Resize();
			break;
		case SIZE_MINIMIZED:
			App_Minimized();
			break;
		}
		break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)(l);
			DWORD dwStyle = (DWORD)GetWindowLongPtr( wnd, GWL_STYLE );
			DWORD dwExStyle = (DWORD)GetWindowLongPtr( wnd, GWL_EXSTYLE );
			HMENU hMenu = GetMenu( wnd );
			RECT rc = { 0, 0, 50, 50 };
			AdjustWindowRect( &rc, dwStyle, hMenu != NULL );
			pMinMaxInfo->ptMinTrackSize.x = rc.right-rc.left;
			pMinMaxInfo->ptMinTrackSize.y = rc.bottom-rc.top;
		}
		return 0;

	case WM_ERASEBKGND:
		return true;
	}

	return false;
}
