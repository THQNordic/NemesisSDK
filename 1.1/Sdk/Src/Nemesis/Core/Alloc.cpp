//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Alloc.h>
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Logging.h>
#include <Nemesis/Core/Memory.h>
#include <Nemesis/Core/Allocator.h>
#include <stdlib.h>

//======================================================================================
namespace nemesis
{
	#if NE_CONFIG < NE_CONFIG_MASTER
	namespace global
	{
		class AllocatorAutoDump_c
		{
		public:
			~AllocatorAutoDump_c()
			{ 
				DebugAllocator_Dump(); 
			}
		};
		static AllocatorAutoDump_c AllocatorAutoDump;
	}
	#endif

	namespace global
	{
		static	Allocator_t	DefaultAllocator = nullptr;
	};
}

//======================================================================================
namespace nemesis
{
	void Allocator_Initialize( Allocator_t alloc )
	{
		if (!alloc)
			return;
		if (global::DefaultAllocator)
			return;
		global::DefaultAllocator = alloc;
	}

	Allocator_t Allocator_GetDefault()
	{
		return global::DefaultAllocator ? global::DefaultAllocator : Allocator_GetFallback();
	}

	void Allocator_Shutdown()
	{
		global::DefaultAllocator = nullptr;
	}
}

//======================================================================================
namespace nemesis
{
	static Allocator_t Allocator_Select( Allocator_t alloc )
	{
		return alloc ? alloc : Allocator_GetDefault();
	}

	ptr_t Mem_Alloc( Allocator_t alloc, size_t size )
	{
		return Allocator_Select( alloc )->Alloc( size );
	}

	ptr_t Mem_Calloc( Allocator_t alloc, size_t size )
	{
		ptr_t ptr = Mem_Alloc( alloc, size );
		if (ptr)
			Mem_Set( ptr, 0, size );
		return ptr;
	}

	ptr_t Mem_Realloc( Allocator_t alloc, ptr_t ptr, size_t size )
	{
		return Allocator_Select( alloc )->Realloc( ptr, size );
	}

	void Mem_Free( Allocator_t alloc, ptr_t ptr )
	{
		return Allocator_Select( alloc )->Free( ptr );
	}

	size_t Mem_SizeOf( Allocator_t alloc, ptr_t ptr )
	{
		return Allocator_Select( alloc )->SizeOf( ptr );
	}
}

//======================================================================================
namespace nemesis
{
	ptr_t Mem_Clone( Allocator_t alloc, cptr_t ptr, size_t size )
	{
		if (!ptr || !size)
			return nullptr;
		ptr_t clone = Mem_Alloc( alloc, size );
		Mem_Cpy( clone, ptr, size );
		return clone;
	}
} 
