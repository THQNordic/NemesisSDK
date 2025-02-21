//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "PacketQueue.h"

//======================================================================================
#include <Nemesis/Core/Process.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void PacketRing_Initialize( PacketRing_s& ring, Allocator_t alloc, int num_streams, int bytes_per_stream )
	{
		ring.ReadPos = 0;
		ring.WritePos = 0;
		ring.Stream.Alloc = alloc;
		ring.Stream.Resize( num_streams );
		for ( int i = 0; i < ring.Stream.Count; ++i )
			RecorderStream_Initialize( ring.Stream[i], ring.Stream.Alloc, bytes_per_stream );
	}

	void PacketRing_Push( PacketRing_s& ring, const Packet& packet, const Chunk* head )
	{
		if (ring.Stream.Count)
		{
			RecorderStream_s& item = ring.Stream[ring.WritePos];
			ring.WritePos = (ring.WritePos + 1) % ring.Stream.Count;
			RecorderStream_Reset( item );
			RecorderStream_Record( item, &packet, sizeof(Packet) );
			RecorderStream_Record( item, head, packet.DataSize() );
		}
	}

	void PacketRing_Pop( PacketRing_s& ring, RecorderStream_s& s )
	{
		if (ring.Stream.Count)
		{
			RecorderStream_s& item = ring.Stream[ring.ReadPos];
			ring.ReadPos = (ring.ReadPos + 1) % ring.Stream.Count;
			RecorderStream_Reset( s );
			RecorderStream_Record( s, item );
			item.Data.Reset();
		}
		else
		{
			RecorderStream_Reset( s );
		}
	}

	void PacketRing_Shutdown( PacketRing_s& ring )
	{
		for ( int i = 0; i < ring.Stream.Count; ++i )
			ring.Stream[i].Data.Clear();
		ring.Stream.Clear();
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void PacketQueue_Initialize( PacketQueue_s& queue, Allocator_t alloc, int num_streams, int bytes_per_stream )
	{
		NeZero(queue);

		PacketRing_Initialize( queue.Ring, alloc, num_streams, bytes_per_stream );

		queue.Work = Semaphore_Create( 0, num_streams );
		queue.Space = Semaphore_Create( num_streams, num_streams );
		CriticalSection_Create( queue.Mutex );
	}

	void PacketQueue_Push( PacketQueue_s& queue, const Packet& packet, const Chunk* head )
	{
		Semaphore_Wait( queue.Space );
		{
			NeLock(queue.Mutex);
			PacketRing_Push( queue.Ring, packet, head );
		}
		Semaphore_Signal( queue.Work, 1 );
	}

	void PacketQueue_Pop( PacketQueue_s& queue, RecorderStream_s& s )
	{
		Semaphore_Wait( queue.Work );
		{
			NeLock(queue.Mutex);
			PacketRing_Pop( queue.Ring, s );
		}
		Semaphore_Signal( queue.Space, 1 );
	}

	void PacketQueue_Close( PacketQueue_s& queue )
	{
		Semaphore_Signal( queue.Work, queue.Ring.Stream.Count );
	}

	void PacketQueue_Shutdown( PacketQueue_s& queue )
	{
		Semaphore_Destroy( queue.Space );
		Semaphore_Destroy( queue.Work );
		CriticalSection_Destroy( queue.Mutex );
		PacketRing_Shutdown( queue.Ring );
	}

} }
