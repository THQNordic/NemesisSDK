//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "graphics_ui_profiler_panels.h"
#include "utilities.h"

//======================================================================================
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace zone_panel
{
	//==================================================================================
	//	control
	namespace control
	{
		/// metrics
		static const float OPTIONS_HEIGHT			= 145.0f;
		static const float OPTIONS_HEIGHT_COLLAPSED = 24.0f;

		/// placement
		static Rect_s ViewToProfiler( const Rect_s& r, bool options_collapsed )
		{ 
			const float h = options_collapsed ? OPTIONS_HEIGHT_COLLAPSED : OPTIONS_HEIGHT;
			return Rect_s { r.x, r.y, r.w, r.h - h }; 
		}

		static Rect_s ViewToOptions( const Rect_s& r, bool options_collapsed )
		{ 
			const float h = options_collapsed ? OPTIONS_HEIGHT_COLLAPSED : OPTIONS_HEIGHT;
			return Rect_s { r.x, r.y + r.h - h, r.w, h }; 
		}

		static Rect_s OptionsToInner( const Rect_s& r )
		{
			return Rect_s { r.x, r.y + OPTIONS_HEIGHT_COLLAPSED, r.w, r.h - OPTIONS_HEIGHT_COLLAPSED };
		}

		static void DrawZoneViewOptions( Instance& panel, const Rect_s& r )
		{
			optionbar::Item items[] =
			{ optionbar::Item( ID(__FUNCTION__, __LINE__), L"Zone Height:"    , panel.ZoneView.Skin->ZoneBarList.ExpandedHeight  , 12.0f,  32.0f )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Max Zone Level:" , panel.ZoneView.ZoneCullSettings.MaxZoneLevel	 ,  0   ,  32    )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Min Group Width:", panel.ZoneView.ZoneCullSettings.MinGroupWidth	 ,  0.0f,  10.0f )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Max Group Space:", panel.ZoneView.ZoneCullSettings.MaxGroupSpacingMs,  0.0f,   1.0f )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Min Zone Width:" , panel.ZoneView.ZoneCullSettings.MinZoneWidth	 ,  0.1f,  10.0f )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Min Frame Width:", panel.ZoneView.ZoneCullSettings.MinFrameWidth	 , 10.0f, 150.0f )
		//	, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Clip Zones:"     , panel.ZoneView.ZoneCullSettings.Clip ? true : false )
			};

			optionbar::Draw( r, panel.ZoneView.Skin->OptionBar, items, NeCountOf(items) );

			panel.ZoneView.Skin->ZoneBarList.ExpandedHeight	  = items[0].Pos.Float;
			panel.ZoneView.ZoneCullSettings.MaxZoneLevel	  = items[1].Pos.Byte;
			panel.ZoneView.ZoneCullSettings.MinGroupWidth	  = items[2].Pos.Float;
			panel.ZoneView.ZoneCullSettings.MaxGroupSpacingMs = items[3].Pos.Float;
			panel.ZoneView.ZoneCullSettings.MinZoneWidth	  = items[4].Pos.Float;
			panel.ZoneView.ZoneCullSettings.MinFrameWidth	  = items[5].Pos.Float;
		//	panel.ZoneView.ZoneCullSettings.Clip			  = items[6].Pos.Bool ? 1 : 0;		
		}

		static void DrawZonePanelOptions( Instance& panel, const Rect_s& r )
		{
			optionbar::Item items[] =
			{ optionbar::Item( ID(__FUNCTION__, __LINE__), L"Auto Scroll:", (panel.State.HScroll == Horizontal::Scroll::Auto) )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Show Idle Zones:", panel.ZoneView.Skin->ZoneBarList.ShowIdleZones )
			};

			optionbar::Draw( r, panel.ZoneView.Skin->OptionBar, items, NeCountOf(items) );

			panel.State.HScroll								= items[0].Pos.Bool ? Horizontal::Scroll::Auto : Horizontal::Scroll::None;
			panel.ZoneView.Skin->ZoneBarList.ShowIdleZones	= items[1].Pos.Bool;
		}

		static void DrawSessionOptions( Instance& panel, Session& session, const Rect_s& r )
		{
			const bool is_paused = profiling::Receiver_IsPaused(session.Receiver);
			const bool is_connected = profiling::Receiver_IsConnected(session.Receiver);

			optionbar::Item items[] =
			{ optionbar::Item( ID(__FUNCTION__, __LINE__), L"Pause:"  , is_paused )
			, optionbar::Item( ID(__FUNCTION__, __LINE__), L"Connect:", is_connected )
			};

			optionbar::Draw( r, panel.ZoneView.Skin->OptionBar, items, NeCountOf(items) );

			if (items[0].Pos.Bool != is_paused)
			{
				profiling::Receiver_Pause(session.Receiver, items[0].Pos.Bool);
			}
			if (items[1].Pos.Bool != is_connected)
			{
				if (is_connected)
					profiling::Receiver_Disconnect( session.Receiver );
				else
					profiling::Receiver_Connect( session.Receiver, session.DefaultIp, session.ReceiverCallback );
			}
		}

		static void DrawDebugOptions( Instance& panel, Session& session, const Rect_s& r )
		{
#if (NE_CONFIG <= NE_CONFIG_DEBUG)
			static float parser_delay_ms = 0.0f;

			optionbar::Item items[] =
			{ optionbar::Item( ID(__FUNCTION__, __LINE__), L"Parser Delay (ms):", parser_delay_ms, 0, 150.0f )
			};

			optionbar::Draw( r, panel.ZoneView.Skin->OptionBar, items, NeCountOf(items) );

			parser_delay_ms = floorf(items[0].Pos.Float);
			Parser_SetDebugDelay( session.Parser, (int)parser_delay_ms);
#endif
		}

		static void DrawOptions( Instance& panel, Session& session, const Rect_s& r )
		{
			if (panel.State.OptionsCollapsed)
				return;

			FillRectangle( r, 0xff7a7a82 );

			const Rect_s inner_rect = Rect_Inflate( r, -2.0f, -2.0f );

			Layout group_layout;
			layout::Begin( group_layout, inner_rect, Vec2_s { 2.0f, 2.0f }, layout::Direction::Horizontal );

			layout::Item group_item;
			group_item = layout::Append( group_layout, 325.0f, group_layout.Bounds.h );
			if (!group_item.Visible)
				return;

			DrawRectangle( group_item.Bounds, Color::DarkGray );
			DrawZoneViewOptions( panel, Rect_Inflate( group_item.Bounds, -8.0f, -4.0f ) );

			group_item = layout::Append( group_layout, 325.0f, group_layout.Bounds.h );
			if (!group_item.Visible)
				return;

			DrawRectangle( group_item.Bounds, Color::DarkGray );
			DrawZonePanelOptions( panel, Rect_Inflate( group_item.Bounds, -8.0f, -4.0f ) );

			group_item = layout::Append( group_layout, 325.0f, group_layout.Bounds.h );
			if (!group_item.Visible)
				return;

			DrawRectangle( group_item.Bounds, Color::DarkGray );
			DrawSessionOptions( panel, session, Rect_Inflate( group_item.Bounds, -8.0f, -4.0f ) );

			group_item = layout::Remainder( group_layout );
			if (!group_item.Visible)
				return;

			DrawRectangle( group_item.Bounds, Color::DarkGray );
			DrawDebugOptions( panel, session, Rect_Inflate( group_item.Bounds, -8.0f, -4.0f ) );
		}

		/// rendering
		static void Draw( Instance& panel, Session& session, const Rect_s& r, Database_t db )
		{
			NePerfScope("Zone Panel");

			const Vec2_s mouse_pos = gui::GetMouse().Pos;
			const Rect_s profiler_rect = ViewToProfiler( r, panel.State.OptionsCollapsed );
			const Rect_s options_rect = ViewToOptions( r, panel.State.OptionsCollapsed );
			{
				gui::profiler::zone_view::Draw( panel.ZoneView, profiler_rect, db );
			}
			{
				FillRectangle( options_rect, 0xffc8c8dc );
				{
					const gui::collapsebar::Item ci = { ID(__FUNCTION__), L"Options", panel.State.OptionsCollapsed, collapsebar::Alignment::Left };
					if (gui::collapsebar::Draw( options_rect, panel.ZoneView.Skin->CollapseBar, ci ).Action)
						panel.State.OptionsCollapsed = !panel.State.OptionsCollapsed;
				}
			}
			{
				DrawOptions( panel, session, OptionsToInner( options_rect ) );
			}
			{
				DrawRectangle( r, Color::Black ); 
			}
			{
				gui::profiler::zone_view::DrawOverlay( panel.ZoneView, profiler_rect, db );
			}
			{
				if (gui::GetMouse().Button[0].NumClicks == 2)
				{
					const gui::profiler::zone_bar::Result& zone_hit = panel.ZoneView.HitZone;
					if (zone_hit.NumZones)
					{
						NamedLocation loc;
						Database_GetLocationByZone( db, zone_hit.FirstZone, loc );
						utilities::OpenFileInVisualStudio( loc.Location.File, loc.Location.Line );
					}
				}
			}
		}
	}

	//==================================================================================
	// public interface

	/// Initializes the panel's data.
	void Initialize( Instance& panel, Skin* skin )
	{
		NeZero(panel.State);
		gui::profiler::zone_view::Initialize( panel.ZoneView, skin );
	}

	/// Frees heap memory allocated by the panel.
	void Shutdown( Instance& panel )
	{
		gui::profiler::zone_view::Shutdown( panel.ZoneView );
	}

	/// Draws a zone view of the given data using the provided view state.
	void Draw( Instance& panel, Session& session, const Rect_s& r, Database_t db )
	{
		switch ( panel.State.HScroll )
		{
		case Horizontal::Scroll::Auto:
			{
				FitToFrameIndex( panel, db, Database_GetNumFrames( db ) );
			}
			break;
		}
		control::Draw( panel, session, r, db );
	}

	void FitToFrameIndex( Instance& panel, Database_t db, int i )
	{ return FitToFrameIndex( panel.ZoneView, db, i ); }

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace timeline_panel
{
	//==================================================================================
	// timeline
	namespace timeline
	{
		static float CalculateTargetFps( const State& state, const FrameRange& cull, Database_t db )
		{
			const Clock clock = Database_GetClock( db );
			switch ( state.VScale )
			{
			case Vertical::Scale::Visible:
				return CalculateMaxFps( cull, db );

			case Vertical::Scale::All:
				return clock.TickToFps( Database_GetMaxFrameDuration( db ) );

			default:
				break;
			}
			return state.TargetFps;
		}

		static bool IsCounterEnabled( Instance& panel, Database_t db, int counter_index )
		{
			const graph_tree::ItemState& item_state = panel.State.GraphTree.CounterStates[ counter_index ];
			return item_state.Enabled;
		}

		/// render
		static void Draw( Instance& panel, const Rect_s& r, const FrameRange& cull, Database_t db )
		{
			// render
			{
				// frame bar
				static const float KEY_FRAME_HEIGHT	 = 50.0f;
				const Rect_s keyframe_rect = Rect_s { r.x, r.y + r.h - KEY_FRAME_HEIGHT, r.w, KEY_FRAME_HEIGHT };
				const Rect_s framebar_rect = Rect_s { r.x, r.y, r.w, r.h - KEY_FRAME_HEIGHT };
				const float target_fps = CalculateTargetFps( panel.State, cull, db );
				FillRectangle( framebar_rect, panel.Skin->FrameBar.BkgndColor );
				gui::profiler::frame_bar::DrawKeyFrames( keyframe_rect, panel.Skin->FrameBar, panel.Zoom, cull, db );
				gui::profiler::frame_bar::DrawFrameBars( framebar_rect, panel.Skin->FrameBar, panel.Zoom, cull, db, target_fps );
				gui::profiler::frame_bar::DrawFpsMarker( framebar_rect, panel.Skin->FrameBar, target_fps, target_fps );

				// counters
				if (Database_GetNumCounters( db ))
				{
					int first_counter = -1;
					int num_counters = 0;
					{
						graph_bar::Visual_s v = 
						{ Color_c( 0, 0.01f ), Color::Goldenrod
						, 0.75f
						, 3.0f
						};

						Counter counter;
						graph_bar::Item item;
						for ( int counter_index = 0; counter_index < Database_GetNumCounters( db ); ++counter_index )
						{
							Database_GetCounter( db, counter_index, counter );
							if (!IsCounterEnabled( panel, db, counter_index ))
								continue;

							const int color_index = counter_index % NeCountOf(graph_tree::GRAPH_COLORS);
							v.LineColor = graph_tree::GRAPH_COLORS[ color_index ];
							item.Counter = counter_index;
							gui::profiler::graph_bar::Draw( framebar_rect, v, item, panel.Zoom, cull, db );
							if (first_counter < 0)
								first_counter = counter_index;
							++num_counters;
						}
					}

					if (panel.State.GraphTree.ActiveCounter < 0)
						panel.State.GraphTree.ActiveCounter = first_counter;
					if (num_counters == 0)
						panel.State.GraphTree.ActiveCounter = -1;

					if (panel.State.GraphTree.ActiveCounter >= 0)
					{
						const float GRAPH_LABEL_WIDTH = 100.0f;
						const Rect_s label_rect = Rect_s { framebar_rect.x, framebar_rect.y, GRAPH_LABEL_WIDTH, framebar_rect.h };

						const graph_label::Visual_s v = 
						{ Color_c( Color::White, 0.25f )
						, { gui::EnsureFont( 9.0f, FontStyle::Default ), Color::SlateBlue } 
						, 4.0f
						, 0.75f
						};
						const graph_label::Item item = { panel.State.GraphTree.ActiveCounter };
						graph_label::Draw( label_rect, v, item, panel.Zoom, cull, db );
					}
				}
			}

			// input
			const Id_t id = Id_Gen(__FUNCTION__);
			if (Rect_Contains( r, gui::GetMouse().Pos ))
				SetHot( id );

			if (gui::IsActive( id ))
			{
				const DragInfo_s& drag = gui::GetDrag();
				const Vec2_s delta_pos = gui::GetMouse().Pos - drag.Pos;
				BeginDrag( id );

				const Clock clock = Database_GetClock( db );
				const int64_t delta_tick = panel.Zoom.PixelToTick( delta_pos.x, clock );
				panel.Scroll -= delta_tick;
			}

			if (gui::IsHot( id ))
			{
				if (gui::GetMouse().Button[0].WentDown)
				{
					SetActive( id );
					BeginDrag( id );
				}
			}
		}
	}

	//==================================================================================
	// toolbar
	namespace toolbar
	{
		typedef gui::toolbar::Item Item;

		/// draws the toolbar
		static void Draw( const Rect_s& view, const gui::toolbar::Visual_s& v, State& state, Rect_s& remain )
		{
			int item_index = 0;
			const Item items[] = 
			{ { ID(__FUNCTION__, item_index++), Item::Label		, L"V Scale:"	}
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"60 fps"		, (state.VScale == Vertical::Scale::FPS) && (state.TargetFps == 60.0f) }
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"30 fps"		, (state.VScale == Vertical::Scale::FPS) && (state.TargetFps == 30.0f) }
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"15 fps"		, (state.VScale == Vertical::Scale::FPS) && (state.TargetFps == 15.0f) }
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"visible"	, (state.VScale == Vertical::Scale::Visible) }
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"all"		, (state.VScale == Vertical::Scale::All) }
			, { ID(__FUNCTION__, item_index++), Item::Delimiter }
			, { ID(__FUNCTION__, item_index++), Item::Label		, L"H Scale:"	}
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"fit all"	, (state.HScale == Horizontal::Scale::Fit) }
			, { ID(__FUNCTION__, item_index++), Item::Delimiter }
			, { ID(__FUNCTION__, item_index++), Item::Label		, L"H Scroll:"	}
			, { ID(__FUNCTION__, item_index++), Item::Button	, L"auto"		, (state.HScroll == Horizontal::Scroll::Auto) }
			, { ID(__FUNCTION__, item_index++), Item::Delimiter }
			};

			const gui::toolbar::Result result = gui::toolbar::Draw( view, v, items, NeCountOf(items), &remain );
			if (result.Action)
			{
				switch (result.Item)
				{
				case 1:
					state.VScale = Vertical::Scale::FPS;
					state.TargetFps = 60.0f;
					break;
				case 2:
					state.VScale = Vertical::Scale::FPS;
					state.TargetFps = 30.0f;
					break;
				case 3:
					state.VScale = Vertical::Scale::FPS;
					state.TargetFps = 15.0f;
					break;
				case 4:
					state.VScale = Vertical::Scale::Visible;
					break;
				case 5:
					state.VScale = Vertical::Scale::All;
					break;
				case 8:
					state.HScale = (state.HScale == Horizontal::Scale::Fit) ? Horizontal::Scale::None : Horizontal::Scale::Fit;
					break;
				case 11:
					state.HScroll = (state.HScroll == Horizontal::Scroll::Auto) ? Horizontal::Scroll::None : Horizontal::Scroll::Auto;
					break;
				}
			}
		}
	}

	//==================================================================================
	// scroll bar
	namespace scrollbar
	{
		static float Draw( const Rect_s& view_rect, const FrameRange& cull, Database_t db )
		{
			const Tick data_range = Database_GetLastEndTick( db ) - Database_GetFirstTick( db );
			const Tick view_range = cull.Time.Duration();

			const float min = 0.0f;
			const float max = (float)data_range;
			const float page = (float)view_range;
			const float pos = (float)(cull.Time.Begin - Database_GetFirstTick( db ));

			if ((data_range == 0) || (view_range == 0))
				return pos;

			const gui::scrollbar::Visual_s v = { gui::scrollbar::Layout::Horizontal };
			const gui::scrollbar::Item item = { pos, page, min, max };
			return gui::scrollbar::Draw( ID(__FUNCTION__), view_rect, v, item ).TrackPos;
		}
	}

	//==================================================================================
	// control
	namespace control
	{
		/// metrics
		static const float TOOLBAR_HEIGHT  = 30.0f;
		static const float TREE_WIDTH      = 250.0f;
		static const float TREE_SPACING	   = 10.0f;
		static const float COLLAPSE_WIDTH  = 24.0f;

		/// placement
		static Rect_s TreeToScroll( const Rect_s& r )
		{ return Rect_Inflate( Rect_s { r.x + r.w - COLLAPSE_WIDTH, r.y + COLLAPSE_WIDTH, COLLAPSE_WIDTH, r.h - COLLAPSE_WIDTH }, -3.0f, -3.0f ); }

		static Rect_s TreeToCtrl( const Rect_s& r )
		{ return Rect_s { r.x, r.y, r.w - COLLAPSE_WIDTH, r.h }; }

		static Rect_s ViewToTree( const Rect_s& r, bool tree_collapsed )
		{ 
			const float w = tree_collapsed ? COLLAPSE_WIDTH : TREE_WIDTH;
			return Rect_s { r.x + r.w - w, r.y, w, r.h }; 
		}

		static Rect_s ViewToTimeline( const Rect_s& r, bool tree_collapsed )
		{ 
			const float w = tree_collapsed ? COLLAPSE_WIDTH : TREE_WIDTH;
			return Rect_s { r.x, r.y, r.w - w - TREE_SPACING, r.h - TOOLBAR_HEIGHT }; 
		}

		static Rect_s ViewToToolbar( const Rect_s& r, bool tree_collapsed )
		{ 
			const float w = tree_collapsed ? COLLAPSE_WIDTH : TREE_WIDTH;
			return Rect_s { r.x, r.y + r.h - TOOLBAR_HEIGHT, r.w - w - TREE_SPACING, TOOLBAR_HEIGHT }; 
		}

		/// rendering
		static void DrawGraphTree( Instance& panel, const Rect_s& view, Database_t db )
		{
			const Rect_s tree_rect = ViewToTree( view, panel.State.GraphTreeCollapsed );

			// collapsible bar
			const gui::collapsebar::Item ci = { ID(__FUNCTION__), L"", panel.State.GraphTreeCollapsed, collapsebar::Alignment::Right };
			if (gui::collapsebar::Draw( tree_rect, panel.Skin->CollapseBar, ci ).Action)
				panel.State.GraphTreeCollapsed = !panel.State.GraphTreeCollapsed;

			DrawRectangle( tree_rect, Color::Black );
			if (panel.State.GraphTreeCollapsed)
				return;

			// scroll bar
			const graph_tree::Visual_s gtv = {};
			const Rect_s ctrl_rect = TreeToCtrl( tree_rect );
			const float tree_height = graph_tree::CalculateHeight( gtv, panel.State.GraphTree, db );
			if (tree_height > ctrl_rect.h)
			{
				const Rect_s scroll_rect = TreeToScroll( tree_rect );
				const gui::scrollbar::Visual_s sv = { gui::scrollbar::Layout::Vertical };
				const gui::scrollbar::Item si = { panel.State.GraphTree.ScrollPos, ctrl_rect.h, 0.0f, tree_height };
				panel.State.GraphTree.ScrollPos = gui::scrollbar::Draw( ID(__FUNCTION__, __LINE__), scroll_rect, sv, si ).TrackPos;
			}
			else
			{
				panel.State.GraphTree.ScrollPos = 0.0f;
			}

			// tree
			graph_tree::Draw( ctrl_rect, gtv, panel.State.GraphTree, db );
		}

		static void DrawTimeline( Instance& panel, const Rect_s& r, Database_t db, const FrameRange& cull )
		{
			PushClip( r );
			timeline::Draw( panel, r, cull, db );
			PopClip();
		}

		static void DrawToolBar( Instance& panel, const Rect_s& view, Database_t db, Rect_s& remain )
		{
			const Rect_s toolbar_rect = ViewToToolbar( view, panel.State.GraphTreeCollapsed );
			toolbar::Draw( toolbar_rect, panel.Skin->ToolBar, panel.State, remain );
		}

		static void Draw( Instance& panel, const Rect_s& view, Database_t db )
		{
			const Clock clock = Database_GetClock( db );
			panel.View = view;

			// input
			if (gui::GetMouse().Wheel)
			{
				// calculate tick
				const Vec2_s mouse_view_pos = ClientToView( panel.View, gui::GetMouse().Pos );
				const int64_t tick = PixelToTick( clock, panel.Zoom, panel.Scroll, mouse_view_pos.x );

				// zoom in/out
				const int count = gui::GetMouse().Wheel;
				const float e = expf(1.0f);
				const float base = 0.5f * e;
				panel.Zoom = ZoomScale( panel.Zoom, base, (float)count /*, 0.001f, 10000.0f*/ );

				// restore tick at client coord
				const int64_t new_tick = PixelToTick( clock, panel.Zoom, panel.Scroll, mouse_view_pos.x );
				const int64_t offset = tick-new_tick;
				panel.Scroll += offset;

				// disable fitting
				panel.State.HScale = Horizontal::Scale::None;
			}

			// zoom & scroll
			const Rect_s timeline_rect = ViewToTimeline( view, panel.State.GraphTreeCollapsed );
			switch (panel.State.HScale)
			{
			case Horizontal::Scale::Fit:
				FitAll( panel, db );
				break;
			default:
				if (panel.State.HScroll)
					GotoEnd( panel, db );
				break;
			}
			panel.Scroll = ClampScrollTick( db, panel.Zoom, timeline_rect.w, panel.Scroll );

			// culling
			FrameRange cull;
			view::EnumFrameGroups( db, timeline_rect, panel.Scroll, panel.Zoom, cull );

			// draw
			Rect_s toolbar_remain;
			DrawGraphTree( panel, view, db );
			DrawTimeline( panel, timeline_rect, db, cull );
			DrawToolBar( panel, view, db, toolbar_remain );
			scrollbar::Draw( toolbar_remain, cull, db );
		}

		/// input
		static void Input( Instance& panel, const Rect_s& view, Database_t db )
		{
			// keyboard
			const Keyboard_s& keyboard = gui::GetKeyboard();

			if (keyboard.Key[ VK_HOME ].AutoRepeat)
			{
				GotoFirstFrame( panel, db );
			}
			else if (keyboard.Key[ VK_END ].AutoRepeat)
			{
				if ( keyboard.Key[ VK_CONTROL ].AutoRepeat )
					GotoEnd( panel, db );
				else
					GotoLastFrame( panel, db );
			}
			else if (keyboard.Key[ VK_LEFT ].AutoRepeat || keyboard.Key[ VK_RIGHT ].AutoRepeat)
			{
				const BOOL ctrl = keyboard.Key[ VK_CONTROL ].IsPressed;
				const BOOL shift = keyboard.Key[ VK_SHIFT ].IsPressed;
				const int amount = keyboard.Key[ VK_LEFT ].AutoRepeat ? -1 : 1;
				const ScrollUnit::Enum unit 
					= shift ? ScrollUnit::Second
					: (ctrl ? ScrollUnit::Millisecond : ScrollUnit::Page)
					;
				Scroll( panel, db, unit, amount );
			}
		}
	}

	//==================================================================================
	// public interface

	/// Initializes the panel's data.
	void Initialize( Instance& panel, Skin* skin )
	{
		NeZero(panel);
		Initialize(panel.State.GraphTree);
		panel.Zoom = ZoomTo( 1.0f / 25.0f );
		panel.State.TargetFps = 30.0f;
		panel.Skin = skin;
	}

	/// Frees heap memory allocated by the panel.
	void Shutdown( Instance& panel )
	{
		Shutdown(panel.State.GraphTree);
	}

	/// Draws a zone view of the given data using the provided view state.
	void Draw( Instance& panel, Session& session, const Rect_s& view, Database_t db )
	{
		NePerfScope("Timeline Panel");
		{
			const graph_tree::ItemState empty_state = {};
			const int num_new_counter_groups = Database_GetNumCounterGroups( db ) - panel.State.GraphTree.GroupStates.Count;
			for ( int i = 0; i < num_new_counter_groups; ++i )
				panel.State.GraphTree.GroupStates.Append(empty_state);

			const int num_new_counters = Database_GetNumCounters( db ) - panel.State.GraphTree.CounterStates.Count;
			for ( int i = 0; i < num_new_counters; ++i )
				panel.State.GraphTree.CounterStates.Append(empty_state);
		}
		control::Draw( panel, view, db );
		control::Input( panel, view, db );
	}

	void GotoFrameNumber( Instance& panel, Database_t db, uint32_t i )
	{ 
		panel.Scroll = Database_GotoFrameNumber( db, i ); 
	}
		
	void GotoFrameIndex( Instance& panel, Database_t db, int i )
	{
		panel.Scroll = Database_GotoFrameIndex( db, i ); 
	}

	void GotoFirstFrame( Instance& panel, Database_t db )
	{
		panel.Scroll = Database_GotoFirstFrame( db );
	}

	void GotoLastFrame( Instance& panel, Database_t db )
	{
		panel.Scroll = Database_GotoLastFrame( db );
	}

	void GotoBegin( Instance& panel, Database_t db )
	{
		panel.Scroll = Database_GotoBegin( db );
	}

	void GotoEnd( Instance& panel, Database_t db )
	{
		const Tick duration = panel.Zoom.PixelToTick( control::ViewToTimeline( panel.View, panel.State.GraphTreeCollapsed ).w, Database_GetClock( db ) );
		panel.Scroll = Database_GotoEnd( db, duration );
	}

	void FitAll( Instance& panel, Database_t db )
	{
		if (Database_GetNumFrames( db ) == 0)
			return;

		GotoFirstFrame( panel, db );

		const Clock clock = Database_GetClock( db );
		const float frame_ms = clock.TickToMs( Database_GetLastEndTick( db ) - Database_GetFirstTick( db ) );
		panel.Zoom = ZoomTo( control::ViewToTimeline( panel.View, panel.State.GraphTreeCollapsed ).w / frame_ms );
	}

	void Scroll( Instance& panel, Database_t db, ScrollUnit::Enum unit, int amount )
	{
		const Clock clock = Database_GetClock( db );
		panel.Scroll = Scroll( control::ViewToTimeline( panel.View, panel.State.GraphTreeCollapsed ).w, panel.Zoom, clock, unit, panel.Scroll, amount );
	}

	int HitTestFrame( const Instance& panel, Database_t db, const Vec2_s& pos )
	{
		return frame_bar::HitTestFrame( control::ViewToTimeline( panel.View, panel.State.GraphTreeCollapsed ), panel.Zoom, panel.Scroll, db, pos );
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace session_panel
{
	namespace
	{
		static Rect_s ItemToLabel( const Rect_s& r )
		{ return Rect_s { r.x + 4.0f, r.y, r.w - 8.0f, r.h }; }

		static Rect_s InlayToItem( const Rect_s& r, int i )
		{ return Rect_s { r.x, r.y + (i * 20.0f), r.w, 20.0f }; }

		static Rect_s ListToInlay( const Rect_s& r )
		{ return Rect_s { r.x + 4.0f, r.y + 22.0f, r.w - 8.0f, r.h - 26.0f }; }

		static Rect_s ListToLabel( const Rect_s& r )
		{ return Rect_s { r.x + 2.0f, r.y + 2.0f, r.w - 4.0f, 16.0f }; }

		static Rect_s ViewToList( const Rect_s& r )
		{ return Rect_s { r.x + 20.0f, r.y + 20.0f, r.w - 40.0f, r.h - 20.0f - 40.0f }; }

		static Rect_s ViewToToolbar( const Rect_s& r )
		{ return Rect_s { r.x + 20.0f, Rect_Bottom( r ) - 40.0f + 4.0f, r.w - 40.0f, 40.0f - 8.0f }; }
	}

	namespace
	{
		static system::IpAddress_t GetPrimaryAdapterIp()
		{
			ULONG sz;
			GetAdaptersInfo( nullptr, &sz );
			IP_ADAPTER_INFO* info = (IP_ADAPTER_INFO*)alloca( sz );
			GetAdaptersInfo( info, &sz );

			uint32_t ip[4];
			system::IpAddress_t addr = {};
			for ( ; info ; info = info->Next )
			{
				if ( StringComparer::Equals( info->IpAddressList.IpAddress.String, "0.0.0.0" ) )
					continue;
				sscanf_s( info->IpAddressList.IpAddress.String, "%u.%u.%u.%u", ip+0, ip+1, ip+2, ip+3 );
				addr.Ip[0] = (uint8_t)ip[0];
				addr.Ip[1] = (uint8_t)ip[1];
				addr.Ip[2] = (uint8_t)ip[2];
				addr.Ip[3] = (uint8_t)ip[3];
			}
			return addr;
		}

		static void ServerList_Refresh( Session& session, system::IpPort_t port )
		{
			// @note: consoles do not accept broadcasts

			using namespace system;
			static Socket_t socket = Udp_Open( 0, system::SocketOption::Broadcast | system::SocketOption::NonBlocking );

			// determine the subnet (c class) of our receiver
			{
				// not trivial
			}

			// broadcast header
			{
				ping::Header hdr;
				hdr.Init();
				IpAddress_t to = GetPrimaryAdapterIp();
				to.Port = port;

				for ( uint8_t i = 1; i < 255; ++i )
				{
					to.Ip[3] = i;
					Udp_Send( socket, to, &hdr, sizeof(hdr) );
				}
			}

			// receive responses
			{
				IpAddress_t from = {};
				ping::Response ack = {};
				while (Udp_Receive( socket, &from, &ack, sizeof(ack) ))
				{
					if (!ack.Header.IsValid())
						continue;
					int item = session.KnownServers.Ensure( from );
					session.KnownServers.Item[item].Status = 1;
				}
			}
		}

		static void ServerList_Refresh( Session& session )
		{
			for ( int i = 0; i < session.KnownServers.NumItems; ++i )
				session.KnownServers.Item[i].Status = 0;
			ServerList_Refresh(session, 16001);
			ServerList_Refresh(session, 17001);
		}
	}

	//==================================================================================
	// public interface
	void Draw( Session& session, const Skin* skin, const Rect_s& view, Database_t db )
	{
		NePerfScope("Session Panel");

		static bool refresh_enable = true;
		if (refresh_enable)
		{
			static system::StopWatch_c delay;
			int64_t elapsed = delay.ElapsedMs64();
			if ((!session.KnownServers.NumItems) || (elapsed >= 1000) )
			{
				ServerList_Refresh( session );
				delay.Start();
			}
		}

		// data
		static int selected = -1;

		// connection
		const system::IpAddress_t target_ip = system::Tcp_GetPeer( Receiver_GetSocket( session.Receiver ) );

		// connection list
		{
			// setup
			const wchar_t* header_label [] = 
			{ L"Default"
			, L"Target"
			, L"Port"
			, L"System"
			, L"Status"
			};
			const float header_width [] = 
			{ 100.0f
			, 200.0f
			, 150.0f
			, 300.0f
			, 200.0f
			};
			const TextFormat::Mask_t item_format[] =
			{ TextFormat::Center | TextFormat::Middle
			, TextFormat::Left   | TextFormat::Middle
			, TextFormat::Left   | TextFormat::Middle
			, TextFormat::Left   | TextFormat::Middle
			, TextFormat::Left   | TextFormat::Middle
			};
			const int num_headers = NeCountOf(header_label);

			// list
			Font_t font		 = EnsureFont( 12.0f, FontStyle::Default );
			Font_t font_bold = EnsureFont( 12.0f, FontStyle::Bold	 );

			Rect_s list_rect = ViewToList( view );
			DrawRectangle( list_rect, Color::Black );

			Rect_s label_rect = ListToLabel( list_rect );

			Rect_s inlay_rect = ListToInlay( list_rect );
			FillRectangle( inlay_rect, Color::Snow );

			Rect_s sub_rect;

			// headers
			{
				sub_rect = label_rect;
				for ( int i = 0; i < num_headers; ++i )
				{
					sub_rect.w = header_width[i] - 2.0f;
					if (Rect_Right( sub_rect ) > Rect_Right( label_rect ) )
						break;
					DrawBox( sub_rect, Color::White, Color::Gray, Color::LightGray );
					DrawString( header_label[i], -1, font, TextFormat::Center | TextFormat::Middle, ItemToLabel( sub_rect ), Color::Black );
					sub_rect.x += sub_rect.w + 2.0f;
				}
				sub_rect.w = Rect_Right( label_rect ) - sub_rect.x - 2.0f;
				DrawBox( sub_rect, Color::White, Color::Gray, Color::LightGray );
			}

			// items
			int hot = -1;
			bool is_def;
			bool is_sel;
			bool is_con;
			bool is_avail;
			Rect_s item_rect;
			TextBuffer& text = gui::GetTextBuffer();
			for ( int i = 0; i < session.KnownServers.NumItems; ++i )
			{
				const system::IpAddress_t& addr = session.KnownServers.Item[i].Address;
				uint32_t status = session.KnownServers.Item[i].Status;

				is_sel = (i == selected);
				is_def = BitwiseComparer::Equals( addr, session.DefaultIp );
				is_con = BitwiseComparer::Equals( addr, target_ip );
				is_avail = ((status & 1) == 1);

				// background
				item_rect = InlayToItem( inlay_rect, i );
				if (i == selected)
					FillRectangle( item_rect, Color::CadetBlue );

				// hot
				if (Rect_Contains( item_rect, gui::GetMouse().Pos ) )
					hot = i;

				// cull
				if (Rect_Bottom( item_rect ) > Rect_Bottom( inlay_rect ))
					return;

				// sub-items
				sub_rect = item_rect;
				for ( int j = 0; j < num_headers; ++j )
				{
					// cull
					sub_rect.w = header_width[j] - 2.0f;
					if (Rect_Right( sub_rect ) > Rect_Right( label_rect ))
						break;

					// format text
					switch ( j )
					{
					case 0:
						text.Length = swprintf_s( text.Text, L"%c", is_def ? 0x2605 : L' ' );
						break;
					case 1:
						text.Length = swprintf_s( text.Text, L"%u.%u.%u.%u", (uint32_t)addr.Ip[0], (uint32_t)addr.Ip[1], (uint32_t)addr.Ip[2], (uint32_t)addr.Ip[3] );
						break;
					case 2:
						text.Length = swprintf_s( text.Text, L"%u", (uint32_t)addr.Port );
						break;
					case 3:
						text.Length = swprintf_s( text.Text, L"" );
						break;
					case 4:
						text.Length = swprintf_s( text.Text, is_con ? L"Connected" : (is_avail ? L"Available" : L"Offline") );
						break;
					}

					// draw sub-item
					DrawString( text.Text, text.Length, is_def ? font_bold : font, item_format[j], ItemToLabel( sub_rect ), is_sel ? Color::White : Color::Black );
					sub_rect.x += sub_rect.w + 2.0f;
				}
			}

			// input
			if (Rect_Contains( inlay_rect, gui::GetMouse().Pos) )
			{
				if (gui::GetMouse().Button[0].WentDown)
					selected = hot;

				if (gui::GetMouse().Button[0].NumClicks == 2)
				{
					if (selected >= 0)
					{
						session.DefaultIp = session.KnownServers.Item[selected].Address;
						viz::SaveSettings( session );
					}
				}
			}
		}

		// toolbar
		{
			bool is_sel = (selected >= 0);
			bool is_def = is_sel && BitwiseComparer::Equals( session.KnownServers.Item[selected].Address, session.DefaultIp );
			bool is_con = is_sel && (BitwiseComparer::Equals( session.KnownServers.Item[selected].Address, target_ip ) && Receiver_IsConnected( session.Receiver ));

			int item_index = 0;
			const toolbar::Item items[] = 
			{ { ID(__FUNCTION__, item_index++), toolbar::Item::Button, L"Refresh", refresh_enable }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Delimiter }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Button, L"Connect"   , is_sel && !is_con }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Button, L"Disconnect", is_sel && is_con }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Delimiter }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Button, L"Default", is_sel && !is_def }
			, { ID(__FUNCTION__, item_index++), toolbar::Item::Delimiter }
			};

			Rect_s remain;
			const gui::toolbar::Result result = gui::toolbar::Draw( ViewToToolbar( view ), skin->ToolBar, items, NeCountOf(items), &remain );

			if (result.Action)
			{
				switch (result.Item)
				{
				case 0:
					refresh_enable = !refresh_enable;
					break;
				case 2:
					profiling::Receiver_Connect( session.Receiver, session.KnownServers.Item[selected].Address, session.ReceiverCallback );
					break;
				case 3:
					profiling::Receiver_Disconnect( session.Receiver );
					break;
				case 5:
					session.DefaultIp = session.KnownServers.Item[selected].Address;
					break;
				}
			}
		}
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace options_panel
{
	void Draw( const Rect_s& view, Database_t db )
	{
		/*
		Rect_s list_rect = view;
		FillRectangle( view, 0xffdddddd, 0xff888888, LinearGradient::Vertical );
		DrawRectangle( view, Color::Black );
		*/
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace about_panel
{
	static const char* FindEnd( const char* text, int len )
	{
		for ( int i = 0; i < len; ++i )
		{
			if ((text[i] == '\n') || (text[i] == '\0'))
				return text + i;
		}
		return nullptr;
	}

	static const char* FormatLine( TextBuffer& text, const char* line, int& remain )
	{
		const char* end = FindEnd( line, remain );
		const int len = end ? (int)(end-line) : remain;
		MultiByteToWideChar( CP_UTF7, 0, line, len, text.Text, NeCountOf(text.Text)-1 );
		text.Text[ len ] = 0;
		text.Length = len;
		if (end)
		{
			const int advance = (len + 1);
			remain -= advance;
			return line + advance;
		}
		remain = 0;
		return nullptr;
	}

	static void DrawContent( const Rect_s& r, const char* data, int size )
	{
		if (!data || !size)
			return;

		// setup visuals
		Font_t small_font = EnsureFont( L"Consolas", 12.0f, FontStyle::Bold );
		const float small_font_height = GetLineHeight(small_font);
		const TextFormat::Mask_t small_fmt = TextFormat::Left	| TextFormat::Top | TextFormat::NoWrap;

		// draw lines
		Rect_s line_rect = r;
		line_rect.h = small_font_height;
		const char* line = data;

		int remain = size;
		bool more = true;
		TextBuffer& text = gui::GetTextBuffer();
		for ( ; remain && (Rect_Bottom( line_rect ) <= Rect_Bottom( r )); )
		{
			line = FormatLine( text, line, remain );
			DrawString( text.Text, text.Length, small_font, small_fmt, line_rect, Color::Black );
			line_rect.y += line_rect.h;
		}
	}

	void Draw( const Rect_s& view, Database_t db )
	{
		const Font_t font = EnsureFont( L"Consolas", 18.0f, FontStyle::Bold );
		const float font_height = GetLineHeight(font);
		const TextFormat::Mask_t fmt = TextFormat::Center | TextFormat::Middle | TextFormat::NoWrap;

		Layout l;
		layout::Begin( l, view, Vec2_s { 1.0f, 1.0f }, layout::Direction::Vertical );
		{
			const layout::Item item = layout::Append( l, view.w, font_height + 8 );
			DrawBox( item.Bounds, Color::Gray, Color::White, Color::DarkGray );
			DrawString( L"Changelog", -1, font, fmt, item.Bounds, Color::Snow );
		}
		{
			const layout::Item item = layout::Remainder( l );
			DrawBox( item.Bounds, Color::Gray, Color::White, Color::LightGray );

			const HRSRC hRes = FindResource( NULL, L"IDR_CHANGE", RT_RCDATA );
			const DWORD size = SizeofResource( NULL, hRes );
			const HGLOBAL hData = LoadResource( NULL, hRes );
			const char* data = (const char*)LockResource( hData );
			DrawContent( Rect_Inflate( item.Bounds, -2.0f, -2.0f ), data, (int)size );
			UnlockResource( hData );
		}
	}

} } } }

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace main_panel
{
	//==================================================================================
	// control
	namespace control
	{
		static void DrawActiveView( Instance& panel, Session& session, const Rect_s& view, Database_t db )
		{
			switch ( panel.ActiveView )
			{
			case View::Session:
				session_panel::Draw( session, panel.ZoneView.ZoneView.Skin, view, db );
				break;
			case View::Timeline:
				timeline_panel::Draw( panel.TimeView, session, view, db );
				break;
			case View::Zones:
				zone_panel::Draw( panel.ZoneView, session, view, db );
				break;
				/*
			case View::Log:
				//log_panel::Draw( panel.LogView, session, view, db );
				break;
			case View::Options:
				options_panel::Draw( view, db );
				break;
				*/
			case View::About:
				about_panel::Draw( view, db );
				break;
			}
		}

		static const gui::tab::Item items [] = 
		{ { L"Target" }
		, { L"Timeline" }
		, { L"Zones" }
		/*
		, { L"Log" }
		, { L"Options" }
		*/
		, { L"About" }
		};
		static const int count = NeCountOf(items);

		static void DrawTabCtrl( Instance& panel, const Rect_s& view, const Session& session, Database_t db )
		{
			panel.ActiveView = (View::Enum)gui::tab::Draw( ID(__FUNCTION__), view, panel.ZoneView.ZoneView.Skin->TabBar, items, count, panel.ActiveView ).Item;
		}

		static const Vec2_s PANEL_MARGIN = Vec2_s { 10.0f, 10.0f };
		static Rect_s ViewToTabCtrl( const Rect_s& r )
		{
			return Rect_s { r.x + PANEL_MARGIN.x, r.y + PANEL_MARGIN.y, r.w - 2.0f * PANEL_MARGIN.x, r.h - 2.0f * PANEL_MARGIN.y };
		}

		static void Draw( Instance& panel, Session& session, const Rect_s& view, Database_t db )
		{
			const Rect_s tab_ctrl_rect = ViewToTabCtrl( view );
			const Rect_s tab_view_rect = gui::tab::CtrlToPage( tab_ctrl_rect );
			DrawTabCtrl( panel, tab_ctrl_rect, session, db );
			DrawActiveView( panel, session, tab_view_rect, db );
		}

		static void Input( Instance& panel, Session& session, const Rect_s& view, Database_t db )
		{
			// mouse
			if (gui::GetMouse().Button[0].NumClicks == 2)
			{
				switch (panel.ActiveView)
				{
				case View::Timeline:
					{
						const int frame_index = timeline_panel::HitTestFrame( panel.TimeView, db, gui::GetMouse().Pos );
						if (frame_index >= 0)
						{
							zone_panel::FitToFrameIndex( panel.ZoneView, db, frame_index );
							panel.ActiveView = View::Zones;
						}
					}
					break;

				default:
					break;
				}
			}

			// keyboard
			const Keyboard_s& keyboard = gui::GetKeyboard();
			if (keyboard.Key[ VK_F1 ].WentDown)
				panel.ActiveView = View::Session;
			else if (keyboard.Key[ VK_F2 ].WentDown)
				panel.ActiveView = View::Timeline;
			else if (keyboard.Key[ VK_F3 ].WentDown)
				panel.ActiveView = View::Zones;
		}
	};

	//==================================================================================
	// public interface

	/// Initializes the panel's data.
	void Initialize( Instance& panel, Skin* skin )
	{
		panel.ActiveView = View::Zones;
		timeline_panel::Initialize( panel.TimeView, skin );
		zone_panel	  ::Initialize( panel.ZoneView, skin );
	}

	/// Frees heap memory allocated by the panel.
	void Shutdown( Instance& panel )
	{
		timeline_panel::Shutdown( panel.TimeView );
		zone_panel	  ::Shutdown( panel.ZoneView );
	}

	/// Draws a main view of the given data using the provided view settings.
	void Draw( Instance& panel, Session& session, const Rect_s& view, Database_t db )
	{
		control::Draw( panel, session, view, db );
		control::Input( panel, session, view, db );
	}

} } } }
