//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Allocator.h>
#include <Nemesis/Core/Alloc.h>
#include <Nemesis/Core/Memory.h>
#include <Nemesis/Core/VMem.h>
#include <Nemesis/Core/Logging.h>
#include <Nemesis/Core/Debug.h>
#include <Nemesis/Core/Array.h>
#include <stdlib.h>

//======================================================================================
// @note:
//	the tracking allocator isn't thread-safe.
//	use with caution.
#define USE_TRACKING_ALLOC 0

//======================================================================================
//																					 Crt
//======================================================================================
namespace nemesis
{
	static ptr_t NE_CALLBK Crt_Realloc( Allocator_t, ptr_t ptr, size_t size )
	{
		if (ptr == nullptr)
		{
			return malloc( size );
		}

		if (size == 0)
		{
			free( ptr );
			return nullptr;
		}

		return realloc( ptr, size );
	}

	static size_t NE_CALLBK Crt_SizeOf( Allocator_t, ptr_t ptr )
	{
		if (!ptr)
			return 0;

	#if NE_PLATFORM_IS_WINDOWS
		return _msize( ptr );
	#else
		return malloc_usable_size( ptr );
	#endif
	}

	static const Allocator_v CrtApi = { Crt_Realloc, Crt_SizeOf };
	static Allocator_s CrtAlloc = { &CrtApi };
} 

//======================================================================================
//																					 Eop
//======================================================================================
namespace nemesis
{
	static ptr_t NE_CALLBK Eop_Realloc( Allocator_t, ptr_t ptr, size_t size )
	{
		if (ptr == nullptr)
		{
			return Eop_Alloc( size );
		}

		if (size == 0)
		{
			Eop_Free( ptr );
			return nullptr;
		}

		const size_t old_size = Eop_SizeOf( ptr );
		const size_t new_size = size;
		ptr_t old_ptr = ptr;
		ptr_t new_ptr = Eop_Alloc( new_size );
		Mem_Cpy( new_ptr, old_ptr, NeMin( old_size, new_size ) );
		Eop_Free( old_ptr );
		return new_ptr;
	}

	static size_t NE_CALLBK Eop_SizeOf( Allocator_t, ptr_t ptr )
	{
		if (!ptr)
			return 0;
		return Eop_SizeOf( ptr );
	}

	static const Allocator_v EopApi = { Eop_Realloc, Eop_SizeOf };
	static Allocator_s EopAlloc = { &EopApi };
}

//======================================================================================
//																				Counting
//======================================================================================
namespace nemesis
{
	static ptr_t NE_CALLBK Counting_Realloc( Allocator_t alloc, ptr_t ptr, size_t size )
	{
		CountingAllocator_t instance = (CountingAllocator_t)alloc;
		size_t old_size = Mem_SizeOf ( instance->Parent, ptr );
		void*  new_ptr	= Mem_Realloc( instance->Parent, ptr, size );
		size_t new_size = Mem_SizeOf ( instance->Parent, new_ptr );
		// @todo: atomic
		instance->TotalBytes -= ptr     ? old_size : 0;
		instance->TotalBytes += new_ptr ? new_size : 0;
		instance->PeakBytes  = NeMax( instance->PeakBytes, instance->TotalBytes );
		instance->TotalCalls ++; 
		return new_ptr;
	}

	static size_t NE_CALLBK Counting_SizeOf( Allocator_t alloc, ptr_t ptr )
	{
		CountingAllocator_t instance = (CountingAllocator_t)alloc;
		return Mem_SizeOf( instance->Parent, ptr );
	}

	static const Allocator_v CountingApi = { Counting_Realloc, Counting_SizeOf };
}

//======================================================================================
//																				Tracking
//======================================================================================
namespace nemesis
{
	static ptr_t NE_CALLBK Tracking_Realloc( Allocator_t alloc, ptr_t ptr, size_t size )
	{
		TrackingAllocator_t instance = (TrackingAllocator_t)alloc;
		size_t old_size = Mem_SizeOf ( instance->Parent, ptr );
		void*  new_ptr	= Mem_Realloc( instance->Parent, ptr, size );
		size_t new_size = Mem_SizeOf ( instance->Parent, new_ptr );
		// @todo: atomic
		instance->TotalBytes -= ptr     ? old_size : 0;
		instance->TotalBytes += new_ptr ? new_size : 0;
		instance->PeakBytes  = NeMax( instance->PeakBytes, instance->TotalBytes );
		instance->TotalCalls ++; 

		uint32_t hash = 0;
		TrackingTrace_s trace;
		NeZero(trace);
		if (new_ptr)
			trace.NumFrames = system::StackTrace_Capture( 4, NeCountOf(trace.Frames), trace.Frames, &hash );

		// @todo:
		//	implement the array member functions in terms of 
		//	free functions and use these directly

		// @note:
		//	since we can't store array directly in the allocator's header
		//	we have to set them up on the fly
		Array<ptr_t> items;
		Array<TrackingTrace_s> traces;
		items.Alloc = instance->Parent;
		items.Data = instance->Items;
		items.Count = instance->ItemCount;
		items.Capacity = instance->ItemCapacity;
		traces.Alloc = instance->Parent;
		traces.Data = instance->Traces;
		traces.Count = instance->TraceCount;
		traces.Capacity = instance->TraceCapacity;

		if (new_ptr != ptr)
		{
			if (ptr)
			{
				const int idx = Array_BinaryFind( items, ptr );
				NeAssert(idx >= 0);
				items.RemoveAt( idx );
				traces.RemoveAt( idx );
			}
			if (new_ptr)
			{
				const int idx = Array_BinaryFind( items, new_ptr );
				NeAssert(idx < 0);
				items.InsertAt( ~idx, new_ptr );
				traces.InsertAt( ~idx, trace );
			}
		}
		else
		{
			const int idx = Array_BinaryFind( items, ptr );
			NeAssert(items[idx] == ptr);
			traces[idx] = trace;
		}

		// @note:
		//	store the array fields
		instance->Items = items.Data;
		instance->ItemCount = items.Count;
		instance->ItemCapacity = items.Capacity;
		instance->Traces = traces.Data;
		instance->TraceCount = traces.Count;
		instance->TraceCapacity = traces.Capacity;

		// @note:
		//	detach the arrays
		//	in order to prevent destruction
		items.Data = nullptr;
		items.Count = 0;
		items.Capacity = 0;
		traces.Data = nullptr;
		traces.Count = 0;
		traces.Capacity = 0;

		return new_ptr;
	}

