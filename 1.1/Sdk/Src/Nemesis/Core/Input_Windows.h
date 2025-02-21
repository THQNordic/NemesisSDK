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
#include <Nemesis/Core/Math.h>
#include <Nemesis/Core/Memory.h>
#include <Nemesis/Core/Process.h>

//======================================================================================
namespace nemesis
{
	static bool IsAsyncKeyPressed( int vk ) 
	{
		const short bits = GetAsyncKeyState( vk ); 
		return NeHasFlag( bits, 0x8000 ); 
	}

	static void MouseButton_Poll( MouseButtonState_s& next, const MouseButtonState_s& prev, system::StopWatch_c& timer, int vk )
	{
		const bool is_pressed	= IsAsyncKeyPressed( vk );
		const bool was_pressed	= prev.IsPressed;
		const bool went_down	= is_pressed && !was_pressed;
		const bool went_up		= !is_pressed && was_pressed;

		next.IsPressed	= is_pressed;
		next.WentDown	= went_down;
		next.WentUp		= went_up;
		next.Reserved	= 0;
		next.NumClicks  = 0;

		if (went_down)
		{
			const uint32_t elapsed_ms = timer.ElapsedMs();
			if (elapsed_ms < 250)
				next.NumClicks = 2;
			else
				next.NumClicks = 1;
			timer.Start();
		}
	}

	static void VirtualKey_Poll( KeyState_s& next, const KeyState_s& prev, system::StopWatch_c& timer, int vk )
	{
		const bool is_pressed	= IsAsyncKeyPressed( vk );
		const bool was_pressed	= prev.IsPressed;
		const bool went_down	= is_pressed && !was_pressed;
		const bool went_up		= !is_pressed && was_pressed;

		next.IsPressed	= is_pressed;
		next.WentDown	= went_down;
		next.WentUp		= went_up;
		next.Reserved	= 0;
		next.AutoRepeat	= went_down;

		if (is_pressed)
		{
			if (went_down)
			{
				timer.Start();
			}
			else
			{
				next.AutoRepeat = (timer.ElapsedMs() >= 250);
			}
		}
	}
}

//======================================================================================
namespace nemesis
{
	namespace global
	{
		struct InputTimer_s
		{
			system::StopWatch_c MouseDown[ MB::COUNT ];
			system::StopWatch_c KeyRepeat[ VK::COUNT ];
		};

		struct InputInstance_s
		{
			MouseState_s	Mouse;
			KeyboardState_s Keyboard;
			InputTimer_s	Timer;
		};

		static InputInstance_s InputInstance = {};
	}
}

//======================================================================================
namespace nemesis
{
	void Input_Notify( Handle_t wnd, uint32_t msg, uint64_t w, uint64_t l )
	{
		switch ( msg )
		{
		case WM_MOUSEWHEEL:
			global::InputInstance.Mouse.Wheel += GET_WHEEL_DELTA_WPARAM( w ) / 120;
			break;

		case WM_CHAR:
			if (w >= ' ')
			{
				wchar_t ch[2] = { (wchar_t)w, 0 };
				wcscat_s( global::InputInstance.Keyboard.Char, ch );
			}
			break;
		}
	}

	Result_t Mouse_Poll( MouseState_s& mouse, Handle_t wnd )
	{
		global::InputTimer_s& timer = global::InputInstance.Timer;
		const MouseState_s& old_mouse = global::InputInstance.Mouse;
			  MouseState_s& new_mouse = mouse;

		POINT pt;
		GetCursorPos( &pt );				const Vec2_s screen_pos = Vec2_s { (float)pt.x, (float)pt.y };
		ScreenToClient( (HWND)wnd, &pt );	const Vec2_s client_pos = Vec2_s { (float)pt.x, (float)pt.y };
		new_mouse.Pos = client_pos;
		new_mouse.Screen = screen_pos;
		new_mouse.Wheel = old_mouse.Wheel;
		MouseButton_Poll( new_mouse.Button[ MB::Left	  ], old_mouse.Button[ MB::Left		 ], timer.MouseDown[ MB::Left	   ], VK_LBUTTON );
		MouseButton_Poll( new_mouse.Button[ MB::Right	  ], old_mouse.Button[ MB::Right	 ], timer.MouseDown[ MB::Right	   ], VK_RBUTTON );
		MouseButton_Poll( new_mouse.Button[ MB::Middle	  ], old_mouse.Button[ MB::Middle	 ], timer.MouseDown[ MB::Middle	   ], VK_MBUTTON );
		MouseButton_Poll( new_mouse.Button[ MB::SideLeft  ], old_mouse.Button[ MB::SideLeft  ], timer.MouseDown[ MB::SideLeft  ], VK_XBUTTON1 );
		MouseButton_Poll( new_mouse.Button[ MB::SideRight ], old_mouse.Button[ MB::SideRight ], timer.MouseDown[ MB::SideRight ], VK_XBUTTON2 );
		global::InputInstance.Mouse = mouse;
		global::InputInstance.Mouse.Wheel = 0;

		return NE_OK;
	}

	Result_t Keyboard_Poll( KeyboardState_s& kb )
	{
		global::InputTimer_s& timer = global::InputInstance.Timer;
		const KeyboardState_s& old_kb = global::InputInstance.Keyboard;
			  KeyboardState_s& new_kb = kb;

		for ( int i = 0; i < VK::COUNT; ++i )
			VirtualKey_Poll( new_kb.Key[i], old_kb.Key[i], timer.KeyRepeat[i], i );

		memcpy_s( new_kb.Char, sizeof(new_kb.Char), old_kb.Char, sizeof(old_kb.Char) );

		global::InputInstance.Keyboard = kb;
		NeZero(global::InputInstance.Keyboard.Char);
		return NE_OK;
	}
}

//======================================================================================
