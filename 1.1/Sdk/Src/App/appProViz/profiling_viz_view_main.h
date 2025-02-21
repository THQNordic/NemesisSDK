//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once

#include "graphics_ui_profiler_panels.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	using namespace gui;
	using namespace gui::profiler;

	/// Main Panel functions and data wrapped as a class
	class MainPanel
	{
		NE_NO_COPY(MainPanel);

	public:
		MainPanel() {}
		~MainPanel() { main_panel::Shutdown( Panel ); }

	public:
		void Initialize( Skin* skin )
		{ main_panel::Initialize( Panel, skin ); }

		void Shutdown()
		{ main_panel::Shutdown( Panel ); }

	public:
		void Draw( Session& session, const Rect_s& view, Database_t db ) 
		{ return main_panel::Draw( Panel, session, view, db ); }

		void ActivateView( main_panel::View::Enum view )
		{ Panel.ActiveView = view; }

	private:
		main_panel::Instance Panel;
	};

} } } }
