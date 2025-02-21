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
#include <Nemesis/Perf/Server.h>
#include <Nemesis/Perf/Protocol.h>
#include <Nemesis/Perf/VisualizerTypes.h>
#include <Nemesis/Core/ProcessTypes.h>
#include <Nemesis/Core/SocketTypes.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	typedef struct Server_s			*Server_t;
	typedef struct Sender_s			*Sender_t;
	typedef struct Buffer_s			*Buffer_t;
	typedef struct BufferPool_s		*BufferPool_t;
	typedef struct MainRecorder_s	*MainRecorder_t;
	typedef struct ThreadRecorder_s	*ThreadRecorder_t;

	using system::TlsId_t;
	using system::Event_t;
	using system::Socket_t;
	using system::Semaphore_t;
	using system::CriticalSection_t;
	typedef nemesis::NamedLocation CallSite_s;

} }
