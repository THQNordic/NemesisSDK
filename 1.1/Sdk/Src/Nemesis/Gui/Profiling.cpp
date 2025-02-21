//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gui/Profiling.h>
#include <Nemesis/Core/Math.h>
#include <math.h>

//======================================================================================
using namespace nemesis::profiling;

//======================================================================================
namespace nemesis { namespace gui
{
	/// Context

	static Rect_s Context_CalcPopup( Context_t dc, const Vec2_s& size )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s bounds_rect = Context_GetDektop( dc ) - Graphics_GetTransform( g );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Vec2_s mouse_off = Vec2_s { 16.0f, 16.0f };
		const Vec2_s popup_pos = mouse_pos + mouse_off;
		const float over_x = (bounds_rect.x + bounds_rect.w - 10.0f) - (popup_pos.x + size.x);
		const float over_y = (bounds_rect.y + bounds_rect.h - 10.0f) - (popup_pos.y + size.y);
		const float over_x_cap = NeMin( 0.0f, over_x );
		Rect_s popup_rect = Rect_Ctor( popup_pos, size );
		popup_rect.x += over_x_cap;
		if (over_y < 0.0f)
			popup_rect.y -= popup_rect.h + /*(over_x_cap ?*/ (mouse_off.y + 8.0f) /*: 0.0f)*/;
		return popup_rect;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Graphics

