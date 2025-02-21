//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "MyApp.h"
#include "MyGui.h"
#include "MyPerf.h"
#include "MyAlloc.h"
#include "MyTrace.h"
#include <Nemesis/Core/Input.h>
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Logging.h>
#include <Nemesis/Font/System.h>
#include <Nemesis/Perf/Server.h>
#include <Nemesis/Perf/Visualizer.h>

//======================================================================================
using namespace nemesis;

//======================================================================================
system::SlimThread_c SpamThread[2] = {};

static void NE_CALLBK Spam( void* context )
{
	struct Sub
	{
		static void Spam( int level )
		{
			NePerfScope("Spam");
			Sleep(0);
			if (level == 0)
				return;
			Spam( level - 1 );
		}
	};

	NePerfThread("Spam");
	system::SlimThread_c* thread = (system::SlimThread_c*)context;
	while (thread->Continue())
	{
		Sub::Spam(0);
	}
}

void StartSpam()
{
	for ( int i = 0; i < NeCountOf(SpamThread); ++i)
	{
		const system::ThreadSetup_s thread_setup = 
		{ ">> spam <<", Spam, SpamThread+i };
		SpamThread[i].Start( thread_setup );
	}
}


NE_NO_INLINE
void StopSpam()
{
	for ( int i = 0; i < NeCountOf(SpamThread); ++i)
		SpamThread[i].Stop();
}

NE_NO_INLINE
void ToggleSpam()
{
	if (SpamThread[0].Continue())
		StopSpam();
	else
		StartSpam();
}

//======================================================================================
static MyAlloc GuiAllocator		 ( "Gui" );
static MyAlloc FontAllocator	 ( "Font" );
static MyAlloc RenderAllocator	 ( "Render" );
static MyAlloc ProfilingAllocator( "Profiler" );

//======================================================================================
MyApp::MyApp()
{
	HasFocus = true;
	FrameMs = 0.0f;
	FrameNumber = 0;
	OverSleptTime = 0;
	Gfx = nullptr;
	Gui = nullptr;
}

bool MyApp::OnInitInstance()
{ 
#if	  (NE_GFX_DRIVER == NE_GFX_DRIVER_D3D9)
	SetWindowTextA(Window, "Gui Demo [D3D9]");
#elif (NE_GFX_DRIVER == NE_GFX_DRIVER_D3D11)
	SetWindowTextA(Window, "Gui Demo [D3D11]");
#else
	SetWindowTextA(Window, "Gui Demo");
#endif

	// event trace
	MyTrace::Initialize();

	// perf - server
	NePerfInit(&ProfilingAllocator.Header);
	NePerfStartSender(16001);

	// perf - client
	MyPerf::Initialize();

	// font
	font::System_Initialize( &FontAllocator.Header );

	// render
	Gfx = gfx::Gfx_Create( &RenderAllocator.Header, Window );
	Gui = gfx::Gui_Create( &RenderAllocator.Header, Gfx );

	// ui
	MyGui::Initialize();
	return true;
}

void MyApp::OnExitInstance()
{
	StopSpam();

	// ui
	gui::System_Shutdown();

	// render
	gfx::Gui_Release( Gui );
	gfx::Gfx_Release( Gfx );
	Gui = nullptr;
	Gfx = nullptr;

	// font
	font::System_Shutdown();

	// perf - client
	MyPerf::Shutdown();

	// perf - server
	NePerfShutdown();

	// event trace
	MyTrace::Shutdown();
}

void MyApp::OnNextFrame()
{
	NePerfScope("Frame");

	// begin
	{
		FrameTimer.Start();
	}

	// work
	{
		MyTrace::PollEvents( nullptr, 0 );
		MyPerf::NextFrame();
	}

	// render
	{
		RenderFrame();
	}

	// cap
	{
		Throttle();
	}

	// end
	{
		FrameMs = FrameTimer.ElapsedMsF();
		++FrameNumber;
	}

	NePerfNextFrame();
}

void MyApp::RenderFrame()
{
	NePerfScope("Render");
	if (!Gfx || !Gui)
		return;

	// get desktop rect
	RECT rc;
	GetClientRect( Window, &rc );

	// input
	MouseState_s mouse = {};
	KeyboardState_s kb = {};
	if (HasFocus)
	{
		Mouse_Poll( mouse, Window );
		Keyboard_Poll( kb );
	}

	// render
	gui::DrawBatch_s batch = {};
	gfx::Gfx_Begin( Gfx );
	gfx::Gfx_Clear( Gfx, 0xaaaaaaaa );
	{
		const Rect_s clip_rect = 
		{ (float)(rc.left)
		, (float)(rc.top)
		, (float)(rc.right-rc.left)
		, (float)(rc.bottom-rc.top)
		};
		gui::Context_t dc = gui::System_GetContext();
		gui::Context_BeginFrame( dc, clip_rect, mouse, kb );
		{
			MyGui::Render();
		}
		gui::Context_EndFrame( dc, &batch );
	}
	{
		gfx::Gfx_Draw_2D( Gfx, &batch, 1 );
	}
	{
		gfx::Gfx_End	( Gfx );
		gfx::Gfx_Present( Gfx );
	}

	if (MyGui::QuitRequested())
		Quit();
}

void MyApp::Throttle()
{
	NePerfScope("Throttle", ScopeType::Idle);
	while (FrameTimer.ElapsedMs() < (1000.0f/60.0f))
		Sleep(0);
}

bool MyApp::CmdProc( HWND hWnd, int wmId, int wmEvent )
{
	if (hWnd != Window)
		return false;
	return false;
}

bool MyApp::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult )
{
	if (hWnd != Window)
		return false;

	Input_Notify( hWnd, msg, wParam, lParam );

	switch ( msg )
	{
	case WM_SETFOCUS:
		HasFocus = true;
		break;

	case WM_KILLFOCUS:
		HasFocus = false;
		break;

	case WM_SIZE:
		Resize();
		break;

	case WM_ERASEBKGND:
		break;

	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			//Paint(ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		if (	( msg >= WM_KEYFIRST && msg <= WM_KEYLAST )
			 ||	( msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST )
			 ||	( msg == WM_MOUSEHOVER ) || ( msg == WM_MOUSELEAVE )
			)
		{
			if (HandleInput( hWnd, msg, wParam, lParam, plResult ))
				return true;
		}
		return false;
	}
	return true;
}

void MyApp::Resize()
{
	if (!Window)
		return;
	if (Gfx)
		Gfx_Resize( Gfx );
}

bool MyApp::HandleInput( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult )
{
	switch ( msg )
	{
	case WM_KEYDOWN:
		{
			if (wParam == VK_F8)
			{
				ToggleSpam();
				return true;
			}
		}
		break;

	default:
		break;
	}

	return false;
}
