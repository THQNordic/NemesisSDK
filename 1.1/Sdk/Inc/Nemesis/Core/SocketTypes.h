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
#include "Types.h"

//======================================================================================
namespace nemesis {	namespace system
{
	/// Generic Socket

	typedef Handle_t	Socket_t;
	typedef uint16_t	IpPort_t;

	struct IpAddress_t
	{
		uint8_t Ip[4];
		IpPort_t Port;
	};

	struct SocketArg
	{
		enum Protocol
		{ Tcp
		, Udp
		};

		enum Option
		{ Broadcast
		, NonBlocking
		};
	};

	struct SocketOption
	{
		enum Flags
		{ None			= 0
		, Broadcast		= 1
		, NonBlocking	= 2
		};
		typedef uint32_t Mask;
	};

} }