	static float Graphics_MeasureMaxStringWidth( Graphics_t g, Font_t font, const cstr_t* label, int count )
	{
		float text_w;
		float max_w = 0.0f;
		for ( int i = 0; i < count; ++i )
		{
			text_w = Graphics_MeasureString( g, label[i], font ).w;
			max_w = NeMax( max_w, text_w );
		}
		return max_w;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Clock

	Clock_s Clock_Build( Tick_t ticks_per_second )
	{
		Clock_s clock;
		clock.TicksPerSecond = ticks_per_second;
		clock.OneOverTicksPerSecond = 1.0f/float(ticks_per_second);
		return clock;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zoom

	Zoom_s Zoom_Default()
	{
		return Zoom_Build( 25.0f / 4.0f );
	}

	Zoom_s Zoom_Build( float pixels_per_ms )
	{
		Zoom_s zoom;
		zoom.PixelsPerMs = pixels_per_ms;
		zoom.PixelsPerSecond = pixels_per_ms * 1000.0f;
		zoom.OneOverPixelsPerSecond = 1.0f / zoom.PixelsPerSecond;
		return zoom;
	}

	Zoom_s Zoom_Scale( const Zoom_s& zoom, float multiplier, float power, float min, float max )
	{
		const float scale = powf( multiplier, power );
		const float value = scale * zoom.PixelsPerMs;
		const float clamped = NeClamp( value, min, max );
		return Zoom_Build( clamped );
	}
	
	Zoom_s Zoom_Scale( const Zoom_s& zoom, float multiplier, float power )
	{
		return Zoom_Scale( zoom, multiplier, power, 1.0f / 1000.0f, 60 * 1000.0f );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Timeline

	void Timeline_Initialize( Timeline_s& timeline, Tick_t ticks_per_second, Tick_t first_tick, float pixels_per_ms )
	{
		timeline.Clock = Clock_Build( ticks_per_second );
		timeline.Scale = Zoom_Build ( pixels_per_ms );
		timeline.Offset = first_tick;
	}

	void Timeline_ZoomAt( Timeline_s& timeline, int amount, float x )
	{
		const Tick_t old_tick = Timeline_CoordToTick( timeline, x );
		timeline.Scale = Zoom_Scale( timeline.Scale, 0.5f * expf( 1.0f ), (float)amount );

		const Tick_t new_tick = Timeline_CoordToTick( timeline, x );
		timeline.Offset += (old_tick - new_tick);
	}

	void Timeline_ScrollCoord( Timeline_s& timeline, float dx )
	{
		timeline.Offset += Timeline_DxToDt( timeline, dx );
	}

	Tick_t Timeline_DxToDt( const Timeline_s& timeline, float dx )
	{
		return timeline.Scale.PixelToTick( dx, timeline.Clock ); 
	}

	float Timeline_DtToDx( const Timeline_s& timeline, Tick_t dt )
	{
		return timeline.Scale.TickToPixel( dt, timeline.Clock );
	}

	Tick_t Timeline_CoordToTick( const Timeline_s& timeline, float x )
	{
		return timeline.Scale.PixelToTick( x, timeline.Clock ) + timeline.Offset;
	}

	float Timeline_TickToCoord( const Timeline_s& timeline, Tick_t t )
	{
		return timeline.Scale.TickToPixel( t - timeline.Offset, timeline.Clock );
	}

	void Timeline_Clamp( Timeline_s& timeline, const Rect_s& r, Database_t db )
	{
		const Tick_t first_tick			= Database_GetFirstTick	   ( db );
		const Tick_t last_begin_tick	= Database_GetLastBeginTick( db );
		const Tick_t last_end_tick		= Database_GetLastEndTick  ( db );
		const Tick_t ticks_in_rect		= timeline.Scale.PixelToTick( r.w, timeline.Clock );
		const Tick_t right_aligned_max	= last_end_tick - ticks_in_rect;
		const Tick_t left_aligned_max	= last_begin_tick + ticks_in_rect;
		const Tick_t min_tick			= first_tick;
		const Tick_t max_tick			= right_aligned_max;
		const Tick_t capped_max_tick	= NeMax( min_tick, max_tick );
		timeline.Offset = NeClamp( timeline.Offset, min_tick, capped_max_tick );
	}

	void Timeline_FitToTickRange( Timeline_s& time, Database_t db, Tick_t first, Tick_t last, float range_width, float view_width )
	{
		// validate args
		if (Database_GetNumFrames( db ) == 0)
			return;
		if (first > last)
			NeSwap( first, last );

		// calc duration in [ms]
		const Clock clock = Database_GetClock( db );
		const float total_ms = clock.TickToMs( last-first );

		// fit to frame
		time.Scale  = Zoom_Build( range_width / total_ms );
		time.Offset = first;

		// center frame in view
		float dx = view_width - range_width;
		if (dx)
			time.Offset -= Timeline_DxToDt( time, 0.5f * dx );
	}

	void Timeline_FitToFrame( Timeline_s& time, Database_t db, int i, float range_width, float view_width )
	{
		if (Database_GetNumFrames( db ) == 0)
			return;
		const int frame_index = NeClamp( i, 0, Database_GetNumFrames( db )-1 );
		const viz::Frame& frame = Database_GetFrames( db )[ frame_index ];
		Timeline_FitToTickRange( time, db, frame.Time.Begin, frame.Time.End, range_width, view_width );
	}	
	
	void Timeline_FitToFrameRange( Timeline_s& time, Database_t db, int first, int count, float range_width, float view_width )
	{
		if (Database_GetNumFrames( db ) == 0)
			return;
		const int first_index = NeClamp( first		  , 0, Database_GetNumFrames( db )-1 );
		const int last_index  = NeClamp( first+count-1, 0, Database_GetNumFrames( db )-1 );
		const viz::Frame& frame_0 = Database_GetFrames( db )[ first_index ];
		const viz::Frame& frame_1 = Database_GetFrames( db )[ last_index  ];
		Timeline_FitToTickRange( time, db, frame_0.Time.Begin, frame_1.Time.End, range_width, view_width );
	}	

	bool Timeline_Mouse( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
		const MouseState_s& mouse = Context_GetMouse( dc );
		const Vec2_s mouse_pos = Context_GetMousePos( dc );
		const Rect_s clip_rect = Context_GetClipRect( dc );
		const bool on_ctrl = Rect_Contains( Rect_Intersect( r, clip_rect ), mouse_pos );
		bool modified = false;

		if (Context_IsDragging( dc, id ))
		{
			const Vec2_s delta = mouse_pos - Context_GetDrag( dc ).Pos;
			if (delta.x)
			{
				Timeline_ScrollCoord( timeline, -delta.x );
				modified = true;
			}
			Context_BeginDrag( dc, id, mouse_pos );
		}

		if (Button_Mouse( dc, id, r, ButtonClick::Press ))
			Context_BeginDrag( dc, id, mouse_pos );

		if (mouse.Wheel && on_ctrl && Context_HasFocus( dc, id ))
		{
			Timeline_ZoomAt( timeline, mouse.Wheel, mouse_pos.x );
			modified = true;
		}

		Timeline_Clamp( timeline, r, db );
		return modified;
	}

	bool Timeline_Keyboard( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
		return false;
	}

	bool Timeline_Input( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
		bool modified = false;
		modified |= Timeline_Mouse	 ( dc, id, r, db, timeline );
		modified |= Timeline_Keyboard( dc, id, r, db, timeline );
		return modified;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static viz::FrameRange FrameRange_Build( Database_t db, const Timeline_s& timeline, float w )
	{
		const Tick_t duration = Timeline_DxToDt( timeline, w );
		const Tick_t end	  = timeline.Offset + duration;
		const Tick_t start	  = timeline.Offset;

		viz::FrameRange cull = {};
		cull.Time.Begin = start;
		cull.Time.End	= end;
		cull.Frames.First = Database_TickToFrame( db, cull.Time.Begin );
		cull.Frames.Count = 0;
		for ( int i = cull.Frames.First; i < Database_GetNumFrames( db ); ++i )
		{
			if (Database_GetFrames( db )[i].Time.Begin >= cull.Time.End)
				break;
			++cull.Frames.Count;
		}
		return cull;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Time Bar

	struct TimeBarVisual_s
	{
		const TimeBarTheme_s* Theme;
		float ItemHeight;
		float MsPerTick;
		bool  Clip;
	};

	static Text_s ScratchBuffer_FormatMs( ScratchBuffer_t scratch, float ms )
	{
		const float total_ms = ms;

		const float ms_per_sec  = 1000.0f;
		const float ms_per_min  =   60.0f * ms_per_sec;
		const float ms_per_hour =   60.0f * ms_per_min;

		const uint32_t hour = (uint32_t)(ms / ms_per_hour); ms -= (ms_per_hour * hour);
		const uint32_t min  = (uint32_t)(ms / ms_per_min );	ms -= (ms_per_min  * min );
		const uint32_t sec  = (uint32_t)(ms / ms_per_sec );	ms -= (ms_per_sec  * sec );

		if (total_ms < 1000.0f)
			return ScratchBuffer_Format( scratch, "%.2fms", ms );
		if (total_ms < ms_per_min)
			return ScratchBuffer_Format( scratch, "%us:%ums", sec, (uint32_t)ms );
		if (total_ms < ms_per_hour)
			return ScratchBuffer_Format( scratch, "%um:%us", min, sec );
		return ScratchBuffer_Format( scratch, "%uh:%um:%us", hour, min, sec );
	}

	void TimeBar_DrawTicks( Context_t dc, const Rect_s& r, const TimeBarVisual_s& v, Database_t db, const Timeline_s& timeline, const viz::FrameGroup& group, const Rect_s& clip, float ms_per_marker )
	{
		Graphics_t g = Context_GetGraphics( dc );

		// r		: coords of the frame group in client space
		// clip_rect: coords of the view  rect  in client space 
		// timeline : zoom and scale

		/*
		// calculate groups's total duration
		const int	 last_frame		  = group.Frames.Last();
		const Tick_t first_group_tick = Database_GetFrames( db )[ group.Frames.First ].Time.Begin;
		const Tick_t last_group_tick  = Database_GetFrames( db )[ last_frame		 ].Time.End;
		*/

		// calculate tick range in view
		const Tick_t first_clip_tick = Timeline_CoordToTick( timeline, clip.x );
		const Tick_t last_clip_tick  = Timeline_CoordToTick( timeline, clip.x+clip.w );

		// calculate marker spacing
		const Tick_t marker_tick	= timeline.Clock.MsToTick( ms_per_marker ); 
		const float  marker_w		= Timeline_DtToDx( timeline, marker_tick );
		if (marker_w < 2.0f)
			return;

		// find first marker tick in clip rect
		float first_marker_x = r.x;
		float first_marker_ms = 0.0f;
		if (r.x < clip.x)
		{
			//
			//							+---------------------------------------+	
			//							|										|				CLIP
			//				+-----------+---------------------------------------+-----------+
			//				*    *    * |  *    *    *    *    *    *    *    * |  *    *		GROUP
			//				+-----------+---\-----------------------------------+-----------+
			//			   /			|	 \									|
			//			  r.x			+-----\----------------------------------+
			//	first_group_tick	   /		first_marker_tick
			//						  clip.x
			//					first_clip_tick
			//
			const float lead_in_w		= clip.x - r.x;
			const float lead_in_markers	= lead_in_w / marker_w;
			const float lead_in_count	= (1.0f + floorf(lead_in_markers));
			first_marker_x  += lead_in_count * marker_w;
			first_marker_ms += lead_in_count * ms_per_marker;
		}
		else
		{
			//
			//							+---------------------------------------+	
			//							|										|				CLIP
			//							|	+-----------+-----------------------------------
			//							|	*    *    *    *    *    *    *    *|    *    *		GROUP
			//				------------+---+-----------------------------------+-----------
			//			   /			|	 \									|
			//			  r.x			+-----\----------------------------------+
			//	first_group_tick	   /		first_marker_tick
			//						  clip.x
			//					first_clip_tick
			//
		}

		const float y0 = r.y;
		const float y1 = r.y + r.h;
		const float clip_end = clip.x + clip.w;
		const float group_end = r.x + r.w;
		const float marker_end = NeMin( clip_end, group_end );
		for ( float x = first_marker_x, ms = first_marker_ms; x < marker_end; x += marker_w, ms += ms_per_marker )
		{
			// marker
			Graphics_DrawLine( g, Vec2_s { x, y0 }, Vec2_s { x, y1 }, Color::White );

			// label
			const Text_s text = ScratchBuffer_FormatMs( Context_GetScratch( dc ), ms );
			const Rect_s label_rect = Rect_s { x, r.y, r.w + (r.x - x), r.h };
			Graphics_DrawString( g, Rect_Margin( label_rect, v.Theme->Metric.LabelMargin ), text, Context_GetFont( dc ), TextFormat::NoWrap, Color::White );
		}
	}

	struct TimeBarContext_s
	{
		Context_t		DC;
		Graphics_t		Gfx;
		Rect_s			Rect;
		Timeline_s		Timeline;
		Database_t		Database;
		TimeBarVisual_s Visual;
	};

	static void NE_CALLBK TimeBar_EnumFrame( void* context, const viz::FrameGroup& item )
	{
		TimeBarContext_s& args = *((TimeBarContext_s*)context);
		Context_t				dc = args.DC;
		Graphics_t				g  = args.Gfx;
		Database_t				db = args.Database;
		const Rect_s&			r  = args.Rect;
		const Timeline_s&		t  = args.Timeline;
		const TimeBarVisual_s&	v  = args.Visual;

		// colors
		const uint32_t text_color = v.Theme->Palette.Frame.Text;

		// item rect
		const viz::Frame& frame_0 = Database_GetFrames( db )[item.Frames.First];
		const viz::Frame& frame_1 = Database_GetFrames( db )[item.Frames.Last()];
		const float frame_x = Timeline_TickToCoord( t, frame_0.Time.Begin );
		const float frame_w = Timeline_DtToDx( t, frame_1.Time.End - frame_0.Time.Begin );

		// frame item
		{
			const Rect_s item_rect = { r.x + frame_x, r.y, frame_w, v.ItemHeight };

			// marker
			const Rect_s marker_rect = { item_rect.x, item_rect.y, 1.0f, r/*item_rect*/.h };
			Graphics_DrawRect( g, marker_rect, text_color );

			// label
			float x0 = frame_x;
			float x1 = frame_x + frame_w;
			if (v.Clip)
			{
				x0 = NeMax( x0, 0.0f );
				x1 = NeMin( x1, r.w  );
			}
			const Rect_s text_rect  = { x0, item_rect.y, x1-x0, item_rect.h };
			const Rect_s label_rect = Rect_Margin( text_rect, v.Theme->Metric.LabelMargin );
			if (label_rect.w >= 0)
			{
				const Font_t font  = v.Theme->Font ? v.Theme->Font : Context_GetFont( dc );
				const float duration_ms = t.Clock.TickToMs( frame_1.Time.End - frame_0.Time.Begin );
				const Text_s text = (item.Frames.Count > 1)
					? Context_FormatString( dc, "%u-%u (%.2f ms)", item.Frames.First, item.Frames.Last(), duration_ms )
					: Context_FormatString( dc, "%u (%.2f ms)", item.Frames.First, duration_ms )
					;
				Graphics_DrawString( g, label_rect, text, font, TextFormat::Left, text_color );
			}
		}

		// time item
		if (v.MsPerTick)
		{
			const Rect_s item_rect = { r.x + frame_x, r.y + v.ItemHeight, frame_w, v.ItemHeight };
			TimeBar_DrawTicks( dc, item_rect, v, db, t, item, r, v.MsPerTick );
		}
	}

	float TimeBar_CalcHeight( Context_t dc, const TimeBarTheme_s& v )
	{
		return 2.0f * TimeBar_CalcItemHeight( dc, v );
	}

	float TimeBar_CalcItemHeight( Context_t dc, const TimeBarTheme_s& v )
	{
		FontInfo_s	font_info = {};
		FontCache_t cache = System_GetFontCache();
		Font_t		font  = v.Font ? v.Font : Context_GetFont( dc );
		FontCache_GetFontInfo( cache, font, font_info );
		return font_info.LineHeight + 2.0f * v.Metric.LabelMargin.y;
	}

	float TimeBar_CalcMsPerTick( const Timeline_s& timeline, float w )
	{
		const Tick_t num_ticks = Timeline_DxToDt( timeline, w );
		const float total_ms = timeline.Clock.TickToMs( num_ticks );
		const float ms_per_x = total_ms / w;
		const float ms_per_marker = 100.0f * ms_per_x;
		const float nice_ms_per_marker = Flt_Nice( ms_per_marker, false );
		return nice_ms_per_marker;
	}

	bool TimeBar_Draw( Context_t dc, Id_t id, const Rect_s& r, Database_t db, const Timeline_s& timeline, const TimeBarLod_s& lod, const TimeBarTheme_s& v )
	{
		NePerfScope("TimeBar");
		if (Context_Cull( dc, r ))
			return false;

		Graphics_t g = Context_GetGraphics( dc );
		const float item_height = TimeBar_CalcItemHeight( dc, v );
		const Rect_s frame_rect = { r.x, r.y, r.w, item_height };
		const Rect_s time_rect  = { r.x, r.y + item_height, r.w, item_height };

		// draw backs
		{
			Graphics_FillRect( g, frame_rect, v.Palette.Frame.Fill );
			Graphics_FillRect( g, time_rect , v.Palette.Time .Fill );
		}

		// draw frames
		{
			const viz::FrameRange cull = FrameRange_Build( db, timeline, r.w );
			const viz::FrameGroupSetup setup = { lod.MinFrameTicks };
			const TimeBarVisual_s visual =
			{ &v
			, item_height
			, TimeBar_CalcMsPerTick( timeline, r.w )
			, true
			};
			TimeBarContext_s context = { dc, g, r, timeline, db, visual };
			Database_EnumFrameGroups( db, cull, setup, TimeBar_EnumFrame, &context );
		}

		return true;
	}

	void TimeBar_Mouse( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void TimeBar_Keyboard( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void TimeBar_Input( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void TimeBar_Do( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline, const TimeBarLod_s& lod, const TimeBarTheme_s& v )
	{
		if (!TimeBar_Draw( dc, id, r, db, timeline, lod, v ))
			return;
		TimeBar_Input( dc, id, r, db, timeline );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Bar

	static const char* ZoneType_FormatSuffix( uint8_t type )
	{
		const char* suffix[] = 
		{ ""
		, " [IDLE]"
		, " [LOCK]"
		};
		return suffix[ type ];
	}

	static void Graphics_DrawRect( Graphics_t g, const Rect_s& r, uint32_t argb0, uint32_t argb1 )
	{
		Graphics_DrawLine( g, Vec2_s { r.x, r.y }	   , Vec2_s { r.x + r.w, r.y }		, argb0 ); // top
		Graphics_DrawLine( g, Vec2_s { r.x, r.y }	   , Vec2_s { r.x, r.y + r.h }		, argb0 ); // left
		Graphics_DrawLine( g, Vec2_s { r.x, r.y + r.h }, Vec2_s { r.x + r.w, r.y + r.h }, argb1 ); // bottom
		Graphics_DrawLine( g, Vec2_s { r.x + r.w, r.y }, Vec2_s { r.x + r.w, r.y + r.h }, argb1 ); // right
	}

	struct ZoneBarVisual_s
	{
		const ZoneBarTheme_s* Theme;
		float ZoneHeight;
		float MinLabelWidth;
		bool  Clip;
	};

	struct ZoneBarContext_s
	{
		Context_t		DC;
		Graphics_t		Gfx;
		Rect_s			Rect;
		Timeline_s		Timeline;
		Database_t		Database;
		ZoneBarVisual_s Visual;
		uint8_t			Thread;
		Vec2_s			Mouse;
		bool			NoIdle;
		ZoneBarState_s*	State;
	};

	static void NE_CALLBK ZoneBar_EnumZone( void* context, const viz::ZoneGroup& item )
	{
		//NePerfScope("Zone");
		ZoneBarContext_s& args = *((ZoneBarContext_s*)context);
		Context_t				dc = args.DC;
		Graphics_t				g  = args.Gfx;
		Database_t				db = args.Database;
		const Rect_s&			r  = args.Rect;
		const Timeline_s&		t  = args.Timeline;
		const ZoneBarVisual_s&	v  = args.Visual;
		const Vec2_s&			m  = args.Mouse;

		// type
		const bool is_group = (item.NumZones > 1);
		const bool is_idle = (item.Type == ScopeType::Idle);
		const bool is_lock = (item.Type == ScopeType::Lock);
		if (is_idle && args.NoIdle)
			return;

		// rect
		float border_h = 1.0f;
		float zone_x = Timeline_TickToCoord( t, item.Time.Begin );
		float zone_w = Timeline_DtToDx( t, item.Time.Duration() );
		float zone_y = item.Level * (v.ZoneHeight - border_h);
		float zone_h = v.ZoneHeight;
		if (v.Clip)
		{
			float x0 = zone_x;
			float x1 = zone_x + zone_w;
			x0 = NeMax( x0, 0.0f );
			x1 = NeMin( x1, r.w  );
			zone_x = x0;
			zone_w = x1-x0;
		}

		const Rect_s zone_rect = 
		{ zone_x + r.x
		, zone_y + r.y
		, zone_w
		, zone_h
		};

		// color
		const ZoneBarColor_s& base_color 
			= is_lock  ? v.Theme->Palette.Lock 
			: is_group ? v.Theme->Palette.Group
			:			 v.Theme->Palette.Zone
			;
		const Color_c ThreadColor = base_color.Fill;
		const Color_s bright_color = Color_Modulate( ThreadColor, 1.2f );
		const Color_s border_color = Color_Modulate( ThreadColor, 0.5f );
		const Color_s zone_color   = (item.Level & 1) ? ThreadColor : bright_color;
		const float	alpha = is_idle ? 0.5f : 1.0f;

		// draw zone
		Graphics_DrawBox( g, zone_rect, Color_ToArgb( border_color, 1.0f ), Color_ToArgb( zone_color, alpha ) );

		// draw label
		if (zone_w >= v.MinLabelWidth)
		{
			// lookup scope
			NamedLocation scope;
			Database_GetLocation( db, item.Location, scope );

			// format label
			const float duration_ms = t.Clock.TickToMs( item.Time.Duration() );
			const Text_s text = is_group 
				? Context_FormatString( dc, "%ux %s (%.2f ms)%s", item.NumZones, scope.Name, duration_ms, ZoneType_FormatSuffix( item.Type ) )
				: Context_FormatString( dc, "%s (%.2f ms)%s"				   , scope.Name, duration_ms, ZoneType_FormatSuffix( item.Type ) )
				;

			// draw label
			Font_t font = v.Theme->Font ? v.Theme->Font : Context_GetFont( dc );
			const Rect_s label_rect = Rect_Margin( zone_rect, v.Theme->Metric.LabelMargin );
			Graphics_DrawString( g, label_rect, text, font, TextFormat::Center | TextFormat::Middle, base_color.Text );
		}

		// hit test
		if (args.State && Rect_Contains( zone_rect, m ))
		{
			args.State->Hot = item;
		}
	}

	float ZoneBar_CalcZoneHeight( Context_t dc, const ZoneBarTheme_s& v )
	{
		FontInfo_s	font_info = {};
		FontCache_t cache = System_GetFontCache();
		Font_t		font  = v.Font ? v.Font : Context_GetFont( dc );
		FontCache_GetFontInfo( cache, font, font_info );
		return font_info.LineHeight + 1.0f * v.Metric.LabelMargin.y;
	}

	float ZoneBar_CalcHeight( Context_t dc, Database_t db, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v )
	{
		viz::Thread thread_info = {};
		Database_GetThread( db, thread, thread_info );
		const uint8_t num_levels = NeClamp<uint8_t>( thread_info.NumLevels, 1, 1+lod.MaxZoneLevel );
		return num_levels * ZoneBar_CalcZoneHeight( dc, v ) + v.Metric.LabelMargin.y;
	}

	void ZoneBar_DrawPopup( Context_t dc, Database_t db, const viz::ZoneGroup& zone_hit )
	{
		NamedLocation loc;
		Database_GetLocation( db, zone_hit.Location, loc ); 

		const profiling::Clock db_clock = Database_GetClock( db );
		const Tick_t duration	 = zone_hit.Time.Duration();
		const float  duration_ms = db_clock.TickToMs( duration );
		const Text_s text_ms	 = ScratchBuffer_FormatMs( Context_GetScratch( dc ), duration_ms );

		char msg[1024] = "";
		Font_t font = Context_GetFont( dc );
		Str_Fmt( msg, sizeof(msg)
			, "%s\n\n"
				"Func: %s\n"
				"File: %s(%u)\n"
				"Duration: %s (%lld ticks)"
			, loc.Name
			, loc.Location.Function
			, loc.Location.File
			, loc.Location.Line
			, text_ms.Utf8
			, duration
			);

		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s text_rect	= Graphics_MeasureString( g, msg, font ); 
		const Rect_s popup_rect = Context_CalcPopup( dc, Rect_Size( text_rect ) );
		const Rect_s box_rect	= Rect_Inflate( popup_rect, Vec2_s { 4.0f, 2.0f } );
		{
			NeGuiScopedModal( dc );
			Graphics_DrawBox( g, box_rect, Color::White, Color::Black );
			Graphics_DrawString( g, popup_rect, msg, font, 0, Color::White );
		}
	}

	bool ZoneBar_Draw( Context_t dc, Id_t id, const Rect_s& r, Database_t db, const Timeline_s& timeline, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v, ZoneBarState_s& s )
	{
		NePerfScope("ZoneBar");
		if (Context_Cull( dc, r ))
			return false;

		Graphics_t g = Context_GetGraphics( dc );

		// font
		FontInfo_s	font_info = {};
		FontCache_t cache = System_GetFontCache();
		Font_t		font  = v.Font ? v.Font : Context_GetFont( dc );
		FontCache_GetFontInfo( cache, font, font_info );

		// draw back
		if (v.Palette.Back.Fill & 0xff000000)
		{
			Graphics_FillRect( g, r, v.Palette.Back.Fill ); 
		}

		// draw zones
		ZoneBarContext_s context = 
		{ dc 
		, g
		, r
		, timeline
		, db
		,	{ &v
			, v.Metric.LabelMargin.y * 2.0f + font_info.LineHeight
			, v.Metric.LabelMargin.x * 2.0f + font_info.MaxCharWidth * 4.0f
			, true
			}
		, thread
		, Context_GetMousePos( dc )
		, lod.NoIdle
		, &s
		};

		viz::FrameRange cull = FrameRange_Build( db, timeline, r.w );

		viz::ZoneGroupSetup setup = {};
		setup.MinFrameTicks			= lod.MinFrameTicks;
		setup.MinZoneTicks			= lod.MinZoneTicks;
		setup.MinGroupTicks			= lod.MinGroupTicks;
		setup.MaxGroupSpacingTicks	= lod.MaxGapTicks;
		setup.MaxZoneLevel			= (uint8_t)lod.MaxZoneLevel;
		setup.Clip					= true;

		Database_EnumZoneGroups( db, cull, setup, thread, ZoneBar_EnumZone, &context );
		return true;
	}

	void ZoneBar_Mouse( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void ZoneBar_Keyboard( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void ZoneBar_Input( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void ZoneBar_Do( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v, ZoneBarState_s& s )
	{
		if (!ZoneBar_Draw( dc, id, r, db, timeline, thread, lod, v, s ))
			return;
		ZoneBar_Input( dc, id, r, db, timeline );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Culling

	ZoneCull_s ZoneCull_Default()
	{
		ZoneCull_s cull;
		cull.MinFrameWidth	   = 50.0f;
		cull.MinZoneWidth	   = 10.0f;
		cull.MinGroupWidth	   = 0.5f;
		cull.MaxGroupSpacingMs = 0.1f;
		cull.MaxZoneLevel	   = 8;
		cull.ShowIdle		   = true;
		cull.ShowEmpty		   = false;
		return cull;
	}

	ZoneBarLod_s ZoneCull_ToLod( const ZoneCull_s& cull, const Zoom_s& zoom, const Clock_s& clock )
	{
		const ZoneBarLod_s lod = 
		{ zoom.PixelToTick( cull.MinFrameWidth, clock )
		, zoom.PixelToTick( cull.MinZoneWidth, clock )
		, zoom.PixelToTick( cull.MinGroupWidth, clock )
		, clock.MsToTick( cull.MaxGroupSpacingMs )
		, cull.MaxZoneLevel
		, !cull.ShowIdle
		, !cull.ShowEmpty
		};
		return lod;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Header

	float ZoneHeader_CalcHeight( Context_t dc, const ZoneHeaderTheme_s& v )
	{
		FontInfo_s	font_info = {};
		FontCache_t cache = System_GetFontCache();
		Font_t		font  = v.Font ? v.Font : Context_GetFont( dc );
		FontCache_GetFontInfo( cache, font, font_info );
		return font_info.LineHeight + 2.0f * v.Metric.LabelMargin.y;
	}

	float ZoneHeader_CalcLabelWidth( Context_t dc, const ZoneHeaderTheme_s& v, Database_t db )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Font_t font = v.Font ? v.Font : Context_GetFont( dc );

		float text_width = 0.0f;
		float max_text_width = 0.0f;
		viz::Thread thread_info = {};

		const int num_threads = Database_GetNumThreads( db );
		for ( int thread = 0; thread < num_threads; ++thread )
		{
			Database_GetThread( db, thread, thread_info );
			text_width = Graphics_MeasureString( g, thread_info.Name, font ).w;
			max_text_width = NeMax( max_text_width, text_width );
		}

		return max_text_width + v.Metric.LabelMargin.x * 2.0f;
	}

	float ZoneHeader_CalcWidth( Context_t dc, const ZoneHeaderTheme_s& v, Database_t db )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float button_width = theme->Metric[ Metric::Expander_Button_Size ].x;
		const float label_width  = ZoneHeader_CalcLabelWidth( dc, v, db );
		return label_width + button_width;
	}

	Rect_s ZoneHeader_CalcButtonRect( Context_t dc, const Rect_s& r, Alignment::Horz align )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float button_w = theme->Metric[ Metric::Expander_Button_Size ].x;
		const float pos = (align == Alignment::Right) ? r.w - button_w : 0.0f;
		return Rect_s { r.x + pos, r.y, button_w, r.h };
	}

	Rect_s ZoneHeader_CalcLabelRect( Context_t dc, const Rect_s& r, Alignment::Horz align )
	{
		Theme_t theme = Context_GetTheme( dc );
		const float button_w = theme->Metric[ Metric::Expander_Button_Size ].x;
		const float label_w = r.w - button_w;
		const float pos = (align != Alignment::Right) ? button_w : 0.0f;
		return Rect_s { r.x + pos, r.y, label_w, r.h };
	}

	bool ZoneHeader_Draw( Context_t dc, Id_t id, const Rect_s& r, cstr_t name, const ZoneHeaderTheme_s& v, const ZoneHeaderState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;

		Graphics_t g = Context_GetGraphics( dc );
		Font_t font = v.Font ? v.Font : Context_GetFont( dc );

		const Rect_s button_rect = ZoneHeader_CalcButtonRect( dc, r, v.Alignment );
		const Rect_s label_rect  = ZoneHeader_CalcLabelRect ( dc, r, v.Alignment );

		// button
		{
			Graphics_FillRect( g, button_rect, v.Palette.Button.Fill );
			Context_DrawArrow( dc, Rect_Margin( button_rect, v.Metric.ButtonMargin ), s.Collapsed ? Arrow::Right : Arrow::Down, v.Palette.Button.Text );
		}

		// label
		{
			const TextFormat::Mask_t fmt = (v.Alignment == Alignment::Left)   ? TextFormat::Left 
										 : (v.Alignment == Alignment::Center) ? TextFormat::Center
										 :										TextFormat::Right;
			Graphics_FillRect  ( g, label_rect, v.Palette.Header.Fill );
			Graphics_DrawString( g, Rect_Margin( label_rect, v.Metric.LabelMargin ), name, font, fmt, v.Palette.Header.Text );
		}
		return true;
	}

	void ZoneHeader_Mouse( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s )
	{
		const Rect_s button_rect = ZoneHeader_CalcButtonRect( dc, r, v.Alignment );
		const bool activated = (Button_Mouse( dc, id, button_rect, ButtonClick::Press	    ) == ButtonAction::Clicked)
							|| (Button_Mouse( dc, id, r			 , ButtonClick::DoubleClick ) == ButtonAction::Clicked);
		if (activated)
			s.Collapsed = !s.Collapsed;
	}

	void ZoneHeader_Keyboard( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s )
	{
	}

	void ZoneHeader_Input( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s )
	{
		ZoneHeader_Mouse	( dc, id, r, v, s );
		ZoneHeader_Keyboard ( dc, id, r, v, s );
	}

	void ZoneHeader_Do( Context_t dc, Id_t id, const Rect_s& r, cstr_t name, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s )
	{
		if (!ZoneHeader_Draw( dc, id, r, name, v, s ))
			return;
		ZoneHeader_Input( dc, id, r, v, s );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static ZoneBarTheme_s ZoneBar_DefaultTheme( Context_t dc )
	{
		ZoneBarTheme_s theme;
		NeZero(theme);
		theme.Font = Context_GetFont( dc );
		theme.Metric.LabelMargin = Vec2_s { 2.0f, 1.0f };
		theme.Palette.Back.Fill = Color_SetAlpha( Color::DimGray, 0x4f );
		theme.Palette.Back.Text = Color::White;
		theme.Palette.Zone.Fill = Color::Goldenrod;
		theme.Palette.Zone.Text = Color::White;
		theme.Palette.Group.Fill = Color::Goldenrod;
		theme.Palette.Group.Text = Color::White;
		theme.Palette.Lock.Fill = Color::Crimson;
		theme.Palette.Lock.Text = Color::White;
		return theme;
	}

	static ZoneHeaderTheme_s ZoneHeader_DefaultTheme( Context_t dc )
	{
		ZoneHeaderTheme_s theme =
		{ Context_GetFont( dc )				// font
		,	{ Vec2_s { 8.0f, 1.0f }			// metric - label  margin
			, Vec2_s { 3.0f, 4.0f }			// metric - button margin
			}						
		,	{ { Color::White, 0xff244c54 }	// palette - header
			, { Color::White, 0xff244c54 }	// palette - button
			}
		, Alignment::Right
		};
		return theme;
	}

	static ZoneHeaderTheme_s GroupHeader_DefaultTheme( Context_t dc )
	{
		ZoneHeaderTheme_s theme =
		{ Context_GetFont( dc )				// font
		,	{ Vec2_s { 8.0f, 6.0f }			// metric - label  margin
			, Vec2_s { 3.0f, 9.0f }			// metric - button margin
			}						
		,	{ { Color::White, 0xff1f3d6c }	// palette - header
			, { Color::White, 0xff1f3d6c }	// palette - button
			}
		, Alignment::Center
		};
		return theme;
	}

	static const uint32_t ThreadColor[] = 
	{ Color::DarkGoldenrod 
	, Color::Olive
	, Color::DarkCyan
	, Color::DarkSlateBlue
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone View

	Vec2_s ZoneView_DoGroup( Context_t dc, Id_t id, const Rect_s& r, Database_t db, uint8_t group, const ZoneBarLod_s& lod, Timeline_s& time, ZoneViewState_s& s )
	{
		viz::Thread			thread_info			= {};
		ZoneBarState_s		zone_bar_state		= {};
		ZoneHeaderState_s	zone_hdr_state		= {};
		ZoneBarTheme_s		zone_bar_theme		= ZoneBar_DefaultTheme( dc );
		ZoneHeaderTheme_s	zone_hdr_theme		= ZoneHeader_DefaultTheme( dc );
		ZoneHeaderTheme_s	group_hdr_theme		= GroupHeader_DefaultTheme( dc );
		const float			group_line_width	= 20.0f;
		const float			group_hdr_height	= ZoneHeader_CalcHeight( dc, group_hdr_theme );
		const float			zone_hdr_width		= ZoneHeader_CalcWidth( dc, zone_hdr_theme, db ) + group_line_width;
		const float			zone_hdr_height		= ZoneHeader_CalcHeight( dc, zone_hdr_theme );
		float				y					= r.y;

		const uint8_t num_threads = (uint8_t)Database_GetNumThreads( db );

		// spacing for group header
		y += group_hdr_height;

		// threads
		if ( !s.Group[ group ] )
		{
			for ( uint8_t i = 0; i < /*group_info.NumThreads*/ num_threads; ++i )
			{
				// get thread info
				const uint8_t thread = i;
				Database_GetThread( db,  thread, thread_info );

				// skip empty threads
				if (lod.NoEmpty && !thread_info.NumLevels)
					continue;

				// calculate heights
				float y0 = y;
				const float bar_height	= ZoneBar_CalcHeight( dc, db, thread, lod, zone_bar_theme );

				// context switch bar
				{
					// @todo: draw cpu bars for the thread
					y += zone_hdr_height;
				}

				// zone bar
				if (!s.Thread[ thread ])
				{
					// patch thread colors
					zone_bar_theme.Palette.Zone.Fill = ThreadColor[ thread % NeCountOf( ThreadColor ) ];
					zone_bar_theme.Palette.Group.Fill = zone_bar_theme.Palette.Zone.Fill;

					// do zones
					const Rect_s bar_rect = { r.x, y, r.w, bar_height };
					ZoneBar_Do( dc, id, bar_rect, db, time, thread, lod, zone_bar_theme, zone_bar_state );

					// advance
					y += bar_height;
				}

				// zone header
				{
					const Rect_s hdr_rect = { r.x, y0, zone_hdr_width, zone_hdr_height };
					zone_hdr_state.Collapsed = s.Thread[ thread ];
					ZoneHeader_Do( dc, Id_Cat( id, thread ), hdr_rect, thread_info.Name, zone_hdr_theme, zone_hdr_state );
					s.Thread[ thread ] = zone_hdr_state.Collapsed;
				}
			}
		}

		// group header
		{
			const Rect_s group_hdr_rect = { r.x, r.y, zone_hdr_width, group_hdr_height };
			zone_hdr_state.Collapsed = s.Group[ group ];
			ZoneHeader_Do( dc, Id_Cat( id, -1 ), group_hdr_rect, "Threads", group_hdr_theme, zone_hdr_state );
			s.Group[ group ] = zone_hdr_state.Collapsed;

			Graphics_t g = Context_GetGraphics( dc );
			const float top = group_hdr_rect.y + group_hdr_rect.h;
			const Rect_s group_line_rect = { r.x, top, 14.0f, y - top };
			Graphics_FillRect( g, group_line_rect, group_hdr_theme.Palette.Header.Fill );
		}

		// tooltip
		{
			if (zone_bar_state.Hot.NumZones)
				ZoneBar_DrawPopup( dc, db, zone_bar_state.Hot );
		}

		// total size
		return Vec2_s { zone_hdr_width, y-r.y };
	}

	Vec2_s ZoneView_Do( Context_t dc, Id_t id, const Rect_s& r, Database_t db, const ZoneBarLod_s& lod, Timeline_s& time, ZoneViewState_s& s )
	{
		Vec2_s size = {};
		const int num_groups = 1;
		for ( int i = 0; i < num_groups; ++i )
			size = size + ZoneView_DoGroup( dc, id, r + Vec2_s { 0.0f, size.y }, db, (uint8_t)i, lod, time, s );
		return size;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	FrameBarTheme_s FrameBar_DefaultTheme( Context_t dc )
	{
		FrameBarTheme_s theme;
		NeZero(theme);
		theme.Palette.Back   .Fill = Color_SetAlpha( Color::DimGray, 0x4f );
		theme.Palette.Back	 .Text = Color::White;
		theme.Palette.Group  .Fill = Color::DarkGray;
		theme.Palette.Group  .Text = Color::White;
		theme.Palette.Target .Fill = Color::DarkGray;
		theme.Palette.Target .Text = Color::White;
		theme.Palette.Range  .Fill = Color::PaleGoldenrod;
		theme.Palette.Range  .Text = Color::Black;
		return theme;
	}

	void FrameBar_DrawPopup( Context_t dc, Database_t db, const FrameBarItem_s& item )
	{
		Font_t font = Context_GetFont( dc );
		const profiling::Clock db_clock = Database_GetClock( db );

		Text_s text;
		char frame_txt	 [64] = "";
		char total_ms_txt[64] = "";
		char peak_ms_txt [64] = "";
		const float total_ms = db_clock.TickToMs( item.Sum );
		const float peak_ms  = db_clock.TickToMs( item.Peak );
		ScratchBuffer_t scratch = Context_GetScratch( dc );
		text = ScratchBuffer_Format  ( scratch, (item.Count > 1) ? "%d-%d (%d)" : "%d", item.First, item.First + item.Count, item.Count ); Str_Cpy( frame_txt, text.Utf8 );
		text = ScratchBuffer_FormatMs( scratch, total_ms ); Str_Cpy( total_ms_txt, text.Utf8 );
		text = ScratchBuffer_FormatMs( scratch, peak_ms  ); Str_Cpy( peak_ms_txt , text.Utf8 );
		text = ScratchBuffer_Format  ( scratch
			, "Frame: %s\n"
			  "Peak:  %s\n"
			  "Total: %s\n"
			, frame_txt
			, peak_ms_txt
			, total_ms_txt
			);
		Graphics_t g = Context_GetGraphics( dc );
		const Rect_s text_rect	= Graphics_MeasureString( g, text, font ); 
		const Rect_s popup_rect = Context_CalcPopup( dc, Rect_Size( text_rect ) );
		const Rect_s box_rect	= Rect_Inflate( popup_rect, Vec2_s { 4.0f, 2.0f } );
		{
			NeGuiScopedModal( dc );
			Graphics_DrawBox( g, box_rect, Color::White, Color::Black );
			Graphics_DrawString( g, popup_rect, text, font, 0, Color::White );
		}
	}

	static const float TIME_BAR_TARGET_HEIGHT_SCALE = 3.0f / 4.0f;

	struct FrameBarGroup_s
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

	static void FrameBar_DrawGroup( Context_t dc, const Rect_s& r, const Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarGroup_s& group, FrameBarState_s& s )
	{
		if (!group.Count)
			return;
		Graphics_t g = Context_GetGraphics( dc );
		group.Width = Timeline_DtToDx( timeline, group.Sum );
		if (!group.Width)
			return;
		const Color_c bar_color	   = v.Palette.Group.Fill;
		const Color_s bright_color = Color_Modulate( bar_color, 1.2f );
		const Color_s border_color = Color_Modulate( bar_color, 0.5f );
		const float target_height	= r.h * TIME_BAR_TARGET_HEIGHT_SCALE;
		const float frame_ms		= timeline.Clock.TickToMs( group.Peak );
		const float relative_time	= frame_ms * lod.TargetFps * (1.0f/1000.0f);
		const float height			= NeMin( relative_time * target_height, r.h );
		const float max_w			= r.w - group.Pos;
		const Rect_s bar_rect { r.x + group.Pos, r.y + r.h - height, NeMin( group.Width, max_w ) + 1.0f, height };
		Graphics_DrawBox( g, bar_rect, Color_ToArgb( border_color, 1.0f ), Color_ToArgb( bright_color, 1.0f ) );
		if (Rect_Contains( bar_rect, Context_GetMousePos( dc ) ))
		{
			s.Hot.First = group.First;
			s.Hot.Count = group.Count;
			s.Hot.Sum	= group.Sum;
			s.Hot.Peak	= group.Peak;
		}
	}

	static void FrameBar_DrawFrames( Context_t dc, const Rect_s& r, const Timeline_s& timeline, Database_t db, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s )
	{
		const Tick min_ticks = timeline.Scale.PixelToTick( lod.MinGroupWidth, timeline.Clock );

		FrameBarGroup_s group = {};

		const int first_frame = 0;
		const int num_frames = Database_GetNumFrames( db );
		const int frame_end = first_frame + num_frames;
		for ( int frame_index = first_frame; frame_index < frame_end; ++frame_index )
		{
			const viz::Frame& frame = Database_GetFrames( db )[ frame_index ];
			const Tick frame_ticks = frame.Time.Duration();

			// add frame to group
			group.Peak = NeMax(group.Peak, frame_ticks);
			group.Sum += frame_ticks;
			++group.Count;

			// accumulate small frames
			if (group.Sum < min_ticks)
				continue;

			// flush accumulated frames
			FrameBar_DrawGroup( dc, r, timeline, lod, v, group, s );
			group.Next();
		}

		FrameBar_DrawGroup( dc, r, timeline, lod, v, group, s );
	}

	static void FrameBar_DrawFpsMarker( Context_t dc, const Rect_s& r, const FrameBarLod_s& lod, const FrameBarTheme_s& v, float marker_fps )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const float target_height	= r.h * TIME_BAR_TARGET_HEIGHT_SCALE;
		const float relative_time	= lod.TargetFps / marker_fps;
		const float marker_height	= NeMin( relative_time * target_height, r.h );
		const Rect_s marker_rect = { r.x, r.y + r.h - marker_height - 1.0f, r.w, 1.0f };
		Graphics_FillRect( g, marker_rect, v.Palette.Target.Fill );

		const Font_t font = v.Font ? v.Font : Context_GetFont( dc );
		const float line_height = Graphics_GetLineHeight( g, font );
		const Rect_s label_rect = { r.x + 2.0f, marker_rect.y - line_height, r.w - 4.0f, line_height };
		const Text_s text = Context_FormatString( dc, "%.0f fps [%.2f ms]", marker_fps, 1000.0f / marker_fps );
		Graphics_DrawString( g, label_rect, text, font, TextFormat::Middle | TextFormat::Right, v.Palette.Target.Text );
	}

	static void FrameBar_DrawFpsMarkers( Context_t dc, const Rect_s& r, const FrameBarLod_s& lod, const FrameBarTheme_s& v )
	{
		float fps = lod.TargetFps;
		for ( int i = 0; i < 4; ++i, fps *= 2.0f )
			FrameBar_DrawFpsMarker( dc, r, lod, v, fps );
	}

	static void FrameBar_DrawBackground( Context_t dc, const Rect_s& r, const FrameBarTheme_s& v )
	{
		Graphics_t g = Context_GetGraphics( dc );
		Graphics_FillRect( g, r, v.Palette.Back.Fill );
	}

	/// Frame Bar

	bool FrameBar_DrawRange( Context_t dc, Id_t id, const Rect_s& r, Database_t db, const Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s, const Timeline_s& range, const Rect_s& r2 /*range_rect*/ )
	{
		if (Context_Cull( dc, r ))
			return false;

		// calculate tick ranges
		const Tick_t view_begin = timeline.Offset;
		const Tick_t view_end   = timeline.Offset + Timeline_DxToDt( timeline, r.w );

		const Tick_t range_begin = range.Offset;
		const Tick_t range_end   = range.Offset + Timeline_DxToDt( range, r2.w );

		// calculate realtive position & size
		const float full_range = (float)(view_end  - view_begin );
		const float view_range = (float)(range_end - range_begin);
		if ((full_range <= 0.0f) || (view_range <= 0.0f))
			return false;

		const float ratio = NeMin( 1.0f, view_range / full_range );
		const float pos = (float)(range_begin - view_begin) / (full_range);

		// calculate abosulte position & size
		const float x = pos   * r.w;
		const float w = NeMax( 1.0f, ratio * r.w );

		// draw
		Graphics_t g = Context_GetGraphics( dc );
		const float CARET_WIDTH  = 3.0f;
		const float CARET_HEIGHT = 2.0f;
		const uint32_t marker_color =						 v.Palette.Range.Fill;
		const uint32_t fill_color   = Color_ToArgb( Color_c( v.Palette.Range.Fill, 0.5f ) );

		Rect_s range_rect = r;
		range_rect.x += x;
		range_rect.w  = w;
		Graphics_FillRect( g, range_rect, fill_color );
		Graphics_DrawRect( g, range_rect, marker_color );

		Rect_s caret_rect = range_rect;
		caret_rect.w = CARET_WIDTH;
		caret_rect.h = CARET_HEIGHT;

		caret_rect.x = range_rect.x - CARET_WIDTH;
		caret_rect.y = range_rect.y;
		Graphics_FillRect( g, caret_rect, marker_color );

		caret_rect.x = range_rect.x + range_rect.w; 
		caret_rect.y = range_rect.y;
		Graphics_FillRect( g, caret_rect, marker_color );

		caret_rect.x = range_rect.x - CARET_WIDTH; 
		caret_rect.y = range_rect.y - CARET_HEIGHT + range_rect.h;
		Graphics_FillRect( g, caret_rect, marker_color );

		caret_rect.x = range_rect.x + range_rect.w; 
		caret_rect.y = range_rect.y - CARET_HEIGHT + range_rect.h;
		Graphics_FillRect( g, caret_rect, marker_color );
		return true;
	}

	bool FrameBar_Draw( Context_t dc, Id_t id, const Rect_s& r, Database_t db, const Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s )
	{
		if (Context_Cull( dc, r ))
			return false;
		FrameBar_DrawBackground( dc, r, v );
		FrameBar_DrawFpsMarkers( dc, r, lod, v );
		FrameBar_DrawFrames( dc, r, timeline, db, lod, v, s );
		return true;
	}

	void FrameBar_Mouse( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void FrameBar_Keyboard( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
	}

	void FrameBar_Input( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline )
	{
		FrameBar_Mouse	 ( dc, id, r, db, timeline );
		FrameBar_Keyboard( dc, id, r, db, timeline );
	}

	void FrameBar_Do( Context_t dc, Id_t id, const Rect_s& r, Database_t db, Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s )
	{
		if (!FrameBar_Draw( dc, id, r, db, timeline, lod, v, s ))
			return;
		FrameBar_Input( dc, id, r, db, timeline );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Panel

	static TimeBarTheme_s TimeBar_DefaultTheme( Context_t dc )
	{
		TimeBarTheme_s theme;
		NeZero(theme);
		theme.Font = Context_GetFont( dc );
		theme.Metric.LabelMargin = Vec2_s { 4.0f, 1.0f };
		theme.Palette.Frame.Fill = Color::LightGray;
		theme.Palette.Frame.Text = Color::Black;
		theme.Palette.Time.Fill = Color::Black;
		theme.Palette.Time.Text = Color::White;
		return theme;
	}

	Vec2_s ZonePanel_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, const ZoneCull_s& cull, ZoneViewState_s& state, Timeline_s& timeline, bool& auto_scroll )
	{
		Vec2_s out = {};
		if (!db)
			return out;

		if (!Database_GetNumFrames( db ))
			return out;

		// init timeline
		if (!timeline.Clock.TicksPerSecond)
		{
			Timeline_Initialize( timeline, Database_GetClock( db ).TicksPerSecond, Database_GetFirstTick( db ), 25.0f / 4.0f );
			Context_SetFocus( dc, id );
		}

		// scoll/zoom
		if (Timeline_Input( dc, id, r, db, timeline ))
			auto_scroll = false;

		// handle auto-scroll
		if (auto_scroll)
			Timeline_FitToFrame( timeline, db, Database_GetNumFrames( db )-2, r.w * 0.75f, r.w );

		// init lod
		const ZoneBarLod_s zone_lod  = ZoneCull_ToLod( cull, timeline.Scale, timeline.Clock );
		const TimeBarLod_s frame_lod = { zone_lod.MinFrameTicks };

		// init themes
		const TimeBarTheme_s time_bar_theme = TimeBar_DefaultTheme( dc );

		// init layout
		float y = r.y;

		// time bar
		{
			const float bar_height = TimeBar_CalcHeight( dc, time_bar_theme );
			const Rect_s bar_rect = { r.x, y, r.w, r.h };
			TimeBar_Do( dc, id, bar_rect, db, timeline, frame_lod, time_bar_theme );
			y += bar_height;
		}

		// zone view
		{
			const Rect_s zone_view_rect = { r.x, y, r.w, r.y + r.h - y };
			y += ZoneView_Do( dc, id, zone_view_rect, db, zone_lod, timeline, state ).y;
		}

		out = Vec2_s { 0.0f, y }; // @todo: don't use this scrollbar. instead, have ZoneView implement scrolling
		return out;
	}

	Vec2_s ZonePanel_Do( Context_t dc, Id_t id, const Rect_s& r, Database_t db, ZonePanel_s& instance )
	{
		return ZonePanel_Do( dc, id, r, db, instance.Cull, instance.State, instance.Timeline, instance.AutoScroll );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Panel

	Vec2_s FramePanel_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, Timeline_s& timeline, Timeline_s& zone_timeline, const Rect_s& zone_rect, bool& auto_scroll )
	{
		if (!db)
			return Vec2_s {};

		if (!Database_GetNumFrames( db ))
			return Vec2_s {};

		if (!timeline.Clock.TicksPerSecond)
			Timeline_Initialize( timeline, Database_GetClock( db ).TicksPerSecond, Database_GetFirstTick( db ), 25.0f / 4.0f );

		const bool auto_fit = true;
		if (auto_fit)
		{
			const Tick_t first		= Database_GetFirstTick( db );
			const Tick_t last		= Database_GetLastEndTick( db );
			const Tick_t duration	= last - first;
			timeline.Scale = Zoom_Build( r.w / timeline.Clock.TickToMs( duration ) );
		}

		const FrameBarTheme_s theme = FrameBar_DefaultTheme( dc );
		const FrameBarLod_s lod = { 6.0f, 15.0f };
		FrameBarState_s s = {};
		FrameBar_Draw		( dc, id, r, db, timeline, lod, theme, s );
		FrameBar_DrawRange	( dc, id, r, db, timeline, lod, theme, s, zone_timeline, zone_rect );
		if (Button_Mouse( dc, id, r, ButtonClick::DoubleClick ))
		{
			if (s.Hot.Count)
			{
				Timeline_FitToFrameRange( zone_timeline, db, s.Hot.First, s.Hot.Count, zone_rect.w * 0.75f, zone_rect.w );
				auto_scroll = false;
			}
		}
		if (s.Hot.Count)
			FrameBar_DrawPopup( dc, db, s.Hot );
		return Vec2_s {};
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf View

	struct PerfView_s
	{
		ZoneCull_s		Cull;
		Parser_t		Parser;
		Database_t		Database;
		bool			AutoDock;
		bool			AutoScroll;
		Rect_s			ZoneRect;
		ZoneViewState_s ZoneView;
		Timeline_s		ZoneTimeline;
		Timeline_s		FrameTimeline;
	};

	static PerfView_s PerfView = 
	{ ZoneCull_Default()
	, nullptr	// db
	, nullptr	// parser
	, true		// auto-dock
	, true		// auto-scroll
	};


	static Vec2_s Window_Perf_Zones( Context_t dc, void* user )
	{
		NePerfScope("Zones");
		PerfView.ZoneRect = Context_GetChild( dc );
		return ZonePanel_Do( dc, NE_UI_ID, PerfView.ZoneRect, PerfView.Database, PerfView.Cull, PerfView.ZoneView, PerfView.ZoneTimeline, PerfView.AutoScroll );
	}

	static Vec2_s Window_Perf_Frames( Context_t dc, void* user )
	{
		NePerfScope("Frames");
		return FramePanel_Do( dc, NE_UI_ID, Context_GetChild( dc ), PerfView.Database, PerfView.FrameTimeline, PerfView.ZoneTimeline, PerfView.ZoneRect, PerfView.AutoScroll );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf Settings

	static Vec2_s Window_Perf_Settings( Context_t dc, void* user )
	{
		NePerfScope("Settings");
		static cstr_t label[] = 
		{ "Auto Dock"
		, "Auto Scroll"
		, "Show Idle" 
		, "Show Empty" 
		, "Recording"
		, "Max Depth"
		};

		Font_t font = Context_GetFont( dc );
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		const Vec2_s border_size = Vec2_s { 4.0f, 2.0f };
		const Rect_s r = Rect_Margin( Context_GetChild( dc ), border_size );
		float text_w = Graphics_MeasureMaxStringWidth( g, font, label, NeCountOf(label) );
		float text_h = Context_GetLineHeight( dc );
		float ctrl_h = Button_CalcSize( dc, "" ).y;
		float item_h = 2.0f + NeMax( text_h + 2.0f, ctrl_h );

		const uint32_t text_color = theme->Visual[Visual::Label].Text[CtrlState::Normal];

		Rect_s pos = Rect_s { r.x, r.y, r.w, item_h };
		for ( int i = 0; i < NeCountOf(label); ++i )
		{
			Graphics_DrawString( g, pos, label[i], font, TextFormat::Middle, text_color ); 
			pos.y += pos.h + 2.0f;
		}

		pos = Rect_s { r.x + text_w + 10.0f, r.y, 100.0f, item_h };
		{
			CheckBox_DoButton( dc, NE_UI_ID, pos, PerfView.AutoDock ? "On" : "Off", PerfView.AutoDock ); 
			pos.y += pos.h + 2.0f; 
		}
		{
			CheckBox_DoButton( dc, NE_UI_ID, pos, PerfView.AutoScroll ? "On" : "Off", PerfView.AutoScroll ); 
			pos.y += pos.h + 2.0f; 
		}
		{
			CheckBox_DoButton( dc, NE_UI_ID, pos, PerfView.Cull.ShowIdle ? "On" : "Off", PerfView.Cull.ShowIdle ); 
			pos.y += pos.h + 2.0f; 
		}
		{
			CheckBox_DoButton( dc, NE_UI_ID, pos, PerfView.Cull.ShowEmpty ? "On" : "Off", PerfView.Cull.ShowEmpty ); 
			pos.y += pos.h + 2.0f; 
		}
		{
			if (PerfView.Parser)
			{
				bool paused = Parser_IsPaused( PerfView.Parser );
				if (Button_Do( dc, NE_UI_ID, pos, paused ? "Start" : "Stop" ))
					Parser_Pause( PerfView.Parser, !paused );
			}
			pos.y += pos.h + 2.0f; 
		}
		{
			float max_depth = (float)PerfView.Cull.MaxZoneLevel; 
			Slider_DoH( dc, NE_UI_ID, pos, 0.0f, 32.0f, max_depth );
			PerfView.Cull.MaxZoneLevel = (uint8_t)max_depth;
			pos.y += pos.h + 2.0f;
		}
		return Vec2_s {};
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf Statistics

	Unit_s Unit_MakeBytes( uint32_t val )
	{
		static cstr_t fmt[] = 
		{ "%.0f"
		, "%.2f KB"
		, "%.2f MB"
		, "%.2f GB"
		, "%.2f TB"
		};
		Unit_s unit = { 1.0f * val, fmt[0] };
		float amount = 1.0f * val;
		for ( int i = 0; i < NeCountOf(fmt); ++i )
		{
			if (amount < 1024.0f)
			{
				unit.Value = amount;
				unit.Format = fmt[i];
				break;
			}
			amount /= 1024.0f;
		}
		return unit;
	}

	static void Perf_Label_Do( Context_t dc, const Rect_s& r, uint32_t val )
	{
		Label_Fmt( dc, r, TextFormat::Left | TextFormat::Middle, "%u", val );
	}

	static void Perf_Progress_Do( Context_t dc, const Rect_s& r, uint32_t val, uint32_t max )
	{
		ProgressBar_Do( dc, Rect_Margin( r, 1.0f ), 0.0f, 1.0f * max, 1.0f * val );
		Label_Fmt( dc, r, TextFormat::Center | TextFormat::Middle, "%u / %u", val, max );
	}

	static void Perf_Progress_DoBytes( Context_t dc, const Rect_s& r, uint32_t val, uint32_t max )
	{
		char fmt[64];
		const Unit_s unit_val = Unit_MakeBytes( val );
		const Unit_s unit_max = Unit_MakeBytes( max );
		Str_Cpy( fmt, unit_val.Format );		
		Str_Cat( fmt, " / " );
		Str_Cat( fmt, unit_max.Format );
		ProgressBar_Do( dc, Rect_Margin( r, 1.0f ), 0.0f, 1.0f * max, 1.0f * val );
		Label_Fmt( dc, r, TextFormat::Center | TextFormat::Middle, fmt, unit_val.Value, unit_max.Value );
	}

	void PerfStat_Do( Context_t dc, const Rect_s& r, const PerfStat_s& item )
	{
		if (item.Max)
		{
			switch ( item.Unit )
			{
			default:
			case PerfStat::Counter:
				return Perf_Progress_Do( dc, r, item.Val, item.Max );
			case PerfStat::Bytes:
				return Perf_Progress_DoBytes( dc, r, item.Val, item.Max );
			}
		}
		return Perf_Label_Do( dc, r, item.Val );
	}

	Vec2_s PerfStatList_Do( Context_t dc, const Rect_s& r, const cstr_t* label, const PerfStat_s* item, int count )
	{
		// measure
		Font_t font = Context_GetFont( dc );
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		float text_w = Graphics_MeasureMaxStringWidth( g, font, label, count );
		float text_h = Context_GetLineHeight( dc );
		float ctrl_h = ProgressBar_CalcSize( dc, 100.0f ).y;
		float item_h = 2.0f + NeMax( text_h + 2.0f, ctrl_h );

		// theme
		const uint32_t text_color = theme->Visual[Visual::Label].Text[CtrlState::Normal];

		// labels
		Rect_s pos;
		pos = Rect_s { r.x, r.y, r.w, item_h };
		for ( int i = 0; i < count; ++i )
		{
			Graphics_DrawString( g, pos, label[i], font, TextFormat::Middle, text_color ); 
			pos.y += pos.h;
		}

		// values
		pos = Rect_s { r.x + text_w + 10.0f, r.y, r.w - text_w - 10.0f, item_h };
		for ( int i = 0; i < count; ++i )
		{
			PerfStat_Do( dc, pos, item[i] );
			pos.y += item_h;
		}

		// extent
		return Vec2_s { text_w + 100.0f, count * item_h };
	}

	Vec2_s PerfStatList_DoView( Context_t dc, const Rect_s& r, const cstr_t* label, const PerfStat_s* item, int count )
	{
		const Vec2_s border_size = Vec2_s { 4.0f, 2.0f };
		const Rect_s inner_rect = Rect_Margin( r, border_size );
		const Vec2_s list_size = PerfStatList_Do( dc, inner_rect, label, item, count );
		return list_size + border_size * 2.0f;
	}

	static Vec2_s View_Perf_Stats_Server( Context_t dc )
	{
		ServerStats_s stats;
		Server_GetStats( stats );
		const cstr_t label[] = 
		{ "Threads"
		, "Backlog"
		, "Names"
		, "Scopes"
		, "Locks"
		};
		const PerfStat_s item[] = 
		{ { stats.Value[ ServerStat::NumThreads  ], stats.Value[ ServerStat::MaxThreads		 ], PerfStat::Counter }
		, { stats.Value[ ServerStat::BacklogSize ], stats.Value[ ServerStat::BacklogCapacity ], PerfStat::Bytes	  }
		, { stats.Value[ ServerStat::NumNames	], 0										  , PerfStat::Counter }
		, { stats.Value[ ServerStat::NumScopes	], 0										  , PerfStat::Counter }
		, { stats.Value[ ServerStat::NumLocks	], 0										  , PerfStat::Counter }
		}; 
		NeStaticAssert( NeCountOf(label) == NeCountOf(item) );
		return PerfStatList_DoView( dc, Context_GetChild( dc ), label, item, NeCountOf(item) );
	}

	static Vec2_s View_Perf_Stats_Database( Context_t dc )
	{
		Database_t db = PerfView.Database;
		const cstr_t label[] = 
		{ "Size"
		, "Cpus"
		, "Threads"
		, "Frames"
		, "Scopes"
		, "Locks"
		, "Locations"
		};
		const PerfStat_s item[] = 
		{ { (uint32_t)	Database_GetSize		( db ), (uint32_t)Database_GetCapacity( db ), PerfStat::Bytes   }
		, {	(uint32_t)	Database_GetNumCpus		( db ), 0									, PerfStat::Counter	}
		, {	(uint32_t)	Database_GetNumThreads	( db ), 0									, PerfStat::Counter }
		, {	(uint32_t)	Database_GetNumFrames	( db ), 0									, PerfStat::Counter }
		, {	(uint32_t)	Database_GetNumScopes	( db ), 0									, PerfStat::Counter }
		, {	(uint32_t)	Database_GetNumLocks	( db ), 0									, PerfStat::Counter }
		, {	(uint32_t)	Database_GetNumLocations( db ), 0									, PerfStat::Counter }
		}; 
		NeStaticAssert( NeCountOf(label) == NeCountOf(item) );
		return PerfStatList_DoView( dc, Context_GetChild( dc ), label, item, NeCountOf(item) );
	}

	static Vec2_s View_Perf_Stats_Parser( Context_t dc )
	{
		return Vec2_s {};
	}

	static CollapseItem_s Perf_Stats_Item[] = 
	{ { View_Perf_Stats_Server	, "Server"	, true }
	, { View_Perf_Stats_Database, "Database", true }
	, { View_Perf_Stats_Parser	, "Parser"	, true }
	};

	static Vec2_s Window_Perf_Stats( Context_t dc, void* user )
	{
		NePerfScope("Statistics");
		Vec2_s item_size = {};
		const Rect_s child_rect = Context_GetChild( dc );
		const Vec2_s list_size = CollapseList_Do( dc, NE_UI_ID, &Perf_Stats_Item[0], NeCountOf( Perf_Stats_Item ), &item_size );
		const Vec2_s max_size = list_size + Vec2_s { 0.0f, child_rect.h - item_size.y };
		return max_size;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf Desktop

	static DesktopWindow_s Perf_Wnd[] = 
	{
		{ { NE_UI_ID, Rect_s(), "Settings"	, false }, { Window_Perf_Settings }, {} }
	,	{ { NE_UI_ID, Rect_s(), "Statistics", false }, { Window_Perf_Stats	  }, {} }
	,	{ { NE_UI_ID, Rect_s(), "Frames"	, false }, { Window_Perf_Frames	  }, {} }
	,	{ { NE_UI_ID, Rect_s(), "Zones"		, false }, { Window_Perf_Zones	  }, {} }
	};

	static void Perf_Wnd_Init( Context_t dc )
	{
		if (Perf_Wnd[0].Wnd.Rect.w && !PerfView.AutoDock)
			return;

		const Vec2_s margin = { 2.0f, 2.0f };
		const Rect_s bounds = Context_GetChild( dc );

		const float left	=						margin.x;
		const float top		=						margin.y;
		const float right	= bounds.x + bounds.w - margin.x;
		const float bottom	= bounds.y + bounds.h - margin.y;

		Perf_Wnd[0].Wnd.Rect = Rect_s { left, top					 , 280.0f,				  150.0f		    };
		Perf_Wnd[1].Wnd.Rect = Rect_s { left, top + 150.0f + margin.y, 280.0f, bottom - top - 150.0f - margin.y };

		float x = left + margin.x + 280.0f;
		float w = right - x;

		Perf_Wnd[2].Wnd.Rect = Rect_s { x, bottom - 200.0f, w,				  200.0f			};
		Perf_Wnd[3].Wnd.Rect = Rect_s { x, top			  , w, bottom - top - 200.0f - margin.y };
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf View

	void PerfView_Do( Context_t dc, Database_t db, Parser_t parser )
	{
		NePerfScope("Perf UI");
		PerfView.Database = db;
		PerfView.Parser = parser;
		Perf_Wnd_Init( dc );
		Desktop_Do( dc, Perf_Wnd, NeCountOf(Perf_Wnd) );
	}

} }

