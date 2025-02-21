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
struct MyGui
{
public:
	static void Initialize	 ();
	static void Shutdown	 ();
	static void Render		 ();
	static bool QuitRequested();
};


//======================================================================================
namespace nemesis { namespace gui
{
	/// Window Manager

	struct DesktopWindowEx_s
	{
		ne::Vec2_s ( NE_CALLBK *Do )(Context_t dc);
		Window_s Instance;
		ne::Vec2_s ScrollSize;
		ne::Vec2_s ScrollOffset;
		ne::gui::Id_t Focus;
		ne::gui::MenuItem_s* MainMenuItems;
		int MainMenuItemCount;
	};

	struct WindowManagerState_s
	{
		DesktopWindowEx_s* Items;
		int* ZOrderIndexes;
		int Count;
	};

	void NE_API WindowManager_Do( Context_t dc, DesktopWindowEx_s* items, int* zorderindexes, int count );

	WindowManagerState_s NE_API WindowManager_Instance( Context_t dc, DesktopWindowEx_s* items, int* zorderindexes, int count );
	void NE_API WindowManager_Do( Context_t dc, WindowManagerState_s& state );
} }

