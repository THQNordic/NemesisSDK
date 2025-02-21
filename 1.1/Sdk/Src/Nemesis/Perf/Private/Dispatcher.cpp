//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Dispatcher.h"

//======================================================================================
#include "Packet.h"
#include "BufferPool.h"

//======================================================================================
#include <Nemesis/Core/Process.h>
#include <Nemesis/Core/Socket.h>

//======================================================================================
#define UNIT_TEST_BACKLOG	0

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling
{ 
	/// x mod y where y is a power of 2
	static uint32_t UInt32_ModPow2( uint32_t x, uint32_t y )
	{ return x & (y-1); }

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	uint32_t DispatchRing_GetCount( DispatchRing_s* ring )
	{
		return (ring->WritePos - ring->ReadPos);
	}

	bool DispatchRing_IsEmpty( DispatchRing_s* ring )
	{
		return DispatchRing_GetCount( ring ) == 0;
	}

	bool DispatchRing_IsFull( DispatchRing_s* ring )
	{
		return DispatchRing_GetCount( ring ) == MAX_NUM_DISPATCH_ITEMS;
	}

	bool DispatchRing_Push( DispatchRing_s* ring, const DispatchItem_s& item )
	{
		if (DispatchRing_IsFull( ring ))
			return false;
		const uint32_t idx = UInt32_ModPow2( ring->WritePos, MAX_NUM_DISPATCH_ITEMS );
		ring->Item[ idx ] = item;
		ring->WritePos++;
		return true;
	}

	bool DispatchRing_Pop( DispatchRing_s* ring, DispatchItem_s& item )
	{
		if (DispatchRing_IsEmpty( ring ))
			return false;
		const uint32_t idx = UInt32_ModPow2( ring->ReadPos, MAX_NUM_DISPATCH_ITEMS );
		item = ring->Item[ idx ];
		ring->ReadPos++;
		return true;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void DispatchQueue_Initialize( DispatchQueue_s* queue )
	{
		CriticalSection_Create( queue->Mutex );
		queue->Reader = Semaphore_Create( 0						, MAX_NUM_DISPATCH_ITEMS );
		queue->Writer = Semaphore_Create( MAX_NUM_DISPATCH_ITEMS, MAX_NUM_DISPATCH_ITEMS );
	}

	void DispatchQueue_Push( DispatchQueue_s* queue, const DispatchItem_s& item )
	{
		Semaphore_Wait( queue->Writer );
		{
			NeLock(queue->Mutex);
			const bool ok = DispatchRing_Push( &queue->Data, item );
			NeAssertOut( ok, "Dispatch queue overflow!" );
		}
		Semaphore_Signal( queue->Reader, 1 );
	}

	bool DispatchQueue_Pop( DispatchQueue_s* queue, DispatchItem_s& item )
	{
		Semaphore_Wait( queue->Reader );
		NeLock(queue->Mutex);
		const bool ok = DispatchRing_Pop( &queue->Data, item );
		if (!ok)
			return false;
		Semaphore_Signal( queue->Writer, 1 );
		return true;
	}

	void DispatchQueue_Close( DispatchQueue_s* queue )
	{
		Semaphore_Signal( queue->Reader, MAX_NUM_DISPATCH_ITEMS );
	}

	void DispatchQueue_Shutdown( DispatchQueue_s* queue )
	{
		Semaphore_Destroy( queue->Writer );
		Semaphore_Destroy( queue->Reader );
		CriticalSection_Destroy( queue->Mutex );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	static void Backlog_Grow( Backlog_s* log )
	{
		Allocator_t alloc = log->Buffer.Alloc;
		Buffer_t item = Mem_Calloc<Buffer_s>( alloc );
		item->Type = BufferType::Meta;
		Packet_Initialize( item );
		log->Buffer.Append( item );
	}

	static Buffer_t Backlog_GetCurrent( Backlog_s* log )
	{
		const int idx = log->Buffer.Count-1; 
		return log->Buffer[idx];
	}

	static bool Backlog_Reserve( Backlog_s* log, uint32_t size )
	{
		Buffer_t item = Backlog_GetCurrent( log );
		uint32_t end = item->Count + size;
		if (end <= BUFFER_SIZE)
			return true;	// current buffer can accomodate size
		if (size > (sizeof(item->Data) - sizeof(Packet)))
			return false;	// too large for an empty buffer
		Backlog_Grow( log );
		return true;
	}

	static void Backlog_Write( Backlog_s* log, cptr_t data, uint32_t size )
	{
		Buffer_t item = Backlog_GetCurrent( log );
		NeAssert(Packet_IsValid(item));
		NeAssert((item->Count + size) <= sizeof(item->Data));
		Mem_Cpy( item->Data + item->Count, data, size ); 
		item->Count += size;
		Packet_Finalize( item );
	}

	static void Backlog_WriteConnectHeader( Backlog_s* log )
	{
		NeAssert(Packet_IsEmpty( Backlog_GetCurrent( log ) ));
		const chunk::Connect chunk = { { chunk::Type::Connect, sizeof( chunk ) }, Clock_GetTick() };
		Backlog_Write( log, &chunk.header, chunk.header.size );
	}

	static void Backlog_UnitTest( Backlog_s* log )
	{
	#if UNIT_TEST_BACKLOG
		// save state
		const int saved_pos = log->Pos;
		const int saved_count = log->Buffer[log->Pos].Count;

		// test 1: append 4k byte-wise
		const uint8_t fill = 0xff;
		const uint32_t size = sizeof(fill);
		const uint32_t count = 4096;
		for ( uint32_t i = 0; i < count; ++i )
		{
			if (!Backlog_Reserve( log, size ))
			{
				NeAssert(false);
			}
			Backlog_Write( log, &fill, size );
		}

		// test 2: append 10 full buffers
		Buffer_s buffer = {};
		Packet_Initialize( &buffer );
		buffer.Count = sizeof(buffer.Data) - buffer.Count;
		Packet_Finalize( &buffer );
		
		for ( int i = 0; i < 10; ++i )
		{
			if (!Backlog_Reserve( log, size ))
			{
				NeAssert(false);
			}
			Backlog_Write( log, &fill, size );
		}

		// test 3: consume the while backlog with alternating sizes
		const uint32_t test_sizes[] = { 1, 7, 8, 15, 16, 42, 127, 128, 255, 256, 1023, 1024, 4096 - sizeof(Packet) };
		for ( int i = 0; Backlog_Reserve( log, test_sizes[i] ); i = (i + 1) % NeCountOf(test_sizes) )
		{
			Backlog_Write( log, &fill, test_sizes[i] );
		}

		// restore state
		log->Pos = saved_pos;
		log->Buffer[log->Pos].Count = saved_count;

		// re-init log buffers
		for ( int i = saved_pos + 1; i < NeCountOf(log->Buffer); ++i )
			Packet_Initialize( log->Buffer + i );
	#endif
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void Backlog_Initialize( Backlog_s* log, Allocator_t alloc )
	{
		log->Buffer.Init( alloc );
		log->Buffer.Reserve( 64 );
		Backlog_Grow( log );
		Backlog_WriteConnectHeader( log );
	}

	Result_t Backlog_Append( Backlog_s* log, Buffer_t buffer )
	{
		if (buffer->Type != BufferType::Meta)
			return NE_OK;
		NeAssertOut(Packet_IsValid( buffer ), "Buffer has no packet header!");
		const uint32_t payload_offset = (uint32_t)(sizeof(Packet));
		const uint32_t payload_size   = buffer->Count - payload_offset;
		if (!Backlog_Reserve( log, payload_size ))
			return NE_ERR_OUT_OF_MEMORY;
		Backlog_Write( log, buffer->Data + payload_offset, payload_size );
		Backlog_UnitTest( log );
		return NE_OK;
	}

	void Backlog_Shutdown( Backlog_s* log )
	{
		Allocator_t alloc = log->Buffer.Alloc;
		const int count = log->Buffer.Count;
		for ( int i = 0; i < count; ++i )
			Mem_Free( alloc, log->Buffer[i] );
		log->Buffer.Clear();
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	static Result_t PeerList_SendBufferTo( Buffer_t buffer, const RemotePeer_s& peer )
	{
		return Tcp_Send( peer.Socket, buffer->Data, buffer->Count ) ? NE_OK : NE_ERROR;
	}

	static Result_t PeerList_SendBacklogTo( Backlog_s* log, RemotePeer_s& peer )
	{
		if (peer.Init)
			return NE_OK;
		Result_t hr;
		const int count = log->Buffer.Count;
		for ( int i = 0; i < count; ++i )
		{
			hr = PeerList_SendBufferTo( log->Buffer[i], peer );
			if (NeFailed(hr))
				return hr;
		}
		peer.Init = 1;
		return NE_OK;
	}

	Result_t PeerList_SendRemote( PeerList_s* list, Backlog_s* log, Buffer_t buffer, int index )
	{
		Result_t hr;
		hr = PeerList_SendBacklogTo( log, list->Remote[ index ] );
		if (NeFailed(hr))
			return hr;
		hr = PeerList_SendBufferTo( buffer, list->Remote[ index ] );
		if (NeFailed(hr))
			return hr;
		return NE_OK;
	}

	static Result_t PeerList_SendBufferTo( Buffer_t buffer, const LocalPeer_s& peer )
	{
		peer.Consumer.Consume( peer.Consumer.Context, buffer->Data, buffer->Count );
		return NE_OK;
	}

	static Result_t PeerList_SendBacklogTo( Backlog_s* log, LocalPeer_s& peer )
	{
		if (peer.Init)
			return NE_OK;
		Result_t hr;
		const int count = log->Buffer.Count;
		for ( int i = 0; i < count; ++i )
		{
			hr = PeerList_SendBufferTo( log->Buffer[i], peer );
			if (NeFailed(hr))
				return hr;
		}
		peer.Init = 1;
		return NE_OK;
	}

	static Result_t PeerList_SendLocal( PeerList_s* list, Backlog_s* log, Buffer_t buffer )
	{
		if (!list->Local.Consumer.Consume)
			return NE_OK;
		Result_t hr;
		hr = PeerList_SendBacklogTo( log, list->Local );
		if (NeFailed(hr))
			return hr;
		hr = PeerList_SendBufferTo( buffer, list->Local );
		if (NeFailed(hr))
			return hr;
		return NE_OK;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void PeerList_Initialize( PeerList_s* list )
	{
		CriticalSection_Create( list->Mutex );
	}

	bool PeerList_IsAttached( PeerList_s* list )
	{
		NeLock(list->Mutex);
		return list->Local.Consumer.Consume != nullptr;
	}

	void PeerList_Attach( PeerList_s* list, const Consumer_s& local )
	{
		NeLock(list->Mutex);
		list->Local.Consumer = local;
		list->Local.Init	 = 0;
	}

	void PeerList_Detach( PeerList_s* list )
	{
		NeLock(list->Mutex);
		NeZero(list->Local);
	}

	int PeerList_FindPeer( PeerList_s* list, Socket_t peer )
	{
		NeLock(list->Mutex);
		for ( int i = 0; i < list->NumRemote; ++i )
		{
			if (list->Remote[i].Socket == peer)
				return i;
		}
		return -1;
	}

	Result_t PeerList_Connect( PeerList_s* list, Socket_t peer )
	{
		NeLock(list->Mutex);
		if (list->NumRemote == MAX_NUM_REMOTE_PEERS)
			return NE_ERR_OUT_OF_MEMORY;
		const RemotePeer_s remote = { peer };
		list->Remote[ list->NumRemote++ ] = remote;
		return NE_OK;
	}

	Result_t PeerList_Disconnect( PeerList_s* list, Socket_t peer )
	{
		NeLock(list->Mutex);
		const int idx = PeerList_FindPeer( list, peer );
		if (idx < 0)
			return NE_ERR_NOT_FOUND;
		Socket_Stop	( list->Remote[ idx ].Socket );
		Socket_Close( list->Remote[ idx ].Socket );
		list->Remote[ idx ] = list->Remote[ --list->NumRemote ];
		return NE_OK;
	}

	void PeerList_Disconnect( PeerList_s* list )
	{
		NeLock(list->Mutex);
		for ( int i = 0; i < list->NumRemote; ++i )
		{
			Socket_Stop	( list->Remote[ i ].Socket );
			Socket_Close( list->Remote[ i ].Socket );
		}
		NeZero( list->Remote );
	}

	void PeerList_Shutdown( PeerList_s* list )
	{
		PeerList_Disconnect( list );
		CriticalSection_Destroy( list->Mutex );
	}

	void PeerList_Dispatch( PeerList_s* list, Backlog_s* log, Buffer_t buffer )
	{
		Result_t hr[ MAX_NUM_REMOTE_PEERS ] = {};
		NeLock(list->Mutex);
		int num_succeeded = 0;
		for ( int i = 0; i < list->NumRemote; ++i )
		{
			hr[i] = PeerList_SendRemote( list, log, buffer, i );
			if (NeSucceeded(hr[i]))
				++num_succeeded;
		}
		PeerList_SendLocal( list, log, buffer );

		if (num_succeeded == list->NumRemote)
			return;

		for ( int i = list->NumRemote-1; i >= 0; --i )
		{
			if (NeSucceeded(hr[i]))
				continue;
			Socket_Stop	( list->Remote[ i ].Socket );
			Socket_Close( list->Remote[ i ].Socket );
			list->Remote[ i ] = list->Remote[ --list->NumRemote ];
		}
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	static void Dispatcher_Run( Dispatcher_s* dispatcher )
	{
		Buffer_s buffer = {};
		DispatchItem_s item = {};
		while ( dispatcher->Worker.Continue )
		{
			if (DispatchQueue_Pop( &dispatcher->Queue, item ))
			{
				NeAssert(item.Buffer->Count <= BUFFER_SIZE);
				NeAssert(item.Buffer->Type == BufferType::Data || item.Buffer->Type == BufferType::Meta);

				// copy
				{
					Mem_Cpy( buffer.Data, item.Buffer->Data, item.Buffer->Count );
					buffer.Count = item.Buffer->Count;
					buffer.Type = item.Buffer->Type;
				}

				// release
				{
					BufferPool_FreeBuffer( item.Pool, item.Buffer );
				}

				// dispatch
				{
					PeerList_Dispatch( &dispatcher->PeerList, &dispatcher->Backlog, &buffer );
				}

				// log meta-data
				{
					Backlog_Append( &dispatcher->Backlog, &buffer );
				}
			}
		}
	}

	static void NE_CALLBK Dispatcher_Proc( void* dispatcher )
	{
		Dispatcher_Run( (Dispatcher_s*) dispatcher );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void Dispatcher_Initialize( Dispatcher_s* dispatcher, Allocator_t alloc )
	{
		DispatchQueue_Initialize( &dispatcher->Queue );
		PeerList_Initialize( &dispatcher->PeerList );
		Backlog_Initialize( &dispatcher->Backlog, alloc );
		const ThreadSetup_s thread_setup = { "[NePerf] Dispatcher", Dispatcher_Proc, dispatcher };
		Worker_Start( &dispatcher->Worker, thread_setup );
	}

	void Dispatcher_Attach( Dispatcher_s* dispatcher, const Consumer_s& local )
	{
		PeerList_Attach( &dispatcher->PeerList, local );
	}

	void Dispatcher_Connect( Dispatcher_s* dispatcher, Socket_t client )
	{
		PeerList_Connect( &dispatcher->PeerList, client );
	}

	void Dispatcher_Push( Dispatcher_s* dispatcher, const DispatchItem_s& item )
	{
		DispatchQueue_Push( &dispatcher->Queue, item );
	}

	void Dispatcher_Shutdown( Dispatcher_s* dispatcher )
	{
		Worker_Stop( &dispatcher->Worker );
			DispatchQueue_Close( &dispatcher->Queue );
		Worker_Wait( &dispatcher->Worker );
		DispatchQueue_Shutdown( &dispatcher->Queue );
		PeerList_Shutdown( &dispatcher->PeerList );
		Backlog_Shutdown( &dispatcher->Backlog );
	}

} }