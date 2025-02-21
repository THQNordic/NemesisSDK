//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "ParserState.h"

//======================================================================================
#include "Database.h"

//======================================================================================
#include <Nemesis/Core/String.h>
#include <Nemesis/Core/Process.h>
#include <float.h>

//======================================================================================
using namespace nemesis::system;
using namespace nemesis::profiling::viz;

//======================================================================================
// todo: 
//
//	Decouple chunk parsing from frame boundaries
//		-> Allows sender to send chunk buffers without having to wait for the end of a frame
//
//	Lazy name registration (allows the sender the send names when it's most convenient - instead of forcing them being sent up front)
//		-> When looking up names, register the "missing" string
//		-> When registering names, overwrite the "missing" string with he actual value

#define AssertChunkSize()\
	NeAssertOut( sizeof(chunk) == chunk.header.size, "Bad chunk size. Expected: %u. Received: %u.", sizeof(chunk), chunk.header.size )

//======================================================================================
namespace nemesis { namespace profiling
{
	static cstr_t StringPool_Alloc( Stack_s& pool, cstr_t text, uint32_t len )
	{
		char* clone = (char*)pool.Alloc( len );
		Mem_Cpy( clone, text, len );
		return clone;
	}

	static cstr_t StringPool_Alloc( Stack_s& pool, cstr_t text )
	{
		return StringPool_Alloc( pool, text, 1+Str_Len( text ) );
	}

	static cstr_t NameTable_Ensure( Array<cstr_t>& names, Stack_s& pool, cstr_t name ) 
	{
		const int pos = Array_BinaryFind<StringComparer>( names, name );
		if (pos >= 0)
			return names[ pos ];
		const char* clone = StringPool_Alloc( pool, name );
		names.InsertAt( ~pos, clone );
		return clone;
	}

	static void NameMap_Ensure( BinaryArrayMap<uint64_t, cstr_t>& map, Stack_s& pool, uint64_t key, uint16_t len, const char* value )
	{
		if (map.Contains( key ))
			return;
		map.Register( key, StringPool_Alloc( pool, value, len ) );
	}

	static int ParsedThreadTable_Find( const ParsedThreadTable_s& table, uint32_t id )
	{
		for ( int i = 0; i < table.Count; ++i )
		{
			if ( table.Id[i] == id )
				return i;
		}
		return -1;
	}

