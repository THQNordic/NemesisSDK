//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "DockHost_Win32.h"
#include "DockMgr.h"

//======================================================================================
#if NE_PLATFORM_IS_WINDOWS

//======================================================================================
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	static int Rect_Width ( const RECT& r ) { return r.right-r.left; }
	static int Rect_Height( const RECT& r ) { return r.bottom-r.top; }

	static IntRect_s HWND_FloatingToScreen( float x, float y, float w, float h, DWORD dwStyle )
	{
		RECT rc = 
			{ (int)x
			, (int)y
			, (int)x + (int)w
			, (int)y + (int)h
			};
		AdjustWindowRect( &rc, dwStyle, FALSE );
		return IntRect_s
			{ rc.left
			, rc.top
			, Rect_Width(rc)
			, Rect_Height(rc)
			};
	}

	static IntRect_s HWND_FloatingToScreen( const Rect_s& r, DWORD dwStyle )
	{
		return HWND_FloatingToScreen( r.x, r.y, r.w, r.h, dwStyle );
	}

	static IntRect_s HWND_FloatingToScreen( HWND hWnd, float x, float y, float w, float h )
	{
		return HWND_FloatingToScreen( x, y, w, h, (DWORD)GetWindowLongW( hWnd, GWL_STYLE ) );
	}

	static IntRect_s HWND_FloatingToScreen( HWND hWnd, const Rect_s& r )
	{
		return HWND_FloatingToScreen( hWnd, r.x, r.y, r.w, r.h );
	}

	static void DockCtrl_SetWndPos( DockCtrl_t ctrl, HWND hWnd )
	{
		if (!ctrl || !hWnd)
			return;
		RECT rc = {};
		GetClientRect( hWnd, &rc );
		ClientToScreen( hWnd, (LPPOINT)&rc );
		const float x = (float)rc.left;
		const float y = (float)rc.top;
		ctrl->FloatPos.x = x;
		ctrl->FloatPos.y = y;
	}

	static void DockCtrl_SetWndSize( DockCtrl_t ctrl, HWND hWnd )
	{
		if (!ctrl || !hWnd)
			return;
		RECT rc = {};
		GetClientRect( hWnd, &rc );
		const float w = (float)Rect_Width(rc);
		const float h = (float)Rect_Height(rc);
		DockCtrl_SetLocalSize( ctrl, w, h );
	}
}

//======================================================================================
#define WS_EX_NONE		0
#define WS_EX_OVERLAY	(WS_EX_TOPMOST|WS_EX_TOOLWINDOW)
#define WS_OVERLAY		(WS_POPUP)
#define WS_FLOATING		(WS_POPUPWINDOW)
#define WS_DOCKED		(WS_CHILD)

//======================================================================================
namespace
{
	static const wchar_t DOCK_HOST_CLASS[] = L"Dock.Host";
}

//======================================================================================
namespace nemesis
{
	typedef struct DockOs_Win32_s  *DockOs_Win32_t;
	typedef struct DockHost_Win32_s *DockHost_Win32_t;

	struct DockOs_Win32_s
	{
	};

	struct DockHost_Win32_s
	{
		DockCtrl_t		Ctrl;
		DockMgr_t		Mgr;
		HWND			Wnd;
		DockSwapChain_t	SwapChain;
	};

}

//======================================================================================
namespace nemesis
{
	static DockHost_Win32_t DockHost_Win32_FromHwnd( HWND hWnd )
	{
		return (DockHost_Win32_t)GetWindowLongPtrW( hWnd, GWLP_USERDATA );
	}

