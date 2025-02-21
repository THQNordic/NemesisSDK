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
#include "ParserData.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ParserState_s
	{
		Database_t Db;
		viz::Frame OpenFrame;
		BinaryArrayMap<uint64_t, cstr_t> Names;
		BinaryArrayMap<uint64_t, int> Locations;
		uint8_t ZoneLevels[ MAX_NUM_THREADS ];
		uint32_t Reset : 1;
	};

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ParserInstance_s
	{
		ParserState_s		State;
		ParsedData_s		ParsedChunks;
		ParsedData_s		ParsedFrames;
		CriticalSection_t	ParsedFramesMutex;
	};

	void ParserInstance_Initialize	( ParserInstance_s& instance, Allocator_t alloc, Database_t db );
	void ParserInstance_Shutdown	( ParserInstance_s& instance );
	void ParserInstance_Reset		( ParserInstance_s& instance );
	void ParserInstance_ParseChunks	( ParserInstance_s& instance, const Chunk* head, uint32_t size, bool big_endian );
	void ParserInstance_JoinFrames	( ParserInstance_s& instance, ParsedData_s& joined );

} }
