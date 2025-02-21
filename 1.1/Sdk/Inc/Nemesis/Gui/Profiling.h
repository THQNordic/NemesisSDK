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
#include "Types.h"
#include <Nemesis/Core/ProcessTypes.h>
#include <Nemesis/Perf/Visualizer.h>

//======================================================================================
namespace nemesis { namespace gui
{
	typedef system::Tick_t Tick_t;

} }

//======================================================================================
namespace nemesis { namespace gui
{
	struct Unit_s
	{
		float  Value;
		cstr_t Format;
	};

	Unit_s NE_API Unit_MakeBytes( uint32_t val );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Clock

	struct Clock_s
	{
		Tick_t TicksPerSecond;
		float OneOverTicksPerSecond;

		float TicksPerMs()			   const { return TicksPerSecond * 0.001f; }
		float TickToMs( Tick_t tick )  const { return OneOverTicksPerSecond * (1000 * tick); }
		float TickToSec( Tick_t tick ) const { return OneOverTicksPerSecond * tick; }
		float TickToFps( Tick_t tick ) const { return (float)TicksPerSecond / (float)tick; }
		Tick_t MsToTick( float ms )    const { return (Tick_t)(ms * TicksPerMs()); }
	};

	Clock_s NE_API Clock_Build( Tick_t ticks_per_second );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zoom

	struct Zoom_s
	{
		float PixelsPerMs;
		float PixelsPerSecond;
		float OneOverPixelsPerSecond;

		float  MsToPixel( float ms ) const								{ return ms * PixelsPerMs; }
		float  PixelToMs( float ms ) const								{ return 1000.0f * ms * OneOverPixelsPerSecond; }

		float  TickToPixel( Tick_t tick	, const Clock_s& clock ) const	{ return MsToPixel( (1000 * tick) * clock.OneOverTicksPerSecond ); }
		Tick_t PixelToTick( float  x	, const Clock_s& clock ) const	{ return (Tick_t) ( (clock.TicksPerSecond * x) * OneOverPixelsPerSecond ); }
	};

	Zoom_s NE_API Zoom_Default();
	Zoom_s NE_API Zoom_Build( float pixels_per_ms );
	Zoom_s NE_API Zoom_Scale( const Zoom_s& zoom, float multiplier, float power );
	Zoom_s NE_API Zoom_Scale( const Zoom_s& zoom, float multiplier, float power, float min, float max );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Timeline

	struct Timeline_s
	{
		Clock_s Clock;
		Zoom_s  Scale;
		Tick_t  Offset;
	};

	void	NE_API Timeline_Initialize	( Timeline_s& timeline, Tick_t ticks_per_second, Tick_t first_tick, float pixels_per_ms );
	void	NE_API Timeline_ZoomAt		( Timeline_s& timeline, int amount, float x );
	void	NE_API Timeline_ScrollCoord	( Timeline_s& timeline, float dx );
	Tick_t	NE_API Timeline_DxToDt		( const Timeline_s& timeline, float  dx );
	float	NE_API Timeline_DtToDx		( const Timeline_s& timeline, Tick_t dt );
	Tick_t	NE_API Timeline_CoordToTick	( const Timeline_s& timeline, float  x );
	float	NE_API Timeline_TickToCoord	( const Timeline_s& timeline, Tick_t t );
	void	NE_API Timeline_Clamp		( Timeline_s& timeline, const Rect_s& r, ne::profiling::Database_t db );

