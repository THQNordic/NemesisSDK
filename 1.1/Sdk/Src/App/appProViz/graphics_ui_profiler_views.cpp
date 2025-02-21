//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "graphics_ui_profiler_views.h"

//======================================================================================
namespace nemesis { namespace gui { namespace profiler { namespace zone_view
{
	//==================================================================================
	// internals
	static const float HEADER_HEIGHT			=  50.0f;
	static const float FOOTER_CONTENT_HEIGHT	= 150.0f;
	static const float FOOTER_BAR_HEIGHT		=  24.0f;
	static const float SCROLLBAR_WIDTH			=  20.0f;
	static const float SCROLLBAR_HEIGHT			=  20.0f;
	static const float NODE_WIDTH				= 150.0f;
	static const float NODE_BORDER				=   4.0f;
	static const float NODE_MARGIN				=   4.0f;
	static const float NODE_ICON_SIZE			=  16.0f;

	//==================================================================================

	/// Calculates the height of a single thread.
	static float CalcThreadHeight( const Item& item, const ItemState& state, int thread_index, Database_t db )
	{
		return zone_bar_list::CalcThreadHeight( thread_index, db, state.CollapsedMask, item.CollapsedHeight, item.ExpandedHeight, item.ExpandedSpacing, item.MaxDepth );
	}

	/// Calculates the height of a single item.
	static float CalcHeight( const Item& item, const ItemState& state, Database_t db )
	{
		switch ( item.Type )
		{
		case Item::Summary:
			{
				if (state.CollapsedMask)
					return item.CollapsedHeight;
				if (Database_GetNumFrames( db ) == 0)
					return item.CollapsedHeight;
				return NeMax( item.CollapsedHeight, item.ExpandedHeight + item.ExpandedSpacing );
			}
			break;

		case Item::Zones:
			{
				float total_height = 0.0f;
				int num_threads = Database_GetNumThreads( db );
				for ( int thread_index = 0; thread_index < num_threads; ++thread_index )
					total_height += CalcThreadHeight( item, state, thread_index, db );
				return total_height;
			}
			break;

		case Item::Locks:
			{
				if (state.CollapsedMask)
					return item.CollapsedHeight;
				return NeMax( item.CollapsedHeight, Database_GetNumLocks( db ) * (item.ExpandedHeight + item.ExpandedSpacing) );
			}
			break;

		case Item::Cpu:
			{
				if (state.CollapsedMask)
					return item.CollapsedHeight;
				return NeMax( item.CollapsedHeight, Database_GetNumCpus( db ) * (item.ExpandedHeight + item.ExpandedSpacing) );
			}
			break;

		case Item::Profile:
			{
				if (state.CollapsedMask)
					return item.CollapsedHeight;
				const int max_locations = item.MaxDepth;
				const int num_locations = NeMin( max_locations, Database_GetNumLocations( db ) );
				const float expanded_height = num_locations * (item.ExpandedHeight + item.ExpandedSpacing) + 2.0f * profile_bar::INLAY_MARGIN;
				return NeMax( item.CollapsedHeight, expanded_height );
			}
			break;

		default:
			break;
		}
		return 0.0f;
	}

	/// Calculates the total height of the given set of items.
	static float CalcHeight( const Item* items, const ItemState* states, int count, Database_t db )
	{
		float height = 0.0f;
		for ( int i = 0; i < count; ++i )
			height += CalcHeight( items[i], states[i], db );
		return height;
	}

	//==================================================================================

	static Rect_s NodeToFace( const Rect_s& r )
	{ return Rect_s { r.x + NODE_BORDER, r.y + NODE_BORDER, r.w - NODE_BORDER*2, r.h - NODE_BORDER*2 }; }

	static Rect_s FaceToIcon( const Rect_s& r )
	{ return Rect_s { r.x + NODE_MARGIN, r.y + NODE_MARGIN, NODE_ICON_SIZE, NODE_ICON_SIZE }; }

	static Rect_s FaceToLabel( const Rect_s& r )
	{ return Rect_s { r.x + NODE_ICON_SIZE + NODE_MARGIN * 2.0f, r.y + NODE_MARGIN + 2.0f, r.w - NODE_ICON_SIZE - NODE_MARGIN * 2.0f, r.h - NODE_MARGIN - 2.0f }; }

