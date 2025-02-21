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
namespace
{
	static bool IsAsyncKeyPressed( int vk ) 
	{
		const short bits = GetAsyncKeyState( vk ); 
		return NeHasFlag( bits, 0x8000 ); 
	}
}

//======================================================================================
namespace nemesis { namespace gui 
{
	class Win32Input
	{
	public:
		Win32Input()
			: Wnd( nullptr )
		{
			NeZero(Null);
			NeZero(Current);
			HasFocus = true;
		}

	public:
		const Mouse_s& GetMouse();
		const Keyboard_s& GetKeyboard();

		virtual void Poll();

	public:
		void HandleMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	private:
		struct State
		{
			Mouse_s		Mouse;
			Keyboard_s	Keyboard;
		};
		State Null;
		State Current;
		bool HasFocus;
	public:
		HWND Wnd;
	};

	const Mouse_s& Win32Input::GetMouse()
	{
		return HasFocus ? Current.Mouse : Null.Mouse;
	}

	const Keyboard_s& Win32Input::GetKeyboard()
	{
		return HasFocus ? Current.Keyboard : Null.Keyboard;
	}

	void Win32Input::Poll()
	{
		// inactive?
		if (!HasFocus)
			return;

		// keyboard
		{
			Keyboard_Poll( Current.Keyboard );
			/*
			for ( int i = 0; i < 256; ++i )
			{
				const bool was_pressed = Current.Keyboard.Key[i].IsPressed;
				const bool is_pressed = (GetAsyncKeyState( i ) & 0x8000) == 0x8000;
				const bool went_down = ((!was_pressed) && is_pressed);
				const bool went_up = (was_pressed && (!is_pressed));

				Keyboard::KeyInfo& key = Current.Keyboard.Key[i];
				key.WasPressed = was_pressed;
				key.IsPressed = is_pressed;
				key.IsRepeated = went_down;
				key.WentDown = went_down;
				key.WentUp = went_up;

				if (is_pressed)
				{
					const uint32_t tick = GetTickCount();
					if (went_down)
					{
						key.RepeatBegin = tick;
					}
					else
					{
						const uint32_t elapsed = tick - key.RepeatBegin;
						if (elapsed >= 250)
						{
							key.IsRepeated = true;
							key.RepeatBegin -= 250;
						}
					}
				}
			}

			const int num_chars = NeMin( Buffer.NumChars, (int)sizeof(Current.Keyboard.Char) );
			Current.Keyboard.NumChars = num_chars;
			memcpy_s( Current.Keyboard.Char, sizeof(Current.Keyboard.Char), Buffer.Char, num_chars * sizeof(wchar_t) );
			*/
		}

		// mouse
		{
			Mouse_Poll( Current.Mouse, Wnd );

			/*
			POINT pt;
			GetCursorPos( &pt );
			ScreenToClient( Wnd, &pt );
			Current.Mouse.Pos = Vec2_s { (float)pt.x, (float)pt.y );

			const bool was_pressed[2] = 
			{ Current.Mouse.Button[0].IsPressed
			, Current.Mouse.Button[1].IsPressed
			};

			const bool is_pressed[2] = 
			{ IsAsyncKeyPressed( VK_LBUTTON )
			, IsAsyncKeyPressed( VK_RBUTTON )
			};

			for ( int i = 0; i < 2; ++i )
			{
				Mouse::ButtonInfo& button = Current.Mouse.Button[i];
				button.WentUp	 = was_pressed[i] && (!is_pressed[i]);
				button.WentDown  = is_pressed[i] && (!was_pressed[i]);
				button.IsPressed = is_pressed[i];
				button.NumClicks == 2 = false;

				const uint32_t tick = GetTickCount();
				if (button.WentDown)
				{
					const uint32_t elapsed = tick - button.RepeatBegin;
					if (elapsed < 250)
					{
						button.NumClicks == 2 = true;
						button.RepeatBegin = 0;
					}
					else
					{
						button.RepeatBegin = tick;
					}
				}
			}

			Current.mouse.Wheel = Buffer.WheelDelta;
			*/
		}

		// null device
		{
			Null.Mouse.Pos = Current.Mouse.Pos;
		}

		// clear buffer
		//NeZero(Buffer);
	}

	void Win32Input::HandleMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		Input_Notify( hWnd, msg, wParam, lParam );

		switch ( msg )
		{
		case WM_SETFOCUS:
			NeTrace("SetFocus\n");
			HasFocus = true;
			break;

		case WM_KILLFOCUS:
			NeTrace("KillFocus\n");
			HasFocus = false;
			break;

			/*
		case WM_MOUSEWHEEL:
			Buffer.WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
			break;

			/*
		case WM_CHAR:
			if (wParam >= ' ')
			{
				if (Buffer.NumChars < NeCountOf(Buffer.Char))
					Buffer.Char[ Buffer.NumChars++ ] = (wchar_t)wParam;
			}
			break;
			*/
		}
	}