	bool	NE_API Timeline_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, Timeline_s& timeline );
	bool	NE_API Timeline_Keyboard	( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, Timeline_s& timeline );
	bool	NE_API Timeline_Input		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, Timeline_s& timeline );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Time Bar

	struct TimeBarMetric_s
	{
		Vec2_s LabelMargin;
	};

	struct TimeBarColor_s
	{
		uint32_t Text;
		uint32_t Fill;
	};

	struct TimeBarPalette_s
	{
		TimeBarColor_s Frame;
		TimeBarColor_s Time;
	};

	struct TimeBarTheme_s
	{
		Font_t			 Font;
		TimeBarMetric_s	 Metric;
		TimeBarPalette_s Palette;
	};

	struct TimeBarLod_s
	{
		Tick_t MinFrameTicks;
	};

	float NE_API TimeBar_CalcHeight		( Context_t dc, const TimeBarTheme_s& v );
	float NE_API TimeBar_CalcItemHeight	( Context_t dc, const TimeBarTheme_s& v );
	bool  NE_API TimeBar_Draw			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, const Timeline_s& timeline, const TimeBarLod_s& lod, const TimeBarTheme_s& v );
	void  NE_API TimeBar_Mouse			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API TimeBar_Keyboard		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API TimeBar_Input			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API TimeBar_Do				( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline, const TimeBarLod_s& lod, const TimeBarTheme_s& v );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Bar

	struct ZoneBarMetric_s
	{
		Vec2_s LabelMargin;
	};

	struct ZoneBarColor_s
	{
		uint32_t Text;
		uint32_t Fill;
	};

	struct ZoneBarPalette_s
	{
		ZoneBarColor_s Back;
		ZoneBarColor_s Zone;
		ZoneBarColor_s Group;
		ZoneBarColor_s Lock;
	};

	struct ZoneBarTheme_s
	{
		Font_t			 Font;
		ZoneBarMetric_s	 Metric;
		ZoneBarPalette_s Palette;
	};

	struct ZoneBarLod_s
	{
		Tick_t	MinFrameTicks;	// Group frames below this duration
		Tick_t	MinZoneTicks;	// Group zones below this duration (callback for zone group, not individual zone)
		Tick_t	MinGroupTicks;	// Skip groups delow this duration
		Tick_t	MaxGapTicks;	// Do not group zones if the gap between them is larger than this
		uint8_t	MaxZoneLevel;	// Skip zones on higher levels
		bool	NoIdle;			// Skip idle zones
		bool	NoEmpty;		// Skip empty threads
	};

	struct ZoneBarState_s
	{
		viz::ZoneGroup Hot;
	};

	float NE_API ZoneBar_CalcZoneHeight	( Context_t dc, const ZoneBarTheme_s& v );
	float NE_API ZoneBar_CalcHeight		( Context_t dc, ne::profiling::Database_t, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v );
	void  NE_API ZoneBar_DrawPopup		( Context_t dc, ne::profiling::Database_t, const viz::ZoneGroup& item );
	bool  NE_API ZoneBar_Draw			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, const Timeline_s& timeline, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v, ZoneBarState_s& s );
	void  NE_API ZoneBar_Mouse			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API ZoneBar_Keyboard		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API ZoneBar_Input			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void  NE_API ZoneBar_Do				( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline, uint8_t thread, const ZoneBarLod_s& lod, const ZoneBarTheme_s& v, ZoneBarState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Culling

	struct ZoneCull_s
	{
		float MinFrameWidth;		// smaller frames are merged into groups
		float MinZoneWidth;			// smaller zones are merged into groups
		float MinGroupWidth;		// smaller groups are discarded
		float MaxGroupSpacingMs;	// maximum gap between zones
		uint8_t MaxZoneLevel;		// zones below are discarded
		uint8_t _unused_0_[3];
		bool ShowIdle;
		bool ShowEmpty;
	};

	ZoneCull_s	 NE_API ZoneCull_Default();
	ZoneBarLod_s NE_API ZoneCull_ToLod( const ZoneCull_s& cull, const Zoom_s& zoom, const Clock_s& clock );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Header

	struct ZoneHeaderMetric_s
	{
		Vec2_s LabelMargin;
		Vec2_s ButtonMargin;
	};

	struct ZoneHeaderColor_s
	{
		uint32_t Text;
		uint32_t Fill;
	};

	struct ZoneHeaderPalette_s
	{
		ZoneHeaderColor_s Header;
		ZoneHeaderColor_s Button;
	};

	struct ZoneHeaderTheme_s
	{
		Font_t				Font;
		ZoneHeaderMetric_s	Metric;
		ZoneHeaderPalette_s Palette;
		Alignment::Horz		Alignment;
	};

	struct ZoneHeaderState_s
	{
		bool Collapsed;
	};

	float NE_API ZoneHeader_CalcHeight	( Context_t dc, const ZoneHeaderTheme_s& v );
	float NE_API ZoneHeader_CalcWidth	( Context_t dc, const ZoneHeaderTheme_s& v, ne::profiling::Database_t );
	bool  NE_API ZoneHeader_Draw		( Context_t dc, Id_t id, const Rect_s& r, cstr_t name, const ZoneHeaderTheme_s& v, const ZoneHeaderState_s& s );
	void  NE_API ZoneHeader_Mouse		( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s );
	void  NE_API ZoneHeader_Keyboard	( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s );
	void  NE_API ZoneHeader_Input		( Context_t dc, Id_t id, const Rect_s& r, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s );
	void  NE_API ZoneHeader_Do			( Context_t dc, Id_t id, const Rect_s& r, cstr_t name, const ZoneHeaderTheme_s& v, ZoneHeaderState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	struct ZoneViewState_s
	{
		bool Group [ 64 ];
		bool Thread[ 64 ];
	};

	Vec2_s NE_API ZoneView_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, const ZoneBarLod_s& lod, Timeline_s& time, ZoneViewState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Bar

	struct FrameBarColor_s
	{
		uint32_t Text;
		uint32_t Fill;
	};

	struct FrameBarPalette_s
	{
		FrameBarColor_s Back;
		FrameBarColor_s Group;
		FrameBarColor_s Target;
		FrameBarColor_s Range;
	};

	struct FrameBarTheme_s
	{
		Font_t			  Font;
		FrameBarPalette_s Palette;
	};

	struct FrameBarLod_s
	{
		float MinGroupWidth;
		float TargetFps;
	};

	struct FrameBarItem_s
	{
		int First;
		int Count;
		Tick Sum;
		Tick Peak;
	};

	struct FrameBarState_s
	{
		FrameBarItem_s Hot;
	};

	FrameBarTheme_s NE_API FrameBar_DefaultTheme( Context_t dc );

	void NE_API FrameBar_DrawPopup	( Context_t dc, ne::profiling::Database_t, const FrameBarItem_s& item );
	bool NE_API FrameBar_Draw		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, const Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s );
	void NE_API FrameBar_Mouse		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void NE_API FrameBar_Keyboard	( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void NE_API FrameBar_Input		( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline );
	void NE_API FrameBar_Do			( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t, Timeline_s& timeline, const FrameBarLod_s& lod, const FrameBarTheme_s& v, FrameBarState_s& s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Zone Panel

	struct ZonePanel_s
	{
		ZoneCull_s		Cull;
		ZoneViewState_s State;
		Timeline_s		Timeline;
		bool			AutoScroll;
	};

	Vec2_s NE_API ZonePanel_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, const ZoneCull_s& cull, ZoneViewState_s& state, Timeline_s& timeline, bool& auto_scroll );
	Vec2_s NE_API ZonePanel_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, ZonePanel_s& instance );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Frame Panel

	Vec2_s NE_API FramePanel_Do( Context_t dc, Id_t id, const Rect_s& r, ne::profiling::Database_t db, Timeline_s& timeline, Timeline_s& zone_timeline, const Rect_s& zone_rect, bool& auto_scroll );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf Statistics

	struct PerfStat
	{
		enum Unit
		{
			Counter
			, Bytes
		};
	};

	struct PerfStat_s
	{
		uint32_t		Val;
		uint32_t		Max;
		PerfStat::Unit	Unit;
	};

	void	PerfStat_Do			(Context_t dc, const Rect_s& r, const PerfStat_s& item);
	Vec2_s	PerfStatList_Do		(Context_t dc, const Rect_s& r, const cstr_t* label, const PerfStat_s* item, int count);
	Vec2_s	PerfStatList_DoView	(Context_t dc, const Rect_s& r, const cstr_t* label, const PerfStat_s* item, int count);

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Perf View

	void NE_API PerfView_Do( Context_t dc, Database_t db, Parser_t parser );

} }
