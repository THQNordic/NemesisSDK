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
#include "AllocTypes.h"

//======================================================================================
namespace nemesis
{
	Allocator_t NE_API Allocator_GetCrt();
	Allocator_t NE_API Allocator_GetEop();
	Allocator_t NE_API Allocator_GetDebug();
	Allocator_t NE_API Allocator_GetFallback();

	void NE_API DebugAllocator_Dump();
}

//======================================================================================
namespace nemesis
{
	typedef struct CountingAllocator_s* CountingAllocator_t;

	struct CountingAllocator_s
	{
		Allocator_s Header;
		Allocator_t Parent;
		cstr_t Name;
		size_t TotalCalls;
		size_t TotalBytes;
		size_t PeakBytes;
	};

	CountingAllocator_s NE_API CountingAllocator_Create	( Allocator_t parent, cstr_t name );
	void				NE_API CountingAllocator_Dump	( CountingAllocator_t alloc );

}

//======================================================================================
namespace nemesis
{
	typedef struct TrackingAllocator_s* TrackingAllocator_t;

	struct TrackingTrace_s
	{
		ptr_t Frames[8];
		int32_t NumFrames;
	};

	struct TrackingAllocator_s
	{
		Allocator_s Header;
		Allocator_t Parent;
		cstr_t Name;
		size_t TotalCalls;
		size_t TotalBytes;
		size_t PeakBytes;
		ptr_t* Items;
		int32_t ItemCount;
		int32_t ItemCapacity;
		TrackingTrace_s* Traces;
		int32_t TraceCount;
		int32_t TraceCapacity;
	};

	TrackingAllocator_s	NE_API TrackingAllocator_Create	( Allocator_t parent, cstr_t name );
	void				NE_API TrackingAllocator_Dump	( TrackingAllocator_t alloc );

} 