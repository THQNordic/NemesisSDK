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
#include "Buffer.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	inline bool Packet_IsValid( Buffer_t buffer )
	{
		return (buffer->Count >= sizeof(Packet))
			&& (buffer->Count <= sizeof(buffer->Data))
			&& (((Packet*)buffer->Data)->header.id == chunk::Type::Packet);
	}

	inline bool Packet_IsEmpty( Buffer_t buffer )
	{
		NeAssert(buffer->Count >= sizeof(Packet));
		return (buffer->Count == sizeof(Packet));
	}

	inline void Packet_Initialize( Buffer_t buffer )
	{
		const Packet packet = { { chunk::Type::Packet, sizeof(packet) } };
		Mem_Cpy( buffer->Data, &packet, sizeof( packet ) );
		buffer->Count = sizeof( packet );
	}

	inline void Packet_Finalize( Buffer_t buffer )
	{
		NeAssert(buffer->Count >= sizeof(Packet));
		Packet* packet = ((Packet*)(buffer->Data));
		packet->header.size = buffer->Count;
	}

} }