	static size_t NE_CALLBK Tracking_SizeOf( Allocator_t alloc, ptr_t ptr )
	{
		TrackingAllocator_t instance = (TrackingAllocator_t)alloc;
		return Mem_SizeOf( instance->Parent, ptr );
	}

	static const Allocator_v TrackingApi = { Tracking_Realloc, Tracking_SizeOf };
}

//======================================================================================
//																				   Debug
//======================================================================================
namespace nemesis
{
#if USE_TRACKING_ALLOC
	static TrackingAllocator_s DebugAlloc = { { &TrackingApi }, &CrtAlloc, "Nemesis" };
#else
	static CountingAllocator_s DebugAlloc = { { &CountingApi }, &CrtAlloc, "Nemesis" }; 
#endif
}

//======================================================================================
//																					 Api
//======================================================================================
namespace nemesis
{
	Allocator_t Allocator_GetCrt()
	{
		return &CrtAlloc;
	}

	Allocator_t Allocator_GetEop()
	{
		return &EopAlloc;
	}

	Allocator_t Allocator_GetDebug()
	{
		return &DebugAlloc.Header;
	}

	Allocator_t Allocator_GetFallback()
	{
	#if NE_CONFIG < NE_CONFIG_MASTER
		return Allocator_GetDebug();
	#else
		return Allocator_GetCrt();
	#endif
	}

	void DebugAllocator_Dump()
	{
	#if USE_TRACKING_ALLOC
		TrackingAllocator_Dump( &DebugAlloc );
	#else
		CountingAllocator_Dump( &DebugAlloc );
	#endif
	}

}

//======================================================================================
namespace nemesis
{
	CountingAllocator_s CountingAllocator_Create( Allocator_t parent, cstr_t name )
	{
		return CountingAllocator_s { { &CountingApi }, parent, name };
	}

	void CountingAllocator_Dump( CountingAllocator_t alloc )
	{
		NeTrace( "*************************\n" );
		NeTrace( "*** Allocator: '%s'\n", alloc->Name );
		NeTrace( "*** Num Calls:  %u\n", alloc->TotalCalls );
		NeTrace( "*** Peak Bytes: %u\n", alloc->PeakBytes  );
		NeTrace( "*** Leak Bytes: %u\n", alloc->TotalBytes );
	}
} 

//======================================================================================
namespace nemesis
{
	TrackingAllocator_s TrackingAllocator_Create( Allocator_t parent, cstr_t name )
	{
		return TrackingAllocator_s { { &TrackingApi }, parent, name };
	}

	void TrackingAllocator_Dump( TrackingAllocator_t alloc )
	{
		NeTrace( "*************************\n" );
		NeTrace( "*** Allocator: '%s'\n", alloc->Name );
		NeTrace( "*** Num Calls:  %u\n", alloc->TotalCalls );
		NeTrace( "*** Peak Bytes: %u\n", alloc->PeakBytes  );
		NeTrace( "*** Leak Bytes: %u\n", alloc->TotalBytes );

		system::Pdb_Initialize();
		for ( int i = 0; i < alloc->ItemCount; ++i )
		{
			NeTrace( "[%d]: 0x%p\n", i, alloc->Items[i] );
			const TrackingTrace_s& trace = alloc->Traces[i];

			uint64_t sym_offset = 0;
			system::PdbSymbolInfo_s sym_info = {};
			for ( int j = 0; j < alloc->Traces[i].NumFrames; ++j )
			{
				system::Pdb_FindSymbolInfoByAddress( trace.Frames[j], &sym_info, &sym_offset );
				NeTrace( "  %s+%u64\n", sym_info.Name, sym_offset );
			}
		}
	}
} 