	static LRESULT WINAPI DockHost_WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		switch (msg)
		{
		case WM_CREATE:
			{
				CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
				DockHost_Win32_t host = (DockHost_Win32_t)cs->lpCreateParams;
				const bool is_main	  = DockCtrl_IsMain( host->Ctrl );
				const bool is_overlay = DockCtrl_IsOverlay( host->Ctrl );

				// attach
				SetWindowLongPtrW( hWnd, GWLP_USERDATA, (LONG_PTR)host );

				// create swap chain
				RECT rc = {};
				GetClientRect( hWnd, &rc );
				const DockMgrApi_Gfx_s* gfx = &host->Mgr->Graphics;
				host->SwapChain = gfx->SwapChain_Create( gfx->User, hWnd, Rect_Width(rc), Rect_Height(rc), is_overlay );
			}
			break;

		case WM_DESTROY:
			{
				DockHost_Win32_t host = DockHost_Win32_FromHwnd( hWnd );
				const DockMgrApi_Gfx_s* gfx = &host->Mgr->Graphics;
				gfx->SwapChain_Release( gfx->User, host->SwapChain );
				host->SwapChain = nullptr;
			}
			break;

		case WM_PAINT:
			break;

		case WM_ERASEBKGND:
			return 1;

		case WM_MOVE:
			{
				DockHost_Win32_t host = DockHost_Win32_FromHwnd( hWnd );
				DockCtrl_SetWndPos( host->Ctrl, hWnd );
			}
			break;

		case WM_SIZE:
			{
				DockHost_Win32_t host = DockHost_Win32_FromHwnd( hWnd );
				DockCtrl_SetWndSize( host->Ctrl, hWnd );

				// resize swap chain
				if (host->SwapChain)
				{
					RECT rc = {};
					GetClientRect( hWnd, &rc );
					const DockMgrApi_Gfx_s* gfx = &host->Mgr->Graphics;
					gfx->SwapChain_Resize( gfx->User, host->SwapChain, NeMax<int>(16, Rect_Width(rc)), NeMax<int>(16, Rect_Height(rc)) );
				}
			}
			break;

		case WM_CLOSE:
			return 0;

		case WM_NCHITTEST:
			break;

		case WM_GETMINMAXINFO:
			break;

		case WM_ACTIVATE:
			{
				DockHost_Win32_t host = DockHost_Win32_FromHwnd( hWnd );
				const bool is_overlay = host && (host->Ctrl->Kind == DockCtrl::Overlay);
				if (is_overlay)
				{
					MARGINS margins = {-1,-1,-1,-1};
					DwmExtendFrameIntoClientArea( hWnd, &margins );
					return 0;
				}
			}
			break;
		}
		return ::DefWindowProcW( hWnd, msg, wParam, lParam );
	}
}

//======================================================================================
namespace nemesis
{
	static const UINT_PTR MAIN_HOST_SUBCLASS_ID = 1;

	static LRESULT WINAPI MainHost_SubclassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
	{
		NeAssert(uIdSubclass == MAIN_HOST_SUBCLASS_ID);
		DockHost_Win32_t host = (DockHost_Win32_t)dwRefData;
		switch (uMsg)
		{
		case WM_SIZE:
			DockCtrl_SetWndSize( host->Ctrl, hWnd );
			break;

		case WM_MOVE:
			DockCtrl_SetWndPos( host->Ctrl, hWnd );
			break;
		}
		return DefSubclassProc( hWnd, uMsg, wParam, lParam );
	}
}

//======================================================================================
namespace nemesis
{
	static DockHost_Win32_t DockOs_Win32_FindHost( const DockCtrl_t* ctrls, int num_ctrl, HWND hWnd )
	{
		for ( int i = 0; i < num_ctrl; ++i )
		{
			const DockHost_Win32_t host = (DockHost_Win32_t)ctrls[i]->Host;
			if (!host)
				continue;
			if (hWnd != host->Wnd)
				continue;
			return host;
		}
		return nullptr;
	}

	static DockHost_Win32_t DockHost_Win32_CreateMain( DockOs_Win32_t os, DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(ctrl->Host == nullptr);

		HWND hWnd = (HWND)ctrl->Mgr->AppWnd;
		RECT rc;
		GetWindowRect( hWnd, &rc );

		DockHost_Win32_t host = Mem_Calloc<DockHost_Win32_s>( ctrl->Mgr->Alloc );
		ctrl->Host = (DockHost_t)host;

		DockCtrl_SetWndPos( ctrl, hWnd );
		DockCtrl_SetWndSize( ctrl, hWnd );
		DockCtrl_SetLocalPos( ctrl, 0.0f, 0.0f );
		host->Ctrl = ctrl;
		host->Mgr = ctrl->Mgr;
		host->Wnd = hWnd;

		SetWindowSubclass( hWnd, MainHost_SubclassProc, MAIN_HOST_SUBCLASS_ID, (DWORD_PTR)host );

		return host;
	}

