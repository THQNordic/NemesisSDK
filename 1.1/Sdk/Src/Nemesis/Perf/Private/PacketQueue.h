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
#include <Nemesis/Perf/Protocol.h>

//======================================================================================
#include "Types.h"

//======================================================================================
#include <Nemesis/Core/Array.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	struct RecorderStream_s
	{
		Array<uint8_t> Data;
	};

	inline void RecorderStream_Initialize( RecorderStream_s& s, Allocator_t alloc, int capacity )
	{
		s.Data.Alloc = alloc;
		s.Data.Reserve( capacity ); 
	}

	inline void RecorderStream_Shutdown( RecorderStream_s& s )
	{
		s.Data.Clear();
	}

	inline int RecorderStream_Remain( const RecorderStream_s& s )
	{
		return s.Data.Capacity - s.Data.Count;
	}

	inline void RecorderStream_Reset( RecorderStream_s& s )
	{
		return s.Data.Reset();
	}

	inline void RecorderStream_Record( RecorderStream_s& s, cptr_t data, int size )
	{
		return s.Data.Append( (const uint8_t*)data, size );
	}

	inline void RecorderStream_Record( RecorderStream_s& s, const RecorderStream_s& from )
	{
		RecorderStream_Record( s, from.Data.Data, from.Data.Count );
	}

	inline bool RecorderStream_TryRecord( RecorderStream_s& s, cptr_t data, int size )
	{
		if (RecorderStream_Remain( s ) >= size)
		{
			RecorderStream_Record( s, data, size );
			return true;
		}
		return false;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct PacketBuffer_s
	{
		Packet* Packet;
	};

	struct PacketRing_s
	{
		int ReadPos;
		int WritePos;
		Array<RecorderStream_s> Stream;
	};

	void PacketRing_Initialize( PacketRing_s& ring, Allocator_t alloc, int num_streams, int bytes_per_stream );
	void PacketRing_Push( PacketRing_s& ring, const Packet& packet, const Chunk* head );
	void PacketRing_Pop( PacketRing_s& ring, RecorderStream_s& s );
	void PacketRing_Shutdown( PacketRing_s& rings );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct PacketQueue_s
	{
		system::Semaphore_t Work;
		system::Semaphore_t Space;
		system::CriticalSection_t Mutex;
		PacketRing_s Ring;
	};

	void PacketQueue_Initialize( PacketQueue_s& queue, Allocator_t alloc, int num_streams, int bytes_per_stream );
	void PacketQueue_Push( PacketQueue_s& queue, const Packet& packet, const Chunk* head );
	void PacketQueue_Pop( PacketQueue_s& queue, RecorderStream_s& s );
	void PacketQueue_Close( PacketQueue_s& queue );
	void PacketQueue_Shutdown( PacketQueue_s& queue );

} }
