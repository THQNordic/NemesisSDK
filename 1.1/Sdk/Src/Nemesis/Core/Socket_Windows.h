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
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

//======================================================================================
namespace nemesis { namespace system
{
	//==================================================================================
	
	namespace
	{
		static WSADATA WsaData = {};
	}

	//==================================================================================

	void Socket_Initialize()
	{
		INT hr;
	#if (NE_PLATFORM == NE_PLATFORM_X360)
		XNetStartupParams xnsp;
		memset( &xnsp, 0, sizeof( xnsp ) );
		xnsp.cfgSizeOfStruct = sizeof( XNetStartupParams );
		xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY | XNET_STARTUP_DISABLE_PEER_ENCRYPTION;
		hr = XNetStartup( &xnsp );
	#endif
		hr = WSAStartup( MAKEWORD(2,2), &WsaData );
		NeUnused(hr);
	}

	void Socket_Shutdown()
	{
		WSACleanup();
	#if (NE_PLATFORM == NE_PLATFORM_X360)
		XNetCleanup();
	#endif
	}

	//==================================================================================

	namespace
	{
		enum { SOCKET_MAX_CONNECTIONS = SOMAXCONN };
		enum 
		{ SHUT_RD	= SD_RECEIVE
		, SHUT_WR	= SD_SEND
		, SHUT_RDWR = SD_BOTH
		};

		typedef SOCKET SocketId_t;
		const SocketId_t SOCKET_ID_INVALID = (SocketId_t)INVALID_SOCKET;

		static int socketclose( SocketId_t socket ) 
		{ return closesocket( socket ); }

		static int socket_set_non_blocking( SocketId_t socket, bool enable ) 
		{
			DWORD non_blocking = enable ? 1 : 1;
			return ioctlsocket( socket, FIONBIO, &non_blocking );
		}

		static int socket_get_last_err()
		{
			return WSAGetLastError();
		}

		static const char* socket_get_err_str( int err )
		{
			#define CASE(x) case x: return #x;
			switch ( err )
			{
			CASE(WSANOTINITIALISED)
			CASE(WSAENETDOWN)
			CASE(WSAEACCES)
			CASE(WSAEFAULT)
			CASE(WSAEINVAL)
			CASE(WSAEINTR)
			CASE(WSAEWOULDBLOCK)
			CASE(WSAENOTSOCK)
			CASE(WSAEADDRINUSE)
			default:
				return "WSAERR";
			}
			#undef CASE
		}

		static const char* socket_get_err_desc( int err )
		{
			switch ( err )
			{
			case WSANOTINITIALISED: return "Either the application has not called WSAStartup, or WSAStartup failed.";
			case WSAENETDOWN:		return "A socket operation encountered a dead network.";
			case WSAEACCES:			return "An attempt was made to access a socket in a way forbidden by its access permissions.";
			case WSAEFAULT:			return "The system detected an invalid pointer address in attempting to use a pointer argument in a call.";
			case WSAEINVAL:			return "An invalid argument was supplied.";
			case WSAEINTR:			return "A blocking operation was interrupted by a call to WSACancelBlockingCall.";
			case WSAEWOULDBLOCK:	return "A non-blocking socket operation could not be completed immediately.";
			case WSAENOTSOCK:		return "An operation was attempted on something that is not a socket.";
			case WSAEADDRINUSE:		return "Only one usage of each socket address (protocol/network address/port) is normally permitted.";
			default:				return "An unknown error occured.";
			}
		}
	}

	namespace
	{
	#if NE_PLATFORM == NE_PLATFORM_X360
			typedef int socklen_t;
	#endif
	}
} }

//======================================================================================
#include "Socket_Ansi.h"
