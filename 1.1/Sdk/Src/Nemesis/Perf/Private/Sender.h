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
#include "Dispatcher.h"

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Sender_s
	{
		Socket_t	 Socket;
		Worker_s	 Worker;
		Dispatcher_s Dispatcher;
	};

	void Sender_Initialize( Sender_s* Sender, Allocator_t alloc );
	Result_t Sender_Start( Sender_s* Sender, system::IpPort_t port );
	void Sender_Stop( Sender_s* Sender );
	void Sender_Attach( Sender_s* sender, const Consumer_s& local );
	void Sender_Connect( Sender_s* sender, Socket_t client );
	void Sender_Push( Sender_s* sender, const DispatchItem_s& item );
	void Sender_Shutdown( Sender_s* Sender );

} }
