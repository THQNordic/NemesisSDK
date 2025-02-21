//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "DockTheme.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	void DockTheme_Initialize( DockTheme_t theme )
	{
		const uint32_t Color_Text				= 0xfff1f1f1;
		const uint32_t Color_Main_Panel			= 0xff2d2d30;
		const uint32_t Color_Dock_Panel			= 0xff252526;
		const uint32_t Color_Dock_Panel_Border	= 0xff3f3f46;
		const uint32_t Color_Splitter_On		= 0x7f000000;
		const uint32_t Color_Splitter_Off		= 0x00000000;
		const uint32_t Color_Indicator			= 0xff007acc;

		theme->FrameBorder			= { 4.0f, 4.0f };
		theme->MinDockedSize		= { 50.0f, 50.0f };
		theme->MinFloatSize			= { 100.0f, 100.0f };
		theme->TabItemSpacing		= { 3.0f, 3.0f };
		theme->OverlayIconSize		= { 24.0f, 24.0f };
		theme->OverlayIconSpace		= { 2.0f, 2.0f };
		theme->OverlayIconColor		= Color::Goldenrod;
		theme->OverlayIconColorHot	= Color::PaleGoldenrod;
		theme->OverlayAreaColor		= Color::CornflowerBlue;
		theme->TearThreshold		= 8.0f;

	#if NE_PLATFORM_IS_WINDOWS
		theme->FrameBorder			= { (float)GetSystemMetrics(SM_CXSIZEFRAME), (float)GetSystemMetrics(SM_CXSIZEFRAME) };
	#endif
	}
}

/*
Blue Theme:
	Float Border:		 32  61 119			Blue 
	Main Bk:			 93 107 153
	Menu Bk:			204 213 240
	Ctrl Header Bk:		 64  86 141
	Tab Ctrl Active:	245 204 132
	Ctrl Bk:			255 255 255
	List Hdr (Inner): 	246 246 246
	List Hdr (Div): 	202 206 219
*/
