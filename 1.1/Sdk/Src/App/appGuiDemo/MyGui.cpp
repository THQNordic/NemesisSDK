//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "MyGui.h"
#include "MyPerf.h"
#include <Nemesis/Core/Math.h>
#include <Nemesis/Core/Process.h>
#include <Nemesis/Perf/All.h>
#include <Nemesis/Gui/Demo.h>
#include <Nemesis/Gui/Profiling.h>
#include <math.h>

//======================================================================================
using namespace nemesis;
using namespace nemesis::profiling;
using namespace nemesis::gui;

//======================================================================================
namespace
{
	struct MyGui_s
	{
		bool ViewDemo;
		bool ViewProfiler;
		bool QuitRequested;
		MenuState_s MenuState;
	};

	static MyGui_s MyGuiInstance = { true, false, false, false, {} };
}

namespace
{
	typedef void (NE_CALLBK *MenuItemProc_t)( void* context );

	static void On_File_Exit( void* context )
	{
		MyGui_s* instance = (MyGui_s*)context;
		instance->QuitRequested = true;
	}

	static void On_View_Demo( void* context )
	{
		MyGui_s* instance = (MyGui_s*)context;
		instance->ViewDemo		= true;
		instance->ViewProfiler	= !instance->ViewDemo;
	}

	static void On_View_Profiler( void* context )
	{
		MyGui_s* instance = (MyGui_s*)context;
		instance->ViewProfiler	= true;
		instance->ViewDemo		= !instance->ViewProfiler;
	}

	static MenuItem_s file_menu[] = 
	{ { "Exit"		, "", {}, On_File_Exit } 
	};

	static MenuItem_s view_menu[] = 
	{ { "Demo"		, "", {}, On_View_Demo	   } 
	, { "Profiler"	, "", {}, On_View_Profiler }
	};

	static MenuItem_s main_menu[] = 
	{ { "File", "", { file_menu, NeCountOf(file_menu) }, nullptr }
	, { "View", "", { view_menu, NeCountOf(view_menu) }, nullptr }
	};

	static Vec2_s MainMenu_Do( Context_t dc )
	{
		const MenuItemList_s main_menu_list = { main_menu, NeCountOf( main_menu ) };
		const Vec2_s main_menu_size = Menu_Main_CalcMenuSize( dc, main_menu_list );

		const Rect_s desktop_rect = Context_GetChild( dc );
		const Rect_s main_menu_rect = { 0.0f, 0.0f, desktop_rect.w, main_menu_size.y };

		Context_BeginModal( dc );
			Menu_DoMain( dc, NE_UI_ID, main_menu_rect, main_menu_list, MyGuiInstance.MenuState );
		Context_EndModal( dc );

		const MenuItem_s* activated_item = MyGuiInstance.MenuState.Activated;
		if (activated_item && activated_item->UserData)
			((MenuItemProc_t)activated_item->UserData)( &MyGuiInstance );

		return main_menu_size;
	}
}

//======================================================================================
static void Theme_Initialize()
{
	Context_t dc = System_GetContext();
	Theme_s* theme = Context_GetTheme( dc );
	Theme_ApplyPreset( theme, ThemePreset::Dark );
}

//======================================================================================
void MyGui::Initialize()
{
	Theme_Initialize();
	Demo_Initialize();
	On_View_Demo( &MyGuiInstance );
}

void MyGui::Shutdown()
{
	Demo_Shutdown();
}

void MyGui::Render()
{
	NePerfScope("Gui");
	Context_t dc = System_GetContext();
	const Vec2_s main_menu_size = MainMenu_Do( dc );
	const Rect_s desktop_rect = Context_GetChild( dc );
	const Rect_s child_rect = { 0.0f, main_menu_size.y, desktop_rect.w, desktop_rect.h - main_menu_size.y };
	Context_BeginChild( dc, child_rect );
	{
		if (MyGuiInstance.ViewDemo)
		{
			Demo_Render();
		}
		else if (MyGuiInstance.ViewProfiler)
		{
			PerfView_Do( dc, MyPerf::GetDatabase(), MyPerf::GetParser() );
		}
	}
	Context_EndChild( dc );
}

bool MyGui::QuitRequested()
{
	return MyGuiInstance.QuitRequested;
}
