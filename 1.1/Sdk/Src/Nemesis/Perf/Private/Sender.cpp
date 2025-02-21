//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Sender.h"

//======================================================================================
#include <Nemesis/Core/Process.h>
#include <Nemesis/Core/Socket.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling
{
	/// Sender Thread 

	static void Sender_Run( Sender_s* Sender )
	{
		Socket_t target = nullptr;
		while ( Sender->Worker.Continue )
		{
			target = Tcp_Accept( Sender->Socket );
			if (target)
				Dispatcher_Connect( &Sender->Dispatcher, target );
		}
	}
	
	static void NE_CALLBK Sender_Proc( void* Sender )
	{
		Sender_Run( (Sender_s*) Sender );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void Sender_Initialize( Sender_s* Sender, Allocator_t alloc )
	{
		Dispatcher_Initialize( &Sender->Dispatcher, alloc );
	}

	Result_t Sender_Start( Sender_s* Sender, IpPort_t port )
	{
		if (Sender->Worker.Thread)
			return NE_ERR_INVALID_CALL;
		Sender->Socket = Tcp_Listen( port );
		if (!Sender->Socket)
			return NE_ERR_NOT_FOUND;
		const ThreadSetup_s thread_setup = { "[NePerf] Server", Sender_Proc, Sender };
		Worker_Start( &Sender->Worker, thread_setup );
		return NE_OK;
	}

	void Sender_Stop( Sender_s* Sender )
	{
		if (!Sender->Worker.Thread)
			return;
		Worker_Stop( &Sender->Worker );
			Tcp_Close( Sender->Socket );
		Worker_Wait( &Sender->Worker );
	}

	void Sender_Attach( Sender_s* sender, const Consumer_s& local )
	{
		Dispatcher_Attach( &sender->Dispatcher, local );
	}

	void Sender_Connect( Sender_s* sender, Socket_t client )
	{
		Dispatcher_Connect( &sender->Dispatcher, client );
	}

	void Sender_Push( Sender_s* sender, const DispatchItem_s& item )
	{
		Dispatcher_Push( &sender->Dispatcher, item );
	}

	void Sender_Shutdown( Sender_s* Sender )
	{
		Sender_Stop( Sender );
		Dispatcher_Shutdown( &Sender->Dispatcher );
	}

} }
