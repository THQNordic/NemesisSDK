//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	/// Calculates the tick for the given pixel position based on the given view settings.
	Tick PixelToTick( const Clock& clock, const ZoomInfo& zoom, Tick scroll_tick, float x )
	{ return zoom.PixelToTick( x, clock ) + scroll_tick; }

	/// Calculates the sub-pixel for the given tick position based on the given view settings.
	float TickToPixel( const Clock& clock, const ZoomInfo& zoom, Tick scroll_tick, Tick tick )
	{ return zoom.TickToPixel( tick - scroll_tick, clock ); }

	/// Fills the zoom information to represent the given fraction of pixels per millisecond.
	ZoomInfo ZoomTo( float pixels_per_ms )
	{
		ZoomInfo zoom;
		zoom.PixelsPerMs = pixels_per_ms;
		zoom.PixelsPerSecond = pixels_per_ms * 1000.0f;
		zoom.OneOverPixelsPerSecond = 1.0f / zoom.PixelsPerSecond;
		return zoom;
	}

	/// Scale the zoom factor by a certain power of a given base value while maintaining a given
	/// zoom range.
	ZoomInfo ZoomScale( const ZoomInfo& zoom, float multiplier, float power, float minimum, float maximum )
	{
		const float scale = powf( multiplier, power );
		const float value = scale * zoom.PixelsPerMs;
		const float clamped = NeClamp( value, minimum, maximum );
		return ZoomTo( clamped );
	}

	/// Scale the zoom factor by a certain power of a given base value while maintaining a given
	/// zoom range.
	ZoomInfo ZoomScale( const ZoomInfo& zoom, float multiplier, float power )
	{
		return ZoomScale( zoom, multiplier, power, 1.0f / 1000.0f, 60 * 1000.0f );	// 1 micro-second -> 1 minute
	}

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	/// Scroll the given view by the provided number of units.
	Tick Scroll( float view_width, const ZoomInfo& zoom, const Clock& clock, ScrollUnit::Enum unit, Tick tick, int amount )
	{
		switch ( unit )
		{
		case ScrollUnit::Millisecond:
			return tick + ((amount * clock.TicksPerSecond) / 1000);

		case ScrollUnit::Second:
			return tick + amount * clock.TicksPerSecond;
			break;

		case ScrollUnit::Page:
			return tick + amount * zoom.PixelToTick( view_width, clock );
			break;

		default:
			return tick;
		}
	}

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	/// Calculates the view coordinate for a given client coordinate.
	Vec2_s ClientToView( const Rect_s& view, const Vec2_s& client_pos )
	{
		Vec2_s view_pos( client_pos );
		view_pos.x -= view.x;
		view_pos.y -= view.y;
		return view_pos;
	}

	/// Calculates the client coordinate for a given view coordinate.
	Vec2_s ViewToClient( const Rect_s& view, const Vec2_s& view_pos )
	{
		Vec2_s client_pos( view_pos );
		client_pos.x += view.x;
		client_pos.y += view.y;
		return client_pos;
	}

} } } }
