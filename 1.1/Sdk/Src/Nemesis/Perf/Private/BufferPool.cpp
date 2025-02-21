//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "BufferPool.h"

//======================================================================================
#include <Nemesis/Core/Process.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	void BufferPool_Initialize( BufferPool_s* pool )
	{
		CriticalSection_Create( pool->Mutex );
		FreeList_Initialize( pool->Buffer );
	}

	Buffer_s* BufferPool_AllocBuffer( BufferPool_s* pool, BufferType::Enum type )
	{
		Buffer_s* buffer = nullptr;
		{
			NeLock( pool->Mutex );
			buffer = FreeList_Alloc( pool->Buffer );
			NeAssertOut( buffer, "BufferPool overflow!" );
			if (buffer)
			{
				++pool->NumBuffers;
				pool->PeakNumBuffers = NeMax(pool->PeakNumBuffers, pool->NumBuffers);
			}
		}
		if (buffer)
			Buffer_Initialize( buffer, type );
		return buffer;
	}

	void BufferPool_FreeBuffer( BufferPool_s* pool, Buffer_s* buffer )
	{
		NeLock( pool->Mutex );
		FreeList_Free( pool->Buffer, buffer );
		--pool->NumBuffers;
	}

	void BufferPool_Shutdown( BufferPool_s* pool )
	{
		CriticalSection_Destroy( pool->Mutex );
	}

} }