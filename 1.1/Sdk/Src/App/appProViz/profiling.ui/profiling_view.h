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
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	/// Scrolling units
	struct ScrollUnit 
	{
		enum Enum { Millisecond, Second, Page };
	};

	/// Zoom information for tick-based views
	struct ZoomInfo
	{
		float PixelsPerMs;
		float PixelsPerSecond;
		float OneOverPixelsPerSecond;

		float MsToPixel( float ms ) const						 { return ms * PixelsPerMs; }
		float TickToPixel( Tick tick, const Clock& clock ) const { return MsToPixel( (1000 * tick) * clock.OneOverTicksPerSecond ); }
		float PixelToMs( float ms ) const						 { return 1000.0f * ms * OneOverPixelsPerSecond; }
		Tick  PixelToTick( float x, const Clock& clock ) const	 { return (Tick) ( (clock.TicksPerSecond * x) * OneOverPixelsPerSecond ); }
	};

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	Tick PixelToTick( const Clock& clock, const ZoomInfo& zoom, Tick scroll_tick, float x );
	float TickToPixel( const Clock& clock, const ZoomInfo& zoom, Tick scroll_tick, Tick tick );

	ZoomInfo ZoomTo( float pixels_per_ms );
	ZoomInfo ZoomScale( const ZoomInfo& zoom, float multiplier, float power );
	ZoomInfo ZoomScale( const ZoomInfo& zoom, float multiplier, float power, float minimum, float maximum );

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	Tick Scroll( float view_width, const ZoomInfo& zoom, const Clock& clock, ScrollUnit::Enum unit, Tick tick, int amount );

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	Vec2_s ClientToView( const Rect_s& view, const Vec2_s& client_pos );
	Vec2_s ViewToToClient( const Rect_s& view, const Vec2_s& view_pos );

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	struct Time
	{
		int Hours;
		int Minutes;
		int Seconds;
		int Milliseconds;
	};

	inline Time MsToTime( float ms )
	{
		static const int MsPerHour = 24*60*1000;
		static const int MsPerMinute = 60*1000;
		static const int MsPerSecond = 1000;
		int x = (int)ms;

		Time tm;
		tm.Hours = x / MsPerHour; x-= MsPerHour * tm.Hours;
		tm.Minutes = x / MsPerMinute; x -= MsPerMinute * tm.Minutes;
		tm.Seconds = x / MsPerSecond; x -= MsPerSecond * tm.Seconds;
		tm.Milliseconds = x;
		return tm;
	}

} } } }
