//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "MyPerf.h"
#include "MyAlloc.h"
#include <Nemesis/Perf/All.h>

//======================================================================================
using namespace nemesis;
using namespace nemesis::system;
using namespace nemesis::profiling;

//======================================================================================
struct MyPerf_s
{
	Parser_t	Parser;
	Database_t  Database;
	Receiver_t	Receiver;
};

static MyPerf_s PerfClient = {};
static MyAlloc  PerfClientAlloc( "PerfClient" );

//======================================================================================
static void NE_CALLBK PacketReceived( void* context, system::Socket_t client, const profiling::Packet& packet, const profiling::Chunk* head )
{
	Parser_ParseData( (Parser_t)context, packet, head, profiling::Parse::Buffered );
}

static void NE_CALLBK DataReceived( void* context, const uint8_t* data, uint32_t size )
{
	const profiling::Packet* packet = (profiling::Packet*)(data);
	const profiling::Chunk* head	= (profiling::Chunk*)(packet+1);
	Parser_ParseData( (Parser_t)context, *packet, head, profiling::Parse::Buffered );
}

//======================================================================================
void MyPerf::Initialize()
{
	// allocator
	Allocator_t alloc = &PerfClientAlloc.Header;

	// db
	DatabaseSetup_s db_setup = {};
	PerfClient.Database = Database_Create( alloc, db_setup );

	// parser
	ParserSetup parser_setup = { PerfClient.Database };
	PerfClient.Parser = Parser_Create( alloc, parser_setup );

	// consumer
	Consumer_s consumer = { DataReceived, PerfClient.Parser };
	Server_SetConsumer( consumer );
	/*
	const system::IpAddress_t host = { { 127, 0, 0, 1 }, 16001 };
	const profiling::ReceiverCallback callback = { PacketReceived, PerfClient.Parser };
	PerfClient.Receiver = Receiver_Create( alloc );
	Receiver_Connect( PerfClient.Receiver, host, callback );
	*/
}

void MyPerf::Shutdown()
{
	Receiver_Destroy( PerfClient.Receiver );
	Parser_Destroy	( PerfClient.Parser );
	Database_Destroy( PerfClient.Database );
	PerfClient.Receiver = nullptr;
	PerfClient.Parser	= nullptr;
	PerfClient.Database = nullptr;
}

Database_t MyPerf::GetDatabase()
{
	return PerfClient.Database;
}

Parser_t MyPerf::GetParser()
{
	return PerfClient.Parser;
}

void MyPerf::NextFrame()
{
	Parser_JoinData( PerfClient.Parser );
}
