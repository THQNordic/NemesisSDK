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
#include "MathTypes.h"

//======================================================================================
namespace nemesis
{
	/// Mouse

	struct MouseButton
	{
		enum Enum
		{ Left
		, Right
		, Middle
		, SideLeft
		, SideRight
		, COUNT
		};
	};

	typedef MouseButton MB;

	struct MouseButtonState_s
	{
		uint8_t IsPressed : 1;
		uint8_t WentDown  : 1;
		uint8_t WentUp	  : 1;
		uint8_t Reserved  : 1;
		uint8_t NumClicks : 4;
	};

	struct MouseState_s
	{
		Vec2_s			   Pos;
		Vec2_s			   Screen;
		int				   Wheel;
		MouseButtonState_s Button[ MouseButton::COUNT ];
	};
}

//======================================================================================
namespace nemesis
{
	/// Keyboard

	struct VirtualKey
	{
		enum Enum
		{ LeftButton		= 0x01
		, RightButton		= 0x02
		, MiddleButton		= 0x04
		, SideLeftButton	= 0x05
		, SideRightButton	= 0x06
		, Cancel			= 0x03
		, Back				= 0x08
		, Tab				= 0x09
		, Clear				= 0x0c
		, Return			= 0x0d
		, Shift				= 0x10
		, Control			= 0x11
		, Alt				= 0x12
		, Pause				= 0x13
		, Captial			= 0x14
		, Kana				= 0x15
		, Junja				= 0x17
		, Final				= 0x18
		, Kanji				= 0x19
		, Escape			= 0x1b
		, Convert			= 0x1c
		, NoConvert			= 0x1d
		, Accept			= 0x1e
		, ModeChange		= 0x1f
		, Space				= 0x20
		, PageUp			= 0x21
		, PageDown			= 0x22
		, End				= 0x23
		, Home				= 0x24
		, Left				= 0x25
		, Up				= 0x26
		, Right				= 0x27
		, Down				= 0x28
		, Select			= 0x29
		, Print				= 0x2a
		, Execute			= 0x2b
		, Snapshot			= 0x2c
		, Insert			= 0x2d
		, Delete			= 0x2e
		, Help				= 0x2f
		, LeftWindows		= 0x5b
		, RightWindows		= 0x5c
		, Apps				= 0x5d
		, Sleep				= 0x5f
		, NumPad0			= 0x60
		, NumPad1			= 0x61
		, NumPad2			= 0x62
		, NumPad3			= 0x63
		, NumPad4			= 0x64
		, NumPad5			= 0x65
		, NumPad6			= 0x66
		, NumPad7			= 0x67
		, NumPad8			= 0x68
		, NumPad9			= 0x69
		, Multiply			= 0x6a
		, Add				= 0x6b
		, Separator			= 0x6c
		, Subtract			= 0x6d
		, Decimal			= 0x6e
		, Divide			= 0x6f
		, F1				= 0x70
		, F2				= 0x71
		, F3				= 0x72
		, F4				= 0x73
		, F5				= 0x74
		, F6				= 0x75
		, F7				= 0x76
		, F8				= 0x77
		, F9				= 0x78
		, F10				= 0x79
		, F11				= 0x7a
		, F12				= 0x7b
		, F13				= 0x7c
		, F14				= 0x7d
		, F15				= 0x7e
		, F16				= 0x7f
		, F17				= 0x80
		, F18				= 0x81
		, F19				= 0x82
		, F20				= 0x83
		, F21				= 0x84
		, F22				= 0x85
		, F23				= 0x86
		, F24				= 0x87
		, NumLock			= 0x90
		, Scroll			= 0x91
		, Equal				= 0x92
		, LeftShift			= 0xa0
		, RightShift		= 0xa1
		, LeftControl		= 0xa2
		, RightControl		= 0xa3
		, LeftAlt			= 0xa4
		, RightAlt			= 0xa5
		, Oem1				= 0xba	// ';:' for US
		, OemPlus			= 0xbb	
		, OemComma			= 0xbc
		, OemMinus			= 0xbd
		, OemPeriod			= 0xbe
		, Oem2				= 0xbf	// '/?' for US
		, Oem3				= 0xc0	// '`~' for US
		, Oem4				= 0xdb	// '[{' for US
		, Oem5				= 0xdc	// '\|' for US
		, Oem6				= 0xdd	// ']}' for US
		, Oem7				= 0xde	// ''"' for US
		, Oem8				= 0xdf
		, OemAX				= 0xe1	// 'AX' key on Japanese AX kbd
		, Oem102			= 0xe2	// "<>" or "\|" on RT 102-key kbd.
		, COUNT				= 0xff
		};
	};

	typedef VirtualKey VK;

	struct KeyState_s
	{
		uint8_t IsPressed : 1;
		uint8_t WentDown  : 1;
		uint8_t WentUp	  : 1;
		uint8_t Reserved  : 4;
		uint8_t AutoRepeat: 1;
	};

	struct KeyboardState_s
	{
		KeyState_s Key [ VirtualKey::COUNT ];
		wchar_t	   Char[ 32 ];
	};
}
