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
#include "Buffer.h"
#include "Constants.h"
#include "FreeList.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	struct BufferPool_s
	{
		CriticalSection_t Mutex;
		uint32_t NumBuffers;
		uint32_t PeakNumBuffers;
		FreeList<Buffer_s, MAX_NUM_POOLED_BUFFERS> Buffer;
	};

	void		BufferPool_Initialize ( BufferPool_s* pool );
	Buffer_s*	BufferPool_AllocBuffer( BufferPool_s* pool, BufferType::Enum type );
	void		BufferPool_FreeBuffer ( BufferPool_s* pool, Buffer_s* buffer );
	void		BufferPool_Shutdown	  ( BufferPool_s* pool );

} }

