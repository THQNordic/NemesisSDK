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
	void NE_API Allocator_Initialize( Allocator_t alloc );
	void NE_API Allocator_Shutdown();
}

//======================================================================================
namespace nemesis
{
	ptr_t	NE_API Mem_Alloc  ( Allocator_t alloc,			  size_t size );
	ptr_t	NE_API Mem_Calloc ( Allocator_t alloc,			  size_t size );
	ptr_t	NE_API Mem_Realloc( Allocator_t alloc, ptr_t ptr, size_t size );
	void	NE_API Mem_Free	  ( Allocator_t alloc, ptr_t ptr );
	size_t	NE_API Mem_SizeOf ( Allocator_t alloc, ptr_t ptr );
}

//======================================================================================
namespace nemesis
{
	ptr_t NE_API Mem_Clone( Allocator_t alloc, cptr_t ptr, size_t size );
} 

//======================================================================================
namespace nemesis
{
	template < typename T >inline T* Mem_Alloc ( Allocator_t alloc )			{ return (T*) Mem_Alloc ( alloc,	   sizeof(T) ); }
	template < typename T >inline T* Mem_Alloc ( Allocator_t alloc, int num )	{ return (T*) Mem_Alloc ( alloc, num * sizeof(T) ); }
	template < typename T >inline T* Mem_Calloc( Allocator_t alloc )			{ return (T*) Mem_Calloc( alloc,	   sizeof(T) ); }
	template < typename T >inline T* Mem_Calloc( Allocator_t alloc, int num )	{ return (T*) Mem_Calloc( alloc, num * sizeof(T) ); }
}

//======================================================================================
#define NeStackAlloc(  T, num ) ((T*)		    alloca( (num) * sizeof(T) ))
#define NeStackCalloc( T, num ) ((T*) Mem_Zero( alloca( (num) * sizeof(T) ), (num) * sizeof(T) ))
