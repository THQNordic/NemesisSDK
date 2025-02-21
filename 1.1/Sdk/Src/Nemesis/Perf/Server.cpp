//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"

//======================================================================================
#include "Private/Recorder.h"
#include "Private/Sender.h"

//======================================================================================
#include <Nemesis/Core/Alloc.h>
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/Socket.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Server_s
	{
		Allocator_t		Alloc;
		MainRecorder_s	Recorder;
		Sender_s		Sender;
		Socket_t		Responder;
	};

	void Server_Initialize( Server_t server, Allocator_t alloc )
	{
		server->Alloc = alloc;
		Sender_Initialize( &server->Sender, alloc );
		MainRecorder_Initialize( &server->Recorder, alloc, &server->Sender );
	}

	void Server_Shutdown( Server_t server )
	{
		Udp_Close( server->Responder );
		Sender_Shutdown( &server->Sender );
		MainRecorder_Shutdown( &server->Recorder );
	}

	void Server_StartSender( Server_t server, system::IpPort_t port )
	{
		Result_t hr = Sender_Start( &server->Sender, port );
		if (NeFailed(hr))
			return;
		server->Responder = Udp_Open( port, SocketOption::NonBlocking );
	}

	void Server_StopSender( Server_t server )
	{
		Sender_Stop( &server->Sender );
		Udp_Close( server->Responder );
		server->Responder = nullptr;
	}

	void Server_Respond( Server_t server )
	{
		ping::Header hdr = {};
		IpAddress_t from = {};
		if (!Udp_Receive( server->Responder, &from, &hdr, sizeof(hdr) ))
			return;
		if (!hdr.IsValid())
			return;
		ping::Response ack = {};
		ack.Header.Init();
		Udp_Send( server->Responder, from, &ack, sizeof(ack) );
	}

	void Server_NextFrame( Server_t server )
	{
		MainRecorder_NextFrame( &server->Recorder );
		Server_Respond( server );
	}

	void Server_GetStats( Server_t server, ServerStats_s& stats )
	{
		NeZero(stats);
		RecorderStats_s recorder_stats;
		MainRecorder_GetStats( &server->Recorder, recorder_stats );
		stats.Value[ ServerStat::MaxThreads ] = MAX_NUM_THREADS;
		stats.Value[ ServerStat::NumThreads ] = recorder_stats.NumThreads;
		for ( uint32_t i = 0; i < recorder_stats.NumThreads; ++i )
		{
			stats.Value[ ServerStat::NumNames  ] += recorder_stats.Thread[ i ].NumNames;
			stats.Value[ ServerStat::NumScopes ] += recorder_stats.Thread[ i ].NumLocations;
			stats.Value[ ServerStat::NumLocks  ] += recorder_stats.Thread[ i ].NumLocks;
		}

		const Backlog_s* backlog = &server->Sender.Dispatcher.Backlog;
		const size_t backlog_capacity = backlog->Buffer.Count * sizeof(Buffer_s::Data);
		size_t backlog_size = 0;
		const int num_buffers = backlog->Buffer.Count;
		for ( int i = 0; i < num_buffers; ++i )
			backlog_size += backlog->Buffer[i]->Count;

		stats.Value[ ServerStat::BacklogCapacity ] = (uint32_t)backlog_capacity;
		stats.Value[ ServerStat::BacklogSize     ] = (uint32_t)backlog_size;
	}

	void Server_SetConsumer( Server_t server, const Consumer_s& consumer )
	{
		Sender_Attach( &server->Sender, consumer );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling 
{ 
	//==================================================================================
	Result_t Server_Create( Allocator_t alloc, Server_t* server )
	{ 
		if (!server)
			return NE_ERR_INVALID_CALL;
		*server = Mem_Calloc<Server_s>( alloc );
		Server_Initialize( *server, alloc );
		Server_SetThreadInfo( *server, "Main Thread" );
		return NE_OK;
	}

	void Server_Release( Server_t server )
	{
		if (!server)
			return;
		Allocator_t alloc = server->Alloc;
		Server_Shutdown( server );
		Mem_Free( alloc, server );
		server = nullptr;
	}

	void Server_SetThreadInfo( Server_t server, const char* name )
	{ return MainRecorder_SetThreadInfo( &server->Recorder, name ); }

	void Server_EnterScopeEx( Server_t server, const NamedLocation& scope, ScopeType::Enum type, int64_t tick )
	{ return MainRecorder_EnterScope( &server->Recorder, scope, type, tick ); }

	void Server_LeaveScopeEx( Server_t server, const NamedLocation& scope, int64_t tick )
	{ return MainRecorder_LeaveScope( &server->Recorder, scope, tick ); }

	void Server_EnterScope( Server_t server, const NamedLocation& scope, ScopeType::Enum type )
	{ return MainRecorder_EnterScope( &server->Recorder, scope, type ); }

	void Server_LeaveScope( Server_t server, const NamedLocation& scope )
	{ return MainRecorder_LeaveScope( &server->Recorder, scope ); }

	void Server_SetMutexInfo( Server_t server, const void* handle, const char* name )
	{ return MainRecorder_SetMutexInfo( &server->Recorder, handle, name ); }

	void Server_EnterMutex( Server_t server, const void* handle )
	{ return MainRecorder_EnterMutex( &server->Recorder, handle ); }

	void Server_LeaveMutex( Server_t server, const void* handle )
	{ return MainRecorder_LeaveMutex( &server->Recorder, handle ); }

	void Server_RecordValue( Server_t server, const char* name, uint32_t v )
	{ return MainRecorder_RecordValue( &server->Recorder, name, v ); }

	void Server_RecordValue( Server_t server, const char* name, float v )
	{ return MainRecorder_RecordValue( &server->Recorder, name, v ); }

	void Server_RecordValue( Server_t server, const char* path, const char* name, uint32_t v )
	{ return MainRecorder_RecordValue( &server->Recorder, path, name, v ); }

	void Server_RecordValue( Server_t server, const char* path, const char* name, float v )
	{ return MainRecorder_RecordValue( &server->Recorder, path, name, v ); }

	void Server_RecordLog( Server_t server, const NamedLocation& scope, const char* text )
	{}

} }