	static button::Action::Enum DrawNode( Id_t id, const Rect_s& r, const Color_c& c, bool collapsed, const wchar_t* name, int name_length )
	{
		const Font_t font = EnsureFont( 11.0f, FontStyle::Default );
		const button::Visual_s v = { { font, Color::Black }, 0xffeeeeee, 0xffaaaaaa, c };

		const Rect_s outer_rect = Rect_s { r.x, r.y, r.w-1.0f, r.h };
		const Rect_s face_rect = NodeToFace( outer_rect );
		const Rect_s label_rect = FaceToLabel( face_rect );
		const Rect_s icon_rect = FaceToIcon( face_rect );

		DrawBar( outer_rect, c, NODE_BORDER );
		DrawRectangle( outer_rect, 0xff9696a5 );
		DrawString( name, name_length, font, TextFormat::Default, label_rect, Color::Black );
		return button::Draw( id, icon_rect, v, collapsed ? L"+" : L"-", -1 );
	}

	/// draws a single item's node
	static void DrawNode( Instance& view, const Rect_s& clip, const Rect_s& r, const Item& item, const ItemState& state, Database_t db )
	{
		switch ( item.Type )
		{
		case Item::Summary:
			{
				if (DrawNode( ID( __FUNCTION__, item.Type ), r, 0xffc8c8dc, (state.CollapsedMask != 0), L"Summary", -1 ))
					view.CollapsedSummary = ~view.CollapsedSummary;
			}
			break;

		case Item::Zones:
			{
				int color_index = 0;
				uint32_t thread_flag = 0;
				bool collapsed = false;
				TextBuffer& text = gui::GetTextBuffer();

				Rect_s thread_rect = r;

				int num_threads = Database_GetNumThreads( db );
				for ( int thread_index = 0; thread_index < num_threads; ++thread_index )
				{
					thread_rect.h = CalcThreadHeight( item, state, thread_index, db );
					{
						if (Rect_Intersects( clip, thread_rect ))
						{
							Thread thread;
							Database_GetThread( db, thread_index, thread );
							thread_flag = 1 << thread_index;
							collapsed = NeHasFlag( state.CollapsedMask, thread_flag );
							color_index = thread_index % view.Skin->ZoneBarList.NumThreadColors;
							text.Length = thread.Name 
										? swprintf_s( text.Text, L"%S", thread.Name )
										: swprintf_s( text.Text, L"<unknown>" /*L"0x%x", data.ThreadId_ts[ thread_index ]*/ );
							if ( DrawNode( ID( __FUNCTION__, item.Type, thread_index ), thread_rect, view.Skin->ZoneBarList.ThreadColors[ color_index ], collapsed, text.Text, text.Length ) )
							{
								if (collapsed)
									view.CollapsedThreads &= ~thread_flag;
								else
									view.CollapsedThreads |= thread_flag;
							}
						}
					}
					thread_rect.y += thread_rect.h;
				}
			}
			break;

		case Item::Locks:
			{
				if (DrawNode( ID( __FUNCTION__, item.Type ), r, 0xff586858, (state.CollapsedMask != 0), L"Locks", -1 ))
					view.CollapsedLocks = ~view.CollapsedLocks;
			}
			break;

		case Item::Cpu:
			{
				if (DrawNode( ID( __FUNCTION__, item.Type ), r, 0xff787890, (state.CollapsedMask != 0), L"Context Switches", -1 ))
					view.CollapsedCpu = ~view.CollapsedCpu;
			}
			break;

		case Item::Profile:
			{
				if (DrawNode( ID( __FUNCTION__, item.Type ), r, 0xff787890, (state.CollapsedMask != 0), L"Profile", -1 ))
					view.CollapsedProfile = ~view.CollapsedProfile;
			}
			break;

		default:
			break;
		}
	}

	//==================================================================================

