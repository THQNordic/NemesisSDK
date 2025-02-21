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
#include <Nemesis/Perf/Visualizer.h>

//======================================================================================
#include "Worker.h"

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Receiver_s
	{
		Allocator_t			Alloc;
		Socket_t			Socket;
		int32_t				Paused;
		Event_t				PauseEvent;
		ReceiverCallback	Callback;
		Worker_s			Worker;
		Array<uint8_t>		Buffer;
	};

	void Receiver_Initialize( Receiver_t rcv, Allocator_t alloc );
	bool Receiver_IsPaused( Receiver_t rcv );
	void Receiver_Pause( Receiver_t rcv, bool pause );
	bool Receiver_IsConnected( Receiver_t rcv );
	Connect::Result Receiver_Connect( Receiver_t rcv, system::IpAddress_t addr, const ReceiverCallback& callback );
	void Receiver_Disconnect( Receiver_t rcv );
	void Receiver_Shutdown( Receiver_t rcv );

} }
