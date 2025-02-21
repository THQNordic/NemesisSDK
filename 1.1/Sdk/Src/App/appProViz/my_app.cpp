//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "my_app.h"
#include <Nemesis/Core/Allocator.h>

using namespace gui;

//======================================================================================
static void PacketReceived( void* context, system::Socket_t client, const profiling::Packet& packet, const profiling::Chunk* head )
{
	// safe to execute on the server's thread
	Parser_ParseData( ((MyApp*)context)->Session.Parser, packet, head, ((MyApp*)context)->ParseMode );
}

//======================================================================================
#if (NE_CONFIG < NE_CONFIG_MASTER)
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
		Sub::Spam(2);
	}
}
#endif

//======================================================================================
struct ParsedCommandLine
{
	system::IpAddress_t Server;
	system::IpPort_t    Port;
	bool				VSync;
};

static void ParseCommandLine( LPCSTR line, ParsedCommandLine& args )
{
	uint32_t tmp[4];
	const char* listen	= strstr(line, "-listen:");
	const char* send	= strstr(line, "-send:");
	const char* server	= strstr(line, "-server:");
	const char* vsync	= strstr(line, "-vsync");
	if (listen)
	{
		sscanf_s(listen, "-listen:%u", tmp+0);
		args.Server.Port = (uint16_t)tmp[0];
	}
	if (send)
	{
		sscanf_s(send, "-send:%u", tmp+0);
		args.Port = (uint16_t)tmp[0];
	}
	if (server)
	{
		sscanf_s(server, "-server:%u.%u.%u.%u", tmp+0, tmp+1, tmp+2, tmp+3);
		args.Server.Ip[0] = (uint8_t)tmp[0];
		args.Server.Ip[1] = (uint8_t)tmp[1];
		args.Server.Ip[2] = (uint8_t)tmp[2];
		args.Server.Ip[3] = (uint8_t)tmp[3];
	}
	args.VSync = (vsync != nullptr);
}

static void ParseCommandLine( LPCWSTR line, ParsedCommandLine& args )
{
	char text[256] = "";
	WideCharToMultiByte( CP_UTF8, 0, line, (int)wcslen(line), text, sizeof(text), nullptr, nullptr );
	ParseCommandLine( text, args );
}

//======================================================================================
struct MyAlloc
{
public:
	explicit MyAlloc( cstr_t name )
	{
		Alloc = CountingAllocator_Create( nullptr, name );
	}

	~MyAlloc()
	{
		CountingAllocator_Dump( &Alloc );
	}

public:
	CountingAllocator_s Alloc;
};

//======================================================================================
MyAlloc alloc_gui	  ( "Gui" );
MyAlloc alloc_font	  ( "Font" );
MyAlloc alloc_render  ( "Render" );
MyAlloc alloc_perf_srv( "Perf Server" );
MyAlloc alloc_perf_rcv( "Perf Client" );
MyAlloc alloc_perf_par( "Perf Parser" );
MyAlloc alloc_perf_db ( "Perf Data" );
MyApp	theApp;

//======================================================================================
MyApp::MyApp()
{
	NeZero(Session);
	FrameMs = 0.0f;
	FrameNumber = 0;
	OverSleptTime = 0;
	ParseMode = Parse::Buffered;
	Gfx = nullptr;
	Gui = nullptr;
}

bool MyApp::OnInitInstance()
{ 
	NePerfInit(&alloc_perf_srv.Alloc.Header);
	NePerfScope("Init");

	// parse command line
	ParsedCommandLine args;
	args.Port = 17001;
	args.Server.Port = 16001;
	args.Server.Ip[0] = 127;
	args.Server.Ip[1] = 0;
	args.Server.Ip[2] = 0;
	args.Server.Ip[3] = 1;
	ParseCommandLine( CommandLine, args );
#if NE_CONFIG == NE_CONFIG_MASTER
	args.VSync = true;
#endif

	profiling::viz::Initialize( Session );

	// initialize profiling server and client
	NePerfStartSender( args.Port );
	viz::LoadSettings( Session );

	DatabaseSetup_s db_setup = {};
	Session.Receiver = Receiver_Create( &alloc_perf_rcv.Alloc.Header );
	Session.Database = Database_Create( &alloc_perf_db.Alloc.Header, db_setup );
	const ParserSetup setup = { Session.Database, 16 * 1024 * 1024 };
	Session.Parser = Parser_Create( &alloc_perf_par.Alloc.Header, setup );
//	Session.DefaultIp = args.Server;
	const profiling::ReceiverCallback callback = { PacketReceived, this	};
	Session.ReceiverCallback = callback;

	// set window title
	{
		char fmt[256] = "";
		char title[256] = "";

		strcpy_s( title, "ProViz 1.0" );
		sprintf_s( fmt, "   [ Server: %u.%u.%u.%u:%u ]", Session.DefaultIp.Ip[0], Session.DefaultIp.Ip[1], Session.DefaultIp.Ip[2], Session.DefaultIp.Ip[3], Session.DefaultIp.Port );
		strcat_s( title, fmt );
	#if NE_ENABLE_PROFILING
		sprintf_s( fmt, "   [ Send: %u ]", args.Port );
		strcat_s( title, fmt );
	#endif
		sprintf_s( fmt, "   [ VSync: %s ]", args.VSync ? "on" : "off" );
		strcat_s( title, fmt );

	#if (NE_GFX_DRIVER == NE_GFX_DRIVER_D3D9)
		strcat_s(title, "   [ D3D9 ]");
	#elif (NE_GFX_DRIVER == NE_GFX_DRIVER_D3D11)
		strcat_s(title, "   [ D3D11 ]");
	#endif

		SetWindowTextA(Window, title);
	}

	// intialize font
	font::System_Initialize( &alloc_font.Alloc.Header );

	// initialize ui
	Gfx = gfx::Gfx_Create( &alloc_render.Alloc.Header, Window );
	Gui = gfx::Gui_Create( &alloc_render.Alloc.Header, Gfx );

	// initialize skin
	viz::MakeDefaultSkin( Skin );
	MainPanel.Initialize( &Skin );

	// mem dump

	return true;
}

