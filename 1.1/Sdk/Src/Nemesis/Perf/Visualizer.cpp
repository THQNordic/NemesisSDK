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
#include "Private/Parser.h"
#include "Private/Receiver.h"
#include "Private/Database.h"

//======================================================================================
namespace nemesis { namespace profiling 
{
	static const NamedLocation INVALID_LOCATION = NamedLocation( "<Unknown>", "<Unknown>", "<Unknown>", 0 );

} }

//======================================================================================
namespace nemesis { namespace profiling 
{
	Receiver_t Receiver_Create( Allocator_t alloc )
	{ 
		Receiver_t rcv = Mem_Calloc<Receiver_s>( alloc );
		Receiver_Initialize( rcv, alloc );
		return rcv;
	}

	void Receiver_Destroy( Receiver_t receiver )
	{ 
		if (!receiver)
			return;
		Allocator_t alloc = receiver->Alloc;
		Receiver_Shutdown( receiver );
		Mem_Free( alloc, receiver );
	}

	system::Socket_t Receiver_GetSocket( Receiver_t receiver )
	{ return receiver->Socket; }

} }

//======================================================================================
namespace nemesis { namespace profiling 
{
	Parser_t Parser_Create( Allocator_t alloc, const ParserSetup& setup )
	{ 
		if (!setup.Database)
			return nullptr;
		Parser_t parser = Mem_Calloc<Parser_s>( alloc );
		Parser_Initialize( parser, alloc, setup );
		return parser; 
	}

	void Parser_Destroy( Parser_t parser )
	{ 
		if (!parser)
			return;
		Allocator_t alloc = parser->Alloc;
		Parser_Shutdown( parser );
		Mem_Free( alloc, parser );
	}

	void Parser_ParseData( Parser_t parser, const Packet& packet, const Chunk* head, Parse::Mode mode )
	{ return Parser_QueueData( parser, packet, head, mode ); }

	void Parser_ResetData( Parser_t parser )
	{ return Parser_QueueReset( parser ); }

} }

//======================================================================================
namespace nemesis { namespace profiling 
{ 
	static const ParsedData_s& Database_GetData( Database_t db )
	{ return db->Data; }

} }

//======================================================================================
namespace nemesis { namespace profiling 
{ 
	Database_t Database_Create( Allocator_t alloc, const DatabaseSetup_s& setup )
	{ 
		Database_t db = Mem_Calloc<Database_s>( alloc );
		Database_Initialize( db, alloc, setup );
		return db;
	}

	void Database_Destroy( Database_t db )
	{ 
		if (!db)
			return;
		Allocator_t alloc = db->Alloc;
		Database_Shutdown( db );
		Mem_Free( alloc, db );
	}

	size_t Database_GetSize( Database_t db )
	{ return Database_TotalSize( db ); }

	size_t Database_GetCapacity( Database_t db )
	{ return db->Setup.MaxNumBytes; }

	void Database_SetCapacity( Database_t db, size_t size )
	{
		const uint32_t default_size = 500 * 1024 * 1024;
		const uint32_t clamped_size = (uint32_t)(NeMin<size_t>(size, UINT32_MAX));
		db->Setup.MaxNumBytes = clamped_size ? clamped_size : default_size;
	}

	const Clock& Database_GetClock( Database_t db )
	{ return db->Data.Clock; }

	Tick Database_GetFirstTick( Database_t db )
	{ 
		const ParsedData_s& data = Database_GetData( db );
		return data.Frames.Count ? data.Frames[0].Time.Begin : 0; 
	}

	Tick Database_GetLastEndTick( Database_t db )
	{ 
		const ParsedData_s& data = Database_GetData( db );
		return data.Frames.Count ? data.Frames.Data[data.Frames.Count-1].Time.End : 0; 
	}

	Tick Database_GetLastBeginTick( Database_t db )
	{ 
		const ParsedData_s& data = Database_GetData( db );
		return data.Frames.Count ? data.Frames.Data[data.Frames.Count-1].Time.Begin : 0;
	}

	Tick Database_GetMaxFrameDuration( Database_t db )
	{ return Database_GetData( db ).MaxFrameDuration; }

	uint32_t Database_GetFirstFrameNumber( Database_t db )
	{ 
		const ParsedData_s& data = Database_GetData( db );
		return data.LastFrameNumber - data.Frames.Count+1; 
	}

	uint32_t Database_GetLastFrameNumber( Database_t db )
	{ return Database_GetData( db ).LastFrameNumber; }

	Tick Database_GotoFrameIndex( Database_t db, int frame_index )
	{ return ParsedData_GotoFrameIndex( Database_GetData( db ), frame_index ); }

	Tick Database_GotoFrameNumber( Database_t db, uint32_t frame_number )
	{ return ParsedData_GotoFrameNumber( Database_GetData( db ), frame_number ); }