	//==================================================================================
	static Rect_s ToNe( const RECT& r )
	{
		return Rect_s
			{ (float)r.left
			, (float)r.top
			, (float)(r.right-r.left)
			, (float)(r.bottom-r.top)
			};
	}

	//==================================================================================
	// globals
	static Win32Input Input;

	//==================================================================================
	// public interface
	void Initialize( Allocator_t alloc, Renderer_t renderer )
	{ System_Initialize( alloc, renderer ); }

	void Shutdown()
	{ System_Shutdown(); }

	void BeginFrame( HWND hWnd )
	{
		RECT rc;
		GetClientRect( hWnd, &rc );
		Input.Wnd = hWnd;
		Input.Poll();

		Context_BeginFrame( System_GetContext(), ToNe( rc ), Input.GetMouse(), Input.GetKeyboard() );
	}

	void HandleMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{ Input.HandleMessage( hWnd, msg, wParam, lParam ); }

	const Mouse_s& GetMouse()
	{ return Context_GetMouse( System_GetContext() ); }

	const Keyboard_s& GetKeyboard()
	{ return Context_GetKeyboard( System_GetContext() ); }

	const DragInfo_s& GetDrag()
	{ return Context_GetDrag( System_GetContext() ); }

	bool IsHot( Id_t id )
	{ return Context_IsHot( System_GetContext(), id ); }

	bool IsActive( Id_t id )
	{ return Context_IsPushed( System_GetContext(), id ); }

	void SetHot( Id_t id )
	{ return Context_SetHot( System_GetContext(), id ); }

	void SetActive( Id_t id )
	{ return Context_SetPushed( System_GetContext(), id ); }

	void BeginDrag( Id_t id, const Vec2_s& offset )
	{ Context_BeginDrag( System_GetContext(), id, offset ); }

	void BeginDrag( Id_t id )
	{ Context_BeginDrag( System_GetContext(), id, GetMouse().Pos ); }

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	//==================================================================================
	// globals
	NE_THREAD_LOCAL static TextBuffer ThreadLocalTextBuffer = {};

	//==================================================================================
	// public interface
	TextBuffer& GetTextBuffer()
	{ return ThreadLocalTextBuffer; }

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	void PushClip( const Rect_s& r ) 
	{ return Graphics_SetClipRect( Context_GetGraphics( System_GetContext() ), &r ); }

	void PopClip()
	{ return Graphics_SetClipRect( Context_GetGraphics( System_GetContext() ), nullptr ); }

	void DrawLine( const Color_c& c, float x0, float y0, float x1, float y1 )
	{ return Graphics_DrawLine( Context_GetGraphics( System_GetContext() ), Vec2_s { x0, y0 }, Vec2_s { x1, y1 }, Color_ToArgb( c ) ); }

	void DrawLine( const Color_c& c, float x0, float y0, float x1, float y1, float thickness )
	{ return Graphics_DrawLine( Context_GetGraphics( System_GetContext() ), Vec2_s { x0, y0 }, Vec2_s { x1, y1 }, Color_ToArgb( c ), thickness ); }

	void DrawRectangle( const Rect_s& r, const Color_c& c )
	{ return Graphics_DrawRect( Context_GetGraphics( System_GetContext() ), r, Color_ToArgb( c ) ); }

	void FillRectangle( const Rect_s& r, const Color_c& c )
	{ return Graphics_FillRect( Context_GetGraphics( System_GetContext() ), r, Color_ToArgb( c ) ); }

	void FillRectangle( const Rect_s& r, const Color_c& c1, const Color_c& c2, LinearGradient::Mode m )
	{ return Graphics_FillRect( Context_GetGraphics( System_GetContext() ), r, Color_ToArgb( c1 ), Color_ToArgb( c2 ), m ); }

	void DrawString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Rect_s& r, const Color_c& c )
	{ return Graphics_DrawString( Context_GetGraphics( System_GetContext() ), r, t, l, f, fmt, Color_ToArgb( c ) ); }

	void DrawString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Vec2_s& p, const Color_c& c )
	{ Graphics_DrawString( Context_GetGraphics( System_GetContext() ), p, t, l, f/*, fmt*/, Color_ToArgb( c ) ); }

	void MeasureString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Rect_s& r, Rect_s* b )
	{ *b = Graphics_MeasureString( Context_GetGraphics( System_GetContext() ), r, t, l, f, fmt );  }

	void MeasureString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Vec2_s& p, Rect_s* b )
	{ *b = Graphics_MeasureString( Context_GetGraphics( System_GetContext() ), t, l, f /*, fmt, p*/ ) + p; }

	float GetLineHeight( Font_t f )
	{ return Graphics_GetLineHeight( Context_GetGraphics( System_GetContext() ), f ); }

	float GetMaxCharWidth( Font_t f )
	{ return Graphics_GetMaxCharWidth( Context_GetGraphics( System_GetContext() ), f ); }

	Font_t EnsureFont( const wchar_t* name, float size, FontStyle::Mask style )
	{ return FontCache_CreateFont( System_GetFontCache(), name, size, style ); }

} }
