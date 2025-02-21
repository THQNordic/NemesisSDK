//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Receiver.h"

//======================================================================================
#include "Constants.h"

//======================================================================================
#include <Nemesis/Core/Process.h>
#include <Nemesis/Core/Socket.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace profiling 
{
	static void ReceiverCallback_Notify( const ReceiverCallback& callback, Socket_t client, const Packet& packet, const Chunk* head )
	{
		if (callback.PacketReceived)
			callback.PacketReceived( callback.UserContext, client, packet, head );
	}

	static bool Receiver_ReceivePacket( Receiver_s* rcv, Packet& packet )
	{
		rcv->Buffer.Resize( packet.DataSize() );
		if (!Tcp_Receive( rcv->Socket, rcv->Buffer.Data, rcv->Buffer.Count ))
			return false;
		ReceiverCallback_Notify( rcv->Callback, rcv->Socket, packet, (Chunk*)rcv->Buffer.Data );
		return true;
	}

	static void Receiver_ReceiveFromServer( Receiver_s* rcv )
	{
		Packet packet;

		for ( bool ok = true; ok; )
		{
			Event_Wait( rcv->PauseEvent );
			if (!Tcp_Receive( rcv->Socket, &packet, sizeof(packet) ))
				return;

			if (packet.header.id == chunk::Type::Packet)
			{
				ok = Receiver_ReceivePacket( rcv, packet );
			}
			else if (packet.header.id == EndianSwap((uint32_t)chunk::Type::Packet))
			{
				packet.header.id = EndianSwap(packet.header.id);
				packet.header.size = EndianSwap(packet.header.size);
				packet.flags = Packet::BigEndian;
				packet.reserved = 0;
				ok = Receiver_ReceivePacket( rcv, packet );
			}
			else
			{
				ok = false;
			}
		}
	}

	static void Receiver_CloseSocket( Receiver_t rcv )
	{
		Tcp_Close( rcv->Socket );
		rcv->Socket = nullptr;
	}

	static void Receiver_Run( Receiver_s* rcv )
	{
		Receiver_ReceiveFromServer( rcv );
		Receiver_CloseSocket( rcv );
	}

	static void NE_CALLBK Receiver_Proc( void* rcv )
	{
		Receiver_Run( (Receiver_s*) rcv );
	}

} }

//======================================================================================
namespace nemesis { namespace profiling 
{
	void Receiver_Initialize( Receiver_t rcv, Allocator_t alloc )
	{
		rcv->Alloc		  = alloc;
		rcv->PauseEvent   = Event_Create( true );
		rcv->Buffer.Alloc = alloc;
		rcv->Buffer.Resize( BUFFER_SIZE );
	}

	bool Receiver_IsPaused( Receiver_t rcv )
	{
		return rcv->Paused != 0;
	}

	void Receiver_Pause( Receiver_t rcv, bool pause )
	{
		if (pause)
		{
			rcv->Paused = 1;
			Event_Unsignal( rcv->PauseEvent );
		}
		else
		{
			rcv->Paused = 0;
			Event_Signal( rcv->PauseEvent );
		}
	}

	bool Receiver_IsConnected( Receiver_t rcv )
	{
		return rcv->Socket != nullptr;
	}

	Connect::Result Receiver_Connect( Receiver_t rcv, IpAddress_t addr, const ReceiverCallback& callback )
	{
		if (rcv->Socket)
			return Connect::AlreadyConnected;

		rcv->Socket = Tcp_Connect( addr );
		if (!rcv->Socket)
			return Connect::Failed;

		rcv->Callback = callback;

		const ThreadSetup_s thread_setup = { "[NePerf] Receiver", Receiver_Proc, rcv };
		Worker_Start( &rcv->Worker, thread_setup );
		return Connect::Ok;
	}

	void Receiver_Disconnect( Receiver_t rcv )
	{
		Receiver_CloseSocket( rcv );
		Receiver_Pause( rcv, false );
		Worker_Stop( &rcv->Worker );
		Worker_Wait( &rcv->Worker );
	}

	void Receiver_Shutdown( Receiver_t rcv )
	{
		Receiver_Disconnect( rcv );
		rcv->Buffer.Clear();
	}

} }
