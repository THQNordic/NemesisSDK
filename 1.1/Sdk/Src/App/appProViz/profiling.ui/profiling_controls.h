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
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace time_bar
	{
		struct Action
		{
			enum Enum { None, Dragging };
		};

		struct Result
		{
			Action::Enum Action;
			Tick TrackTick;
		};

		struct Visual_s
		{
			Font_t LargeFont;
			Font_t MediumFont;
			Font_t SmallFont;
			Color_c BkgndColor;
			Color_c TickColor;
			Color_c FrameColor[4];
			Color_c TextColor [4];
		};

		float CalculateHeight( const Visual_s& v );
		Result Draw( Id_t id, const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const FrameGroupSetup& setup );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	namespace summary_bar
	{
		struct Visual_s
		{
			Font_t LargeFont;
			Font_t SmallFont;
			Color_c BkgndColors	 [2];
			Color_c DelimiterColors[2];
		};

		struct State
		{
			bool Collapsed;
		};

		void Draw( const Rect_s& r, const Visual_s& v, const State& s, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const FrameGroupSetup& setup );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace lock_bar
	{
		struct Result
		{
			int Event;
		};

		struct Visual_s
		{
			Color_c BkgndColors[2];
			const Color_c* ThreadColors;
			int NumThreadColors;
			label::Visual_s Label;
			Color_c LockColor;
			Color_c EventColor;
			Color_c EnterColor;
			Color_c LeaveColor;
			float LockHeight;
			float LockSpacing;
		};

		struct State
		{
			bool Collapsed;
		};

		Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace cpu_bar
	{
		struct Result
		{
		};

		struct Visual_s
		{
			label::Visual_s Label;
			Color_c BkgndColors[2];
			Color_c TrackColor;

			const Color_c* ThreadColors;
			int NumThreadColors;

			float CpuHeight;
			float CpuSpacing;
		};

		struct State
		{
			bool Collapsed;
			uint32_t CollapsedThreadMask;
		};

		Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace zone_bar
	{
		static const float INLAY_MARGIN  = 1.0f;
		static const float INLAY_SPACING = 4.0f;
		static const float LABEL_MARGIN  = 4.0f;

		struct Result
		{
			int Thread;
			int FirstZone;
			int NumZones;
			TickInterval Time;
		};

		struct Visual_s
		{
			Font_t Font;
			Color_c BkgndColors[2];
			Color_c ZoneColor;
			Color_c TextColor;

			float ZoneHeight;
			float ZoneSpacing;

			bool ShowIdleZones;
			bool Collapsed;
		};

		Result Draw( const Rect_s& r, const Visual_s& v, Database_t db, const ZoomInfo& zoom, const FrameRange& frames, const ZoneGroupSetup& setup, int thread_index );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace zone_bar_list
	{
		struct Visual_s
		{
			Font_t Font;
			Color_c BkgndColors[2];
			const Color_c* ThreadColors;
			int NumThreadColors;
			float CollapsedHeight;
			float ExpandedHeight;
			float ExpandedSpacing;
			bool ShowIdleZones;
		};

		struct State
		{
			uint32_t CollapsedMask;
		};

		struct Result
		{
			zone_bar::Result HitZone;
		};

		float CalcThreadHeight( int thread_index, Database_t db, uint32_t collapse_mask, float collapsed_height, float expanded_height, float expanded_spacing, int max_depth );
		Result Draw( const Rect_s& clip, const Rect_s& r, const Visual_s& v, const State& s, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, const ZoneGroupSetup& setup );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace frame_bar
	{
		struct Visual_s
		{
			Color_c BkgndColor;
			Color_c FrameBarColor;
			Color_c FpsMarkerColor;
			Color_c RangeMarkerColor;
			label::Visual_s KeyFrameLabel;
		};

		void DrawBackground ( const Rect_s& r, const Visual_s& v );
		void DrawFpsMarker  ( const Rect_s& r, const Visual_s& v, float target_fps, float marker_fps );
		void DrawRangeMarker( const Rect_s& r, const Visual_s& v, const FrameRange& cull, const FrameRange& range );
		void DrawFrameBars  ( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db, float target_fps );
		void DrawKeyFrames  ( const Rect_s& r, const Visual_s& v, const ZoomInfo& zoom, const FrameRange& cull, Database_t db );

		int HitTestFrame( const Rect_s& r, const ZoomInfo& zoom, Tick scroll_tick, Database_t db, const Vec2_s& pos );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace graph_bar
	{
		struct Result
		{
			int HitCounter;
		};

		struct Item
		{
			int Counter;
		};

		struct Visual_s
		{
			Color_c BkgndColor;
			Color_c LineColor;
			float HeightScale;
			float MinLineWidth;
		};

		Result Draw( const Rect_s& r, const Visual_s& v, const Item& item, const ZoomInfo& zoom, const FrameRange& cull, Database_t db );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace graph_label
	{
		struct Item
		{
			int Counter;
		};

		struct Visual_s
		{
			Color_c BkgndColor;
			label::Visual_s Label;
			float LabelIndent;
			float HeightScale;
		};

		void Draw( const Rect_s& r, const Visual_s& v, const Item& item, const ZoomInfo& zoom, const FrameRange& cull, Database_t db );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	namespace graph_tree
	{
		static const Color_c GRAPH_COLORS[] = 
		{ 
            Color::Chartreuse,
            Color::Coral,
            Color::CornflowerBlue,
            Color::Cornsilk,
            Color::Crimson,
            Color::Cyan,
            Color::DarkCyan,
            Color::DarkGoldenrod,
            Color::DarkGray,
            Color::DarkGreen,
            Color::DarkKhaki,
            Color::DarkOliveGreen,
            Color::DarkOrange,
            Color::DarkSalmon,
            Color::DarkSeaGreen,
            Color::DarkSlateBlue,
            Color::DarkSlateGray,
            Color::DarkTurquoise,
            Color::DeepPink,
            Color::DimGray,
            Color::DodgerBlue,
            Color::Firebrick,
            Color::FloralWhite,
            Color::ForestGreen,
            Color::Fuchsia,
            Color::Gainsboro,
            Color::Green,
            Color::GreenYellow,
            Color::Honeydew,
            Color::HotPink,
            Color::IndianRed,
            Color::Indigo,
            Color::Ivory,
            Color::Khaki,
            Color::Lavender,
		};

		struct ItemState
		{
			bool Enabled;
			bool Collapsed;
			bool _padding_[2];
		};

		struct State
		{
			float ScrollPos;
			int ActiveCounter;
			Array<ItemState> GroupStates;
			Array<ItemState> CounterStates;
		};

		struct Visual_s
		{
		};

		float CalculateHeight( const Visual_s& v, const State& state, Database_t db );
		void  Draw( const Rect_s& r, const Visual_s& v, State& state, Database_t db );

		void Initialize( State& state );
		void Shutdown( State& state );
	}

} } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler
{
	using namespace profiling::viz;
	using namespace profiling::viz::view;

	namespace profile_bar
	{
		static const float INLAY_MARGIN = 1.0f;

		struct Result
		{
		};

		struct Visual_s
		{
			Color_c BkgndColors[2];
			Color_c BarColors  [2];
			label::Visual_s Label;
			float ItemHeight;
			float ItemSpacing;
		};

		struct State
		{
			bool Collapsed;
		};

		Result Draw( const Rect_s& r, const Visual_s& v, const State& s, Database_t db, const FrameRange& frames, const ZoomInfo& zoom, const FrameGroupSetup& setup, HotSpotGroup& spots, int max_level );
	}

} } }
