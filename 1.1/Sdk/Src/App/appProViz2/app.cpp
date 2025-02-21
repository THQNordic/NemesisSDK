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
#include "doc.h"
#include "view.h"
#include "gui_d3d11.h"
#include "dock_d3d11.h"
#include <Nemesis/Core/Allocator.h>

//======================================================================================
using namespace nemesis;
using namespace nemesis::system;
using namespace nemesis::graphics;
using namespace nemesis::gui;

//======================================================================================
struct Arena_s
{
public:
	explicit Arena_s(cstr_t name)
	{
		Alloc = CountingAllocator_Create(nullptr, name);
	}

	~Arena_s()
	{
		CountingAllocator_Dump(&Alloc);
	}

public:
	CountingAllocator_s Alloc;
};

//======================================================================================
// App - Private
//======================================================================================
struct App_s
{
	Allocator_t		  Alloc;
	Gfx_d3d11_t		  Gfx;
	Gui_d3d11_t		  Gui;
	DockMgr_t		  Dock;
	HWND			  Wnd;
	bool			  Focus;
	bool			  Minimized;
	StopWatch_c		  ElapsedTime;
	MouseState_s	  Mouse;
	KeyboardState_s	  Keyboard;
	Array<DockCtrl_t> Ctrls;
	Doc_s			  Doc;
};

static App_s   App = {};
static Arena_s Arena("App");

//--------------------------------------------------------------------------------------
static Rect_s App_GetClientRect()
{
	RECT rc;
	GetClientRect(App.Wnd, &rc);
	return Rect_s { 0.0f, 0.0f, float(rc.right-rc.left), float(rc.bottom-rc.top) };
}

static int App_Initialize( HWND hWnd, HINSTANCE hInstance, LPCSTR pszTitle )
{
	// core
	ne::core::Initialize( nullptr );

	// app
	Allocator_t alloc = &Arena.Alloc.Header;
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

	DockCtrl_t zones      = DockCtrl_Create( App.Dock, { "Zones"     , { 1000, 100, 800, 600 }, { ZoneTab_Do	   } });
	DockCtrl_t frames     = DockCtrl_Create( App.Dock, { "Frames"    , { 1000, 100, 800, 200 }, { FrameTab_Do	   } });
	DockCtrl_t servers    = DockCtrl_Create( App.Dock, { "Servers"   , { 1000, 100, 400, 600 }, { ServerTab_Do     } });
	DockCtrl_t settings   = DockCtrl_Create( App.Dock, { "Settings"  , { 1000, 100, 400, 600 }, { SettingsTab_Do   } });
	DockCtrl_t statistics = DockCtrl_Create( App.Dock, { "Statistics", { 1000, 100, 400, 600 }, { StatisticsTab_Do } });
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), zones     , ne::DockSide::Center );
	DockCtrl_Dock( zones					  , frames	  , ne::DockSide::Bottom );
	DockCtrl_Dock( DockMgr_GetMain( App.Dock ), servers   , ne::DockSide::Left   );
	DockCtrl_Dock( servers					  , settings  , ne::DockSide::Bottom );
	DockCtrl_Dock( settings					  , statistics, ne::DockSide::Bottom );

	// perf server
	NePerfInit(alloc);

	// document
	Doc_Initialize(&App.Doc, alloc);

	return 0;
}

static void App_Shutdown()
{
	for ( int i = 0; i < App.Ctrls.Count; ++i )
		DockCtrl_Destroy( App.Ctrls[i] );
	App.Ctrls.Clear();
	DockMgr_Release( App.Dock );

	NePerfShutdown();
	Doc_Shutdown(&App.Doc);

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
	Parser_JoinData(App.Doc.Parser);
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

Doc_t NE_API App_GetDoc()
{
	return &App.Doc;
}