	static ZoneGroupSetup ToTicks( const ZoneCullSetup& setup, const ZoomInfo& zoom, const Clock& clock )
	{
		const Tick min_frame_ticks = zoom.PixelToTick( setup.MinFrameWidth, clock );
		const Tick min_group_ticks = zoom.PixelToTick( setup.MinGroupWidth, clock );
		const Tick min_zone_ticks = zoom.PixelToTick( setup.MinZoneWidth, clock );
		const Tick threshold_ticks = clock.MsToTick( setup.MaxGroupSpacingMs );
		const ZoneGroupSetup result = 
		{ zoom.PixelToTick( setup.MinFrameWidth, clock )
		, zoom.PixelToTick( setup.MinZoneWidth, clock )
		, zoom.PixelToTick( setup.MinGroupWidth, clock )
		, clock.MsToTick( setup.MaxGroupSpacingMs )
		, setup.MaxZoneLevel
		, setup.Clip
		};
		return result;
	}

	/// draws a single item
	static void DrawItem( Instance& view, const Rect_s& clip, const Rect_s& r, const Item& item, const ItemState& state, Database_t db )
	{
		switch ( item.Type )
		{
		case Item::Summary:
			{
				NePerfScope("summary");
				const summary_bar::State s = { view.CollapsedSummary != 0 };
				const FrameGroupSetup frame_setup = { view.FrameZoom.PixelToTick( view.ZoneCullSettings.MinFrameWidth, Database_GetClock(db) ) };
				summary_bar::Draw( r, view.Skin->SummaryBar, s, view.FrameZoom, view.CulledFrames, db, frame_setup );
			}
			break;

		case Item::Zones:
			{
				NePerfScope("threads");
				const zone_bar_list::State s = { state.CollapsedMask };
				const ZoneGroupSetup zone_setup = ToTicks(view.ZoneCullSettings, view.FrameZoom, Database_GetClock(db) );
				view.HitZone = zone_bar_list::Draw( clip, r, view.Skin->ZoneBarList, s, view.FrameZoom, view.CulledFrames, db, zone_setup ).HitZone;
			}
			break;

		case Item::Locks:
			{
				NePerfScope("locks");
				const lock_bar::State s = { view.CollapsedLocks != 0 };
				const FrameGroupSetup frame_setup = { view.FrameZoom.PixelToTick( view.ZoneCullSettings.MinFrameWidth, Database_GetClock(db) ) };
				lock_bar::Draw( r, view.Skin->LockBar, s, db, view.CulledFrames, view.FrameZoom, frame_setup );
			}
			break;

		case Item::Cpu:
			{
				NePerfScope("context switches");
				const cpu_bar::State s = { view.CollapsedCpu != 0, view.CollapsedThreads };
				const FrameGroupSetup frame_setup = { view.FrameZoom.PixelToTick( view.ZoneCullSettings.MinFrameWidth, Database_GetClock(db) ) };
				cpu_bar::Draw( r, view.Skin->CpuBar, s, db, view.CulledFrames, view.FrameZoom, frame_setup );
			}
			break;

		case Item::Profile:
			{
				NePerfScope("hot spots");
				const profile_bar::State s = { view.CollapsedProfile != 0 };
				const FrameGroupSetup frame_setup = { view.FrameZoom.PixelToTick( view.ZoneCullSettings.MinFrameWidth, Database_GetClock(db) ) };
				profile_bar::Draw( r, view.Skin->ProfileBar, s, db, view.CulledFrames, view.FrameZoom, frame_setup, view.HotSpots, 10 );
			}
			break;

		default:
			break;
		}

		// input
		const Id_t id = ID( __FUNCTION__, item.Type );

		if (Rect_Contains( clip, gui::GetMouse().Pos ))
			SetHot( id );

		if (gui::IsActive( id ))
		{
			const DragInfo_s& drag = gui::GetDrag();
			const Vec2_s delta_pos = gui::GetMouse().Pos - drag.Pos;
			BeginDrag( id );
			
			const Clock clock = Database_GetClock( db );
			const int64_t delta_tick = view.FrameZoom.PixelToTick( delta_pos.x, clock );
			view.FrameScroll -= delta_tick;
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

	//==================================================================================
	static float CalculateFooterHeight( bool collapsed )
	{
		if (collapsed)
			return FOOTER_BAR_HEIGHT;
		return FOOTER_BAR_HEIGHT + FOOTER_CONTENT_HEIGHT;
	}

	static Rect_s CtrlToHeader( const Rect_s& r )
	{
		Rect_s header_rect = r;
		header_rect.h = HEADER_HEIGHT;
		return header_rect;
	}

	static Rect_s CtrlToClient( const Rect_s& r, bool footer_collapsed )
	{
		Rect_s client_rect = r;
		client_rect.y	   += HEADER_HEIGHT;
		client_rect.w  -= SCROLLBAR_WIDTH;
		client_rect.h -= HEADER_HEIGHT;
		client_rect.h -= CalculateFooterHeight( footer_collapsed );
		return client_rect;
	}

	static Rect_s CtrlToScrollV( const Rect_s& r, bool footer_collapsed )
	{
		Rect_s scroll_rect = r;
		scroll_rect.x	   += r.w;
		scroll_rect.x	   -= SCROLLBAR_WIDTH;
		scroll_rect.w   = SCROLLBAR_WIDTH;
		scroll_rect.y	   += HEADER_HEIGHT;
		scroll_rect.h -= HEADER_HEIGHT;
		scroll_rect.h -= CalculateFooterHeight( footer_collapsed );
		return scroll_rect;
	}

	static Rect_s CtrlToScrollH( const Rect_s& r, bool footer_collapsed )
	{
		const float footer_height = CalculateFooterHeight( footer_collapsed );

		Rect_s scroll_rect = r;
		scroll_rect.x	   += NODE_WIDTH;
		scroll_rect.w  -= NODE_WIDTH;
		scroll_rect.w  -= SCROLLBAR_WIDTH;
		scroll_rect.y	   += r.h;
		scroll_rect.y	   -= CalculateFooterHeight( footer_collapsed );
		scroll_rect.y	   += 2.0f;
		scroll_rect.h  = SCROLLBAR_HEIGHT; 
		return scroll_rect;
	}

	static Rect_s CtrlToFooter( const Rect_s& r, bool footer_collapsed )
	{
		const float footer_height = CalculateFooterHeight( footer_collapsed );

		Rect_s footer_rect = r;
		footer_rect.y	   += r.h;
		footer_rect.y	   -= footer_height;
		footer_rect.h	= footer_height;
		return footer_rect;
	}

	static Rect_s PartToNode( const Rect_s& r )
	{
		Rect_s node_rect = r;
		node_rect.w = NODE_WIDTH;
		return node_rect;
	}

	static Rect_s PartToTrack( const Rect_s& r )
	{
		Rect_s track_rect = r;
		track_rect.x	 += NODE_WIDTH;
		track_rect.w -= NODE_WIDTH;
		return track_rect;
	}

	static float CalcTrackWidth( const Rect_s& r )
	{
		return PartToTrack( CtrlToHeader( r ) ).w - SCROLLBAR_WIDTH;
	}

	//==================================================================================

	static void DrawHeader( Instance& view, const Rect_s& r, Database_t db )
	{
		// bar
		{
			Rect_s item_rect = PartToTrack(r);
			PushClip( item_rect );
			{
				const FrameGroupSetup setup = { view.FrameZoom.PixelToTick( view.ZoneCullSettings.MinFrameWidth, Database_GetClock(db) ) };
				const time_bar::Result result = time_bar::Draw( ID(__FUNCTION__), item_rect, view.Skin->TimeBar, view.FrameZoom, view.CulledFrames, db, setup );
				if (result.Action)
					view.FrameScroll = result.TrackTick;
			}
			PopClip();
		}

		// node
		{
			Rect_s node_rect = PartToNode(r);
			{
				node_rect.w -= 1.0f;
				DrawBox( node_rect, Color::Gray, Color::Gray, 0xffc8c8dc );

				const button::Visual_s v = 
				{ { EnsureFont( 9.0f, FontStyle::Default ), Color::Black }
				, 0xffeeeeee, 0xffaaaaaa, Color::White 
				};

				Rect_s button_rect = FaceToIcon( NodeToFace( node_rect ) );
				button_rect.w = 40.0f;
				button_rect.h = 20.0f;
				if ( button::Draw( ID(__FUNCTION__, __LINE__), button_rect, v, L"- all", -1 ) )
				{
					view.CollapsedSummary = 0xffffffff;
					view.CollapsedProfile = 0xffffffff;
					view.CollapsedLocks   = 0xffffffff;
					view.CollapsedCpu	  = 0xffffffff;
					view.CollapsedThreads = 0xffffffff;
				}

				button_rect.x += 50.0f;
				if ( button::Draw( ID(__FUNCTION__, __LINE__), button_rect, v, L"+ all", -1 ) )
				{
					view.CollapsedSummary = 0;
					view.CollapsedProfile = 0;
					view.CollapsedLocks   = 0;
					view.CollapsedCpu	  = 0;
					view.CollapsedThreads = 0;
				}
			}
		}
	}

	static void DrawItems( Instance& view, const Rect_s& r, Database_t db, const Item* items, const ItemState* states, int count )
	{
		// bars
		{
			Rect_s item_rect = PartToTrack(r);
			PushClip( item_rect );

			item_rect.y -= view.VScrollPos;
			for ( int i = 0; i < count; ++i )
			{
				const Item& item = items[i];
				const ItemState& state = states[i];
				item_rect.h = CalcHeight( item, state, db );
				{
					if ( Rect_Intersects( r, item_rect ) )
						DrawItem( view, r, item_rect, item, state, db );
				}
				item_rect.y += item_rect.h;
			}
			PopClip();
		}

		// nodes
		{
			Rect_s node_rect = PartToNode(r);
			PushClip( node_rect );

			node_rect.y -= view.VScrollPos;
			for ( int i = 0; i < count; ++i )
			{
				const Item& item = items[i];
				const ItemState& state = states[i];
				node_rect.h = CalcHeight( item, state, db );
				{
					if ( Rect_Intersects( r, node_rect ) )
						DrawNode( view, r, node_rect, item, state, db );
				}
				node_rect.y += node_rect.h;
			}
			PopClip();
		}
	}

	static void DrawScrollV( Instance& view, const Rect_s& r, Database_t db, const Item* items, const ItemState* states, int count )
	{
		FillRectangle( r, 0xffc8c8dc );

		const float min  = 0.0f;
		const float max  = CalcHeight( items, states, count, db );
		const float page = r.h;
		if ( max <= page )
		{
			view.VScrollPos = 0.0f;
			return;
		}

		Rect_s scroll_rect = r;
		scroll_rect = Rect_Inflate( scroll_rect, -2.0f, -2.0f );

		const float pos = NeClamp( view.VScrollPos, 0.0f, max-page );
		const scrollbar::Visual_s v = { scrollbar::Layout::Vertical };
		const scrollbar::Item item = { pos, page, min, max };
		view.VScrollPos = scrollbar::Draw( ID( __FUNCTION__ ), scroll_rect, v, item ).TrackPos;
	}

	static void DrawScrollH( Instance& view, const Rect_s& r, Database_t db, const Item* items, const ItemState* states, int count )
	{
		const Tick data_range = Database_GetLastEndTick( db ) - Database_GetFirstTick( db );
		const Tick view_range = view.CulledFrames.Time.Duration();
		if ((data_range == 0) || (view_range == 0))
			return;

		const float min	= 0.0f;
		const float max = (float)data_range;
		const float page = (float)view_range;
		const float pos = (float)(view.CulledFrames.Time.Begin - Database_GetFirstTick( db ));
		if ( max <= page )
			return;

		Rect_s scroll_rect = r;
		scroll_rect = Rect_Inflate( scroll_rect, -2.0f, -2.0f );

		const trackbar::Visual_s v = { 16.0f, trackbar::Layout::Horizontal };
		const trackbar::Item item = { pos, min, NeMax( min, (max - page) ) };
		const trackbar::Result result = trackbar::Draw( ID(__FUNCTION__), scroll_rect, v, item );
		if (result.Action)
			view.FrameScroll = Database_GetFirstTick( db ) + (Tick)result.TrackPos;
	}

	static void DrawFooter( Instance& view, const Rect_s& r, Database_t db )
	{
		// footer
		Rect_s header_rect = r;
		FillRectangle( header_rect, 0xffc8c8dc );

		const gui::collapsebar::Item ci = { ID(__FUNCTION__), L"Timeline", view.CollapsedFooter != 0, collapsebar::Alignment::Left };
		if (gui::collapsebar::Draw( header_rect, view.Skin->CollapseBar, ci ).Action)
			view.CollapsedFooter = ~view.CollapsedFooter;

		// content
		if ( view.CollapsedFooter )
			return;

		// timeline
		{
			NePerfScope("timeline");

			// layout 
			Rect_s timeline_rect = r;
			timeline_rect.y		 += FOOTER_BAR_HEIGHT;
			timeline_rect.h -= FOOTER_BAR_HEIGHT;
			timeline_rect = Rect_Inflate( timeline_rect, -2.0f, -2.0f );
			DrawRectangle( timeline_rect, Color::DarkGray );
			timeline_rect = Rect_Inflate( timeline_rect, -2.0f, -2.0f );

			// culling
			const Clock clock = Database_GetClock( db );
			const float total_ms = clock.TickToMs( Database_GetLastEndTick( db ) - Database_GetFirstTick( db ) );
			const Tick scroll_fit = Database_GotoFirstFrame( db );
			const ZoomInfo zoom_fit = view::ZoomTo( timeline_rect.w / total_ms );

			FrameRange cull_fit;
			view::EnumFrameGroups( db, timeline_rect, scroll_fit, zoom_fit, cull_fit );

			const float target_fps = 15.0f; //CalculateMaxFps( cull_fit, db );

			// draw
			FillRectangle( timeline_rect, view.Skin->FrameBar.BkgndColor );
			for ( float i = 1.0f; i >= 0.25f; i -= 0.25f )
				frame_bar::DrawFpsMarker( timeline_rect, view.Skin->FrameBar, target_fps, target_fps * i );
			frame_bar::DrawFrameBars( timeline_rect, view.Skin->FrameBar, zoom_fit, cull_fit, db, target_fps );
			frame_bar::DrawRangeMarker( timeline_rect, view.Skin->FrameBar, cull_fit, view.CulledFrames );
		}
	}

	static void DrawPopup( Instance& view, const Rect_s& r, Database_t db )
	{
		if ( !gui::IsActive( 0 ) )
			return;

		if (view.HitZone.NumZones <= 0)
			return;

		const Clock clock = Database_GetClock( db );
		const bool is_group = (view.HitZone.NumZones > 1);
		NamedLocation location;
		Database_GetLocationByZone( db, view.HitZone.FirstZone, location );

		// format duration
		wchar_t duration[32] = L"";
		FormatDurationW( view.HitZone.Time.Duration(), clock, duration, NeCountOf(duration) );

		// format text
		TextBuffer& text = gui::GetTextBuffer();

		if (is_group)
		{
			text.Length = swprintf_s
				( text.Text
				, L"Duration: %s (%d zones)\n"
				  L"\n"
				  L"Name: %S\n"
				  L"Func: %S\n"
				  L"File: %S (%u)\n"
				, duration
				, view.HitZone.NumZones
				, location.Name
				, location.Location.Function
				, location.Location.File
				, location.Location.Line
				);
		}
		else
		{
			text.Length = swprintf_s
				( text.Text
				, L"Duration: %s\n"
				  L"\n"
				  L"Name: %S\n"
				  L"Func: %S\n"
				  L"File: %S (%u)\n"
				, duration
				, location.Name
				, location.Location.Function
				, location.Location.File
				, location.Location.Line
				);
		}

		// draw
		const popup::Visual_s v = 
		{ { EnsureFont( 12.0f, FontStyle::Default ), Color::Black }
		, is_group ? Color::PaleTurquoise : Color::PaleGoldenrod
		};

		popup::Draw( r, v, text.Text, text.Length );
	}

	//==================================================================================

	static void Cull( Instance& view, const Rect_s& r, Database_t db )
	{
		const uint32_t thread_collapse_mask = ~view.CollapsedThreads;
		EnumFrameGroups( db, r, view.FrameScroll, view.FrameZoom, view.CulledFrames );
	}

	//==================================================================================

	static bool Zoom( Instance& view, const Rect_s& r, Database_t db, const Vec2_s& mouse, float delta )
	{
		// don't zoom if we didn't hit the client area
		Rect_s part_rect = PartToTrack(r);
		if (!Rect_Contains( part_rect , mouse))
			return false;

		// calculate tick
		const Clock clock = Database_GetClock( db );
		const Vec2_s mouse_track_pos = Vec2_s { mouse.x - part_rect.x, mouse.y - part_rect.y };
		const int64_t tick = PixelToTick( clock, view.FrameZoom, view.FrameScroll, mouse_track_pos.x );

		// zoom in/out
		const float e = expf(1.0f);
		const float base = 0.5f * e;
		view.FrameZoom = ZoomScale( view.FrameZoom, base, delta /*, 4.0f, 100000.0f*/ );

		// restore tick at client coord
		const int64_t new_tick = PixelToTick( clock, view.FrameZoom, view.FrameScroll, mouse_track_pos.x );
		const int64_t offset = tick-new_tick;
		view.FrameScroll += offset;
		return true;
	}

	//==================================================================================

	static void Reset( zone_bar::Result& zone_hit )
	{
		NeZero(zone_hit);
	}

	static void Reset( ZoneCullSetup& setup )
	{
		setup.MinFrameWidth = 10.0f;
		setup.MinZoneWidth = 3.0f;
		setup.MinGroupWidth = 0.5f;
		setup.MaxGroupSpacingMs = 0.1f;
		setup.MaxZoneLevel = 32;
		setup.Clip = true;
	}

	//==================================================================================
	// public interface
	void Initialize( Instance& view, Skin* skin )
	{
		NeZero(view);
		NeZero(view.FrameScroll);
		NeZero(view.VScrollPos);
		Reset(view.HitZone);
		Reset(view.ZoneCullSettings);

		view.FrameZoom = ZoomTo( 25.0f / 4 );

		view.CollapsedSummary = 0;
		view.CollapsedProfile = 0;
		view.CollapsedLocks   = 0;
		view.CollapsedThreads = 0;
		view.CollapsedFooter  = 0;

		view.Skin = skin;
	}

	void Shutdown( Instance& view )
	{
		view.HotSpots.Data.Clear();
	}

	//==================================================================================

	void Draw( Instance& view, const Rect_s& r, Database_t db )
	{
		const cpu_bar::Visual_s&		 cv = view.Skin->CpuBar;
		const lock_bar::Visual_s&		 lv = view.Skin->LockBar;
		const profile_bar::Visual_s&	 pv = view.Skin->ProfileBar;
		const zone_bar_list::Visual_s& zv = view.Skin->ZoneBarList;

		static const float SUMMARY_HEIGHT = 105.0f;

		// setup items
		const Item items [] =
		{ { Item::Summary, zv.CollapsedHeight, SUMMARY_HEIGHT	, 0.0f				, 0 }
		, { Item::Zones  , zv.CollapsedHeight, zv.ExpandedHeight, zv.ExpandedSpacing, view.ZoneCullSettings.MaxZoneLevel }
		, { Item::Locks  , zv.CollapsedHeight, lv.LockHeight	, lv.LockSpacing	, 0 }
		/*
		, { Item::Cpu    , zv.CollapsedHeight, cv.CpuHeight		, cv.CpuSpacing		, 0 }
		, { Item::Profile, zv.CollapsedHeight, pv.ItemHeight	, pv.ItemSpacing	, 10 }
		*/
		};

		const ItemState states [] =
		{ { view.CollapsedSummary }
		, { view.CollapsedThreads }
		, { view.CollapsedLocks   }
		/*
		, { view.CollapsedCpu     }
		, { view.CollapsedProfile }
		*/
		};

		NeStaticAssert( NeCountOf(items) == NeCountOf(states) );

		// init
		view.ViewRect = r;
		Reset(view.HitZone);

		// layout
		const bool footer_collapsed = (view.CollapsedFooter != 0);
		const Rect_s header_rect   = CtrlToHeader ( r );
		const Rect_s footer_rect   = CtrlToFooter ( r, footer_collapsed );
		const Rect_s client_rect   = CtrlToClient ( r, footer_collapsed );
		const Rect_s scroll_rect_v = CtrlToScrollV( r, footer_collapsed );
		const Rect_s scroll_rect_h = CtrlToScrollH( r, footer_collapsed );
		view.FrameScroll = ClampScrollTick( db, view.FrameZoom, PartToTrack(client_rect).w, view.FrameScroll );

		// draw
		Cull	   ( view, PartToTrack( header_rect ), db );
		DrawHeader ( view, header_rect  , db );
		DrawItems  ( view, client_rect  , db, items, states, NeCountOf(items) );
		DrawScrollV( view, scroll_rect_v, db, items, states, NeCountOf(items) );
		DrawFooter ( view, footer_rect  , db );
		DrawScrollH( view, scroll_rect_h, db, items, states, NeCountOf(items) );

		// input
		{
			// mouse
			{
				const Mouse_s& mouse = gui::GetMouse();
				if (mouse.Wheel)
					Zoom( view, r, db, mouse.Pos, (float)mouse.Wheel );

				if (mouse.Button[0].NumClicks == 2)
				{
					if (view.CollapsedFooter == 0)
					{
						Rect_s timeline_rect = footer_rect;
						timeline_rect.y		 += FOOTER_BAR_HEIGHT;
						timeline_rect.h -= FOOTER_BAR_HEIGHT;
						timeline_rect = Rect_Inflate( timeline_rect, -2.0f, -2.0f );
						timeline_rect = Rect_Inflate( timeline_rect, -2.0f, -2.0f );

						const Clock clock = Database_GetClock( db );
						const float total_ms = clock.TickToMs( Database_GetLastEndTick( db ) - Database_GetFirstTick( db ) );
						const Tick scroll_fit = Database_GotoFirstFrame( db );
						const ZoomInfo zoom_fit   = view::ZoomTo( timeline_rect.w / total_ms );

						const int frame_index = frame_bar::HitTestFrame( timeline_rect, zoom_fit, scroll_fit, db, mouse.Pos );
						if ( frame_index >= 0 )
							FitToFrameIndex( view, db, frame_index );
					}
				}
			}

			// keyboard
			{
				const Keyboard_s& keyboard = gui::GetKeyboard();

				if (keyboard.Key[ VK_HOME ].AutoRepeat)
				{
					GotoFirstFrame( view, db );
				}
				else if (keyboard.Key[ VK_END ].AutoRepeat)
				{
					if ( keyboard.Key[ VK_CONTROL ].AutoRepeat )
						GotoEnd( view, db );
					else
						GotoLastFrame( view, db );
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
					Scroll( view, db, unit, amount );
				}
			}
		}
	}

	void DrawOverlay( Instance& view, const Rect_s& r, Database_t db )
	{
		const bool footer_collapsed = (view.CollapsedFooter != 0);
		const Rect_s client_rect = CtrlToClient ( r, footer_collapsed );
		if ( Rect_Contains( client_rect, gui::GetMouse().Pos ) )
			DrawPopup( view, r, db );
	}

	//==================================================================================

	void GotoFrameNumber( Instance& view, Database_t db, uint32_t i )
	{ view.FrameScroll = Database_GotoFrameNumber( db, i ); }
		
	void GotoFrameIndex( Instance& view, Database_t db, int i )
	{ view.FrameScroll = Database_GotoFrameIndex( db, i ); }

	void GotoFirstFrame( Instance& view, Database_t db )
	{ view.FrameScroll = Database_GotoFirstFrame( db ); }

	void GotoLastFrame( Instance& view, Database_t db )
	{ view.FrameScroll = Database_GotoLastFrame( db ); }

	void GotoBegin( Instance& view, Database_t db )
	{ view.FrameScroll = Database_GotoBegin( db ); }

	void GotoEnd( Instance& view, Database_t db )
	{ 
		const Tick duration = view.FrameZoom.PixelToTick( CalcTrackWidth( view.ViewRect ), Database_GetClock(db) );
		view.FrameScroll = Database_GotoEnd( db, duration );
	}

	void FitToFrameIndex( Instance& view, Database_t db, int i )
	{
		if (Database_GetNumFrames( db ) == 0)
			return;

		const Clock clock = Database_GetClock( db );
		const int frame_index = NeClamp( i, 0, Database_GetNumFrames( db )-1 );
		const Frame& frame = Database_GetFrames( db )[ frame_index ];
		const float frame_ms = clock.TickToMs( frame.Time.Duration() );
		const float track_width = CalcTrackWidth( view.ViewRect );

		GotoFrameIndex( view, db, frame_index );
		view.FrameZoom = ZoomTo( track_width / frame_ms );
	}

	void Scroll( Instance& view, Database_t db, ScrollUnit::Enum unit, int amount )
	{ 
		const Clock clock = Database_GetClock( db );
		view.FrameScroll = view::Scroll( CalcTrackWidth( view.ViewRect ), view.FrameZoom, clock, unit, view.FrameScroll, amount ); 
	}

} } } }