	static DockHost_Win32_t DockHost_Win32_CreateFloating( DockOs_Win32_t os, DockCtrl_t ctrl )
	{
		NeAssert(!DockCtrl_IsMain(ctrl));
		NeAssert(ctrl->Host == nullptr);

		// convert text to wide char
		cstr_t utf8_text = DockCtrl_GetText( ctrl );
		const int num_utf8 = 1+Str_Len( utf8_text );
		const int num_wide = MultiByteToWideChar( CP_UTF8, MB_PRECOMPOSED, utf8_text, num_utf8, nullptr, 0 );
		wchar_t* wide_text = NeStackCalloc( wchar_t, (num_wide+1) );
		MultiByteToWideChar( CP_UTF8, MB_PRECOMPOSED, utf8_text, num_utf8, wide_text, num_wide+1 );

		// setup style
		const bool is_overlay = DockCtrl_IsOverlay( ctrl );
		const HWND hParent = (HWND)ctrl->Mgr->AppWnd;
		const DWORD dwStyle = is_overlay ? WS_OVERLAY : WS_FLOATING;
		const DWORD dwExStyle = is_overlay ? WS_EX_OVERLAY : WS_EX_NONE;

		// calculate window rect according to style
		const IntRect_s rc = HWND_FloatingToScreen( ctrl->FloatPos.x, ctrl->FloatPos.y, ctrl->LocalRect.w, ctrl->LocalRect.h, dwStyle );

		// create host
		DockHost_Win32_t host = Mem_Calloc<DockHost_Win32_s>( ctrl->Mgr->Alloc );
		ctrl->Host = (DockHost_t)host;
		host->Ctrl = ctrl;
		host->Mgr = ctrl->Mgr;
		host->Wnd = CreateWindowExW
			( dwExStyle
			, DOCK_HOST_CLASS
			, wide_text
			, dwStyle
			, rc.x
			, rc.y
			, rc.w
			, rc.h
			, hParent
			, NULL
			, (HINSTANCE)ctrl->Mgr->Instance
			, host
			);
		DockHost_Win32_Show( (DockHost_t)host );
		UpdateWindow( host->Wnd );
		BringWindowToTop( host->Wnd );
		return host;
	}

	static void DockHost_Win32_RenderMain( DockHost_Win32_t host )
	{
		DockCtrl_DoRoot( host->Ctrl );
	}

	static void DockHost_Win32_RenderFloating( DockHost_Win32_t host )
	{
		if (!host->SwapChain)
			return;

		// gather metrics
		RECT rc;
		GetClientRect( host->Wnd, &rc );
		const Rect_s host_rect = 
			{ 0.0f
			, 0.0f
			, (float)Rect_Width(rc)
			, (float)Rect_Height(rc) 
			};

		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( host->Wnd, &pt );
		const Vec2_s mouse_pos = { (float)pt.x, (float)pt.y };

		// @note: 
		//	it's important for the swap chain to
		//	be bound and active *prior* to calling
		//	DockCtrl_DoRoot in order to allow for
		//	the control to do custom rendering.

		// bind and clear swap chain
		const bool is_debug = (NE_CONFIG == NE_CONFIG_DEBUG);
		const bool is_overlay = DockCtrl_IsOverlay( host->Ctrl );
		const uint32_t clear_color = (is_debug && !is_overlay) ? Color_SetAlpha( Color::Pink, 0x7f ) : 0;
		const DockMgrApi_Gfx_s* gfx = &host->Mgr->Graphics;
		gfx->SwapChain_Begin( gfx->User, host->SwapChain );
		gfx->Scene_Begin( gfx->User );
		gfx->Scene_Clear( gfx->User, clear_color );

		// draw to batch
		DrawBatch_s batch = {};
		Context_BeginHost( host->Ctrl->Dc, host_rect, mouse_pos );
		DockCtrl_DoRoot( host->Ctrl );
		Context_EndHost( host->Ctrl->Dc, &batch );

		// render to swap chain
		gfx->Scene_Draw( gfx->User, &batch, 1 );
		gfx->Scene_End( gfx->User );
		gfx->SwapChain_End( gfx->User, host->SwapChain );
		gfx->SwapChain_Present( gfx->User, host->SwapChain );
	}

}

//======================================================================================
namespace nemesis
{
	static void DockOs_Win32_Release( DockOs_Win32_t os )
	{
	}

	static int DockOs_Win32_OrderZ( DockOs_Win32_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts )
	{
		int pos = 0;
		for ( HWND hNext = GetTopWindow( HWND_DESKTOP ); hNext; hNext = GetNextWindow( hNext, GW_HWNDNEXT ) )
		{
			const DockHost_Win32_t host = DockOs_Win32_FindHost( ctrls, num_ctrl, hNext );
			if (!host)
				continue;
			hosts[pos++] = (DockHost_t)host;
		}
		return pos;
	}

	static DockHost_Win32_t DockHost_Win32_Create( DockOs_Win32_t os, DockCtrl_t ctrl )
	{
		return DockCtrl_IsMain( ctrl )
			? DockHost_Win32_CreateMain( os, ctrl )
			: DockHost_Win32_CreateFloating( os, ctrl );
	}

	static bool	DockHost_Win32_IsActive( DockHost_Win32_t host )
	{
		if (!host || !host->Wnd)
			return false;
		return GetActiveWindow() == host->Wnd;
	}

