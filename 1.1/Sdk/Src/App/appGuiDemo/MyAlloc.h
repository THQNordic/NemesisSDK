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
#include <Nemesis/Core/AllocTypes.h>
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Logging.h>

//======================================================================================
struct MyAlloc
{
public:
	explicit MyAlloc( const char* name )
		: Name( name ) 
	{
		Api.Realloc = Realloc;
		Api.SizeOf = SizeOf;
		Header.Api = &Api;
		TotalBytes = 0;
		PeakBytes = 0;
	}

	~MyAlloc()
	{
		NeTrace( "*************************\n" );
		NeTrace( "*** Allocator: '%s'\n", Name );
		NeTrace( "*** Peak Bytes: %u\n", PeakBytes );
		NeTrace( "*** Leak Bytes: %u\n", TotalBytes );
	}

public:
	static void* NE_CALLBK Realloc( ne::Allocator_t alloc, void* ptr, size_t size )
	{
		size_t old_size = SizeOf( alloc, ptr );
		void*  new_ptr	= realloc( ptr, size );
		size_t new_size = SizeOf( alloc, new_ptr );

		MyAlloc* instance = (MyAlloc*)alloc;
		instance->TotalBytes -= ptr     ? old_size : 0;
		instance->TotalBytes += new_ptr ? new_size : 0;
		instance->PeakBytes = NeMax( instance->PeakBytes, instance->TotalBytes );

		return new_ptr;
	}

	static size_t NE_CALLBK SizeOf( ne::Allocator_t alloc, void* ptr ) 
	{
		return ptr ? _msize( ptr ) : 0;
	}

public:
	ne::Allocator_s Header;
	ne::Allocator_v Api;
	size_t TotalBytes;
	size_t PeakBytes;
	ne::cstr_t Name;
};
