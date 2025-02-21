//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once
#include "App.h"
//======================================================================================
#include <Nemesis\Core\Process.h>

//======================================================================================
class MyApp
	: public App
{
public:
	MyApp();

public:
	bool CmdProc( HWND hWnd, int wmId, int wmEvent );
	bool WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult );

public:
	virtual bool OnInitInstance();
	virtual void OnExitInstance();
	virtual void OnNextFrame();

public:
	void RenderFrame();
	void Throttle();

public:
	void Resize();

public:
	bool HandleInput( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult );

public:
	bool HasFocus;
	float FrameMs;
	uint32_t FrameNumber;
	uint32_t OverSleptTime;
	ne::system::StopWatch_c FrameTimer;
	ne::system::StopWatch_c ElapsedTime;
	ne::graphics::Gfx_t Gfx;
	ne::graphics::Gui_t Gui;
};
