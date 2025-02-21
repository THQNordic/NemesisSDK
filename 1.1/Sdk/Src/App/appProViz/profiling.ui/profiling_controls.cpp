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
namespace nemesis
{
	static float NiceNumber( float x, bool round )
	{
		float expv;				/* exponent of x */
		float f;				/* fractional part of x */
		float nf;				/* nice, rounded fraction */

		expv = floorf(log10f(x));
		f = x/powf(10.0f, expv);		/* between 1 and 10 */
		if (round)
		if (f<1.5f) nf = 1.0f;
		else if (f<3.0f) nf = 2.0f;
		else if (f<7.0f) nf = 5.0f;
		else nf = 10.0f;
		else
		if (f<=1.0f) nf = 1.0f;
		else if (f<=2.0f) nf = 2.0f;
		else if (f<=5.0f) nf = 5.0f;
		else nf = 10.0f;
		return nf*powf(10.0f, expv);
	}
}

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	static void FormatMs( TextBuffer& text, float ms )
	{
		if ( ms < 1000.0f)
			text.Length = swprintf_s( text.Text, L"%.3f ms", ms );
		else if ( ms < (60 * 1000.0f) )
			text.Length = swprintf_s( text.Text, L"%.3f s", ms * (1.0f/1000.0f) );
		else
			text.Length = swprintf_s( text.Text, L"%.3f min", ms * (1.0f/(60.0f * 1000.0f)) );
	}

	static void FormatHMS( TextBuffer& text, float ms )
	{
		const Time tm = MsToTime( ms );
		text.Length = text.Length = swprintf_s( text.Text, L"%02d:%02d.%03d", tm.Hours * 60 + tm.Minutes, tm.Seconds, tm.Milliseconds );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	static Color_c TintAndBlend( const Color_c& c, float tint, float blend )
	{
		Color_c c2 = c * tint;
		c2.a *= blend;
		return c2;
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	static void BytesToUnits( float bytes, float* out_value, const char** out_unit )
	{
		struct Unit
		{
			const float scale;
			const char* name;
		};

		static Unit UNITS[] =
		{ { 1024.0f * 1024.0f * 1024.0f, "GB" }
		, { 1024.0f * 1024.0f		   , "MB" }
		, { 1024.0f					   , "KB" }
		};

		const char* unit = "bytes";
		float value = bytes;
		for ( int i = 0; i < NeCountOf(UNITS); ++i )
		{
			if ( value >= UNITS[i].scale )
			{ 
				value /= UNITS[i].scale;
				unit = UNITS[i].name; 
				break;
			}
		}
		*out_value = value;
		*out_unit = unit;
	}

	static void BytesToUnits( uint32_t bytes, float* out_value, const char** out_unit )
	{ return BytesToUnits( (float)bytes, out_value, out_unit ); }

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace time_bar
{
	//==================================================================================
	// internals
	static float LARGE_TICK_HEIGHT	= 10.0f;
	static float SMALL_TICK_HEIGHT	= 4.0f;
	static float INLAY_MARGIN		= 16.0f;
	static float TEXT_INDENT		= 5.0f;

	struct DrawFrameGroupContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		const FrameRange& cull;
		Database_t db;
		const float tick_ms;
		Rect_s frame_rect;
	};

	static void DrawGroupBackground( const Rect_s& r, const Visual_s& v, int first_frame, int num_frames )
	{ 
		const bool is_single = (num_frames == 1);
		if (is_single)
			FillRectangle( r, v.FrameColor[ first_frame & 1 ] ); 
		else
			FillRectangle( r, v.FrameColor[2], v.FrameColor[3], LinearGradient::ForwardDiagonal );
	}

	static void DrawGroupBeginMarker( const Rect_s& r, const Visual_s& v )
	{ DrawLine( v.TickColor, r.x, r.y, r.x, r.y + LARGE_TICK_HEIGHT ); }

	static void DrawGroupLabels( const Rect_s& r, const Visual_s& v, const FrameGroup& g, Database_t db )
	{
		const Clock clock = Database_GetClock( db );
		const bool is_single = (g.Frames.Count == 1);
		const int color_index = is_single ? (g.Frames.First & 1) : 2;
		const float indent_top = GetLineHeight( v.SmallFont ) + 1.0f;

		// lead-in
		Rect_s text_rect;
		Rect_s bounds_rect;
		{
			const int last_frame = g.Frames.Last();
			const Tick first_tick = Database_GetFrames( db )[ g.Frames.First ].Time.Begin;
			const Tick last_tick  = Database_GetFrames( db )[ last_frame     ].Time.End;
			const float group_ms = clock.TickToMs( last_tick - first_tick );

			TextBuffer& text = gui::GetTextBuffer();
			text.Length = is_single 
				? swprintf_s( text.Text, L"Frame: %u [%.2f ms]"     , g.Frames.First + Database_GetFirstFrameNumber( db ), group_ms )
				: swprintf_s( text.Text, L"Frame: %u - %u [%.2f ms]", g.Frames.First + Database_GetFirstFrameNumber( db ), last_frame + Database_GetFirstFrameNumber( db ), group_ms )
				;
			MeasureString( text.Text, text.Length, v.LargeFont, TextFormat::NoWrap, r, &bounds_rect );

			text_rect.x		 = r.x      + TEXT_INDENT;
			text_rect.y		 = r.y		+ INLAY_MARGIN + indent_top;
			text_rect.h = r.h - INLAY_MARGIN - indent_top;
			text_rect.w  = NeMin( bounds_rect.w, r.w  - TEXT_INDENT - TEXT_INDENT );
			DrawString( text.Text, text.Length, v.LargeFont, TextFormat::NoWrap, text_rect, v.TextColor[ color_index ] );
		}

		// lead-out
		{ 
			const int last_frame = g.Frames.Last();
			const Tick last_tick  = Database_GetFrames( db )[ last_frame ].Time.End;
			const float group_ms = clock.TickToMs( last_tick - Database_GetFirstTick( db ) );

			const Time tm = MsToTime( group_ms );

			TextBuffer& text = gui::GetTextBuffer();
			text.Length = swprintf_s( text.Text, L"%02d:%02d.%03d", tm.Hours * 60 + tm.Minutes, tm.Seconds, tm.Milliseconds );
			MeasureString( text.Text, text.Length, v.MediumFont, TextFormat::NoWrap, r, &bounds_rect );
			if ((text_rect.w + bounds_rect.w + 2.0f) < r.w )
			{
				text_rect = bounds_rect;
				text_rect.x += r.w - bounds_rect.w - TEXT_INDENT;
				text_rect.y += indent_top + INLAY_MARGIN;
				DrawString( text.Text, text.Length, v.MediumFont, TextFormat::Right | TextFormat::NoWrap, text_rect, v.TextColor[ color_index ] );
			}
		}
	}

	static void DrawGroupTicks( const Rect_s& clip, const Rect_s& r, const Visual_s& v, const FrameGroup& g, const ZoomInfo& zoom, Database_t db, float tick_ms )
	{
		// calculate groups's total duration in [ms]
		const Clock clock = Database_GetClock( db );
		const int last_frame = g.Frames.Last();
		const Tick first_tick = Database_GetFrames( db )[ g.Frames.First ].Time.Begin;
		const Tick last_tick  = Database_GetFrames( db )[ last_frame     ].Time.End;
		const float group_ms = clock.TickToMs( last_tick - first_tick );

		// calculate the visible interval 
		Rect_s tmp = clip;
		tmp = Rect_Intersect( tmp, r );
		const float x_offset_left	 = NeMax( 0.0f, clip.x - r.x );
		const float ms_offset_left	 = zoom.PixelToMs( x_offset_left );
		const float ms_duration		 = zoom.PixelToMs( tmp.w );
		const float ms_tick_offset   = floorf((ms_offset_left / tick_ms)) * tick_ms;

		const float first_visible_ms = NeMax( ms_tick_offset, tick_ms );
		const float last_visible_ms  = ms_offset_left + ms_duration;

		// draw the ticks
		float x;
		const float y1 = r.y + r.h;
		const float y0 = r.y + r.h - LARGE_TICK_HEIGHT;
		for ( float ms = first_visible_ms; ms <= last_visible_ms; ms += tick_ms )
		{
			x = r.x + zoom.MsToPixel( ms );

			// tick mark
			DrawLine( v.TickColor, x, y0, x, y1 );

			// tick label
			TextBuffer& text = gui::GetTextBuffer();
			FormatMs( text, ms );
			tmp = Rect_s { x, r.y, r.w + (r.x - x), r.h };
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, tmp, v.TickColor );
		}
	}

	static void DrawFrameGroup( void* context, const FrameGroup& g )
	{
		DrawFrameGroupContext& args = *((DrawFrameGroupContext*)context);

		// layout
		const Clock clock = Database_GetClock( args.db );
		const Frame& frame_0 = Database_GetFrames( args.db )[g.Frames.First];
		const Frame& frame_1 = Database_GetFrames( args.db )[g.Frames.Last()];
		const float x0 = args.zoom.TickToPixel( frame_0.Time.Begin - args.cull.Time.Begin, clock );
		const float x1 = args.zoom.TickToPixel( frame_1.Time.End   - args.cull.Time.Begin, clock );
		const float w = x1-x0;

		args.frame_rect = args.r;
		args.frame_rect.x += x0;
		args.frame_rect.w = w;

		// draw
		DrawGroupBackground( args.frame_rect, args.v, g.Frames.First, g.Frames.Count );
		DrawGroupBeginMarker( args.frame_rect, args.v );
		DrawGroupTicks( args.r, args.frame_rect, args.v, g, args.zoom, args.db, args.tick_ms );
		DrawGroupLabels( args.frame_rect, args.v, g, args.db );
	}

	static void DrawFramesLod( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const FrameGroupSetup& setup )
	{
		// calculate tick mark duration
		const Clock clock = Database_GetClock( db );
		const Tick num_ticks = cull.Time.Duration();
		const float total_ms = clock.TickToMs( num_ticks );
		const float total_x = r.w;
		const float ms_per_x = total_ms / total_x;
		const float ms_per_marker = 100.0f * ms_per_x;
		const float nice_ms_per_marker = NiceNumber( ms_per_marker, false );

		// draw frame groups
		DrawFrameGroupContext context = { r, v, zoom, cull, db, nice_ms_per_marker };
		Database_EnumFrameGroups( db, cull, setup, DrawFrameGroup, &context );
	}

	//==================================================================================
	// public interface
	float CalculateHeight( const Visual_s& v )
	{
		return INLAY_MARGIN 
			 + LARGE_TICK_HEIGHT 
			 + GetLineHeight( v.LargeFont )
			 + GetLineHeight( v.SmallFont )
			 + 1.0f
			 ;
	}

	Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const FrameGroupSetup& setup )
	{
		NePerfScope("timebar");

		// draw backgound
		FillRectangle( r, v.BkgndColor );

		// draw frames
		DrawFramesLod( r, v, zoom, cull, db, setup );

		// draw tick line
		if (cull.Frames.Count)
			DrawLine( v.TickColor, r.x, Rect_Bottom( r ) - 0.5f, Rect_Right( r ), Rect_Bottom( r ) - 0.5f );

		// input
		Result result = { Action::None, cull.Time.Begin };

		if (Rect_Contains( r, gui::GetMouse().Pos ))
			SetHot( id );

		if ( gui::IsActive( id ) )
		{
			const DragInfo_s& drag = gui::GetDrag();
			const Vec2_s delta_pos = drag.Pos - gui::GetMouse().Pos;
			BeginDrag( id );

			const Clock clock = Database_GetClock( db );
			const Tick delta_tick = zoom.PixelToTick( delta_pos.x, clock );
			result.Action = Action::Dragging;
			result.TrackTick += delta_tick;
			return result;
		}
		
		if ( gui::IsHot( id ) )
		{
			if (gui::GetMouse().Button[0].WentDown)
			{
				SetActive( id );
				BeginDrag( id );
			}
		}

		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace summary_bar
{
	static const Vec2_s INLAY_MARGIN = Vec2_s { 5.0f, 5.0f };
	static const float COLUMN_SPACING = 3.0f;

	struct DrawFrameGroupContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		const FrameRange& cull;
		Database_t db;

		Rect_s frame_rect;
	};

	static void DrawGroupDelimiter( const Rect_s& r, const Visual_s& v )
	{
		const Rect_s delimiter_rect = Rect_s { r.x, r.y, 1.0f, r.h };
		FillRectangle( delimiter_rect, v.DelimiterColors[0], v.DelimiterColors[1], LinearGradient::Vertical );
	}

	static void DrawGroupHeader( const Rect_s& r, const Visual_s& v, const FrameGroup& g, Database_t db )
	{
		TextBuffer& text = gui::GetTextBuffer();
		text.Length = (g.Frames.Count == 1)
				? swprintf_s( text.Text, L"Frame: %u"     , g.Frames.First + Database_GetFirstFrameNumber( db ) )
				: swprintf_s( text.Text, L"Frame: %u - %u", g.Frames.First + Database_GetFirstFrameNumber( db ), g.Frames.Last() + Database_GetFirstFrameNumber( db ) )
				;
		DrawString( text.Text, text.Length, v.LargeFont, TextFormat::NoWrap, r, Color::White );
	}

	static void DrawGroupValues( const Rect_s& r, const Visual_s& v, const FrameGroup& g, Database_t db )
	{
		// labels
		const wchar_t* label = 
			L"Duration: \n"
			L"Location: \n"
			L"Zones: \n"
			L"Payload: \n"
			L"Parsed: \n"
			L"Clock: \n"
			;

		Rect_s text_bounds;
		MeasureString( label, -1, v.SmallFont, TextFormat::Default, r, &text_bounds );
		DrawString   ( label, -1, v.SmallFont, TextFormat::Default, r, Color::White );
		if (text_bounds.w >= r.w)
			return;

		// values
		const Clock clock = Database_GetClock( db );
		TextBuffer& text = gui::GetTextBuffer();
		const float line_height = gui::GetLineHeight( v.SmallFont );

		Rect_s value_bounds;
		value_bounds.x = r.x + text_bounds.w + COLUMN_SPACING;
		value_bounds.y = r.y;
		value_bounds.w = r.w - (text_bounds.w + COLUMN_SPACING);
		value_bounds.h = r.h;

		Layout l;
		layout::Begin( l, value_bounds, Vec2_s { 0.0f, 0.0f }, layout::Direction::Vertical );
		
		// duration
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			const Tick num_ticks = 
				  Database_GetFrames( db )[ g.Frames.Last() ].Time.End 
				- Database_GetFrames( db )[ g.Frames.First  ].Time.Begin
				;
			if (g.Frames.Count == 1)
				FormatMs( text, clock.TickToMs( num_ticks ) );
			else
				FormatHMS( text, clock.TickToMs( num_ticks ) );
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}

		// location
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			const Tick begin_tick = Database_GetFrames( db )[ g.Frames.First ].Time.Begin;
			const Tick frame_tick = begin_tick - Database_GetFirstTick( db );
			FormatHMS( text, clock.TickToMs( frame_tick ) );
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}

		// zones
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			uint32_t total_scope_events = 0;
			const int frame_end = g.Frames.End();
			for ( int frame_index = g.Frames.Begin(); frame_index < frame_end; ++frame_index )
				total_scope_events += Database_GetFrames( db )[ frame_index ].NumScopeEvents;
			text.Length = swprintf_s( text.Text, L"%u", total_scope_events/2 );
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}

		// size
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			uint32_t total_size = 0;
			/*
			const int frame_end = g.Frames.End();
			for ( int frame_index = g.Frames.Begin(); frame_index < frame_end; ++frame_index )
				total_size += Database_GetFrames( db )[ frame_index ].TotalSize();
			*/

			float total;
			const char* total_unit;
			BytesToUnits( total_size, &total, &total_unit );

			if (g.Frames.Count == 1)
			{
				text.Length = swprintf_s( text.Text, L"%.3f %S", total, total_unit );
			}
			else
			{
				float avg;
				const char* avg_unit;
				BytesToUnits( ((float)total_size)/((float)g.Frames.Count), &avg, &avg_unit );
				text.Length = swprintf_s( text.Text, L"%.3f %S [avg: %.2f %S]", total, total_unit, avg, avg_unit );
			}
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}

		// parsed size
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			uint32_t total_size = 0;
			const int frame_end = g.Frames.End();
			for ( int frame_index = g.Frames.Begin(); frame_index < frame_end; ++frame_index )
				total_size += Database_GetFrames( db )[ frame_index ].ParsedBytes;

			float total;
			const char* total_unit;
			BytesToUnits( total_size, &total, &total_unit );

			if (g.Frames.Count == 1)
			{
				text.Length = swprintf_s( text.Text, L"%.3f %S", total, total_unit );
			}
			else
			{
				float avg;
				const char* avg_unit;
				BytesToUnits( ((float)total_size)/((float)g.Frames.Count), &avg, &avg_unit );
				text.Length = swprintf_s( text.Text, L"%.3f %S [avg: %.2f %S]", total, total_unit, avg, avg_unit );
			}
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}

		// clock
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, line_height );
			if (!item.Visible)
				return;
			text.Length = swprintf_s( text.Text, L"%.3f MHz", clock.TicksPerMs() );
			DrawString( text.Text, text.Length, v.SmallFont, TextFormat::NoWrap, item.Bounds, Color::White );
		}
	}

	static void DrawGroupLabel( const Rect_s& r, const Visual_s& v, const FrameGroup& g, Database_t db )
	{
		// layout
		Layout l;
		layout::Begin( l, Rect_Inflate( r, -INLAY_MARGIN.x, -INLAY_MARGIN.y ), Vec2_s { 0.0f, 1.0f }, layout::Direction::Vertical );

		// header
		{
			const layout::Item item = layout::Append( l, l.Bounds.w, gui::GetLineHeight( v.LargeFont ) );
			if (!item.Visible)
				return;
			DrawGroupHeader( item.Bounds, v, g, db );
		}

		// values
		{
			const layout::Item item = layout::Remainder( l );
			if (!item.Visible)
				return;
			DrawGroupValues( item.Bounds, v, g, db );
		}
	}

	static void DrawFrameGroup( void* context, const FrameGroup& g )
	{
		DrawFrameGroupContext& args = *((DrawFrameGroupContext*)context);

		// layout
		const Clock clock = Database_GetClock( args.db );
		const Frame& frame_0 = Database_GetFrames( args.db )[g.Frames.First];
		const Frame& frame_1 = Database_GetFrames( args.db )[g.Frames.Last()];
		const float x0 = args.zoom.TickToPixel( frame_0.Time.Begin - args.cull.Time.Begin, clock );
		const float x1 = args.zoom.TickToPixel( frame_1.Time.End   - args.cull.Time.Begin, clock );
		const float w = x1-x0;

		args.frame_rect = args.r;
		args.frame_rect.x += x0;
		args.frame_rect.w = w;

		// draw
		DrawGroupDelimiter( args.frame_rect, args.v );
		DrawGroupLabel    ( args.frame_rect, args.v, g, args.db );
	}

	static void DrawFramesLod( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const FrameGroupSetup& setup )
	{
		DrawFrameGroupContext context = { r, v, zoom, cull, db };
		Database_EnumFrameGroups( db, cull, setup, DrawFrameGroup, &context );
	}

	void Draw( const Rect_s& r, const Visual_s& v, const State& s, const ZoomInfo& zoom, const FrameRange& frame_range, Database_t db, const FrameGroupSetup& setup )
	{
		FillRectangle( r, v.BkgndColors[0], v.BkgndColors[1], LinearGradient::Vertical );
		if (s.Collapsed)
			return;
		DrawFramesLod( r, v, zoom, frame_range, db, setup );
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace lock_bar
{
	static const float LABEL_MARGIN = 4.0f;

	struct DrawLockBarsContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		Database_t db;
		const FrameRange& frames;
		Result& result;

		float prev_x;
		int   prev_thread;
	};

	static void DrawLockBars( void* context, const LockEvent& ev, int event_index )
	{
		DrawLockBarsContext& args = *((DrawLockBarsContext*)context);
		const Rect_s& r = args.r;
		const Visual_s& v = args.v;
		const ZoomInfo& zoom = args.zoom;
		const Clock clock = Database_GetClock( args.db );
		const FrameRange& frames = args.frames;
		Result& result = args.result;

		// dot
		const float x = zoom.TickToPixel( ev.Tick - frames.Time.Begin, clock );
		const float y = ev.Lock * (v.LockHeight + v.LockSpacing) + v.LockHeight/2;

		// bar
		if ((args.prev_thread == ev.Thread) && (!ev.Enter))
		{
			const float w = x - args.prev_x - 10.0f;
			if ( w > 2.0f )
				FillRectangle( Rect_s { r.x + args.prev_x + 8.0f, r.y + y + 1.0f, w, 4.0f }, v.ThreadColors[ ev.Thread % v.NumThreadColors ] );
		}

		// next
		args.prev_thread = ev.Thread;
		args.prev_x = x;
	}

	struct DrawLockEventsContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		Database_t db;
		const FrameRange& frames;
		Result& result;
	};

	static void DrawLockEvents( void* context, const LockEvent& ev, int event_index )
	{
		DrawLockEventsContext& args = *((DrawLockEventsContext*)context);
		const Rect_s& r = args.r;
		const Visual_s& v = args.v;
		const ZoomInfo& zoom = args.zoom;
		const Clock clock = Database_GetClock( args.db );
		const FrameRange& frames = args.frames;
		Result& result = args.result;

		const float x = zoom.TickToPixel( ev.Tick - frames.Time.Begin, clock );
		const float y = ev.Lock * (v.LockHeight + v.LockSpacing) + v.LockHeight/2;

		Rect_s event_rect { r.x + x, r.y + y, 6.0f, 6.0f };
		if (Rect_Contains( event_rect, gui::GetMouse().Pos ))
			result.Event = event_index;
		FillRectangle( event_rect, v.EventColor );
		event_rect = Rect_Inflate( event_rect, -1.0f, -1.0f );
		FillRectangle( event_rect, ev.Enter ? Color::OrangeRed : Color::OliveDrab );
	}

	struct DrawFrameGroupContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		Database_t db;
		const FrameRange& frames;
		Result& result;
	};

	static void DrawFrameGroup( void* context, const FrameGroup& g )
	{
		DrawFrameGroupContext& args = *((DrawFrameGroupContext*)context);
		if (g.Frames.Count > 1)
			return;

		const FrameRange range = 
		{ args.frames.Time.Begin
		, args.frames.Time.End
		, g.Frames.First
		, g.Frames.Count
		};

		const int num_locks = Database_GetNumLocks( args.db );
		DrawLockEventsContext event_args = { args.r, args.v, args.zoom, args.db, range, args.result };
		for ( int lock_index = 0; lock_index < num_locks; ++lock_index )
			Database_EnumLockEvents( args.db, lock_index, range.Frames.First, range.Frames.Count, DrawLockEvents, &event_args );
	}

	Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup )
	{
		Result result = { -1 };

		// background
		FillRectangle( r, v.BkgndColors[0], v.BkgndColors[1], LinearGradient::Vertical );
		if (s.Collapsed)
			return result;

		// tracks
		Rect_s lock_rect;
		const int num_locks = Database_GetNumLocks( db );
		for ( int lock_index = 0; lock_index < num_locks; ++lock_index )
		{
			const float y = lock_index * (v.LockHeight + v.LockSpacing) + v.LockHeight/2;
			lock_rect = Rect_s { r.x, r.y + y + 2.0f, r.w, 2.0f };
			FillRectangle( lock_rect, v.LockColor );
		}
		
		// labels
		Lock lock;
		TextBuffer& text = gui::GetTextBuffer();
		for ( int lock_index = 0; lock_index < num_locks; ++lock_index )
		{
			const float y = lock_index * (v.LockHeight + v.LockSpacing);
			lock_rect = Rect_s { r.x + LABEL_MARGIN, r.y + y, r.w - 2 * LABEL_MARGIN, r.h - y };

			Database_GetLock( db, lock_index, lock );
			text.Length = lock.Name
					? swprintf_s( text.Text, L"%d: %S", lock_index, lock.Name )
					: swprintf_s( text.Text, L"%d: <0x%llx>", lock_index, (uint64_t)lock.Handle_t );
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::NoWrap, lock_rect, v.Label.TextColor );
		}

		// events
		{
			DrawFrameGroupContext args = { r, v, zoom, db, frames, result };
			Database_EnumFrameGroups( db, frames, setup, DrawFrameGroup, &args );
		}

		// bars
		{
			DrawLockBarsContext args = { r, v, zoom, db, frames, result };
			for ( int lock_index = 0; lock_index < num_locks; ++lock_index )
			{
				args.prev_thread = -1;
				Database_EnumLockEvents( db, lock_index, frames.Frames.First, frames.Frames.Count, DrawLockBars, &args );
			}
		}
		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace cpu_bar
{
	static const float LABEL_MARGIN = 4.0f;

	struct DrawCpuGroupContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		const FrameRange& frames;
		Database_t db;
		Result& result;
	};

	static void DrawCpuGroups( void* context, const CpuGroup& g )
	{
		const DrawCpuGroupContext& args = *((DrawCpuGroupContext*)context);
		const Clock clock = Database_GetClock( args.db );

		// color
		const int color_index = (g.Thread % args.v.NumThreadColors);
		const Color_c& color = args.v.ThreadColors[ color_index ];

		// layout
		const float x0 = args.zoom.TickToPixel( g.Time.Begin - args.frames.Time.Begin, clock );
		const float x1 = args.zoom.TickToPixel( g.Time.End   - args.frames.Time.Begin, clock );
		const float w = x1-x0;

		const float y0 = (0.5f + g.Cpu ) * (args.v.CpuHeight + args.v.CpuSpacing);
		const float y1 = (0.5f + g.Cpu2) * (args.v.CpuHeight + args.v.CpuSpacing);
		const float h = (args.v.CpuHeight - args.v.CpuSpacing)/2;

		// draw
		if (g.Cpu == g.Cpu2)
		{
			const Rect_s bar_rect { args.r.x + x0, args.r.y + y0, w, h };
			FillRectangle( bar_rect, color, color * 0.5f, LinearGradient::BackwardDiagonal );
			DrawBorder( bar_rect, color * 1.25f, color * 0.25f );
		}
		else
		{
			const float half_height = 0.5f * h - 1.5f;

			const Rect_s bar_rect_1 { args.r.x + x0, args.r.y + y0, w, half_height };
			FillRectangle( bar_rect_1, color, color * 0.5f, LinearGradient::BackwardDiagonal );
			DrawBorder( bar_rect_1, color * 1.25f, color * 0.25f );

			const Rect_s bar_rect_2 { args.r.x + x0, args.r.y + y1 + h - half_height, w, half_height };
			FillRectangle( bar_rect_2, color, color * 0.5f, LinearGradient::BackwardDiagonal );
			DrawBorder( bar_rect_2, color * 1.25f, color * 0.25f );
		}
	}

	Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup )
	{
		Result result = {};

		// background
		FillRectangle( r, v.BkgndColors[0], v.BkgndColors[1], LinearGradient::Vertical );
		if (s.Collapsed)
			return result;

		// tracks
		Rect_s cpu_rect;
		const int num_cpus = Database_GetNumCpus( db );
		for ( int cpu_index = 0; cpu_index < num_cpus; ++cpu_index )
		{
			const float y = cpu_index * (v.CpuHeight + v.CpuSpacing) + v.CpuHeight/2;
			cpu_rect = Rect_s { r.x, r.y + y + 2.0f, r.w, 2.0f };
			FillRectangle( cpu_rect, v.TrackColor );
		}
		
		// labels
		TextBuffer& text = gui::GetTextBuffer();
		for ( int cpu_index = 0; cpu_index < num_cpus; ++cpu_index )
		{
			const float y = cpu_index * (v.CpuHeight + v.CpuSpacing);
			cpu_rect = Rect_s { r.x + LABEL_MARGIN, r.y + y, r.w - 2 * LABEL_MARGIN, r.h - y };
			text.Length = swprintf_s( text.Text, L"Cpu %u", cpu_index );
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::NoWrap, cpu_rect, v.Label.TextColor );
		}

		// bars
		const uint32_t num_threads = (uint32_t)Database_GetNumThreads( db );
		for ( uint32_t thread_index = 0; thread_index < num_threads; ++thread_index )
		{
			const uint32_t flag = (1 << thread_index);
			if (NeHasFlag(s.CollapsedThreadMask, flag))
				continue;
			NePerfScope("cpu thread");
			DrawCpuGroupContext args = { r, v, zoom, frames, db, result };
			const CpuGroupSetup group_setup = {};
			Database_EnumCpuGroups( db, frames, group_setup, thread_index, DrawCpuGroups, &args );
		}

		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace zone_bar
{
	static const char* GetEnterZoneSuffix( uint8_t type )
	{
		const char* suffix[] = 
		{ ""
		, " [IDLE]"
		, " [LOCK]"
		};
		return suffix[ type ];
	}

	struct DrawZoneGroupsContext
	{
		const Rect_s& r;
		const Visual_s& v;
		const ZoomInfo& zoom;
		const FrameRange& cull;
		uint8_t clip;
		Database_t db;

		Color_c zone_fill_color   [2];
		Color_c zone_border_color [2];
		Color_c group_fill_color  [2];
		Color_c group_border_color[2];
		Color_c block_fill_color  [2];
		Color_c block_border_color[2];
		float tint_value        [3];
		float label_threshold;

		Result& result;
	};

	static void DrawZoneGroups( void* context, const ZoneGroup& group )
	{
		const bool is_group = (group.NumZones > 1);

		DrawZoneGroupsContext& args = *((DrawZoneGroupsContext*)context);
		const Visual_s& v = args.v;
		const bool is_idle = (group.Type == ScopeType::Idle);
		if (is_idle && (!v.ShowIdleZones))
			return;

		// color
		const bool is_block = (group.Type == ScopeType::Lock);
		const Color_c* fill_color = is_block ? args.block_fill_color : (is_group ? args.group_fill_color : args.zone_fill_color);
		const Color_c* border_color = is_block ? args.block_border_color : (is_group ? args.group_border_color : args.zone_border_color);
		const float label_threshold = args.label_threshold;
		Result& result = args.result;

		const float tint = args.tint_value[ group.Index % 3 ];
		const float alpha = is_idle ? 0.5f : 1.0f;

		// layout
		const Clock clock = Database_GetClock( args.db );
		const Rect_s& r = args.r;
		const ZoomInfo& zoom = args.zoom;
		const FrameRange& cull = args.cull;
		float x0 = zoom.TickToPixel( group.Time.Begin - cull.Time.Begin, clock );
		float x1 = zoom.TickToPixel( group.Time.End   - cull.Time.Begin, clock );
		float w = x1-x0;
		if (args.clip)
		{
			x0 = NeMax( x0, 0.0f );
			x1 = NeMin( x1, r.w );
			w = x1-x0;
		}

		Rect_s zone_rect;
		zone_rect.y = r.y + group.Level * (v.ZoneHeight + v.ZoneSpacing) + INLAY_MARGIN;
		zone_rect.x = r.x + x0;
		zone_rect.w = w;
		zone_rect.h = v.ZoneHeight;

		// hit test
		if ( Rect_Contains( zone_rect, gui::GetMouse().Pos ) )
		{
			result.Thread = group.Thread;
			result.FirstZone = group.Index;
			result.NumZones = group.NumZones;
			result.Time = group.Time;
		}

		// zone inlay
		FillRectangle( zone_rect, TintAndBlend( fill_color[ 0 ], tint, alpha ), TintAndBlend( fill_color[ 1 ], tint, alpha ), LinearGradient::BackwardDiagonal );
		if ( zone_rect.w < 1.5f )
			return;

		// zone border
		DrawBorder( zone_rect, border_color[ 0 ] * tint, border_color[ 1 ] * tint );
		if ( zone_rect.w < label_threshold )
			return;

		// zone label
		NamedLocation loc;
		Database_GetLocation( args.db, group.Location, loc );

		TextBuffer& text = gui::GetTextBuffer();
		text.Length = is_group
			? swprintf_s( text.Text, L"%d x %S", group.NumZones, loc.Name )
			: swprintf_s( text.Text, L"%S (%.2f ms)%S", loc.Name, clock.TickToMs( group.Time.Duration() ), GetEnterZoneSuffix( group.Type ) );
			;
		DrawString( text.Text, text.Length, v.Font, TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap, Rect_Inflate( zone_rect, -LABEL_MARGIN, 0.0f ), v.TextColor );
	}

	Result Draw( const Rect_s& r, const Visual_s& v, Database_t db, const ZoomInfo& zoom, const FrameRange& cull, const ZoneGroupSetup& setup, int thread_index )
	{
		Result result = {};
		FillRectangle( r, v.BkgndColors[0], v.BkgndColors[1], LinearGradient::Vertical );
		if (v.Collapsed)
			return result;

		const float max_char_width = GetMaxCharWidth( v.Font );
		const float label_threshold = max_char_width + LABEL_MARGIN;
		const Color_c block_color			 = Color::Crimson;
		const Color_c group_color			 = Color::CornflowerBlue;
		const Color_c block_fill_color  [] = { block_color * 0.75f, block_color * 0.50f };
		const Color_c block_border_color[] = { block_color * 0.80f, block_color * 0.20f };
		const Color_c group_fill_color  [] = { group_color * 0.75f, group_color * 0.50f };
		const Color_c group_border_color[] = { group_color * 0.80f, group_color * 0.20f };
		const Color_c zone_fill_color   [] = { v.ZoneColor * 0.75f, v.ZoneColor * 0.50f };
		const Color_c zone_border_color [] = { v.ZoneColor * 0.80f, v.ZoneColor * 0.20f };
		const float tint_value		  [] = { 1.25f, 1.0f, 0.9f };

		DrawZoneGroupsContext args = 
		{ r
		, v
		, zoom
		, cull
		, setup.Clip
		, db
		, { zone_fill_color   [0], zone_fill_color   [1] }
		, { zone_border_color [0], zone_border_color [1] }
		, { group_fill_color  [0], group_fill_color  [1] }
		, { group_border_color[0], group_border_color[1] }
		, { block_fill_color  [0], block_fill_color  [1] }
		, { block_border_color[0], block_border_color[1] }
		, { tint_value[0], tint_value[1], tint_value[2] }
		, label_threshold
		, result
		};
		Database_EnumZoneGroups( db, cull, setup, thread_index, DrawZoneGroups, &args );
		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace zone_bar_list
{
	float CalcThreadHeight( int thread_index, Database_t db, uint32_t collapse_mask, float collapsed_height, float expanded_height, float expanded_spacing, int max_depth )
	{
		const uint32_t thread_flag = (1 << thread_index);
		if ( NeHasFlag( collapse_mask, thread_flag ) )
			return collapsed_height;
		Thread thread;
		Database_GetThread( db, thread_index, thread );
		const int num_levels = NeMin( (int)thread.NumLevels, (1+max_depth) );
		const float thread_height = num_levels * ( expanded_height + expanded_spacing ) + zone_bar::INLAY_MARGIN + zone_bar::INLAY_SPACING;
		return NeMax( collapsed_height, thread_height );
	}

	Result Draw( const Rect_s& clip, const Rect_s& r, const Visual_s& v, const State& s, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const ZoneGroupSetup& setup )
	{
		Result result = {};

		zone_bar::Visual_s thread_visual = 
		{ v.Font
		, v.BkgndColors[0]
		, v.BkgndColors[1]
		, Color::White
		, Color::White
		, v.ExpandedHeight
		, v.ExpandedSpacing
		, v.ShowIdleZones
		};

		Rect_s thread_rect = r;
		int color_index = 0;
		int num_threads = Database_GetNumThreads( db );
		for ( int thread_index = 0; thread_index < num_threads; ++thread_index )
		{
			// calculate thread rect
			thread_rect.h = CalcThreadHeight( thread_index, db, s.CollapsedMask, v.CollapsedHeight, v.ExpandedHeight, v.ExpandedSpacing, setup.MaxZoneLevel );

			// below top?
			if (Rect_Bottom( thread_rect ) > clip.y)
			{
				// draw
				color_index = thread_index % v.NumThreadColors;
				thread_visual.ZoneColor = v.ThreadColors[ color_index ];
				thread_visual.Collapsed = NeHasFlag( s.CollapsedMask, (uint32_t)(1 << thread_index) );
				const zone_bar::Result thread_result = zone_bar::Draw( thread_rect, thread_visual, db, zoom, cull, setup, thread_index );
				if (thread_result.NumZones)
					result.HitZone = thread_result;
			}

			// advance
			thread_rect.y += thread_rect.h;

			// below bottom?
			if (thread_rect.y >= Rect_Bottom( clip ))
				break;
		}		
		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace frame_bar
{
	namespace
	{
		/// metrics
		static const float FRAME_WIDTH_MIN	 = 3.0f;
		static const float KEY_FRAME_SPACING = 150.0f;

		struct FrameBarGroup
		{
			int First;
			int Count;
			Tick Sum;
			Tick Peak;
			float Pos;
			float Width;

			void Next()
			{
				First += Count;
				Pos += Width;
				Count = 0;
				Sum = 0;
				Peak = 0;
				Width = 0.0f;
			}
		};

		static void DrawFrameBarGroup( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const Clock& clock, float target_fps, float target_height, const Color_c& c1, const Color_c& c2, FrameBarGroup& group )
		{
			//NePerfScope("frame group");
			if (!group.Count)
				return;
			group.Width = zoom.TickToPixel( group.Sum, clock );
			const float frame_ms = clock.TickToMs( group.Peak );
			const float relative_time = frame_ms * target_fps * (1.0f/1000.0f);
			const float height = NeMin( relative_time * target_height, r.h );
			const float max_w = r.w - group.Pos;
			const Rect_s bar_rect { r.x + group.Pos, r.y + r.h - height, NeMin( group.Width, max_w ), height };
			FillRectangle( bar_rect, c1, c2, LinearGradient::Horizontal );
		}
	}

	void DrawFpsMarker( const Rect_s& r, const Visual_s& v, float target_fps, float marker_fps )
	{
		const float ratio = marker_fps / target_fps;
		if ( ratio > 1.0f )
			return;

		const float target_height = (r.h * 3.0f) / 4.0f;
		const float marker_hight = ratio * target_height;

		const Rect_s target_rect = Rect_s { r.x, Rect_Bottom( r ) - marker_hight - 1.0f, r.w, 2.0f };
		FillRectangle( target_rect, v.FpsMarkerColor );
	}

	void DrawRangeMarker( const Rect_s& r, const Visual_s& v, const FrameRange& cull, const FrameRange& range )
	{
		// calculate realtive position & size
		const float full_range = (float)(cull.Time.End - cull.Time.Begin);
		const float view_range = (float)(range.Time.End - range.Time.Begin);
		if ((full_range <= 0.0f) || (view_range <= 0.0f))
			return;

		const float ratio = NeMin( 1.0f, view_range / full_range );
		const float pos = (float)(range.Time.Begin - cull.Time.Begin) / (full_range);

		// calculate abosulte position & size
		const float x = pos   * r.w;
		const float w = NeMax( 1.0f, ratio * r.w );

		// draw
		static const float CARET_WIDTH  = 3.0f;
		static const float CARET_HEIGHT = 2.0f;

		Rect_s range_rect = r;
		range_rect.x	  += x;
		range_rect.w   = w;
		FillRectangle( range_rect, v.RangeMarkerColor + 0.25f );
		DrawRectangle( range_rect, v.RangeMarkerColor );

		Rect_s caret_rect = range_rect;
		caret_rect.w = CARET_WIDTH;
		caret_rect.h = CARET_HEIGHT;

		caret_rect.x = range_rect.x - CARET_WIDTH;
		caret_rect.y = range_rect.y;
		FillRectangle( caret_rect, v.RangeMarkerColor );

		caret_rect.x = range_rect.x + range_rect.w; 
		caret_rect.y = range_rect.y;
		FillRectangle( caret_rect, v.RangeMarkerColor );

		caret_rect.x = range_rect.x - CARET_WIDTH; 
		caret_rect.y = range_rect.y - CARET_HEIGHT + range_rect.h;
		FillRectangle( caret_rect, v.RangeMarkerColor );

		caret_rect.x = range_rect.x + range_rect.w; 
		caret_rect.y = range_rect.y - CARET_HEIGHT + range_rect.h;
		FillRectangle( caret_rect, v.RangeMarkerColor );
	}

	void DrawFrameBars( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, float target_fps )
	{
		NePerfScope("frame groups");
		const Color_c bright = 0xffaaaaaa;
		const Color_c dark   = 0xff555555;
		const Color_c c1 = Color_Blend( bright, v.FrameBarColor, 0.5f );
		const Color_c c2 = Color_Blend( dark  , v.FrameBarColor, 0.5f );

		const Clock clock = Database_GetClock( db );

		const float target_height = (r.h * 3.0f) / 4.0f;
		const Tick min_ticks = zoom.PixelToTick( FRAME_WIDTH_MIN, clock );

		FrameBarGroup group = {};

		const int frame_end = cull.Frames.First + cull.Frames.Count;
		for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = Database_GetFrames( db )[ frame_index ];
			const Tick frame_ticks = frame.Time.Duration();

			// add frame to group
			group.Peak = NeMax(group.Peak, frame_ticks);
			group.Sum += frame_ticks;
			++group.Count;

			// accumulate small frames
			if (group.Sum < min_ticks)
				continue;

			// flush accumulated frames
			DrawFrameBarGroup( r, v, zoom, clock, target_fps, target_height, c1, c2, group );
			group.Next();
		}

		DrawFrameBarGroup( r, v, zoom, clock, target_fps, target_height, c1, c2, group );
	}

	void DrawKeyFrames( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db )
	{
		NePerfScope("key frames");

		// scratch
		TextBuffer& text = gui::GetTextBuffer();
		Time time = {};

		// visuals
		Font_t font_large = EnsureFont( 12.0f, FontStyle::Default );
		Font_t font_small = EnsureFont( 10.0f, FontStyle::Default );

		// layout
		const float tick_y0 = r.y + 2.0f;
		const float tick_y1 = r.y + 8.0f;
		const float large_text_y = r.y + 12.0f;
		const float small_text_y = r.y + 14.0f;

		// frames
		const Clock clock = Database_GetClock( db );
		Vec2_s text_pos;
		Rect_s text_bounds;
		float prev_x = -r.w;
		const int frame_end = cull.Frames.First + cull.Frames.Count;
		for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = Database_GetFrames( db )[ frame_index ];
			const float x0 = r.x + zoom.TickToPixel( frame.Time.Begin - cull.Time.Begin, clock );
			const float x1 = r.x + zoom.TickToPixel( frame.Time.End   - cull.Time.Begin, clock );
			const float d = (x0 - prev_x);

			// key frame
			if ( d >= KEY_FRAME_SPACING )
			{
				prev_x = x0;
				DrawLine( Color::Black, x0, tick_y0, x0, tick_y1 );
				text_pos = Vec2_s { x0, large_text_y };
				text.Length = swprintf_s( text.Text, L"%u", Database_GetFirstFrameNumber( db ) + frame_index );
				DrawString( text.Text, text.Length, font_large, TextFormat::NoWrap, text_pos, Color::Black );
				MeasureString( text.Text, text.Length, font_large, TextFormat::NoWrap, text_pos, &text_bounds );
				text_pos.x += 2.0f + text_bounds.w;
				text_pos.y += 2.0f;
				time = MsToTime( clock.TickToMs( frame.Time.Begin - Database_GetFirstTick( db ) ) );
				text.Length = swprintf_s( text.Text, L"[%02d:%02d:%02d.%03d]", time.Hours, time.Minutes, time.Seconds, time.Milliseconds );
				DrawString( text.Text, text.Length, font_small, TextFormat::NoWrap, text_pos, Color::Navy );
			}
		}
	}

	int HitTestFrame( const Rect_s& r, const ZoomInfo& zoom, Tick scroll_tick, Database_t db, const Vec2_s& pos )
	{
		if (Rect_Contains( r, pos ))
		{
			const Clock clock = Database_GetClock( db );
			const float view_x = pos.x - r.x;
			const Tick tick = zoom.PixelToTick( view_x, clock );
			return Database_TickToFrame( db, scroll_tick + tick );
		}
		return -1;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace graph_bar
{
	Result Draw( const Rect_s& r, const Visual_s& v, const Item& item, const ZoomInfo& zoom, const FrameRange& cull, Database_t db )
	{
		NePerfScope("graph");

		Result result = { -1 };

		// empty?
		if (cull.Frames.Count == 0)
			return result;

		// find extrema
		const Clock clock = Database_GetClock( db );
		Counter counter;
		Database_GetCounter( db, item.Counter, counter );
		float float_min = counter.Minimum;
		float float_max = counter.Maximum;

		// calculate max height 
		const float height_scale = v.HeightScale ? v.HeightScale : 1.0f;
		const float scaled_height = r.h * height_scale;
		const float min_width = NeMax( 1.0f, v.MinLineWidth );

		// draw
		{
			FillRectangle( r, v.BkgndColor );

			int num_values = 0;
			float x, y;
			float prev_x = r.x;
			float prev_y = Rect_Bottom( r );

			float dy = 0.0f;
			float prev_dy = 0.0f;

			CounterValue value;
			const int frame_end = cull.Frames.First + cull.Frames.Count;
			for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
			{
				const Frame& frame = Database_GetFrames( db )[ frame_index ];
				const int value_end = frame.FirstCounterValue + frame.NumCounterValues;
				for ( int value_index = frame.FirstCounterValue; value_index < value_end; ++value_index )
				{
					Database_GetCounterValue( db, value_index, value );
					if (value.Name != counter.Name)
						continue;

					// calculate normalized value
					const float normalized_value = (value.Value - float_min) / (float_max - float_min);

					// calculate end coords
					x = r.x + zoom.TickToPixel( frame.Time.End - cull.Time.Begin, clock );
					y = Rect_Bottom( r ) - normalized_value * scaled_height;

					// first value?
					if (0 == num_values++)
					{
						prev_x = r.x + zoom.TickToPixel( frame.Time.Begin - cull.Time.Begin, clock );
						prev_y = y;
					}

					// calculate slope
					dy = y - prev_y;

					// below threshold?
					const float delta_x = x-prev_x;
					if (delta_x < min_width)
					{
						/*
						// no change in orientation?
						const bool sign_now = dy > 0.0f;
						const bool sign_prev = prev_dy > 0.0f;
						if ( sign_now == sign_prev )
						*/
							continue;
					}
						
					// draw line segment
					NePerfScope("segment");
					DrawLine( v.LineColor, prev_x, prev_y, x, y );
					prev_x = x;
					prev_y = y;
					prev_dy = dy;
				}
			}
		}

		return result;
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace graph_label
{
	struct ExponentInfo
	{
		float Exponent;
		const wchar_t* Suffix;
	};

	static const ExponentInfo EXPONENTS [] =
	{ {  9.0f, L"G" }
	, {  6.0f, L"M" }
	, {  3.0f, L"K" }
	, {  0.0f, L"" }
	/*
	, { -3.0f, L"m" }
	, { -6.0f, L"\u00b5" }
	, { -9.0f, L"n" }
	*/
	};

	static void FormatValue( float value, TextBuffer& text )
	{
		const float exp	= log10f( value );
		for ( int i = 0; i < NeCountOf(EXPONENTS); ++i )
		{
			const ExponentInfo& e = EXPONENTS[i];
			if ( exp >= e.Exponent )
			{
				text.Length = swprintf_s( text.Text, L"%.3f%s", value / powf( 10.0f, e.Exponent ), e.Suffix );
				return;
			}
		}

		text.Length = swprintf_s( text.Text, L"%.3f", value );
	}

	void Draw( const Rect_s& r, const Visual_s& v, const Item& item, const ZoomInfo& zoom, const FrameRange& cull, Database_t db )
	{
		FillRectangle( r, v.BkgndColor );

		TextBuffer& text = gui::GetTextBuffer();
		Counter counter;
		Database_GetCounter( db, item.Counter, counter );

		const float scaled_height = v.HeightScale * r.h;
		const float scaled_maximum = counter.Maximum / v.HeightScale;

		//if (counter.Minimum == counter.Maximum)
		{
			FormatValue( counter.Maximum, text );
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::NoWrap, Vec2_s { r.x + v.LabelIndent, Rect_Bottom( r ) - scaled_height }, v.Label.TextColor );
			return;
		}

		/*
		const float range = counter.Maximum - counter.Minimum;
		const float value_scale = 1.0f / range;

		const float line_height = GetLineHeight( v.Label.Font );
		const float item_height = 5.0f * line_height;

		const float num_steps  = r.h / item_height;

		const float step	   = range / num_steps;
		const float nice_step  = NiceNumber( step, true );
		const float nice_start = counter.Minimum; //NiceNumber( counter.Minimum , false );

		int count = 0;
		for ( float value = nice_start; value <= scaled_maximum; value += nice_step )
		{
			// calculate normalized value
			const float normalized_value = (value - counter.Minimum) * value_scale;

			// calculate end coords
			const float y = r.Bottom() - normalized_value * scaled_height;

			// draw label
			FormatValue( value, text );
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::NoWrap, Vec2_s { r.x + v.LabelIndent, y ), v.Label.TextColor );
			++count;
			if (count > 30)
				break;
		}

		//NePerfCounter("viz/timeline/label/values", (uint32_t)count);
		*/
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace graph_tree
{
	//==================================================================================
	// internals
	static bool DrawCounter( Layout& l, const tree_ctrl::Visual_s& v, Database_t db, State& state, int counter_index, int depth )
	{
		// layout
		layout::Item item = layout::Append( l, l.Bounds.w, tree_ctrl::CalculateItemHeight( v ) );
		if (!item.Visible)
			return false;

		Counter counter;
		Database_GetCounter( db, counter_index, counter );
		ItemState& counter_state = state.CounterStates[ counter_index ];
		const int counter_id  = counter_index; //(int)counter.Name;
		const int color_index = counter_id % NeCountOf(GRAPH_COLORS);

		// color marker
		Rect_s marker_rect = item.Bounds;
		marker_rect.w = tree_ctrl::DEPTH_INDENT;
		marker_rect.x    += tree_ctrl::DEPTH_INDENT * (depth-1);
		marker_rect = Rect_Inflate( marker_rect, -2.0f, -1.0f );
		FillRectangle( marker_rect, GRAPH_COLORS[ color_index ] );
		DrawRectangle( marker_rect, Color::Black );

		// item
		item.Bounds.x	  += tree_ctrl::DEPTH_INDENT * depth;
		item.Bounds.w -= tree_ctrl::DEPTH_INDENT * depth;
		//const Color c = Color_Blend( Color::LightGray, GRAPH_COLORS[ color_index ], counter_state.Enabled ? 0.75f : 0.25f );
		const Color_c c = Color_Blend( Color::LightGray, Color::CornflowerBlue, counter_state.Enabled ? 0.7f : 0.0f );
		gui::DrawBar( Rect_Inflate( item.Bounds, -2.0f, 0.0f ), c, 1.0f );

		// label
		const char* part = strrchr( counter.Name, '/' );
		const char* name = part ? part + 1 : counter.Name;

		TextBuffer& text = gui::GetTextBuffer();
		text.Length = swprintf_s( text.Text, L"%S", name );
		DrawString( text.Text, text.Length, v.Label.Font, TextFormat::Middle | TextFormat::NoWrap, Rect_Inflate( item.Bounds, -6.0f, 0.0f ), 0xff3e3e2e );

		// input
		const Id_t id = ID( __FUNCTION__, __LINE__, counter_id );
		if (button::Input( id, item.Bounds ))
			state.CounterStates[ counter_index ].Enabled = !state.CounterStates[ counter_index ].Enabled;
		return true;
	}

	static bool DrawGroup( Layout& l, const tree_ctrl::Visual_s& v, Database_t db, State& state, int group_index, int depth )
	{
		CounterGroup group;
		Database_GetCounterGroup( db, group_index, group );
		layout::Item item = layout::Append( l, l.Bounds.w, tree_ctrl::CalculateItemHeight( v ) );
		item.Bounds.x	  += tree_ctrl::DEPTH_INDENT * depth;
		item.Bounds.w -= tree_ctrl::DEPTH_INDENT * depth;

		gui::DrawBar( Rect_Inflate( item.Bounds, -2.0f, -1.0f ), Color::Goldenrod, 1.0f );
		TextBuffer& text = gui::GetTextBuffer();

		char buffer[64] = "";
		strncpy_s( buffer, group.Name, group.Length );
		const char* part = strrchr( buffer, '/' );
		const char* name = part ? part + 1 : buffer;
		text.Length = swprintf_s( text.Text, L"%S", name );
		DrawString( text.Text, text.Length, v.Label.Font, TextFormat::Middle | TextFormat::NoWrap, Rect_Inflate( item.Bounds, -6.0f, 0.0f ), 0xff3e3e2e );

		const Id_t id = ID( __FUNCTION__, __LINE__, group_index );
		if (button::Input( id, item.Bounds ))
			state.GroupStates[ group_index ].Collapsed = !state.GroupStates[ group_index ].Collapsed;

		return item.Visible;
	}

	static bool DrawCounterGroup( Layout& l, const tree_ctrl::Visual_s& v, Database_t db, State& state, int group_index, int depth )
	{
		// group header
		DrawGroup( l, v, db, state, group_index, depth );

		CounterGroup group;
		Database_GetCounterGroup( db, group_index, group );
		const ItemState& group_state = state.GroupStates[ group_index ];
		if (group_state.Collapsed)
			return true;

		// child groups
		CounterGroup child;
		int num_children = Database_GetNumCounterGroups( db );
		for ( int child_index = group_index + 1; child_index < num_children; ++child_index )
		{
			Database_GetCounterGroup( db, child_index, child );
			if (child.Parent != group_index)
				continue;

			if (!DrawCounterGroup( l, v, db, state, child_index, depth + 1 ))
				return false;
		}

		// child counters
		Counter counter;
		int num_counters = Database_GetNumCounters( db );
		for ( int counter_index = 0; counter_index < num_counters; ++counter_index )
		{
			Database_GetCounter( db, counter_index, counter );
			if (counter.Group != (uint32_t)group_index)
				continue;

			if (!DrawCounter( l, v, db, state, counter_index, depth + 1 ))
				return false;
		}

		return true;
	}

	static int CountItems( const State& state, Database_t db, int group_index )
	{
		int num_items = 1;
		CounterGroup group;
		Database_GetCounterGroup( db, group_index, group );
		const ItemState& group_state = state.GroupStates[ group_index ];
		if (group_state.Collapsed)
			return num_items;

		CounterGroup child;
		int num_children = Database_GetNumCounterGroups( db );
		for ( int child_index = group_index + 1; child_index < num_children; ++child_index )
		{
			Database_GetCounterGroup( db, child_index, child );
			if (child.Parent != group_index)
				continue;
			num_items += CountItems( state, db, child_index );
		}

		Counter counter;
		int num_counters = Database_GetNumCounters( db );
		for ( int counter_index = 0; counter_index < num_counters; ++counter_index )
		{
			Database_GetCounter( db, counter_index, counter );
			if (counter.Group != (uint32_t)group_index)
				continue;
			++num_items;
		}
		return num_items;
	}

	//==================================================================================
	// public interface
	float CalculateHeight( const Visual_s& v, const State& state, Database_t db )
	{
		const int num_groups = Database_GetNumCounterGroups( db );
		if (!num_groups)
			return 0.0f;

		const tree_ctrl::Visual_s tv = 
		{   { gui::EnsureFont( 10.0f, FontStyle::Bold ), Color::Black }
		, { { gui::EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, Color::White }
		};

		const int num_items = CountItems( state, db, 0 );
		const float item_height = 1.0f + tree_ctrl::CalculateItemHeight( tv );
		return num_items * item_height;
	}

	void Draw( const Rect_s& r, const Visual_s& v, State& state, Database_t db )
	{
		// update state
		{
			if (state.ActiveCounter >= Database_GetNumCounters( db ))
				state.ActiveCounter = -1;
		}

		// background
		{
			FillRectangle( r, 0xffe6e6f5 );
			DrawRectangle( r, Color::Black );
		}

		// client
		gui::PushClip( r );
		{
			if (Database_GetNumCounterGroups( db ))
			{
				const tree_ctrl::Visual_s tv = 
				{   { gui::EnsureFont( 10.0f, FontStyle::Bold ), Color::Black }
				, { { gui::EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, Color::White }
				};

				Layout l;
				layout::Begin( l, r, Vec2_s { 0.0f, 1.0f }, layout::Direction::Vertical, Vec2_s { 0.0f, -state.ScrollPos } );
				DrawCounterGroup( l, tv, db, state, 0, 0 );
			}
		}
		gui::PopClip();
	}

	void Initialize( State& state )
	{
		NeZero(state);
		state.ActiveCounter = -1;
	}

	void Shutdown( State& state )
	{
		state.GroupStates.Clear();
		state.CounterStates.Clear();
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace profile_bar
{
	static const float MARGIN = 5.0f;

	//==================================================================================
	// internals
	static void DrawFrameGroup( const Rect_s& r, const Visual_s& v, Database_t db, const IndexRange& frames, const ZoomInfo& zoom, const HotSpot* items, int count )
	{
		// delimiter
		DrawLine( 0xff3c3c48, r.x, r.y, r.x, Rect_Bottom( r ) );

		// setup
		TextBuffer& text = gui::GetTextBuffer();

		Rect_s text_rect = r;
		text_rect = Rect_Inflate( text_rect, -MARGIN * 2.0f, 0.0f );
		text_rect.h = v.ItemHeight;

		Rect_s bar_rect = r;
		bar_rect = Rect_Inflate( bar_rect, -MARGIN, -MARGIN );
		bar_rect.h = v.ItemHeight;

		// invariants
		const Tick total_duration 
			= Database_GetFrames( db )[ frames.Last () ].Time.End
			- Database_GetFrames( db )[ frames.Begin() ].Time.Begin;
		const float one_over_total_duration = 1.0f / ((float)total_duration);

		// bars
		const float max_bar_width = r.w - 2.0f * MARGIN;
		for ( int i = 0; i < count; ++i )
		{
			const float score = one_over_total_duration * items[i].Duration;
			const float score_value = NeMin( 1.0f, score );
			bar_rect.y = r.y + i * (v.ItemHeight + v.ItemSpacing) + INLAY_MARGIN;
			bar_rect.w = max_bar_width * score_value;
			FillRectangle( bar_rect, v.BarColors[ i % NeCountOf(v.BarColors) ] );
		}

		// labels
		for ( int i = 0; i < count; ++i )
		{
			const HotSpot& item = items[i];
			NamedLocation location;
			Database_GetLocation( db, item.Location, location );
			text_rect.y = r.y + i * (v.ItemHeight + v.ItemSpacing) + INLAY_MARGIN;
			text.Length = swprintf_s( text.Text, L"%S", location.Name );
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::Middle | TextFormat::NoWrap, text_rect, v.Label.TextColor );
		}

		// values
		if ( text_rect.w < 120.0f )
			return;

		for ( int i = 0; i < count; ++i )
		{
			const HotSpot& item = items[i];
			const float score_value = one_over_total_duration * items[i].Duration;
			text.Length = swprintf_s( text.Text, L"%u x (%.2f%%)", item.NumSamples, 100.0f * score_value );
			text_rect.y = r.y + i * (v.ItemHeight + v.ItemSpacing) + INLAY_MARGIN;
			DrawString( text.Text, text.Length, v.Label.Font, TextFormat::Right | TextFormat::Middle | TextFormat::NoWrap, text_rect, v.Label.TextColor );
		}
	}

	struct DrawFrameGroupContext
	{
		const Rect_s& r;
		const Visual_s& v;
		Database_t db;
		const FrameRange& cull;
		const ZoomInfo& zoom;
		const FrameGroupSetup& setup;
		HotSpotGroup& spots;
		int max_level;
		Result result;
	};

	static void DrawFrameGroup( void* context, const FrameGroup& g )
	{
		DrawFrameGroupContext& args = *((DrawFrameGroupContext*)context);

		// layout
		const Clock clock = Database_GetClock( args.db );
		const Frame& frame_0 = Database_GetFrames( args.db )[g.Frames.First];
		const Frame& frame_1 = Database_GetFrames( args.db )[g.Frames.Last()];
		const float x0 = args.zoom.TickToPixel( frame_0.Time.Begin - args.cull.Time.Begin, clock );
		const float x1 = args.zoom.TickToPixel( frame_1.Time.End   - args.cull.Time.Begin, clock );
		const float w = x1-x0;

		const Rect_s group_rect = Rect_s { args.r.x + x0, args.r.y, w, args.r.h };
		const Rect_s r = Rect_Intersect( group_rect, args.r );

		// background
		DrawLine( 0xff3c3c48, r.x, r.y, r.x, Rect_Bottom( r ) );

		// cull
		const float min_frame_width = args.zoom.TickToPixel( args.setup.MinFrameTicks, clock );
		const float max_bar_width = r.w - 2.0f * MARGIN;
		if ( max_bar_width < min_frame_width )
			return;

		// build hot spots
		{
			NePerfScope( "build hot spots" );
			const HotSpotRange range = { g.Frames.First, g.Frames.Count, args.max_level };
			Database_BuildHotSpots( args.db, range, args.spots );
		}

		// draw hot spots
		{
			NePerfScope( "draw hot spots" );
			DrawFrameGroup( r, args.v, args.db, g.Frames, args.zoom, args.spots.Data.Data, NeMin( args.spots.Data.Count, args.max_level ) );
		}
	}

	//==================================================================================
	// public interface
	Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup, HotSpotGroup& spots, int max_level )
	{
		Result result;
		FillRectangle( r, v.BkgndColors[0], v.BkgndColors[1], LinearGradient::Vertical );
		if (s.Collapsed)
			return result;
		DrawFrameGroupContext args = { r, v, db, frames, zoom, setup, spots, max_level };
		Database_EnumFrameGroups( db, frames, setup, DrawFrameGroup, &args );
		result = args.result;
		return result;
	}

} } } }