	static int ParsedThreadTable_Ensure( ParsedThreadTable_s& table, uint32_t id )
	{
		const int existing = ParsedThreadTable_Find( table, id );
		if (existing >= 0)
			return existing;
		NeAssert( table.Count < MAX_NUM_THREADS );
		table.Id[ table.Count ] = id;
		return table.Count++;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	static const uint8_t* Read( const uint8_t* pos, void* buffer, size_t size )
	{
		Mem_Cpy( buffer, pos, size );
		return pos + size;
	}

	template < typename T >
	inline const uint8_t* Read( const uint8_t* pos, T& v )
	{ return Read( pos, &v, sizeof(v) ); }

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	/// Parses a single "name list" chunk.
	static void RegisterNames( ParserState_s& state, const chunk::NameList* chunk )
	{
		//NePerfScope("register names");
		uint64_t key = 0;
		uint16_t len;
		const uint8_t* pos = (const uint8_t*)(chunk) + sizeof(*chunk);
		for ( uint32_t i = 0; i < chunk->numItems; ++i )
		{
			pos = Read( pos, key );
			pos = Read( pos, len );
			NameMap_Ensure( state.Names, state.Db->StringPool, key, len, (const char*) pos );
			pos += len;
		}
	}

	/// Parses a single "name list" chunk.
	static void RegisterNames_BigEndian( ParserState_s& state, const chunk::NameList* chunk )
	{
		//NePerfScope("register names (big endian)");
		uint64_t key = 0;
		uint16_t len;
		const uint8_t* pos = (const uint8_t*)(chunk) + sizeof(*chunk);
		const uint32_t num_items = EndianSwap( chunk->numItems );
		for ( uint32_t i = 0; i < num_items; ++i )
		{
			pos = Read( pos, key );
			pos = Read( pos, len );
			key = EndianSwap( key );
			len = EndianSwap( len );
			NameMap_Ensure( state.Names, state.Db->StringPool, key, len, (const char*) pos );
			pos += len;
		}
	}

	//==================================================================================
	static int ParsedData_EnsureCpu( ParsedData_s& data, uint8_t cpu_id )
	{
		data.NumCpus = NeMax( data.NumCpus, (uint8_t)(1+cpu_id) );
		return cpu_id;
	}

	//==================================================================================

	static void RegisterThreadName( ParserState_s& state, ParsedData_s& data, const chunk::ThreadInfo& chunk )
	{
		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		state.Names.Lookup( chunk.name, data.Threads.Item[ thread_index ].Name );
	}

	//==================================================================================
	static const NamedLocation INVALID_LOCATION = { "Unknown", "Unknown", "Unknown", 0 };

	static uint64_t MakeLocationKey( const uint32_t thread_id, const uint32_t location_id )
	{
		U64_LH key;
		key.High = thread_id;
		key.Low = location_id;
		return key.Value;
	}

	static void EnsureLocation( ParserState_s& state, ParsedData_s& data, uint64_t key, const NamedLocation& location )
	{
		const int key_idx = state.Locations.IndexOf( key ); 
		if (key_idx >= 0)
		{
			const int loc_idx = state.Locations.Values[ key_idx ];
			NeAssert(BitwiseComparer::Equals(data.Locations[loc_idx], location));
			NeUnused(loc_idx);
			return;
		}
		state.Locations.Register( key, data.Locations.Count );
		data.Locations.Append( location );
	}

	static void RegisterLocations( ParserState_s& state, ParsedData_s& data, const chunk::LocationList& chunk )
	{
		// chunks are sent after locations.
		// names are sent before locations.
		// however, names are not associated with particular threads.
		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		NeUnused(thread_index);

		// register each location
		NamedLocation location = INVALID_LOCATION;
		for ( uint32_t i = 0; i < chunk.numItems; ++i )
		{
			const chunk::LocationItem& it = chunk.item[i];
			state.Names.Lookup( it.name, location.Name				);
			state.Names.Lookup( it.func, location.Location.Function	);
			state.Names.Lookup( it.file, location.Location.File		);
			location.Location.Line = it.line;

			const uint64_t key = MakeLocationKey( chunk.threadId, it.id );
			EnsureLocation( state, data, key, location );
		}
	}

	static void RegisterLocations_BigEndian( ParserState_s& state, ParsedData_s& data, const chunk::LocationList& chunk )
	{
		const uint32_t threadId = EndianSwap( chunk.threadId );
		const uint32_t numItems = EndianSwap( chunk.numItems );

		// chunks are sent after locations.
		// names are sent before locations.
		// however, names are not associated with particular threads.
		const int thread_index = ParsedThreadTable_Ensure( data.Threads, threadId );
		NeUnused(thread_index);

		// register each location
		NamedLocation location = INVALID_LOCATION;
		for ( uint32_t i = 0; i < numItems; ++i )
		{
			NeZero(location);
			const chunk::LocationItem& it = chunk.item[i];
			state.Names.Lookup( EndianSwap( it.name ), location.Name				);
			state.Names.Lookup( EndianSwap( it.func ), location.Location.Function	);
			state.Names.Lookup( EndianSwap( it.file ), location.Location.File		);
			location.Location.Line = EndianSwap( it.line );
			EnsureLocation( state, data, MakeLocationKey( threadId, EndianSwap( it.id ) ), location );
		}
	}

	//==================================================================================

	/// Parses a single "enter scope_event" chunk.
	static void EnterScopeEvent( ParserState_s& state, ParsedData_s& data, const chunk::EnterScope& chunk, ScopeType::Enum type )
	{
		AssertChunkSize();

		const uint64_t location_key = MakeLocationKey( chunk.threadId, chunk.location );
		const bool has_locaction = state.Locations.Contains( location_key );
		const int location_index = has_locaction ? state.Locations[ location_key ] : -1;
		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		const int cpu_index = ParsedData_EnsureCpu( data, chunk.cpuId );

		const ScopeEvent ev = 
		{ chunk.timeStamp
		, (uint32_t)location_index
		, (uint8_t)thread_index
		, (uint8_t)cpu_index
		, (uint8_t)type
		, 1
		};
		data.Scopes.Append( ev );

		// state
		++state.OpenFrame.NumScopeEvents;
		++state.ZoneLevels[ thread_index ];

		// data
		data.Threads.Item[ thread_index ].NumLevels = NeMax(data.Threads.Item[ thread_index ].NumLevels, state.ZoneLevels[ thread_index ]);
	}

	/// Parses a single "leave scope_event" chunk.
	static void LeaveScopeEvent( ParserState_s& state, ParsedData_s& data, const chunk::LeaveScope& chunk )
	{
		AssertChunkSize();

		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		const int cpu_index = ParsedData_EnsureCpu( data, chunk.cpuId );

		const ScopeEvent ev = 
		{ chunk.timeStamp
		, 0
		, (uint8_t)thread_index
		, (uint8_t)cpu_index
		, 0
		, 0
		};
		data.Scopes.Append( ev );

		// state
		++state.OpenFrame.NumScopeEvents;
		if (state.ZoneLevels[ thread_index ] > 0)
		  --state.ZoneLevels[ thread_index ];
	}

	//==================================================================================

	/// Return a lock index for the given lock name
	/// or -1 if the lock name has not yet been registered.
	static int FindLock( const ParsedData_s& data, uint64_t handle )
	{
		for ( int i = 0; i < data.Locks.Count; ++i )
		{
			if ( data.Locks[i].Handle_t == handle )
				return i;
		}
		return -1;
	}

	/// Return a lock index for the given lock name.
	static int EnsureLock( ParsedData_s& data, uint64_t handle )
	{
		const int existing = FindLock( data, handle );
		if (existing >= 0)
			return existing;
		
		Lock& lock = data.Locks.Append();
		lock.Handle_t = handle;
		return data.Locks.Count-1;
	}

	/// Parses a single "enter lock" chunk.
	static void EnterLock( ParserState_s& state, ParsedData_s& data, const chunk::EnterLock& chunk )
	{
		AssertChunkSize();

		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		const int lock_index = EnsureLock( data, chunk.lockId );
		LockEvent& ev = data.LockEvents.Append();
		ev.Enter = true;
		ev.Lock = (uint8_t)lock_index;
		ev.Thread = (uint8_t)thread_index;
		ev.Tick = chunk.timeStamp;
		++state.OpenFrame.NumLockEvents;
	}

	/// Parses a single "leave lock" chunk.
	static void LeaveLock( ParserState_s& state, ParsedData_s& data, const chunk::LeaveLock& chunk )
	{
		AssertChunkSize();

		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		const int lock_index = EnsureLock( data, chunk.lockId );
		LockEvent& ev = data.LockEvents.Append();
		ev.Enter = false;
		ev.Lock = (uint8_t)lock_index;
		ev.Thread = (uint8_t)thread_index;
		ev.Tick = chunk.timeStamp;
		++state.OpenFrame.NumLockEvents;
	}

	static void RegisterLockName( ParserState_s& state, ParsedData_s& data, const chunk::MutexInfo& chunk )
	{
		const int lock_index = EnsureLock( data, chunk.handle );
		state.Names.Lookup( chunk.name, data.Locks[ lock_index ].Name );
	}

	//==================================================================================
	namespace 
	{
		struct CounterGroupComparer
		{
			static bool Equals( const CounterGroup& lhs, const CounterGroup& rhs )
			{
				if (lhs.Length != rhs.Length)
					return false;
				return (0 == Str_Cmp( lhs.Name, rhs.Name, lhs.Length ));
			}
		};

		struct CounterComparer
		{
			static int Compare( const Counter& counter, const char* name )
			{ return StringComparer::Compare( counter.Name, name ); }
		};
	}

	/// Return a counter group index for the given counter name.
	static int EnsureRootCounterGroup( ParsedData_s& data )
	{
		if (data.CounterGroups.Count)
			return 0;
		const CounterGroup root = { "root", (uint32_t)Str_Len("root"), -1 };
		data.CounterGroups.Append(root);
		return 0;
	}

	/// Return a counter group index for the given counter name.
	static int EnsureCounterGroup( ParsedData_s& data, const char* name, uint32_t len, int32_t parent )
	{
		const CounterGroup item = { name, len, parent };
		const int found = Array_LinearFind<CounterGroupComparer>( data.CounterGroups, item );
		if (found >= 0)
			return found;
		data.CounterGroups.Append( item );
		return data.CounterGroups.Count - 1;
	}

	/// Return a counter group index for the given counter name.
	static int EnsureCounterGroup( ParsedData_s& data, const char* name )
	{
		const char* next = name;
		int32_t parent = EnsureRootCounterGroup( data );
		for ( ; ; )
		{
			next = Str_Chr( next + 1, '/' );
			if (!next)
				break;
			parent = EnsureCounterGroup( data, name, (int)(next-name), parent );
		}
		return parent;
	}

	/// Return a counter index for the given counter name.
	static int EnsureCounter( ParsedData_s& data, const char* name )
	{
		const int pos = Array_BinaryFind<CounterComparer>( data.Counters, name );
		if (pos >= 0)
			return pos;

		Counter counter = {};
		counter.Name = name;
		counter.Group = EnsureCounterGroup( data, name );
		counter.Minimum =  FLT_MAX;
		counter.Maximum = -FLT_MAX;

		const int idx = ~pos;
		data.Counters.InsertAt( idx, counter );
		return idx;
	}

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const char* name, float float_value )
	{
		const int counter_index = EnsureCounter( data, name );

		CounterValue& value = data.CounterValues.Append();
		value.Name = name;
		value.Value = float_value;

		Counter& counter = data.Counters[ counter_index ];
		counter.Minimum = NeMin( counter.Minimum, float_value );
		counter.Maximum = NeMax( counter.Maximum, float_value );

		++state.OpenFrame.NumCounterValues;
	}

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, uint64_t name_id, float float_value )
	{
		const char* name = "<missing>";
		state.Names.Lookup( name_id, name );
		RegisterCounter( state, data, name, float_value );
	}
	
	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_U32_64& chunk )
	{ RegisterCounter( state, data, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_U32_32& chunk )
	{ RegisterCounter( state, data, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Float_64& chunk )
	{ RegisterCounter( state, data, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Float_32& chunk )
	{ RegisterCounter( state, data, chunk.name, (float) chunk.value ); }

	//==================================================================================

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, uint64_t path_id, uint64_t name_id, float float_value )
	{
		const char* path = "<missing>";
		const char* name = "<missing>";
		state.Names.Lookup( path_id, path );
		state.Names.Lookup( name_id, name );

		static char merged[256] = "";
		Str_Cpy( merged, path );
		Str_Cat( merged, name );
		
		const char* unique = NameTable_Ensure( state.Db->NameTable, state.Db->StringPool, merged );
		RegisterCounter( state, data, unique, float_value );
	}

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Path_U32_64& chunk )
	{ RegisterCounter( state, data, chunk.path, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Path_U32_32& chunk )
	{ RegisterCounter( state, data, chunk.path, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Path_Float_64& chunk )
	{ RegisterCounter( state, data, chunk.path, chunk.name, (float) chunk.value ); }

	static void RegisterCounter( ParserState_s& state, ParsedData_s& data, const chunk::Counter_Path_Float_32& chunk )
	{ RegisterCounter( state, data, chunk.path, chunk.name, (float) chunk.value ); }

	//==================================================================================

	static void RegisterLog( ParserState_s& state, ParsedData_s& data, const chunk::Log& chunk )
	{
		const uint64_t location_key = MakeLocationKey( chunk.threadId, chunk.location );
		const int thread_index = ParsedThreadTable_Ensure( data.Threads, chunk.threadId );
		const int location_index = state.Locations[ location_key ];
		const int text_len = chunk.header.size - sizeof(chunk);
		LogItem& item = data.LogItems.Append();
		item.Text = StringPool_Alloc( state.Db->StringPool, chunk.text, text_len );;
		item.Location = location_index;
		item.Thread = (uint8_t)thread_index;
	}

	//==================================================================================

	static void EndFrame( ParserInstance_s& instance, const chunk::EndFrame& chunk )
	{
		//NePerfScope("end of frame");

		// close open frame
		{
			instance.State.OpenFrame.Time.Begin = chunk.beginTick;
			instance.State.OpenFrame.Time.End = chunk.endTick;
		}

		// update parsed chunks data
		{
			Frame& frame = instance.ParsedChunks.Frames.Append();
			frame = instance.State.OpenFrame;

			// update limits
			const int64_t frame_duration = frame.Time.Duration();
			if (frame_duration > instance.ParsedChunks.MaxFrameDuration)
				instance.ParsedChunks.MaxFrameDuration = frame_duration;

			// update clock
			instance.ParsedChunks.Clock.Initialize( chunk.tickRate );

			// update frame numbering
			instance.ParsedChunks.LastFrameNumber = chunk.frameNumber;
		}

		// re-open frame
		{
			instance.State.OpenFrame.Time.End = instance.State.OpenFrame.Time.Begin;
			instance.State.OpenFrame.FirstScopeEvent = 0;
			instance.State.OpenFrame.NumScopeEvents = 0;
			instance.State.OpenFrame.FirstLockEvent = 0;
			instance.State.OpenFrame.NumLockEvents = 0;
			instance.State.OpenFrame.FirstCounterValue = 0;
			instance.State.OpenFrame.NumCounterValues = 0;
			instance.State.OpenFrame.ParsedBytes = 0;
		}

		// move completed frame to parsed frame data
		{
			NeLock/*Profiled*/( instance.ParsedFramesMutex );
			ParsedData_Append( instance.ParsedFrames, instance.ParsedChunks );
			ParsedData_ResetFrames( instance.ParsedChunks );
		}
	}

	//==================================================================================

	/// Determines the entering scope type for the given chunk identifer.
	static ScopeType::Enum GetEnterScopeType( uint32_t id )
	{
		switch (id)
		{
		default:
			break;
		case chunk::Type::EnterIdleScope:
			return ScopeType::Idle;
		case chunk::Type::EnterLockScope:
			return ScopeType::Lock;
		}
		return ScopeType::Regular;
	}

	/// Parses chunks generated by a little-endian machine.
	static void ParseChunksLittleEndian( ParserInstance_s& instance, const Chunk* head, uint32_t size )
	{
		ParserState_s& state = instance.State;
		ParsedData_s& data = instance.ParsedChunks;

		const Chunk* prev = nullptr;
		const Chunk* pos = head;
		const Chunk* end = NeSkip(pos, size);
		for ( ; pos < end; pos = NeSkip( pos, pos->size ) )
		{
			instance.State.OpenFrame.ParsedBytes += pos->size;

			switch ( pos->id )
			{
			case chunk::Type::EnterLockScope:
			case chunk::Type::EnterIdleScope:
			case chunk::Type::EnterScope:
				EnterScopeEvent( state, data, *reinterpret_cast<const chunk::EnterScope*>(pos), GetEnterScopeType( pos->id ) );
				break;

			case chunk::Type::LeaveScope:
				LeaveScopeEvent( state, data, *reinterpret_cast<const chunk::LeaveScope*>(pos) );
				break;

			case chunk::Type::EnterLock:
				EnterLock( state, data, *reinterpret_cast<const chunk::EnterLock*>(pos) );
				break;

			case chunk::Type::LeaveLock:
				LeaveLock( state, data, *reinterpret_cast<const chunk::LeaveLock*>(pos) );
				break;

			case chunk::Type::NameList:
				RegisterNames( state, reinterpret_cast<const chunk::NameList*>(pos) );
				break;

			case chunk::Type::ThreadInfo:
				RegisterThreadName( state, data, *reinterpret_cast<const chunk::ThreadInfo*>(pos) );
				break;

			case chunk::Type::MutexInfo:
				RegisterLockName( state, data, *reinterpret_cast<const chunk::MutexInfo*>(pos) );
				break;

			case chunk::Type::Counter_U32_32:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_U32_32*>(pos) );
				break;

			case chunk::Type::Counter_U32_64:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_U32_64*>(pos) );
				break;

			case chunk::Type::Counter_Float_32:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Float_32*>(pos) );
				break;

			case chunk::Type::Counter_Float_64:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Float_64*>(pos) );
				break;

			case chunk::Type::Counter_Path_U32_32:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Path_U32_32*>(pos) );
				break;

			case chunk::Type::Counter_Path_U32_64:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Path_U32_64*>(pos) );
				break;

			case chunk::Type::Counter_Path_Float_32:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Path_Float_32*>(pos) );
				break;

			case chunk::Type::Counter_Path_Float_64:
				RegisterCounter( state, data, *reinterpret_cast<const chunk::Counter_Path_Float_64*>(pos) );
				break;

			case chunk::Type::EndFrame:
				EndFrame( instance, *reinterpret_cast<const chunk::EndFrame*>(pos) );
				break;

			case chunk::Type::LocationList:
				RegisterLocations( state, data, *reinterpret_cast<const chunk::LocationList*>(pos) );
				break;

			case chunk::Type::Log:
				RegisterLog( state, data, *reinterpret_cast<const chunk::Log*>(pos) );
				break;

			case chunk::Type::Connect:
				ParserInstance_Reset( instance );
				break;

			default:
				//NeAssertOut(false, "Invalid chunk type: %d!", pos->id);
				// stop parsing here because the size field
				//	cannot be relied updon
				pos = end;	
				break;
			}

			prev = pos;
		}
		NeUnused(prev);
	}

	/// Parses chunks generated by a big-endian machine.
	static void ParseChunksBigEndian( ParserInstance_s& instance, const Chunk* head, uint32_t size )
	{
		ParserState_s& state = instance.State;
		ParsedData_s& data = instance.ParsedChunks;

		union
		{
			chunk::EnterScope				enter_scope				;
			chunk::LeaveScope				leave_scope				;
			chunk::EnterLock				enter_lock				;
			chunk::LeaveLock				leave_lock				;
			chunk::ThreadInfo				name_thread_64			;
			chunk::MutexInfo				name_lock_64			;
			chunk::Counter_U32_32			counter_u32_32			;
			chunk::Counter_U32_64			counter_u32_64			;
			chunk::Counter_Float_32			counter_float_32		;
			chunk::Counter_Float_64			counter_float_64		;
			chunk::Counter_Path_U32_32		counter_path_u32_32		;
			chunk::Counter_Path_U32_64		counter_path_u32_64		;
			chunk::Counter_Path_Float_32	counter_path_float_32	;
			chunk::Counter_Path_Float_64	counter_path_float_64	;
			chunk::EndFrame					end_frame				;
			chunk::Connect					connect					;
		};

		uint32_t id;
		const Chunk* pos = head;
		const Chunk* end = NeSkip(pos, size);
		for ( ; pos < end; pos = NeSkip( pos, EndianSwap( pos->size ) ) )
		{
			id = EndianSwap( pos->id );
			instance.State.OpenFrame.ParsedBytes += EndianSwap( pos->size );

			switch ( id )
			{
			case chunk::Type::EnterLockScope:
			case chunk::Type::EnterIdleScope:
			case chunk::Type::EnterScope:
				EndianSwap( *reinterpret_cast<const chunk::EnterScope*>(pos), enter_scope );
				EnterScopeEvent( state, data, enter_scope, GetEnterScopeType( enter_scope.header.id ) );
				break;

			case chunk::Type::LeaveScope:
				EndianSwap( *reinterpret_cast<const chunk::LeaveScope*>(pos), leave_scope );
				LeaveScopeEvent( state, data, leave_scope );
				break;

			case chunk::Type::EnterLock:
				EndianSwap( *reinterpret_cast<const chunk::EnterLock*>(pos), enter_lock );
				EnterLock( state, data, enter_lock );
				break;

			case chunk::Type::LeaveLock:
				EndianSwap( *reinterpret_cast<const chunk::LeaveLock*>(pos), leave_lock );
				LeaveLock( state, data, leave_lock );
				break;

			case chunk::Type::NameList:
				RegisterNames_BigEndian( state, reinterpret_cast<const chunk::NameList*>(pos) );
				break;

			case chunk::Type::ThreadInfo:
				EndianSwap( *reinterpret_cast<const chunk::ThreadInfo*>(pos), name_thread_64 );
				RegisterThreadName( state, data, name_thread_64 );
				break;

			case chunk::Type::MutexInfo:
				EndianSwap( *reinterpret_cast<const chunk::MutexInfo*>(pos), name_lock_64 );
				RegisterLockName( state, data, name_lock_64 );
				break;

			case chunk::Type::Counter_U32_32:
				EndianSwap( *reinterpret_cast<const chunk::Counter_U32_32*>(pos), counter_u32_32 );
				RegisterCounter( state, data, counter_u32_32 );
				break;

			case chunk::Type::Counter_U32_64:
				EndianSwap( *reinterpret_cast<const chunk::Counter_U32_64*>(pos), counter_u32_64 );
				RegisterCounter( state, data, counter_u32_64 );
				break;

			case chunk::Type::Counter_Float_32:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Float_32*>(pos), counter_float_32 );
				RegisterCounter( state, data, counter_float_32 );
				break;

			case chunk::Type::Counter_Float_64:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Float_64*>(pos), counter_float_64 );
				RegisterCounter( state, data, counter_float_64 );
				break;

			case chunk::Type::Counter_Path_U32_32:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Path_U32_32*>(pos), counter_path_u32_32 );
				RegisterCounter( state, data, counter_path_u32_32 );
				break;

			case chunk::Type::Counter_Path_U32_64:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Path_U32_64*>(pos), counter_path_u32_64 );
				RegisterCounter( state, data, counter_path_u32_64 );
				break;

			case chunk::Type::Counter_Path_Float_32:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Path_Float_32*>(pos), counter_path_float_32 );
				RegisterCounter( state, data, counter_path_float_32 );
				break;

			case chunk::Type::Counter_Path_Float_64:
				EndianSwap( *reinterpret_cast<const chunk::Counter_Path_Float_64*>(pos), counter_path_float_64 );
				RegisterCounter( state, data, counter_path_float_64 );
				break;

			case chunk::Type::EndFrame:
				EndianSwap( *reinterpret_cast<const chunk::EndFrame*>(pos), end_frame );
				EndFrame( instance, end_frame );
				break;

			case chunk::Type::LocationList:
				RegisterLocations_BigEndian( state, data, *reinterpret_cast<const chunk::LocationList*>(pos) );
				break;

			case chunk::Type::Log:
				break;

			case chunk::Type::Connect:
				ParserInstance_Reset( instance );
				break;

			default:
				//NeAssertOut(false, "Invalid chunk type: %d!", pos->id);
				// stop parsing here because the size field
				//	cannot be relied updon
				pos = end;	
				break;
			}
		}
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	/// Initializes data member for first use.
	static void ParserState_Initialize( ParserState_s& state, Allocator_t alloc )
	{
		state.Names.Keys.Alloc = alloc;
		state.Names.Values.Alloc = alloc;
		state.Locations.Keys.Alloc = alloc;
		state.Locations.Values.Alloc = alloc;
	}

	/// Frees dynamic memory allocated by the data set.
	static void ParserState_Shutdown( ParserState_s& state )
	{
		state.Names.Clear();
		state.Locations.Clear();
	}

	/// Resets data members withot freeing allocated memory.
	static void ParserState_Reset( ParserState_s& state )
	{
		NeZero(state.OpenFrame);
		NeZero(state.ZoneLevels);
		state.Names.Reset();
		state.Locations.Reset();
		Database_ResetStrings( state.Db );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	/// Initializes data member for first use.
	void ParserInstance_Initialize( ParserInstance_s& instance, Allocator_t alloc, Database_t db )
	{
		CriticalSection_Create( instance.ParsedFramesMutex );
		ParserState_Initialize( instance.State, alloc );
		ParsedData_Initialize( instance.ParsedChunks , alloc );
		ParsedData_Initialize( instance.ParsedFrames , alloc );
		instance.State.Db = db;
	}

	/// Frees dynamic memory.
	void ParserInstance_Shutdown( ParserInstance_s& instance )
	{
		ParserState_Shutdown( instance.State );
		ParsedData_Shutdown( instance.ParsedChunks );
		ParsedData_Shutdown( instance.ParsedFrames );
		CriticalSection_Destroy( instance.ParsedFramesMutex );
	}

	/// Resets data members without freeing allocated memory.
	void ParserInstance_Reset( ParserInstance_s& instance )
	{
		NeLock/*Profiled*/(instance.ParsedFramesMutex);
		ParserState_Reset( instance.State );
		ParsedData_Reset( instance.ParsedChunks );
		ParsedData_Reset( instance.ParsedFrames );
		instance.State.Reset = true;
	}

	/// Parses a stream of profiling chunks.
	void ParserInstance_ParseChunks( ParserInstance_s& instance, const Chunk* head, uint32_t size, bool big_endian )
	{
		return big_endian
			? ParseChunksBigEndian   ( instance, head, size )
			: ParseChunksLittleEndian( instance, head, size );
	}

	/// Moves fully parsed frames from the parser to the joined data set. 
	void ParserInstance_JoinFrames( ParserInstance_s& instance, ParsedData_s& joined )
	{
		if (instance.State.Reset)
		{
			instance.State.Reset = false;
			ParsedData_Reset( joined );
		}
		NeLock/*Profiled*/( instance.ParsedFramesMutex );
		ParsedData_Append( joined, instance.ParsedFrames );
		ParsedData_ResetFrames( instance.ParsedFrames );
	}

} }
