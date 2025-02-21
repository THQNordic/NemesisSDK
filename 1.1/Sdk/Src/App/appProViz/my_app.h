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
#include "profiling_viz_view_main.h"

//======================================================================================
using namespace nemesis::framework;

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
	void AdvanceFrame();
	void RenderFrame();
	void PresentFrame();
	void SendFrame();
	void Throttle();

public:
	void RenderStats();
	void RenderOverlays();

public:
	void Resize();

public:
	bool HandleInput( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult );

public:
	void Connect();
	void Loopback();
	void Disconnect();
	void StartSpam();
	void StopSpam();

public:
	profiling::viz::Skin Skin;
	profiling::viz::Session Session;
	profiling::viz::view::MainPanel MainPanel;
	float FrameMs;
	uint32_t FrameNumber;
	uint32_t OverSleptTime;
	system::StopWatch_c FrameTimer;
	profiling::Parse::Mode ParseMode;
#if (NE_CONFIG < NE_CONFIG_MASTER)
	system::SlimThread_c SpamThread[4];
#endif
	gfx::Gfx_t Gfx;
	gfx::Gui_t Gui;
};

//======================================================================================
extern MyApp theApp;
