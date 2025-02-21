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
#include "Buffer.h"
#include "Worker.h"
#include "Constants.h"

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct DispatchItem_s
	{
		Buffer_t		Buffer;
		BufferPool_s*	Pool;
		uint32_t		Thread;
	};

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct DispatchRing_s
	{
		uint32_t		ReadPos;
		uint32_t		WritePos;
		DispatchItem_s	Item[ MAX_NUM_DISPATCH_ITEMS ];
	};

	uint32_t DispatchRing_GetCount( DispatchRing_s* ring );
	bool DispatchRing_IsEmpty( DispatchRing_s* ring );
	bool DispatchRing_IsFull( DispatchRing_s* ring );
	bool DispatchRing_Push( DispatchRing_s* ring, const DispatchItem_s& item );
	bool DispatchRing_Pop( DispatchRing_s* ring, DispatchItem_s& item );

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct DispatchQueue_s
	{
		Semaphore_t			Reader;
		Semaphore_t			Writer;
		CriticalSection_t	Mutex;
		DispatchRing_s		Data;
	};

	void DispatchQueue_Initialize( DispatchQueue_s* queue );
	void DispatchQueue_Push( DispatchQueue_s* queue, const DispatchItem_s& item );
	bool DispatchQueue_Pop( DispatchQueue_s* queue, DispatchItem_s& item );
	void DispatchQueue_Close( DispatchQueue_s* queue );
	void DispatchQueue_Shutdown( DispatchQueue_s* queue );

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Backlog_s
	{
		Array<Buffer_t> Buffer;
	};

	void Backlog_Initialize( Backlog_s* log, Allocator_t alloc );
	Result_t Backlog_Append( Backlog_s* log, Buffer_t buffer );
	void Backlog_Shutdown( Backlog_s* log );

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct LocalPeer_s
	{
		Consumer_s	Consumer;
		uint32_t	Init;
	};

	struct RemotePeer_s
	{
		Socket_t Socket;
		uint32_t Init;
	};

	struct PeerList_s
	{
		CriticalSection_t Mutex;
		LocalPeer_s		  Local;
		int				  NumRemote;
		RemotePeer_s	  Remote[ MAX_NUM_REMOTE_PEERS ];
	};

	void PeerList_Initialize( PeerList_s* list );
	bool PeerList_IsAttached( PeerList_s* list );
	void PeerList_Attach( PeerList_s* list, const Consumer_s& local );
	void PeerList_Detach( PeerList_s* list );
	int PeerList_FindPeer( PeerList_s* list, Socket_t peer );
	Result_t PeerList_Connect( PeerList_s* list, Socket_t peer );
	Result_t PeerList_Disconnect( PeerList_s* list, Socket_t peer );
	void PeerList_Disconnect( PeerList_s* list );
	void PeerList_Shutdown( PeerList_s* list );
	void PeerList_Dispatch( PeerList_s* list, Backlog_s* log, Buffer_t buffer );

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Dispatcher_s
	{
		DispatchQueue_s	Queue;
		PeerList_s		PeerList;
		Backlog_s		Backlog;
		Worker_s		Worker;
	};

	void Dispatcher_Initialize( Dispatcher_s* dispatcher, Allocator_t alloc );
	void Dispatcher_Attach( Dispatcher_s* dispatcher, const Consumer_s& local );
	void Dispatcher_Connect( Dispatcher_s* dispatcher, Socket_t client );
	void Dispatcher_Push( Dispatcher_s* dispatcher, const DispatchItem_s& item );
	void Dispatcher_Shutdown( Dispatcher_s* dispatcher );

} }
