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
#include <Nemesis/Perf/Server.h>
#include <Nemesis/Perf/Visualizer.h>

//======================================================================================
#include "Types.h"
#include "Constants.h"

//======================================================================================
#include <Nemesis/Core/Map.h>
#include <Nemesis/Core/Stack.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ParsedThreadTable_s
	{
		uint32_t	Id  [ MAX_NUM_THREADS ];
		viz::Thread	Item[ MAX_NUM_THREADS ];
		uint8_t		Count;
		uint8_t		_pad_[7];
	};

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ParsedData_s
	{
		Clock		Clock;
		uint8_t		NumCpus;
		uint8_t		_padding_[3];
		int64_t		MaxFrameDuration;
		uint32_t	LastFrameNumber;

		ParsedThreadTable_s			Threads;
		Array<viz::Frame>			Frames;
		Array<viz::ScopeEvent>		Scopes;
		Array<viz::LockEvent>		LockEvents;
		Array<viz::CounterValue>	CounterValues;
		Array<viz::Lock>			Locks;
		Array<viz::Counter>			Counters;
		Array<viz::CounterGroup>	CounterGroups;
		Array<NamedLocation>		Locations;
		Array<viz::LogItem>			LogItems;

		uint32_t FirstFrameNumber()	const { return LastFrameNumber-Frames.Count+1; }

		uint32_t TotalSize() const
		{ return (uint32_t)
			( sizeof(*this)
			+ Array_GetCountSize(Frames)
			+ Array_GetCountSize(Scopes)
			+ Array_GetCountSize(LockEvents)
			+ Array_GetCountSize(CounterValues)
			+ Array_GetCountSize(Locks)
			+ Array_GetCountSize(Counters)
			+ Array_GetCountSize(CounterGroups)
			+ Array_GetCountSize(Locations)
			);
		}
	};

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	Tick ParsedData_GotoFrameIndex( const ParsedData_s& data, int frame_index );
	Tick ParsedData_GotoFrameNumber( const ParsedData_s& data, uint32_t frame_number );
	Tick ParsedData_GotoFirstFrame( const ParsedData_s& data );
	Tick ParsedData_GotoLastFrame( const ParsedData_s& data );
	Tick ParsedData_GotoBegin( const ParsedData_s& data );
	Tick ParsedData_GotoEnd( const ParsedData_s& data, Tick ticks_in_view );

	int ParsedData_TickToFrameIndex( const ParsedData_s& data, Tick tick );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	void ParsedData_EnumFrameGroups( const ParsedData_s& data, const viz::FrameRange& cull, const viz::FrameGroupSetup& setup, EnumFrameGroupsFunc func, void* context );
	void ParsedData_EnumZoneGroups( const ParsedData_s& data, const viz::FrameRange& cull, const viz::ZoneGroupSetup& setup, int thread_index, EnumZoneGroupsFunc func, void* context );
	void ParsedData_EnumCpuGroups( const ParsedData_s& data, const viz::FrameRange& cull, const viz::CpuGroupSetup& setup, int thread_index, EnumCpuGroupsFunc func, void* context );
	void ParsedData_EnumLockEvents( const ParsedData_s& data, int lock_index, int first_frame, int num_frames, EnumLockEventFunc func, void* context );
	void ParsedData_BuildHotSpots( const ParsedData_s& data, const viz::HotSpotRange& range, viz::HotSpotGroup& group );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct Limit_s
	{
		uint32_t MaxNumBytes;
		uint32_t MaxNumFrames;
	};

	void ParsedData_Initialize	( ParsedData_s& data, Allocator_t alloc );
	void ParsedData_Shutdown	( ParsedData_s& data );
	void ParsedData_Reset		( ParsedData_s& data );
	void ParsedData_ResetFrames	( ParsedData_s& data );
	void ParsedData_MakeRoom	( ParsedData_s& data, const Limit_s& limit, uint32_t num_frames, size_t num_bytes );
	void ParsedData_Append		( ParsedData_s& data, const ParsedData_s& src );
	
} }
