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
#include "Constants.h"
#include "BufferPool.h"

//======================================================================================
#include <Nemesis/Core/HashTable.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ThreadRecorderStats_s
	{
		uint32_t NumNames;
		uint32_t NumLocations;
		uint32_t NumLocks;
		size_t	 SizeOfNames;
		size_t	 SizeOfLocations;
		size_t	 SizeOfLocks;
	};

	struct ThreadRecorder_s
	{
		CriticalSection_t	Mutex;
		uint8_t				Index;
		uint8_t				_pad_[7];
		Buffer_t			Data;
		Buffer_t			Meta;
		BufferPool_t		Pool;
		Sender_s*			Sender;
		HashTable_64_32_s	NameMap;
		Array<cstr_t>		NameVal;
		Array<uint16_t>		NameLen;
		HashTable_64_32_s	SiteMap;
		Array<CallSite_s>	SiteVal;
		Array<uint8_t>		ThreadKey;
		Array<cstr_t>		ThreadVal;
		Array<cptr_t>		MutexKey;
		Array<cstr_t>		MutexVal;
		int					FlushName;
		int					FlushSite;
		int					FlushThread;
		int					FlushMutex;
	};

	void ThreadRecorder_Initialize		( ThreadRecorder_t tr, Allocator_t alloc, uint8_t index, BufferPool_t pool, Sender_s* sender );
	void ThreadRecorder_Shutdown		( ThreadRecorder_t tr );
	void ThreadRecorder_GetStats		( ThreadRecorder_t tr, ThreadRecorderStats_s& stats );
	void ThreadRecorder_RegisterNames   ( ThreadRecorder_t tr, const cstr_t* names, int count );
	int  ThreadRecorder_RegisterCallSite( ThreadRecorder_t tr, const CallSite_s& site );
	void ThreadRecorder_RegisterThread	( ThreadRecorder_t tr, uint8_t index, cstr_t name );
	void ThreadRecorder_RegisterMutex	( ThreadRecorder_t tr, cptr_t handle, cstr_t name );
	void ThreadRecorder_Record			( ThreadRecorder_t tr, const Chunk& chunk );
	void ThreadRecorder_Flush			( ThreadRecorder_t tr );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct RecorderStats_s
	{
		uint32_t			  NumThreads;
		ThreadRecorderStats_s Thread[ MAX_NUM_THREADS ];
	};

	struct MainRecorder_s
	{
		Allocator_t			Alloc;
		CriticalSection_t	Mutex;
		TlsId_t				Tls;
		Sender_s*			Sender;
		chunk::EndFrame		Frame;
		uint8_t				NumThreads;
		ThreadRecorder_t	Thread[ MAX_NUM_THREADS ];
		BufferPool_s		BufferPool;
	};

	void MainRecorder_Initialize( MainRecorder_t mr, Allocator_t alloc, Sender_s* sender );
	void MainRecorder_Shutdown  ( MainRecorder_t mr );
	void MainRecorder_NextFrame ( MainRecorder_t mr );
	void MainRecorder_GetStats	( MainRecorder_t mr, RecorderStats_s& stats );

	void MainRecorder_SetThreadInfo( MainRecorder_t mr, cstr_t name );
	void MainRecorder_EnterScope( MainRecorder_t mr, const CallSite_s& site, ScopeType::Enum type, int64_t tick );
	void MainRecorder_LeaveScope( MainRecorder_t mr, const CallSite_s& site, int64_t tick );
	void MainRecorder_EnterScope( MainRecorder_t mr, const CallSite_s& site, ScopeType::Enum type );
	void MainRecorder_LeaveScope( MainRecorder_t mr, const CallSite_s& site );

	void MainRecorder_SetMutexInfo( MainRecorder_t mr, cptr_t handle, cstr_t name );
	void MainRecorder_EnterMutex( MainRecorder_t mr, cptr_t handle );
	void MainRecorder_LeaveMutex( MainRecorder_t mr, cptr_t handle );

	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t name, uint32_t v );
	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t name, float v );
	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t path, cstr_t name, uint32_t v );
	void MainRecorder_RecordValue( MainRecorder_t mr, cstr_t path, cstr_t name, float v );

} }