//======================================================================================
namespace nemesis { namespace profiling 
{ 
	//==================================================================================
	static Server_s* TheServer = nullptr;

	//==================================================================================
	Result_t Server_Initialize( Allocator_t alloc )
	{ 
		if (TheServer)
			return NE_ERR_INVALID_CALL;
		TheServer = Mem_Calloc<Server_s>( alloc );
		Server_Initialize( TheServer, alloc );
		Server_SetThreadInfo( "Main Thread" );
		return NE_OK;
	}

	void Server_Shutdown()
	{
		if (!TheServer)
			return;
		Allocator_t alloc = TheServer->Alloc;
		Server_Shutdown( TheServer );
		Mem_Free( alloc, TheServer );
		TheServer = nullptr;
	}

	void Server_Reset()
	{}

	void Server_NextFrame()
	{ return Server_NextFrame( TheServer ); }

	void Server_GetStats( ServerStats_s& stats )
	{ 
		if (TheServer)
			return Server_GetStats( TheServer, stats ); 
		NeZero(stats);
		return;
	}

	void Server_SetConsumer( const Consumer_s& consumer )
	{ 
		if (TheServer)
			return Server_SetConsumer( TheServer, consumer ); 
	}

	void Server_SetThreadInfo( const char* name )
	{ return MainRecorder_SetThreadInfo( &TheServer->Recorder, name ); }

	void Server_EnterScopeEx( const NamedLocation& scope, ScopeType::Enum type, int64_t tick )
	{ return MainRecorder_EnterScope( &TheServer->Recorder, scope, type, tick ); }

	void Server_LeaveScopeEx( const NamedLocation& scope, int64_t tick )
	{ return MainRecorder_LeaveScope( &TheServer->Recorder, scope, tick ); }

	void Server_EnterScope( const NamedLocation& scope, ScopeType::Enum type )
	{ return MainRecorder_EnterScope( &TheServer->Recorder, scope, type ); }

	void Server_LeaveScope( const NamedLocation& scope )
	{ return MainRecorder_LeaveScope( &TheServer->Recorder, scope ); }

	void Server_SetMutexInfo( const void* handle, const char* name )
	{ return MainRecorder_SetMutexInfo( &TheServer->Recorder, handle, name ); }

	void Server_EnterMutex( const void* handle )
	{ return MainRecorder_EnterMutex( &TheServer->Recorder, handle ); }

	void Server_LeaveMutex( const void* handle )
	{ return MainRecorder_LeaveMutex( &TheServer->Recorder, handle ); }

	void Server_RecordValue( const char* name, uint32_t v )
	{ return MainRecorder_RecordValue( &TheServer->Recorder, name, v ); }

	void Server_RecordValue( const char* name, float v )
	{ return MainRecorder_RecordValue( &TheServer->Recorder, name, v ); }

	void Server_RecordValue( const char* path, const char* name, uint32_t v )
	{ return MainRecorder_RecordValue( &TheServer->Recorder, path, name, v ); }

	void Server_RecordValue( const char* path, const char* name, float v )
	{ return MainRecorder_RecordValue( &TheServer->Recorder, path, name, v ); }

	void Server_RecordLog( const NamedLocation& scope, const char* text )
	{}

	void Server_StartSender( uint16_t port )
	{ return Server_StartSender( TheServer, port ); }

	void Server_StopSender()
	{ return Server_StopSender( TheServer ); }
} }
