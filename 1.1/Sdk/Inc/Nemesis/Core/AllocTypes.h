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

//======================================================================================
namespace nemesis
{
	struct Allocator_s;
	typedef Allocator_s* Allocator_t;

	struct Allocator_v
	{
		void*  (NE_CALLBK *Realloc)( Allocator_t alloc, void* ptr, size_t size );
		size_t (NE_CALLBK *SizeOf) ( Allocator_t alloc, void* ptr );
	};

	struct Allocator_s
	{
		const Allocator_v* Api;

		void*  Realloc( void* ptr, size_t size ) { return Api->Realloc( this, ptr, size ); }
		void*  Alloc  ( size_t size )			 { return Realloc( nullptr, size ); }
		void   Free	  ( void* ptr )				 { if (ptr) Realloc( ptr, 0 ); }
		size_t SizeOf ( void* ptr )				 { return Api->SizeOf ( this, ptr ); }
	};
}
