//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Parser.h"

//======================================================================================
#include "Database.h"

//======================================================================================
#include <Nemesis/Core/Process.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling
{
	static Packet MakeResetPacket()
	{
		const Packet packet = 
		{ { chunk::Type::Packet, sizeof(packet) }
		, 0
		};
		return packet;
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	static void Parser_Delay( Parser_t parser )
	{
	#if (NE_CONFIG <= NE_CONFIG_DEBUG)
		if (!parser->Delay)
			return;
		Thread_SleepMs( parser->Delay );
	#endif
	}

	static void Parser_Process( Parser_t parser, PacketBuffer_s& item )
	{
		if (item.Packet->DataSize())
		{
			ParserInstance_ParseChunks( parser->Instance, (const Chunk*)(item.Packet+1), item.Packet->DataSize(), NeHasFlag( item.Packet->flags, Packet::BigEndian ) );
			Parser_Delay( parser );
		}
		else
		{
			ParserInstance_Reset( parser->Instance );
		}
	}

	static void Parser_Run( Parser_t parser )
	{
		RecorderStream_s s;
		RecorderStream_Initialize( s, parser->Alloc, 1024*1024 );

		PacketBuffer_s p;
		for ( ; parser->Worker.Continue ; )
		{
			PacketQueue_Pop( parser->Queue, s );
			if (!s.Data.Count)
				continue;
			p.Packet = (Packet*)s.Data.Data;
			Parser_Process( parser, p );
		}
	}

	static void NE_CALLBK Parser_Proc( void* arg )
	{
		Parser_t parser = (Parser_t)arg;
		Parser_Run( parser );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	void Parser_Initialize( Parser_t parser, Allocator_t alloc, const ParserSetup& setup )
	{
		parser->Alloc = alloc;
		ParserInstance_Initialize( parser->Instance, alloc, setup.Database );
		PacketQueue_Initialize( parser->Queue, alloc, 8, 8*64*1024 );

		const ThreadSetup_s thread_setup = { "[NePerf] Parser", Parser_Proc, parser };
		Worker_Start( &parser->Worker, thread_setup );
	}

	void Parser_Shutdown( Parser_t parser )
	{
		Worker_Stop( &parser->Worker );
			PacketQueue_Close( parser->Queue );
		Worker_Wait( &parser->Worker );
		PacketQueue_Shutdown( parser->Queue );
		ParserInstance_Shutdown( parser->Instance );
	}

	bool Parser_IsPaused( Parser_t parser )
	{
		return parser->Paused != 0;
	}

	void Parser_Pause( Parser_t parser, bool pause )
	{
		parser->Paused = pause ? 1 : 0;
	}

	void Parser_SetDebugDelay( Parser_t parser, int ms )
	{
		parser->Delay = ms;
	}

	void Parser_QueueData( Parser_t parser, const Packet& packet, const Chunk* head, Parse::Mode mode )
	{
		PacketQueue_Push( parser->Queue, packet, head );
	}

	void Parser_QueueReset( Parser_t parser )
	{
		PacketQueue_Push( parser->Queue, MakeResetPacket(), nullptr );
	}

	void Parser_JoinData( Parser_t parser )
	{
		if (!parser->Instance.ParsedFrames.Frames.Count)
			return;

		if (parser->Paused)
			return;

		const Limit_s limit = { parser->Instance.State.Db->Setup.MaxNumBytes, parser->Instance.State.Db->Setup.MaxNumFrames };
		ParsedData_MakeRoom( parser->Instance.State.Db->Data, limit, parser->Instance.ParsedFrames.Frames.Count, parser->Instance.ParsedFrames.TotalSize() );
		ParserInstance_JoinFrames( parser->Instance, parser->Instance.State.Db->Data );
	}

} }
