//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "App.h"
#include <Nemesis/Core/Core.h>

//======================================================================================
App::App()
	: CommandLine(nullptr)
{
}

App::~App()
{
}

bool App::InitInstance( HINSTANCE hInst, HWND hWnd, HACCEL hAccel, LPTSTR lpCmdLine )
{
	ne::core::Initialize( nullptr );
	Instance = hInst;
	Window = hWnd;
	Accelerator = hAccel;
	CommandLine = lpCmdLine;
	if (OnInitInstance())
		return true;
	return false;
}

void App::ExitInstance()
{
	OnExitInstance();
	ne::core::Shutdown();
}

int App::Run()
{
	MSG msg = {};

	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			//NePerfScope("Message");

			if (!Accelerator || !TranslateAccelerator(msg.hwnd, Accelerator, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			OnNextFrame();
		}
	}

	ExitInstance();
	return (int) msg.wParam;
}

void App::Quit()
{
	PostQuitMessage( 0 );
}
