//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once
#include "profiling_viz_view.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	struct ZoneCullSetup
	{
		float MinFrameWidth;		
		float MinZoneWidth;			// smaller zones are merged into groups
		float MinGroupWidth;		// smaller groups are discarded
		float MaxGroupSpacingMs;	// maximum time gap between zones
		uint8_t    MaxZoneLevel;			// zones below are discarded
		uint8_t    Clip;					// clip zones and groups against the view
	};

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace zone_view
	{
		struct Item
		{
			enum Enum { Summary, Zones, Locks, Cpu, Profile };

			Enum Type;
			float CollapsedHeight;
			float ExpandedHeight;
			float ExpandedSpacing;
			int	  MaxDepth;
		};

		struct ItemState
		{
			uint32_t CollapsedMask;
		};

		struct Instance
		{
			// visual
			Skin* Skin;

			// layout
			Rect_s ViewRect;

			// transform
			ZoomInfo	FrameZoom;
			Tick		FrameScroll;
			float		VScrollPos;

			// popup
			zone_bar::Result HitZone;

			// settings
			uint32_t			  CollapsedSummary;
			uint32_t			  CollapsedProfile;
			uint32_t			  CollapsedLocks;
			uint32_t			  CollapsedCpu;
			uint32_t			  CollapsedThreads;
			uint32_t			  CollapsedFooter;
			ZoneCullSetup ZoneCullSettings;

			// cached data
			FrameRange		CulledFrames;
			HotSpotGroup	HotSpots;
		};

		//------------------------------------------------------------------------------

		void Initialize( Instance& view, Skin* skin );
		void Shutdown  ( Instance& view );

		void Draw		( Instance& view, const Rect_s& r, Database_t db );
		void DrawOverlay( Instance& view, const Rect_s& r, Database_t db );

		void GotoFrameNumber( Instance& panel, Database_t db, uint32_t i );
		void GotoFrameIndex	( Instance& panel, Database_t db, int i );
		void GotoFirstFrame	( Instance& panel, Database_t db );
		void GotoLastFrame	( Instance& panel, Database_t db );
		void GotoBegin		( Instance& panel, Database_t db );
		void GotoEnd		( Instance& panel, Database_t db );
		void FitToFrameIndex( Instance& panel, Database_t db, int i );
		void Scroll			( Instance& panel, Database_t db, ScrollUnit::Enum unit, int amount );
	}

} } }