void MyApp::OnExitInstance()
{
#if (NE_CONFIG < NE_CONFIG_MASTER)
	StopSpam();
#endif
	MainPanel.Shutdown();
	gfx::Gui_Release(Gui);
	gfx::Gfx_Release(Gfx);
	Gui = nullptr;
	Gfx = nullptr;
	font::System_Shutdown();
	profiling::Receiver_Destroy(Session.Receiver);
	Session.Receiver = nullptr;
	profiling::Parser_Destroy(Session.Parser);
	Session.Parser = nullptr;
	profiling::Database_Destroy(Session.Database);
	Session.Database = nullptr;
	NePerfShutdown();
}

void MyApp::OnNextFrame()
{
	// begin
	{
		FrameTimer.Start();
	}

	// work
	{
		NePerfLog("Begin Frame");
		NePerfScope("Frame");
		AdvanceFrame();
		RenderFrame();
		PresentFrame();
		NePerfLog("End Frame");
	}

	// send
	{
		SendFrame();
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
}

void MyApp::AdvanceFrame()
{
	NePerfScope("Advance");
	Parser_JoinData( Session.Parser );
}

void MyApp::RenderFrame()
{
	NePerfScope("Render");
	gui::BeginFrame( Window );
	gfx::Gfx_Begin(Gfx);
	gfx::Gfx_Clear(Gfx, 0xaaaaaaaa);
	{
		RenderOverlays();
		RenderStats();
	}
	DrawBatch_s batch = {};
	Context_EndFrame(System_GetContext(), &batch);
	gfx::Gfx_Draw_2D(Gfx, &batch, 1);
	gfx::Gfx_End(Gfx);
	gfx::Gfx_Present(Gfx);
}

void MyApp::PresentFrame()
{
}

void MyApp::SendFrame()
{
	NePerfFunc;
	NePerfNextFrame();
}

void MyApp::Throttle()
{
	NePerfScope("Limit FPS", ScopeType::Idle);
	while (FrameTimer.ElapsedMs() < 33)
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

	gui::HandleMessage( hWnd, msg, wParam, lParam );

	switch ( msg )
	{
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
		Gfx_Resize(Gfx);
}

void MyApp::RenderStats()
{
	wchar_t sz[64] = L"";
	int len = swprintf_s(sz, L"Frame: %u (%6.2f ms)", FrameNumber, FrameMs);

	RECT rc;
	GetClientRect(Window, &rc);
	rc.top	 += 8;
	rc.right -= 8;
	const Rect_s r 
		{ (float)rc.left
		, (float)rc.top
		, (float)(rc.right-rc.left)
		, (float)(rc.bottom-rc.top)
		};

	FontCache_t fc = System_GetFontCache();
	Graphics_t g = Context_GetGraphics( System_GetContext() );
	Font_t font = FontCache_CreateFont( fc, L"Consolas", 12.0f, FontStyle::Default );
	Graphics_DrawString( g, r, sz, len, font, TextFormat::Center | TextFormat::NoWrap, Color::White );
}

static Rect_s ClientToMainPanel( RECT rc )
{
	return Rect_s
		{ (float)rc.left
		, (float)rc.top
		, (float)(rc.right-rc.left)
		, (float)(rc.bottom-rc.top)
		};
}

void MyApp::RenderOverlays()
{
	RECT rc;
	GetClientRect( Window, &rc );
	MainPanel.Draw( Session, ClientToMainPanel( rc ), Session.Database );
}

void MyApp::Connect()
{
	if (Receiver_IsConnected(Session.Receiver))
		return;
	Receiver_Connect( Session.Receiver, Session.DefaultIp, Session.ReceiverCallback );
}

void MyApp::Loopback()
{
	if (Receiver_IsConnected(Session.Receiver))
		return;
	const system::IpAddress_t ip = { 127, 0, 0, 1, 17001 };
	Receiver_Connect( Session.Receiver, ip, Session.ReceiverCallback );
}

void MyApp::Disconnect()
{
	profiling::Receiver_Disconnect( Session.Receiver );
}

void MyApp::StartSpam()
{
#if (NE_CONFIG < NE_CONFIG_MASTER)
	for ( int i = 0; i < NeCountOf(SpamThread); ++i)
	{
		const system::ThreadSetup_s thread_setup = 
		{ ">> spam <<", Spam, SpamThread+i };
		SpamThread[i].Start( thread_setup );
	}
#endif
}

void MyApp::StopSpam()
{
#if (NE_CONFIG < NE_CONFIG_MASTER)
	for ( int i = 0; i < NeCountOf(SpamThread); ++i)
		SpamThread[i].Stop();
#endif
}

bool MyApp::HandleInput( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult )
{
	// app
	switch ( msg )
	{
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_F4:
			Connect();
			return true;

		case VK_F5:
			Loopback();
			return true;

		case VK_F6:
			Disconnect();
			return true;

		case VK_F8:
			StartSpam();
			return true;

		case VK_F9:
			StopSpam();
			return true;

		default:
			break;
		}
		break;
	}

	// gui
	return false;
}
