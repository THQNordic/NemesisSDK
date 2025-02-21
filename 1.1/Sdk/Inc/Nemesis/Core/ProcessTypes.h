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
namespace nemesis {	namespace system
{
	/// Cpu

	typedef int64_t		Tick_t;
	typedef uint32_t	CpuId_t;

	/// Thread

	typedef Handle_t	Thread_t;
	typedef uint32_t	ThreadId_t;
	typedef void (NE_CALLBK *ThreadProc)( void* context );

	struct ThreadSetup_s
	{
		const char* Name;
		ThreadProc Proc;
		void* Context;
	};

	/// Thread Local Storage

	typedef uint32_t TlsId_t;

	/// Synchronization Objects

	typedef Handle_t Event_t;
	typedef Handle_t Semaphore_t;

	typedef struct	{ uint8_t Data[64]; } CriticalSection_t;

} }
