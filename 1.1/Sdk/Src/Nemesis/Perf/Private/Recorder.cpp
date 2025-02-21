//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Recorder.h"

//======================================================================================
#include "Packet.h"

//======================================================================================
#include "Sender.h"

//======================================================================================
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Core/Process.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
//																		 THREAD RECORDER
//======================================================================================
namespace nemesis { namespace profiling
{
	static bool ThreadRecorder_FlushNameTable( ThreadRecorder_t tr, Buffer_t buffer )
	{
		uint64_t id  = 0;
		uint16_t len = 0;
		chunk::NameList header = { { (uint32_t)chunk::Type::NameList, 0 }, 1 };
		for ( int i = tr->FlushName; i < tr->NameVal.Count; ++i )
		{
			const size_t chunk_size	 = sizeof(header) + sizeof(id) + sizeof(len) + tr->NameLen[i];
			const size_t remain_size = sizeof(buffer->Data) - buffer->Count;
			if (chunk_size > remain_size)
				return false;

			header.header.size	= (uint32_t)chunk_size;
			id					= (uint64_t)tr->NameVal[i];
			len					= tr->NameLen[i];
			Mem_Cpy( buffer->Data + buffer->Count, &header		 , sizeof(header) ); buffer->Count += sizeof(header);
			Mem_Cpy( buffer->Data + buffer->Count, &id			 , sizeof(id)	  ); buffer->Count += sizeof(id);
			Mem_Cpy( buffer->Data + buffer->Count, &len			 , sizeof(len)	  ); buffer->Count += sizeof(len);
			Mem_Cpy( buffer->Data + buffer->Count, tr->NameVal[i], len			  ); buffer->Count += len;
			++tr->FlushName;
		}
		return true;
	}

	static bool ThreadRecorder_FlushSiteTable( ThreadRecorder_t tr, Buffer_t buffer )
	{
		chunk::LocationItem item   = {};
		chunk::LocationList header = { { chunk::Type::LocationList, 0 }, 1, tr->Index };
		for ( int i = tr->FlushSite; i < tr->SiteVal.Count; ++i )
		{
			const size_t chunk_size		= sizeof(header) + sizeof(item);
			const size_t remain_size	= sizeof(buffer->Data) - buffer->Count;
			if (chunk_size > remain_size)
				return false;
			header.header.size	= (uint32_t)chunk_size;
			const NamedLocation& loc = tr->SiteVal[i];
			item.name = (size_t)loc.Name;
			item.func = (size_t)loc.Location.Function;
			item.file = (size_t)loc.Location.File;
			item.line = (uint64_t)loc.Location.Line;
			item.id   = (uint32_t)i;
			Mem_Cpy( buffer->Data + buffer->Count, &header, sizeof(header) ); buffer->Count += sizeof(header);
			Mem_Cpy( buffer->Data + buffer->Count, &item  , sizeof(item)   ); buffer->Count += sizeof(item);
			++tr->FlushSite;
		}
		return true;
	}

	static bool ThreadRecorder_FlushThreadTable( ThreadRecorder_t tr, Buffer_t buffer )
	{
		chunk::ThreadInfo header = { { chunk::Type::ThreadInfo, sizeof(header) } };
		for ( int i = tr->FlushThread; i < tr->ThreadKey.Count; ++i )
		{
			const size_t chunk_size = sizeof(header);
			const size_t remain_size = sizeof(buffer->Data) - buffer->Count;
			if (chunk_size > remain_size)
				return false;
			header.threadId = tr->ThreadKey[i];
			header.name		= (uint64_t)tr->ThreadVal[i];
			Mem_Cpy( buffer->Data + buffer->Count, &header, sizeof(header) ); buffer->Count += sizeof(header);
			++tr->FlushThread;
		}
		return true;
	}

