//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once
#include "graphics_ui_profiler_views.h"

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace zone_panel
	{
		struct Horizontal
		{
			struct Scroll { enum Mode { None, Auto }; };
		};

		struct State
		{
			Horizontal::Scroll::Mode HScroll;
			bool OptionsCollapsed;
			//float ZoneHeight;
			//bool ShowIdleZones;
		};

		struct Instance
		{
			State State;
			zone_view::Instance ZoneView;
		};

		void Initialize( Instance& panel, Skin* skin );
		void Shutdown( Instance& panel );
		void Draw( Instance& panel, Session& session, const Rect_s& r, Database_t db );
		void FitToFrameIndex( Instance& panel, Database_t db, int i );
	}

	namespace timeline_panel
	{
		struct Vertical
		{
			struct Scale { enum Mode { FPS, Visible, All }; };
		};

		struct Horizontal
		{
			struct Scale  { enum Mode { None, Fit }; };
			struct Scroll { enum Mode { None, Auto }; };
		};

		struct State
		{
			Vertical::Scale::Mode VScale;
			Horizontal::Scale::Mode HScale;
			Horizontal::Scroll::Mode HScroll;
			float TargetFps;

			graph_tree::State GraphTree;
			bool GraphTreeCollapsed;
		};

		struct Instance
		{
			Skin*	 Skin;
			Rect_s	 View;
			Tick	 Scroll;
			ZoomInfo Zoom;
			State	 State;
		};

		void Initialize( Instance& panel, Skin* skin );
		void Shutdown( Instance& panel );
		void Draw( Instance& panel, Session& session, const Rect_s& view, Database_t db );
		void GotoFrameNumber( Instance& panel, Database_t db, uint32_t i );
		void GotoFrameIndex( Instance& panel, Database_t db, int i );
		void GotoFirstFrame( Instance& panel, Database_t db );
		void GotoLastFrame( Instance& panel, Database_t db );
		void GotoBegin( Instance& panel, Database_t db );
		void GotoEnd( Instance& panel, Database_t db );
		void FitAll( Instance& panel, Database_t db );
		void Scroll( Instance& panel, Database_t db, ScrollUnit::Enum unit, int amount );
		int HitTestFrame( const Instance& panel, Database_t db, const Vec2_s& pos );
	}

	namespace session_panel
	{
		void Draw( Session& session, const Skin* skin, const Rect_s& view, Database_t db );
	}

	namespace options_panel
	{
		void Draw( const Rect_s& view, Database_t db );
	}

	namespace about_panel
	{
		void Draw( const Rect_s& view, Database_t db );
	}

	namespace main_panel
	{
		struct View
		{
			enum Enum { Session, Timeline, Zones/*, Log, Options*/, About };
		};

		struct Instance
		{
			View::Enum ActiveView;
			timeline_panel::Instance TimeView;
			zone_panel::Instance	 ZoneView;
		};

		void Initialize( Instance& panel, Skin* skin );
		void Shutdown( Instance& panel );
		void Draw( Instance& panel, Session& session, const Rect_s& view, Database_t db );
	}

} } }
