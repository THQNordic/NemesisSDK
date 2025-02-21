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
#include <Nemesis/Core/Socket.h>
#include "Platform.h"

//======================================================================================
#if ((NE_PLATFORM == NE_PLATFORM_WIN32) || (NE_PLATFORM == NE_PLATFORM_WIN64))
#	include "Socket_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_DURANGO)
#	include "Socket_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_SCARLETT)
#	include "Socket_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS4)
#	include "Socket_Orbis.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS5)
#	include "Socket_Orbis.h"
#else
#	error Unrecognized platform...
#endif

//======================================================================================
namespace nemesis { namespace system
{
	//==================================================================================

	Socket_t Tcp_Listen( IpPort_t port )
	{
		Socket_t socket = Socket_Open( SocketArg::Tcp );
		if (!socket)
			return nullptr;

		IpAddress_t addr = { { 0, 0, 0, 0 }, port };
		if (!Socket_Bind( socket, addr ))
		{
			Socket_Close( socket );
			return nullptr;
		}

		if (!Socket_Listen( socket ))
		{
			Socket_Close( socket );
			return nullptr;
		}

		return socket;
	}

	Socket_t Tcp_Connect( IpAddress_t addr )
	{
		Socket_t client = Socket_Open( SocketArg::Tcp );
		if (!client)
			return nullptr;

		if (!Socket_Connect( client, addr ))
		{
			Socket_Close( client );
			return nullptr;
		}

		return client;
	}

	Socket_t Tcp_Accept( Socket_t server )
	{
		return Socket_Accept( server );
	}

	bool Tcp_Send( Socket_t socket, const void* buffer, size_t size )
	{
		return Socket_Send( socket, buffer, size );
	}

	bool Tcp_Receive( Socket_t socket, void* buffer, size_t size )
	{
		return Socket_Receive( socket, buffer, size );
	}

	IpAddress_t Tcp_GetPeer( Socket_t socket )
	{
		return Socket_GetPeer( socket );
	}

	void Tcp_Close( Socket_t socket )
	{
		Socket_Stop( socket );
		Socket_Close( socket );
	}

	//==================================================================================

	Socket_t Udp_Open( IpPort_t port, SocketOption::Mask opt )
	{
		Socket_t socket = Socket_Open( SocketArg::Udp );
		if (!socket)
			return nullptr;

		IpAddress_t addr = { { 0, 0, 0, 0 }, port };
		if (!Socket_Bind( socket, addr ))
		{
			Socket_Close( socket );
			return nullptr;
		}

		if ( opt & SocketOption::Broadcast )
			Socket_SetOption( socket, SocketArg::Broadcast, true );

		if ( opt & SocketOption::NonBlocking )
			Socket_SetOption( socket, SocketArg::NonBlocking, true );

		return socket;
	}

	bool Udp_Send( Socket_t socket, IpAddress_t to, const void* buffer, size_t size )
	{
		return Socket_SendTo( socket, to, buffer, size );
	}

	bool Udp_Receive( Socket_t socket, IpAddress_t* from, void* buffer, size_t size )
	{
		return Socket_ReceiveFrom( socket, from, buffer, size );
	}

	void Udp_Close( Socket_t socket )
	{
		Socket_Close( socket );
	}

} }