	static bool ThreadRecorder_FlushMutexTable( ThreadRecorder_t tr, Buffer_t buffer )
	{
		chunk::MutexInfo header = { { chunk::Type::MutexInfo, sizeof(header) } };
		for ( int i = tr->FlushMutex; i < tr->MutexKey.Count; ++i )
		{
			const size_t chunk_size = sizeof(header);
			const size_t remain_size = sizeof(buffer->Data) - buffer->Count;
			if (chunk_size > remain_size)
				return false;
			header.handle = (uint64_t)tr->MutexKey[i];
			header.name   = (uint64_t)tr->MutexVal[i];
			Mem_Cpy( buffer->Data + buffer->Count, &header, sizeof(header) ); buffer->Count += sizeof(header);
			++tr->FlushMutex;
		}
		return true;
	}

	static int ThreadRecorder_RegisterName( ThreadRecorder_t tr, cstr_t name )
	{
		const uint32_t idx = HashTable_Get( tr->NameMap, (uint64_t)name, UINT32_MAX );
		if (idx != UINT32_MAX)
			return (int)idx;
		const uint32_t id = tr->NameVal.Count;
		const uint16_t len = (uint16_t)(1+Str_Len( name ));
		HashTable_Set( tr->NameMap, (uint64_t)name, id );
		tr->NameVal.Append( name );
		tr->NameLen.Append( len );
		return id;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	static bool ThreadRecorder_DispatchBuffer( ThreadRecorder_t tr, Buffer_t buffer )
	{
		if (Packet_IsEmpty(buffer))
			return false;
		Packet_Finalize( buffer );
		const DispatchItem_s item = { buffer, tr->Pool, tr->Index };
		Sender_Push( tr->Sender, item );
		return true;
	}

	static void ThreadRecorder_AllocData( ThreadRecorder_t tr ) 
	{
		tr->Data = BufferPool_AllocBuffer( tr->Pool, BufferType::Data );
		Packet_Initialize( tr->Data );
	}

	static void ThreadRecorder_AllocMeta( ThreadRecorder_t tr ) 
	{
		tr->Meta = BufferPool_AllocBuffer( tr->Pool, BufferType::Meta );
		Packet_Initialize( tr->Meta );
	}

	static void ThreadRecorder_DispatchMeta( ThreadRecorder_t tr )
	{
		if (ThreadRecorder_DispatchBuffer( tr, tr->Meta ))
			ThreadRecorder_AllocMeta( tr );
	}

	static void ThreadRecorder_WriteMeta( ThreadRecorder_t tr )
	{
		while (!ThreadRecorder_FlushNameTable( tr, tr->Meta ))
			ThreadRecorder_DispatchMeta( tr );

		while(!ThreadRecorder_FlushSiteTable( tr, tr->Meta ))
			ThreadRecorder_DispatchMeta( tr );

		while (!ThreadRecorder_FlushThreadTable( tr, tr->Meta ))
			ThreadRecorder_DispatchMeta( tr );

		while (!ThreadRecorder_FlushMutexTable( tr, tr->Meta ))
			ThreadRecorder_DispatchMeta( tr );
	}

	static void ThreadRecorder_DispatchData( ThreadRecorder_t tr ) 
	{
		{
			ThreadRecorder_WriteMeta( tr );
			ThreadRecorder_DispatchMeta( tr );
		}
		{
			if (ThreadRecorder_DispatchBuffer( tr, tr->Data ))
				ThreadRecorder_AllocData( tr );
		}
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	void ThreadRecorder_Initialize( ThreadRecorder_t tr, Allocator_t alloc, uint8_t index, BufferPool_t pool, Sender_s* sender )
	{
		system::CriticalSection_Create( tr->Mutex );

		tr->Index	= index;
		tr->Pool	= pool;
		tr->Sender	= sender;

		HashTable_Init( tr->NameMap, alloc );
		tr->NameVal	 .Alloc = alloc;
		tr->NameLen	 .Alloc = alloc;
		HashTable_Init( tr->SiteMap, alloc );
		tr->SiteVal	 .Alloc = alloc;
		tr->ThreadKey.Alloc = alloc;
		tr->ThreadVal.Alloc = alloc;
		tr->MutexKey .Alloc = alloc;
		tr->MutexVal .Alloc = alloc;

		ThreadRecorder_AllocData( tr );
		ThreadRecorder_AllocMeta( tr );
	}

	void ThreadRecorder_Shutdown( ThreadRecorder_t tr )
	{
		HashTable_Clear( tr->NameMap );
		tr->NameVal.Clear();
		tr->NameLen.Clear();
		HashTable_Clear( tr->SiteMap );
		tr->SiteVal.Clear();
		tr->ThreadKey.Clear();
		tr->ThreadVal.Clear();
		tr->MutexKey.Clear();
		tr->MutexVal.Clear();

		system::CriticalSection_Destroy( tr->Mutex );
	}

	void ThreadRecorder_GetStats( ThreadRecorder_t tr, ThreadRecorderStats_s& stats )
	{
		NeLock(tr->Mutex);
		stats.NumNames			= tr->NameVal.Count;
		stats.NumLocations		= tr->SiteVal.Count;
		stats.NumLocks			= tr->MutexKey.Count;
		stats.SizeOfNames		= tr->NameMap.Capacity * (sizeof( tr->NameMap.Key[0] ) + sizeof( tr->NameMap.Val[0] ))
								+ Array_GetCapacitySize( tr->NameVal  ) 
								+ Array_GetCapacitySize( tr->NameLen  );
		stats.SizeOfLocations	= tr->SiteMap.Capacity * (sizeof( tr->SiteMap.Key[0] ) + sizeof( tr->SiteMap.Val[0] ))
								+ Array_GetCapacitySize( tr->SiteVal  );
		stats.SizeOfLocks		= Array_GetCapacitySize( tr->MutexKey )
								+ Array_GetCapacitySize( tr->MutexVal );
	}

	void ThreadRecorder_RegisterNames( ThreadRecorder_t tr, const cstr_t* names, int count )
	{
		NeLock(tr->Mutex);
		for ( int i = 0; i < count; ++i )
			ThreadRecorder_RegisterName( tr, names[i] );
	}

	int ThreadRecorder_RegisterCallSite( ThreadRecorder_t tr, const CallSite_s& site )
	{
		NeLock(tr->Mutex);
		// @todo: store ids instead of names -> halves the size of CallSite
		const int32_t name_id = ThreadRecorder_RegisterName( tr, site.Name );
		const int32_t func_id = ThreadRecorder_RegisterName( tr, site.Location.File );
		const int32_t file_id = ThreadRecorder_RegisterName( tr, site.Location.Function );
		const uint64_t key = (((uint64_t)file_id) << 32) | site.Location.Line;
		const uint32_t found = HashTable_Get( tr->SiteMap, key, UINT32_MAX );
		if (found != UINT32_MAX)
			return (int)found;
		const uint32_t site_id = tr->SiteVal.Count;
		HashTable_Set( tr->SiteMap, key, site_id );
		tr->SiteVal.Append( site );
		return site_id;
	}

	void ThreadRecorder_RegisterThread( ThreadRecorder_t tr, uint8_t index, cstr_t name )
	{
		NeLock(tr->Mutex);
		const int idx = Array_LinearFind( tr->ThreadKey, index );
		if (idx >= 0)
			return;
		tr->ThreadKey.Append( index );
		tr->ThreadVal.Append( name );
	}

	void ThreadRecorder_RegisterMutex( ThreadRecorder_t tr, cptr_t handle, cstr_t name )
	{
		NeLock(tr->Mutex);
		const int idx = Array_LinearFind( tr->MutexKey, handle );
		if (idx >= 0)
			return;
		tr->MutexKey.Append( handle );
		tr->MutexVal.Append( name );
	}

	void ThreadRecorder_Record( ThreadRecorder_t tr, const Chunk& chunk )
	{
		NeLock(tr->Mutex);
		{
			const uint32_t new_size = tr->Data->Count + chunk.size;
			if (new_size > sizeof(tr->Data->Data))
				ThreadRecorder_DispatchData( tr );
		}
		{
			Mem_Cpy( tr->Data->Data + tr->Data->Count, &chunk, chunk.size );
			tr->Data->Count += chunk.size;
		}
	}

	void ThreadRecorder_Flush( ThreadRecorder_t tr )
	{
		NeLock(tr->Mutex);
		ThreadRecorder_DispatchData( tr );
	}

} }

//======================================================================================
//																		   MAIN RECORDER
//======================================================================================
namespace nemesis { namespace profiling
{ 
	static uint32_t MakeChunkId( ScopeType::Enum type )
	{
		switch (type)
		{
		default:
			return chunk::Type::EnterScope;
		case ScopeType::Lock:
			return chunk::Type::EnterLockScope;
		case ScopeType::Idle:
			return chunk::Type::EnterIdleScope;
		}
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	static ThreadRecorder_t MainRecorder_GetCurrentThread( MainRecorder_t mr )
	{
		return (ThreadRecorder_t)Tls_GetValue( mr->Tls );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	static ThreadRecorder_t MainRecorder_CreateThreadRecorder( MainRecorder_t mr )
	{
		ThreadRecorder_t tr = MainRecorder_GetCurrentThread( mr );
		if (tr)
			return tr;
		NeLock( mr->Mutex );
		NeAssertOut( mr->NumThreads < NeCountOf(mr->Thread), "Recorder thread overflow!" );
		tr = Mem_Calloc<ThreadRecorder_s>( mr->Alloc );
		ThreadRecorder_Initialize( tr, mr->Alloc, mr->NumThreads, &mr->BufferPool, mr->Sender );
		Tls_SetValue( mr->Tls, tr );
		mr->Thread[ mr->NumThreads++ ] = tr;
		return tr;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	void MainRecorder_Initialize( MainRecorder_t mr, Allocator_t alloc, Sender_s* sender )
	{
		mr->Alloc = alloc;
		mr->Sender = sender;
		CriticalSection_Create( mr->Mutex );

		mr->Tls = Tls_Alloc();
		BufferPool_Initialize( &mr->BufferPool );

		mr->Frame.header.id = chunk::Type::EndFrame;
		mr->Frame.header.size = sizeof(mr->Frame);
		mr->Frame.beginTick = Clock_GetTick();
		mr->Frame.endTick	= mr->Frame.beginTick;
		mr->Frame.tickRate	= Clock_GetFreq();
		mr->Frame.frameNumber = 0;
	}

	void MainRecorder_Shutdown( MainRecorder_t mr )
	{
		for ( int i = 0; i < mr->NumThreads; ++i )
		{
			ThreadRecorder_t tr = mr->Thread[i];
			ThreadRecorder_Shutdown( tr );
			Mem_Free( mr->Alloc, tr );
		}
		CriticalSection_Destroy( mr->Mutex );
	}

	void MainRecorder_NextFrame( MainRecorder_t mr )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;

		// write frame
		{
			mr->Frame.endTick = Clock_GetTick();
			ThreadRecorder_Record( thread, mr->Frame.header );
			++mr->Frame.frameNumber;
			mr->Frame.beginTick = mr->Frame.endTick;
		}

		// flush threads
		NeLock(mr->Mutex);
		for ( int i = 0; i < mr->NumThreads; ++i )
			ThreadRecorder_Flush( mr->Thread[i] );
	}

	void MainRecorder_GetStats( MainRecorder_t mr, RecorderStats_s& stats )
	{
		NeLock(mr->Mutex);
		NeZero(stats);
		stats.NumThreads = mr->NumThreads;
		for ( int i = 0; i < mr->NumThreads; ++i )
			ThreadRecorder_GetStats( mr->Thread[i], stats.Thread[i] );
	}

	void MainRecorder_SetThreadInfo( MainRecorder_t mr, cstr_t name )
	{
		if ( !name )
			return;
		ThreadRecorder_t thread = MainRecorder_CreateThreadRecorder( mr );
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_RegisterThread( thread, thread->Index, name );
	}

	void MainRecorder_EnterScope( MainRecorder_t mr, const CallSite_s& site, ScopeType::Enum type, int64_t tick )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
		const chunk::EnterScope chunk = 
		{ { MakeChunkId( type ), sizeof(chunk) }
		, thread->Index
		, (uint8_t)Cpu_GetIndex()
		, { 0, 0 }
		, (uint32_t)ThreadRecorder_RegisterCallSite( thread, site )
		, tick
		};
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_LeaveScope( MainRecorder_t mr, const CallSite_s& site, int64_t tick )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
		const chunk::LeaveScope chunk = 
		{ { chunk::Type::LeaveScope, sizeof(chunk) }
		, thread->Index
		, (uint8_t)Cpu_GetIndex()
		, { 0, 0 }
		, (uint32_t)ThreadRecorder_RegisterCallSite( thread, site )
		, tick
		};
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_EnterScope( MainRecorder_t mr, const CallSite_s& site, ScopeType::Enum type )
	{
		const int64_t tick = Clock_GetTick();
		return MainRecorder_EnterScope( mr, site, type, tick );
	}

	void MainRecorder_LeaveScope( MainRecorder_t mr, const CallSite_s& site )
	{
		const int64_t tick = Clock_GetTick();
		return MainRecorder_LeaveScope( mr, site, tick );
	}

	void MainRecorder_SetMutexInfo( MainRecorder_t mr, cptr_t handle, cstr_t name )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_RegisterMutex( thread, handle, name );
	}

	void MainRecorder_EnterMutex( MainRecorder_t mr, cptr_t handle )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
		const chunk::EnterLock chunk = 
		{ { chunk::Type::EnterLock, sizeof(chunk) }
		, thread->Index
		, (uint8_t)Cpu_GetIndex()
		, { 0, 0 }
		, Clock_GetTick()
		, (size_t)handle
		};
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_LeaveMutex( MainRecorder_t mr, cptr_t handle )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
		const chunk::LeaveLock chunk = 
		{ { chunk::Type::LeaveLock, sizeof(chunk) }
		, thread->Index
		, (uint8_t)Cpu_GetIndex()
		, { 0, 0 }
		, Clock_GetTick()
		, (size_t)handle
		};
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t name, uint32_t v )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
	#if (NE_PROCESSOR == NE_PROCESSOR_X86)
		const chunk::Counter_U32_32 chunk = 
		{ { chunk::Type::Counter_U32_32, sizeof(chunk) }
		, v
		, (size_t)name
		};
	#else
		const chunk::Counter_U32_64 chunk = 
		{ { chunk::Type::Counter_U32_64, sizeof(chunk) }
		, v
		, (size_t)name
		};
	#endif
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t name, float v )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
	#if (NE_PROCESSOR == NE_PROCESSOR_X86)
		const chunk::Counter_Float_32 chunk = 
		{ { chunk::Type::Counter_U32_32, sizeof(chunk) }
		, v
		, (size_t)name
		};
	#else
		const chunk::Counter_Float_64 chunk = 
		{ { chunk::Type::Counter_U32_64, sizeof(chunk) }
		, v
		, (size_t)name
		};
	#endif
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t path, cstr_t name, uint32_t v )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
	#if (NE_PROCESSOR == NE_PROCESSOR_X86)
		const chunk::Counter_Path_U32_32 chunk = 
		{ { chunk::Type::Counter_Path_U32_32, sizeof(chunk) }
		, v
		, (size_t)name
		, (size_t)path
		};
	#else
		const chunk::Counter_Path_U32_64 chunk = 
		{ { chunk::Type::Counter_Path_U32_64, sizeof(chunk) }
		, v
		, (size_t)name
		, (size_t)path
		};
	#endif
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_Record( thread, chunk.header );
	}

	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t path, cstr_t name, float v )
	{
		ThreadRecorder_t thread = MainRecorder_GetCurrentThread( mr );
		if ( !thread )
			return;
	#if (NE_PROCESSOR == NE_PROCESSOR_X86)
		const chunk::Counter_Path_Float_32 chunk = 
		{ { chunk::Type::Counter_Path_U32_32, sizeof(chunk) }
		, v
		, (size_t)name
		, (size_t)path
		};
	#else
		const chunk::Counter_Path_Float_64 chunk = 
		{ { chunk::Type::Counter_Path_U32_64, sizeof(chunk) }
		, v
		, (size_t)name
		, (size_t)path
		};
	#endif
		ThreadRecorder_RegisterNames( thread, &name, 1 );
		ThreadRecorder_Record( thread, chunk.header );
	}

} }
