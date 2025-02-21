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
#include "PacketQueue.h"
#include "ParserState.h"
#include "Worker.h"

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Parser_s
	{
		Allocator_t Alloc;
		Worker_s Worker;
		PacketQueue_s Queue;
		ParserInstance_s Instance;
		int32_t Delay;
		int32_t Paused;
	};

	void Parser_Initialize( Parser_t parser, Allocator_t alloc, const ParserSetup& setup );
	bool Parser_IsPaused( Parser_t parser );
	void Parser_Pause( Parser_t parser, bool pause );
	void Parser_SetDebugDelay( Parser_t parser, int ms );
	void Parser_QueueData( Parser_t parser, const Packet& packet, const Chunk* head, Parse::Mode mode );
	void Parser_QueueReset( Parser_t parser );
	void Parser_JoinData( Parser_t parser );
	void Parser_Shutdown( Parser_t parser );
	
} }