	static void	DockHost_Win32_Move( DockHost_Win32_t host )
	{
		if (!host || !host->Wnd)
			return;
		NeAssert(!DockCtrl_IsMain(host->Ctrl));
		const DockCtrl_t ctrl = host->Ctrl;
		const IntRect_s rc = HWND_FloatingToScreen( host->Wnd, ctrl->FloatPos.x, ctrl->FloatPos.y, ctrl->LocalRect.w, ctrl->LocalRect.h );
		MoveWindow( host->Wnd, rc.x, rc.y, rc.w, rc.h, TRUE );
	}

	static void	DockHost_Win32_Move_Rect( DockHost_Win32_t host, const Rect_s& r )
	{
		if (!host)
			return;
		if (!host->Wnd)
			return;
		NeAssert(!DockCtrl_IsMain(host->Ctrl));
		const IntRect_s rc = HWND_FloatingToScreen( host->Wnd, r );
		MoveWindow( host->Wnd, rc.x, rc.y, rc.w, rc.h, TRUE );
	}

	static void	DockHost_Win32_Show( DockHost_Win32_t host )
	{
		if (!host || !host->Wnd)
			return;
		ShowWindow( host->Wnd, host->Ctrl->Hidden ? SW_HIDE : SW_SHOW );
	}

	static void	DockHost_Win32_Render( DockHost_Win32_t host )
	{
		const bool is_main = (host->Ctrl->Mgr->AppWnd == host->Wnd);
		return is_main 
			? DockHost_Win32_RenderMain	 ( host )
			: DockHost_Win32_RenderFloating( host );
	}

	static void	DockHost_Win32_Release( DockHost_Win32_t host )
	{
		if (!host)
			return;
		if (host->Ctrl)
			host->Ctrl->Host = nullptr;
		if (host->Wnd != host->Mgr->AppWnd)
			DestroyWindow( host->Wnd );
		else
			RemoveWindowSubclass( host->Wnd, MainHost_SubclassProc, MAIN_HOST_SUBCLASS_ID );
		Mem_Free( host->Mgr->Alloc, host );
	}

}

//======================================================================================
namespace nemesis
{
	DockOs_t DockOs_Win32_Create( ptr_t user )
	{
	    WNDCLASSEXW wcex;
		NeZero(wcex);
	
		wcex.cbSize			= sizeof(wcex);
		wcex.style          = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc    = DockHost_WndProc;
		wcex.cbClsExtra     = 0;
		wcex.cbWndExtra     = 0;
	//	wcex.hInstance      = hInstance;
		wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+2);
		wcex.lpszClassName  = DOCK_HOST_CLASS;

		ATOM atom = RegisterClassExW(&wcex);

		return nullptr;
	}

	void DockOs_Win32_Release( DockOs_t os )
	{
		return DockOs_Win32_Release( (DockOs_Win32_t)(os) );
	}

	int DockOs_Win32_OrderZ( DockOs_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts )
	{
		return DockOs_Win32_OrderZ( (DockOs_Win32_t)(os), ctrls, num_ctrl, hosts );
	}

	DockHost_t DockHost_Win32_Create( DockOs_t os, DockCtrl_t ctrl )
	{
		return (DockHost_t)DockHost_Win32_Create( (DockOs_Win32_t)(os), ctrl );
	}

	bool DockHost_Win32_IsActive( DockHost_t host )
	{
		return DockHost_Win32_IsActive( (DockHost_Win32_t)(host) );
	}

	void DockHost_Win32_Move( DockHost_t host )
	{
		return DockHost_Win32_Move( (DockHost_Win32_t)(host) );
	}

	void DockHost_Win32_Move_Rect( DockHost_t host, const Rect_s& r )
	{
		return DockHost_Win32_Move_Rect( (DockHost_Win32_t)(host), r );
	}

	void DockHost_Win32_Show( DockHost_t host )
	{
		return DockHost_Win32_Show( (DockHost_Win32_t)(host) );
	}

	void DockHost_Win32_Render( DockHost_t host )
	{
		return DockHost_Win32_Render( (DockHost_Win32_t)(host) );
	}

	void DockHost_Win32_Release( DockHost_t host )
	{
		return DockHost_Win32_Release( (DockHost_Win32_t)(host) );
	}
}

//======================================================================================
namespace nemesis
{
	DockMgrApi_Os_s	DockOs_Win32_GetApi()
	{
		return DockMgrApi_Os_s
			{ DockOs_Win32_Create
			, DockOs_Win32_Release
			, DockOs_Win32_OrderZ
			, DockHost_Win32_Create
			, DockHost_Win32_IsActive
			, DockHost_Win32_Move
			, DockHost_Win32_Move_Rect
			, DockHost_Win32_Show
			, DockHost_Win32_Render
			, DockHost_Win32_Release
			, nullptr
			};
		
	}
}
#endif
