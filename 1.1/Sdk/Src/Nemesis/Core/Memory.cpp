//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <memory.h>
#include <Nemesis/Core/Assert.h>

//======================================================================================
namespace nemesis
{
	void* Mem_Zero( void* dst, size_t size )
	{ return memset( dst, 0, size ); }

	void* Mem_Set( void* dst, int val, size_t size )
	{ return memset( dst, val, size ); }

	void* Mem_Cpy( void* dst, const void* src, size_t size )
	{ return memcpy( dst, src, size ); }

	void* Mem_Cpy( void* dst, size_t dst_size, const void* src, size_t src_size ) 
	{ 
		NeAssertOut( dst_size >= src_size, "Buffer Overflow! Destination buffer will be truncated." );
		const size_t truncated_size = NeMin( dst_size, src_size );
		return Mem_Cpy( dst, src, truncated_size ); 
	}

	void* Mem_Mov( void* dst, const void* src, size_t size )
	{ return memmove( dst, src, size );	}

	int	Mem_Cmp( const void* lhs, const void* rhs, size_t size )
	{ return memcmp( lhs, rhs, size ); }

}
