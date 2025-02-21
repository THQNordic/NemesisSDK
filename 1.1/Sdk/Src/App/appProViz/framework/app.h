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
namespace nemesis { namespace framework	
{
	class App
	{
		NE_NO_COPY(App);

	public:
		App();
		~App();

	public:
		bool InitInstance( HINSTANCE hInst, HWND hWnd, HACCEL hAccel, LPTSTR lpCmdLine );
		void ExitInstance();

	public:
		int Run();

	public:
		virtual bool OnInitInstance() { return true; }
		virtual void OnExitInstance() {}
		virtual void OnNextFrame()	  {}

	public:
		HINSTANCE Instance;
		HACCEL	  Accelerator;
		HWND	  Window;
		LPTSTR	  CommandLine;
	};

} }