	Tick Database_GotoFirstFrame( Database_t db )
	{ return ParsedData_GotoFirstFrame( Database_GetData( db ) ); }

	Tick Database_GotoLastFrame( Database_t db )
	{ return ParsedData_GotoLastFrame( Database_GetData( db ) ); }

	Tick Database_GotoBegin( Database_t db )
	{ return ParsedData_GotoBegin( Database_GetData( db ) ); }

	Tick Database_GotoEnd( Database_t db, Tick ticks_in_view )
	{ return ParsedData_GotoEnd( Database_GetData( db ), ticks_in_view ); }

	int Database_TickToFrame( Database_t db, Tick tick )
	{ return ParsedData_TickToFrameIndex( Database_GetData( db ), tick ); }

	int Database_GetNumCpus( Database_t db )
	{ return (int)Database_GetData( db ).NumCpus; }

	int Database_GetNumThreads( Database_t db )
	{ return Database_GetData( db ).Threads.Count; }

	void Database_GetThread( Database_t db, int index, viz::Thread& item )
	{ item = Database_GetData( db ).Threads.Item[ index ]; }

	int Database_GetNumFrames( Database_t db )
	{ return Database_GetData( db ).Frames.Count; }

	void Database_GetFrame( Database_t db, int index, viz::Frame& item )
	{ item = Database_GetData( db ).Frames[ index ]; }

	const viz::Frame* Database_GetFrames( Database_t db )
	{ return Database_GetData( db ).Frames.Data; }

	int Database_GetNumLocks( Database_t db )
	{ return Database_GetData( db ).Locks.Count; }

	void Database_GetLock( Database_t db, int index, viz::Lock& item )
	{ item = Database_GetData( db ).Locks[ index ]; }

	int Database_GetNumCounters( Database_t db )
	{ return Database_GetData( db ).Counters.Count; }

	void Database_GetCounter( Database_t db, int index, viz::Counter& item )
	{ item = Database_GetData( db ).Counters[ index ]; }

	int Database_GetNumCounterGroups( Database_t db )
	{ return Database_GetData( db ).CounterGroups.Count; }

	void Database_GetCounterGroup( Database_t db, int index, viz::CounterGroup& item )
	{ item = Database_GetData( db ).CounterGroups[ index ]; }

	int Database_GetNumCounterValues( Database_t db )
	{ return Database_GetData( db ).CounterValues.Count; }

	void Database_GetCounterValue( Database_t db, int index, viz::CounterValue& item )
	{ item = Database_GetData( db ).CounterValues[ index ]; }

	int Database_GetNumLocations( Database_t db )
	{ return Database_GetData( db ).Locations.Count; }

	void Database_GetLocation( Database_t db, int index, NamedLocation& item )
	{ 
		const ParsedData_s& data = Database_GetData( db );
		if (Array_IsValidIndex(data.Locations, index))
			item = data.Locations.Data[ index ];
		else
			item = INVALID_LOCATION;
	}

	void Database_GetLocationByZone( Database_t db, int zone, NamedLocation& item )
	{ return Database_GetLocation( db, Database_GetData( db ).Scopes.Data[ zone ].Location, item ); }

	int Database_GetNumScopes( Database_t db )
	{ return Database_GetData( db ).Scopes.Count; }

	void Database_EnumFrameGroups( Database_t db, const viz::FrameRange& cull, const viz::FrameGroupSetup& setup, EnumFrameGroupsFunc func, void* context )
	{ return ParsedData_EnumFrameGroups( Database_GetData( db ), cull, setup, func, context ); }

	void Database_EnumZoneGroups( Database_t db, const viz::FrameRange& cull, const viz::ZoneGroupSetup& setup, int thread_index, EnumZoneGroupsFunc func, void* context )
	{ return ParsedData_EnumZoneGroups( Database_GetData( db ), cull, setup, thread_index, func, context ); }

	void Database_EnumCpuGroups( Database_t db, const viz::FrameRange& cull, const viz::CpuGroupSetup& setup, int thread_index, EnumCpuGroupsFunc func, void* context )
	{ return ParsedData_EnumCpuGroups( Database_GetData( db ), cull, setup, thread_index, func, context ); }

	void Database_EnumLockEvents( Database_t db, int lock_index, int first_frame, int num_frames, EnumLockEventFunc func, void* context )
	{ return ParsedData_EnumLockEvents( Database_GetData( db ), lock_index, first_frame, num_frames, func, context ); }

	void Database_BuildHotSpots( Database_t db, const viz::HotSpotRange& range, viz::HotSpotGroup& group )
	{ return ParsedData_BuildHotSpots( Database_GetData( db ), range, group ); }

} }
