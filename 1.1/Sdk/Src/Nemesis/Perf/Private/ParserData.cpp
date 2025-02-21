//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "ParserData.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	static Tick FrameTable_FirstTick( const Array<viz::Frame>& frames ) { return frames.Count ? frames[0].Time.Begin : 0; }
	static Tick FrameTable_LastEndTick( const Array<viz::Frame>& frames ) { return frames.Count ? frames[frames.Count-1].Time.End : 0; }
	static Tick FrameTable_LastBeginTick( const Array<viz::Frame>& frames ) { return frames.Count ? frames[frames.Count-1].Time.Begin : 0; }

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct Zone_s
	{
		TickInterval Time;
		uint32_t Location;
		uint32_t EnterScope;
		uint32_t LeaveScope;
		uint8_t Thread;
		uint8_t Level;
		uint8_t Type: 2;
		uint8_t Cpu0: 6;
		uint8_t Cpu1: 6;
		uint8_t Flags: 2;
	};

	struct ZoneStack_s
	{
		enum { MAX_NUM_ITEMS = 64 };

		Zone_s Item[ MAX_NUM_ITEMS ];
		uint8_t NumItems;
		uint8_t _pad_[3];

		void Push( const viz::ScopeEvent& ev, int index )
		{
			NeAssert(ev.Enter == 1);
			NeAssert(NumItems < MAX_NUM_ITEMS);
			Zone_s& zone = Item[ NumItems ];
			zone.Time.Begin = ev.Time;
			zone.Time.End = ev.Time;
			zone.Location = ev.Location;
			zone.EnterScope = index;
			zone.LeaveScope = index;
			zone.Thread = ev.Thread;
			zone.Level = NumItems;
			zone.Cpu0 = ev.Cpu;
			zone.Cpu1 = ev.Cpu;
			zone.Type = ev.Type;
			zone.Flags = 0;
			++NumItems;
		}

		int Pop( const viz::ScopeEvent& ev, int index )
		{
			NeAssert(ev.Enter == 0);
			if (!NumItems)
				return -1;
			--NumItems;
			Zone_s& zone = Item[NumItems];
			NeAssert(zone.Thread == ev.Thread);
			zone.Time.End = ev.Time;
			zone.LeaveScope = index;
			zone.Cpu1 = ev.Cpu;
			return NumItems;
		}

		int Parse( const viz::ScopeEvent& ev, int index )
		{
			if (ev.Enter)
			{
				Push( ev, index );
				return -1;
			}
			return Pop( ev, index );
		}
	};

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	using namespace viz;

	namespace
	{
		// @note: fails when there are gaps in between frames
		struct FrameComparer
		{
			static int Compare( const Frame& frame, Tick tick )
			{
				if ( tick < frame.Time.Begin )
					return 1;
				if ( tick >= frame.Time.End )
					return -1;
				return 0;
			}
		};
	}

	/// Calculates the tick offset for a given frame index
	Tick ParsedData_GotoFrameIndex( const ParsedData_s& data, int frame_index )
	{
		if ( (frame_index >= 0) && (frame_index < data.Frames.Count) )
			return data.Frames.Data[ frame_index ].Time.Begin;

		if ( data.Frames.Count )
			return data.Frames[0].Time.Begin;

		return 0;
	}

	/// Calculates the tick offset for a given frame number.
	Tick ParsedData_GotoFrameNumber( const ParsedData_s& data, uint32_t frame_number )
	{
		return ParsedData_GotoFrameIndex( data, frame_number - data.FirstFrameNumber() );
	}
	
	/// Calculates the tick offset for the first frame.
	Tick ParsedData_GotoFirstFrame( const ParsedData_s& data )
	{
		return ParsedData_GotoFrameIndex( data, 0 );
	}

	/// Calculates the tick offset for the last frame.
	Tick ParsedData_GotoLastFrame( const ParsedData_s& data )
	{
		return ParsedData_GotoFrameIndex( data, data.Frames.Count-1 );
	}

	/// Calculates the minimal tick offset.
	Tick ParsedData_GotoBegin( const ParsedData_s& data )
	{
		return ParsedData_GotoFirstFrame( data );
	}

	/// Calculates the tick offset required to right-align the end of the 
	/// last frame with the view's client rectangle. 
	Tick ParsedData_GotoEnd( const ParsedData_s& data, Tick ticks_in_view )
	{
		const Tick last_end_tick = FrameTable_LastEndTick( data.Frames);
		const Tick right_aligned_max = last_end_tick - ticks_in_view;
		return right_aligned_max;
	}

	/// Find the earliest frame containing the given tick.
	static int BinaryTickToFrame( const ParsedData_s& data, Tick tick )
	{
		const int pos = Array_BinaryFind<FrameComparer>(data.Frames, tick);
		return (pos >= 0) ? pos : -1;
	}

	/// Finds the index of the first frame whose time interval contains the given tick.
	/// If the tick preceeds the start of the first frame or exceeds the end of the last
	/// frame the function returns zero or the last frame's index, respectively.
	int ParsedData_TickToFrameIndex( const ParsedData_s& data, Tick tick )
	{
		if (data.Frames.Count == 0)
			return 0;

		// before first frame?
		if (tick <= data.Frames.Data[0].Time.Begin)
			return 0;

		// past last frame?
		const int last_frame_index = data.Frames.Count-1;
		if (tick >= data.Frames.Data[last_frame_index].Time.End)
			return last_frame_index;

		// find the (left-most) frame
		const int frame_index = BinaryTickToFrame( data, tick );
		if (frame_index >= 0)
			return frame_index;

		NeAssertOut( frame_index >= 0 /*data.Frames.Data[frame_index].Time.Contains( tick )*/, "Binary search failed because there's no frame at the given tick (it's a gap)." );
		return 0; //LinearTickToFrame( data, tick );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	using namespace viz;

	struct FrameBatch_s
	{
		IndexRange Frames;
	};

	static void FlushBatch( const FrameBatch_s& batch, EnumFrameGroupsFunc func, void* context )
	{
		if (!batch.Frames.Count)
			return;

		const FrameGroup group = 
		{ batch.Frames 
		};
		func( context, group );
	}

	static void EnumFrameGroupsLod( const ParsedData_s& data, const FrameRange& cull, const FrameGroupSetup& setup, EnumFrameGroupsFunc func, void* context )
	{
		if (!cull.Frames.Count)
			return;

		FrameBatch_s batch = {};
		batch.Frames.First = cull.Frames.Begin();

		const int frame_end = cull.Frames.End();
		for ( int frame_index = cull.Frames.Begin(); frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = data.Frames.Data[ frame_index ];

			// accumulate
			if (frame.Time.Duration() < setup.MinFrameTicks)
			{
				++batch.Frames.Count;
				continue;
			}

			// flush
			FlushBatch( batch, func, context );

			// reset
			batch.Frames.First = frame_index;
			batch.Frames.Count = 1;
		}

		// flush last batch
		FlushBatch( batch, func, context );
	}

	/// Determines the visible set of frames and their local placement.
	void ParsedData_EnumFrameGroups( const ParsedData_s& data, const FrameRange& cull, const FrameGroupSetup& setup, EnumFrameGroupsFunc func, void* context )
	{
		//NePerfScope("cull frames");
		EnumFrameGroupsLod( data, cull, setup, func, context );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	using namespace viz;

	struct ZoneBatch_s
	{
		Zone_s Zone;
		int Count;
	};

	struct ZoneCullInfo_s
	{
		TickInterval View;
		Tick LastFrameEndTick;
		Tick MinGroupTicks;
		Tick MinZoneTicks;
		Tick ThresholdTicks;
		int MaxZoneLevel;
	};

	static void FlushBatch( const ZoneBatch_s& batch, const ZoneCullInfo_s& cull, EnumZoneGroupsFunc func, void* context )
	{
		// handle empty batch
		if (batch.Count == 0)
			return;

		//NeAssert( batch.Zone.Thread	>= 0 );
		NeAssert( batch.Zone.Time.Begin >= 0 );
		NeAssert( batch.Zone.Time.End   >= 0 );
		NeAssert( batch.Zone.Time.End   >= batch.Zone.Time.Begin );

		// cull group
		if ( batch.Zone.Time.Duration() < cull.MinGroupTicks )
			return;

		if ( !batch.Zone.Time.Intersects( cull.View ) )
			return;

		// output the item
		const ZoneGroup item = 
		{ batch.Zone.Time
		, batch.Zone.Location
		, batch.Zone.Thread
		, batch.Zone.Level
		, batch.Zone.Type
		, batch.Zone.Cpu0
		, batch.Zone.Cpu1
		, batch.Zone.Flags
		, batch.Zone.EnterScope
		, (uint32_t)batch.Count
		};
		func( context, item );
	}

	static void AddZoneToBatch( ZoneBatch_s& batch, const Zone_s& zone, const ParsedData_s& data, const ZoneCullInfo_s& cull, EnumZoneGroupsFunc func, void* context )
	{
		// depth-cull zone
		if ( zone.Level > cull.MaxZoneLevel )
			return;

		// group zone
		const Tick zone_duration = zone.Time.Duration();
		const bool is_open = (batch.Count > 0);
		const bool is_tiny = (zone_duration < cull.MinZoneTicks);
		const bool is_same = (batch.Zone.Location == zone.Location) && (batch.Zone.Level == zone.Level);

		// accumulate zones
		if ( is_tiny && is_same && is_open )
		{
			const Tick gap_ticks = zone.Time.Begin - batch.Zone.Time.End;
			const bool is_near = gap_ticks < cull.ThresholdTicks;
			if (is_near)
			{
				if (zone.Time.Begin < batch.Zone.Time.Begin) batch.Zone.Time.Begin = zone.Time.Begin;
				if (zone.Time.End   > batch.Zone.Time.End)	 batch.Zone.Time.End   = zone.Time.End;
				++batch.Count;
				return;
			}
		}

		// flush current batch
		FlushBatch( batch, cull, func, context );

		// begin batch
		batch.Zone = zone;
		batch.Count = 1;
	}

	static void FlushOpenZones( ZoneStack_s& stack, ZoneBatch_s& batch, const ParsedData_s& data, const ZoneCullInfo_s& cull, EnumZoneGroupsFunc func, void* context )
	{
		while (stack.NumItems)
		{
			const int index = --stack.NumItems;
			Zone_s& zone = stack.Item[index];
			zone.Time.End = cull.LastFrameEndTick;
			AddZoneToBatch( batch, zone, data, cull, func, context );
		}
	}

	static void EnumZoneGroupsLod( const ParsedData_s& data, const FrameRange& cull, const ZoneGroupSetup& setup, int thread_index, EnumZoneGroupsFunc func, void* context )
	{
		if (!cull.Frames.Count)
			return;

		// setup constants
		const int frame_end = NeMin(cull.Frames.End()+2, data.Frames.Count);
		const Tick last_end_tick = data.Frames.Data[ frame_end-1 ].Time.End;

		const ZoneCullInfo_s zone_cull = 
		{ cull.Time
		, last_end_tick
		, setup.MinGroupTicks
		, setup.MinZoneTicks
		, setup.MaxGroupSpacingTicks
		, setup.MaxZoneLevel
		};

		// init batch
		const ZoneBatch_s empty_batch = {};
		ZoneBatch_s batch = empty_batch;
		ZoneStack_s stack = {};

		// cull frames
		for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
		{
			// cull frame
			const Frame& frame = data.Frames.Data[ frame_index ];
			if (frame.Time.Duration() < setup.MinFrameTicks)
			{
				stack.NumItems = 0;
				continue;
			}

			// cull zones
			{
				const int end = frame.FirstScopeEvent + frame.NumScopeEvents;
				for ( int i = frame.FirstScopeEvent; i < end; ++i )
				{
					const ScopeEvent& ev = data.Scopes.Data[ i ];
					if (ev.Thread != thread_index)
						continue;

					const int index = stack.Parse( ev, i );
					if (index < 0)
						continue;

					const Zone_s& zone = stack.Item[index];
					AddZoneToBatch( batch, zone, data, zone_cull, func, context );
				}
			}
		}

		//FlushOpenZones( stack, batch, data, zone_cull, func, context );
		FlushBatch( batch, zone_cull, func, context );
	}

	/// Enumerates the visible set of zones and their local placement.
	void ParsedData_EnumZoneGroups( const ParsedData_s& data, const FrameRange& cull, const ZoneGroupSetup& setup, int thread_index, EnumZoneGroupsFunc func, void* context )
	{
		//NePerfScope("cull zones");
		if (!data.Frames.Count)
			return;
		EnumZoneGroupsLod( data, cull, setup, thread_index, func, context );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	using namespace viz;

	struct CpuBatch_s
	{
		TickInterval Time;
		uint32_t NumEvents;
		uint8_t Thread;
		uint8_t Level;
		uint8_t Cpu;
		uint8_t Cpu2;
		uint8_t Initialized;
		uint8_t _pad_[3];
	};

	static void FlushBatch( const CpuBatch_s& batch, EnumCpuGroupsFunc func, void* context )
	{
		if (!batch.NumEvents)
			return;

		const CpuGroup group = 
		{ batch.Time
		, batch.Thread
		, batch.Cpu
		, batch.Cpu2
		};
		func( context, group );
	}

	static void ParseEvent( CpuBatch_s& batch, const Frame& frame, const ScopeEvent& ev, EnumCpuGroupsFunc func, void* context )
	{
		if (!batch.Initialized)
		{
			if (!ev.Enter)
				return;
			batch.Time.Begin = ev.Time;
			batch.Time.End = ev.Time;
			batch.NumEvents = 1;
			batch.Thread = ev.Thread;
			batch.Cpu = ev.Cpu;
			batch.Cpu2 = ev.Cpu;
			batch.Initialized = true;
			batch.Level = 1;
			return;
		}

		NeAssert(batch.Thread == ev.Thread);
		if (batch.Cpu != ev.Cpu)
		{
			// flush the 'old cpu' batch
			FlushBatch( batch, func, context );

			// fill in and immediately flush the
			// 'switch between cpus' batch
			batch.Time.Begin = batch.Time.End;
			batch.Time.End = ev.Time;
			batch.Cpu2 = ev.Cpu;
			batch.NumEvents = 1;
			FlushBatch( batch, func, context );

			// begin the 'new cpu' batch
			batch.Cpu = ev.Cpu;
			batch.Time.Begin = ev.Time;
			batch.NumEvents = ev.Enter ? 1 : 0;
			batch.Level += (ev.Enter ? 1 : -1);
			return;
		}

		batch.Time.End = ev.Time;
		batch.Level += (ev.Enter ? 1 : -1);
		++batch.NumEvents;

		if (ev.Enter)
		{
			if (1 == batch.NumEvents)
				batch.Time.Begin = ev.Time;
			return;
		}

		if (batch.Level > 0)
			return;

		FlushBatch( batch, func, context );
		batch.NumEvents = 0;
	}

	void ParsedData_EnumCpuGroups( const ParsedData_s& data, const FrameRange& cull, const CpuGroupSetup& setup, int thread_index, EnumCpuGroupsFunc func, void* context )
	{
		CpuBatch_s batch;
		NeZero( batch );

		const int frame_end = cull.Frames.End();
		for ( int frame_index = cull.Frames.First; frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = data.Frames.Data[ frame_index ];

			const int event_end = frame.FirstScopeEvent + frame.NumScopeEvents;
			for ( int event_index = frame.FirstScopeEvent; event_index < event_end; ++event_index )
			{
				const ScopeEvent& ev = data.Scopes.Data[ event_index ];
				if (ev.Thread != thread_index)
					continue;
				ParseEvent( batch, frame, ev, func, context );
			}
		}

		batch.Time.End = cull.Time.End;
		FlushBatch( batch, func, context );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	using namespace viz;

	/// Enumerates lock events in the given range of frames.
	/// If the given lock index is < 0, the function enumerates all lock events, otherwise only the lock events for the given lock index.
	void ParsedData_EnumLockEvents( const ParsedData_s& data, int lock_index, int first_frame, int num_frames, EnumLockEventFunc func, void* context )
	{
		const int frame_end = first_frame + num_frames;
		for ( int frame_index = first_frame; frame_index < frame_end; ++frame_index )
		{
			const Frame& frame = data.Frames.Data[frame_index];
			const int event_end = frame.FirstLockEvent + frame.NumLockEvents;
			for ( int event_index = frame.FirstLockEvent; event_index < event_end; ++event_index )
			{
				const LockEvent& ev = data.LockEvents.Data[event_index];
				if ((lock_index < 0) || (ev.Lock == lock_index))
					func( context, ev, event_index );
			}
		}
	}

	/// Builds hots spots for a given range for frames.
	void ParsedData_BuildHotSpots( const ParsedData_s& data, const HotSpotRange& range, HotSpotGroup& group )
	{
		/*
		// reset data
		group.Data.Resize(data.Locations.Count);
		ZeroMemory( group.Data.Data, Array_GetCountSize( group.Data ) );

		// build data
		for ( int i = 0; i < range.NumFrames; ++i )
		{
			// gather hot spots
			const int frame_index = i + range.FirstFrame;
			const Frame& frame = data.Frames.Data[ frame_index ];
			for ( uint32_t j = 0; j < frame.NumZones; ++j )
			{
				const int zone_index = j + frame.FirstZone;
				const ScopeEvent& zone = data.Scopes.Data.Data[ zone_index ];
				switch (zone.Type)
				{
				default:
					break;
				case ScopeType::Idle:
					continue;
				}
				const Tick duration = zone.Time.Duration();
				HotSpot& spot = group.Data.Data[ zone.Location ];
				spot.Location = zone.Location;
				spot.Duration += duration;
				++spot.NumSamples;
			}
		}

		// sort hot-spots
		struct Sorter
		{
			static int SortHotSpots( void* , const void* l, const void* r )
			{
				const HotSpot& lhs = *static_cast<const HotSpot*>(l);
				const HotSpot& rhs = *static_cast<const HotSpot*>(r);
				const Tick diff = (rhs.Duration - lhs.Duration);
				if (diff < 0)
					return -1;
				if (diff > 0)
					return 1;
				return 0;
			}
		};

		qsort_s( group.Data.Data, group.Data.Count, sizeof(group.Data.Data[0]), Sorter::SortHotSpots, nullptr );
		*/
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	/// Initializes data member for first use.
	void ParsedData_Initialize( ParsedData_s& data, Allocator_t alloc )
	{
		NeZero( data );
		data.Frames.Alloc			= alloc;
		data.Scopes.Alloc			= alloc;
		data.LockEvents.Alloc		= alloc;
		data.CounterValues.Alloc	= alloc;
		data.Locks.Alloc			= alloc;
		data.Counters.Alloc			= alloc;
		data.CounterGroups.Alloc	= alloc;
		data.Locations.Alloc		= alloc;
		data.Locations.Alloc		= alloc;
		data.LogItems.Alloc			= alloc;
	}

	/// Frees dynamic memory allocated by the data set.
	void ParsedData_Shutdown( ParsedData_s& data )
	{
		data.Frames.Clear();
		data.Scopes.Clear();
		data.LockEvents.Clear();
		data.CounterValues.Clear();
		data.Locks.Clear();
		data.Counters.Clear();
		data.CounterGroups.Clear();
		data.Locations.Clear();
		data.LogItems.Clear();
	}

	/// Resets data members without freeing allocated memory.
	void ParsedData_Reset( ParsedData_s& data )
	{
		NeZero( data.Threads );
		data.MaxFrameDuration = 0;
		data.LastFrameNumber  = 0;
		NeZero(data.Clock);
		data.Frames.Reset();
		data.Scopes.Reset();
		data.LockEvents.Reset();
		data.CounterValues.Reset();
		data.LogItems.Reset();
		data.Locks.Reset();
		data.Counters.Reset();
		data.CounterGroups.Reset();
		data.Locations.Reset();
	}

	void ParsedData_ResetFrames( ParsedData_s& data )
	{
		data.Frames.Reset();
		data.Scopes.Reset();
		data.LockEvents.Reset();
		data.CounterValues.Reset();
		data.LogItems.Reset();
		data.MaxFrameDuration = 0;
		data.LastFrameNumber = 0;
	}

	/// Removes frames from the beginning of the data set until there's enough
	/// memory available to append the given number of data bytes.
	void ParsedData_MakeRoom( ParsedData_s& data, const Limit_s& limit, uint32_t num_frames, size_t num_bytes )
	{
		//NePerfScope("make room");
		if (	((limit.MaxNumFrames <= 0) || ((data.Frames.Count + num_frames) < limit.MaxNumFrames))
			 && ((limit.MaxNumBytes  <= 0) || ((data.TotalSize()  + num_bytes ) < limit.MaxNumBytes )) )
		{
			return;
		}

		ParsedData_ResetFrames( data );
	}

	void ParsedData_Append( ParsedData_s& dst, const ParsedData_s& src )
	{
		// no frames yet?
		if (src.Frames.Count == 0)
			return;

		// merge threads
		dst.Threads = src.Threads;

		// merge cpus
		dst.NumCpus = src.NumCpus;

		// update lock names (they might have changed and we can't - yet? - figure out if they did)
		for ( int i = 0; i < dst.Locks.Count; ++i )
			dst.Locks[i] = src.Locks[i];

		// merge globals
		const int num_new_locations = src.Locations.Count - dst.Locations.Count;
		dst.Locations.Append( src.Locations.Data + dst.Locations.Count, num_new_locations );

		const int num_new_locks = src.Locks.Count - dst.Locks.Count;
		dst.Locks.Append( src.Locks.Data + dst.Locks.Count, num_new_locks );

		const int num_new_counters = src.Counters.Count - dst.Counters.Count;
		dst.Counters.Append( src.Counters.Data + dst.Counters.Count, num_new_counters );

		const int num_new_counter_groups = src.CounterGroups.Count - dst.CounterGroups.Count;
		dst.CounterGroups.Append( src.CounterGroups.Data + dst.CounterGroups.Count, num_new_counter_groups );

		for ( int i = 0; i < dst.Counters.Count; ++i )
			dst.Counters[i] = src.Counters[i];

		// src items
		const int first_frame = dst.Frames.Count;
		const int first_scope_event = dst.Scopes.Count;
		const int first_lock_event = dst.LockEvents.Count;
		const int first_counter_value = dst.CounterValues.Count;
		dst.Frames.Append( src.Frames );
		dst.Scopes.Append( src.Scopes );
		dst.LockEvents.Append( src.LockEvents );
		dst.CounterValues.Append( src.CounterValues );
		dst.LogItems.Append( src.LogItems );

		// adjust frame ranges
		for ( int i = first_frame; i < dst.Frames.Count; ++i )
		{
			dst.Frames.Data[i].FirstScopeEvent += first_scope_event;
			dst.Frames.Data[i].FirstLockEvent += first_lock_event;
			dst.Frames.Data[i].FirstCounterValue += first_counter_value;
		}

		// update totals
		dst.Clock = src.Clock;
		dst.MaxFrameDuration = NeMax( dst.MaxFrameDuration, src.MaxFrameDuration );
		dst.LastFrameNumber = src.LastFrameNumber;
	}

} }
