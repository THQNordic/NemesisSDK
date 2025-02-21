//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "profiling_viz_view.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	//==================================================================================
	// public interface

	/// Limits the given tick offset within the given view according the the provided zoom settings.
	///
	/// The function allows for an empty area at the end of the view if the given tick would otherwise
	/// cause the last frame to become right-aligned instead of left-aligned.
	Tick ClampScrollTick( Database_t db, const ZoomInfo& zoom, float view_width, Tick tick )
	{
		const Clock clock = Database_GetClock( db );
		const Tick first_tick = Database_GetFirstTick( db );
		const Tick last_end_tick = Database_GetLastEndTick( db );
		const Tick last_begin_tick = Database_GetLastBeginTick( db );
		const Tick ticks_in_rect = zoom.PixelToTick( view_width, clock );

		const Tick right_aligned_max = last_end_tick - ticks_in_rect;
		const Tick left_aligned_max = last_begin_tick + ticks_in_rect;

		const Tick min_tick = first_tick;
		const Tick max_tick = right_aligned_max;

		const Tick capped_max_tick = NeMax( min_tick, max_tick );

		return NeClamp( tick, min_tick, capped_max_tick );
	}

	/// Determines the visible frame of ticks and frames based on the 
	/// given view settings.
	void EnumFrameGroups( Database_t db, const Rect_s& view, Tick scroll_tick, const ZoomInfo& zoom, FrameRange& cull )
	{
		NeZero( cull );
		if (!Database_GetNumFrames( db ))
			return;

		// determine visible tick range
		const Clock clock = Database_GetClock( db );
		cull.Time.Begin = scroll_tick;
		cull.Time.End = scroll_tick + zoom.PixelToTick( view.w, clock );

		// find visible frame range based on visible tick range
		cull.Frames.First = Database_TickToFrame( db, cull.Time.Begin );
		cull.Frames.Count = 0;
		for ( int i = cull.Frames.First; i < Database_GetNumFrames( db ); ++i )
		{
			if (Database_GetFrames( db )[i].Time.Begin >= cull.Time.End)
				break;
			++cull.Frames.Count;
		}
	}

	/// Calculates the maximum frame rate in the given rage of frames.
	float CalculateMaxFps( const FrameRange& cull, Database_t db )
	{
		Tick max_duration = 0;
		Tick frame_ticks = 0;
		const int frame_end = cull.Frames.First + cull.Frames.Count;
		for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = Database_GetFrames( db )[frame_index];
			frame_ticks = frame.Time.Duration();
			if (frame_ticks > max_duration)
				max_duration = frame_ticks;
		}

		const Clock clock = Database_GetClock( db );
		return clock.TickToFps( max_duration );
	}

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	Vec2_s MouseMessageToPos( HWND hWnd, UINT msg, LPARAM lParam )
	{
		POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
		switch ( msg )
		{
		case WM_MOUSEWHEEL:
			ScreenToClient( hWnd, &pt );
			break;
		}
		return Vec2_s { (float)pt.x, (float)pt.y };
	}

} } } }
