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
namespace nemesis { namespace system
{
	//==================================================================================

	namespace 
	{
		static uint32_t IpEncode( IpAddress_t v )
		{
			return (v.Ip[0] << 24) | (v.Ip[1] << 16) | (v.Ip[2] << 8) | v.Ip[3];
		}

		static void IpDecode( uint32_t ip, IpAddress_t& addr )
		{
			addr.Ip[0] = (uint8_t)((ip & 0xff000000) >> 24);
			addr.Ip[1] = (uint8_t)((ip & 0x00ff0000) >> 16);
			addr.Ip[2] = (uint8_t)((ip & 0x0000ff00) >>  8);
			addr.Ip[3] = (uint8_t)((ip & 0x000000ff));
		}

		static sockaddr_in IpToAddr( IpAddress_t v )
		{
			sockaddr_in addr;
			addr.sin_family		 = AF_INET;
			addr.sin_addr.s_addr = htonl( IpEncode( v ) );
			addr.sin_port		 = htons( v.Port );
			return addr;
		}

		static sockaddr_in PortToAddr( IpPort_t port )
		{
			sockaddr_in addr;
			addr.sin_family      = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port		 = htons( port );
			return addr;
		}

		static IpAddress_t AddrToIp( const sockaddr_in& v )
		{
			IpAddress_t addr;
			IpDecode( ntohl( v.sin_addr.s_addr ), addr );
			addr.Port = ntohs( v.sin_port );
			return addr;
		}
	}

	//==================================================================================

	namespace
	{
		static Socket_t Translate( SocketId_t socket )
		{ 
			if (socket == SOCKET_ID_INVALID)
				return nullptr;
			if (socket == 0 )
				return Socket_t(-1);
			return reinterpret_cast<Socket_t>(socket);
		}

		static SocketId_t Translate( Socket_t socket )
		{ 
			if (socket == 0)
				return SOCKET_ID_INVALID;
			if (socket == Socket_t(-1))
				return 0;
			return reinterpret_cast<SocketId_t>(socket); 
		}

		static bool Succeeded( int hr )
		{ return hr >= 0; }
	}

	//==================================================================================

	Socket_t Socket_Open( SocketArg::Protocol proto )
	{
		int type = (proto == SocketArg::Tcp) ? SOCK_STREAM : SOCK_DGRAM;
		int prot = (proto == SocketArg::Tcp) ? IPPROTO_TCP : IPPROTO_UDP;
		return Translate( socket( AF_INET, type , prot ) );
	}

	void Socket_Close( Socket_t socket )
	{ socketclose( Translate( socket ) ); }

	void Socket_Stop( Socket_t socket )
	{ 
		if (socket)
			shutdown( Translate( socket ), SHUT_RDWR ); 
	}

	bool Socket_Bind( Socket_t socket, IpAddress_t port )
	{
		const sockaddr_in address = IpToAddr( port );
		return Succeeded( bind( Translate( socket ), (sockaddr*)&address, (int)sizeof(sockaddr_in) ) );
	}

	bool Socket_Listen( Socket_t socket )
	{ return Succeeded( listen( Translate( socket ), SOCKET_MAX_CONNECTIONS ) ); }

	Socket_t Socket_Accept( Socket_t socket )
	{ return Translate( accept( Translate(socket), nullptr, nullptr ) ); }

	bool Socket_Connect( Socket_t socket, IpAddress_t addr )
	{ 
		const sockaddr_in address = IpToAddr( addr );
		return Succeeded( connect( Translate( socket ), (sockaddr*)&address, sizeof(address) ) );
	}

	IpAddress_t	Socket_GetAddress( Socket_t socket )
	{
		sockaddr_in address = {};
		socklen_t len = sizeof(address);
		int hr = getsockname( Translate( socket ), (sockaddr*)&address, &len );
		return AddrToIp( address );
	}

	IpAddress_t	Socket_GetPeer( Socket_t socket )
	{
		sockaddr_in address = {};
		socklen_t len = sizeof(address);
		int hr = getpeername( Translate( socket ), (sockaddr*)&address, &len );
		return AddrToIp( address );
	}

	void Socket_SetOption( Socket_t socket, SocketArg::Option option, bool enable )
	{
		switch ( option )
		{
		case SocketArg::Broadcast:
			{
				int broadcast = 1;
				setsockopt( Translate( socket ), SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, (int)sizeof(broadcast) );
			}
			break;
		case SocketArg::NonBlocking:
			socket_set_non_blocking( Translate( socket ), enable );
			break;
		}
	}

	bool Socket_Send( Socket_t socket, const void* data, size_t size )
	{ 
		const SocketId_t sid = Translate( socket );
		const char* pos = (const char*)data;
		const char* end = pos + size;
		while ( pos < end )
		{
			int written = send( sid, pos, (int)(end-pos), 0 );
			if (written > 0)
				pos += written;
			else
				return false;
		}
		return true;
	}

	bool Socket_Receive( Socket_t socket, void* data, size_t size )
	{
		const SocketId_t sid = Translate( socket );
		char* pos = (char*)data;
		char* end = pos + size;
		while ( pos < end )
		{
			int read = recv( sid, pos, (int)(end-pos), 0 );
			if (read > 0)
				pos += read;
			else
				return false;
		}
		return true;
	}

	bool Socket_SendTo( Socket_t socket, IpAddress_t addr, const void* data, size_t size )
	{
		sockaddr_in address = IpToAddr( addr );
		int len = (int)size;
		int hr = sendto( Translate( socket ), (const char*)data, len, 0, (sockaddr*)&address, (int)sizeof(sockaddr_in) );
		return (hr == len);
	}

	bool Socket_ReceiveFrom( Socket_t socket, IpAddress_t* addr, void* data, size_t size )
	{
		sockaddr_in address = {};
		socklen_t l = sizeof(sockaddr_in);
		int len = (int)size;
		int hr = recvfrom( Translate( socket ), (char*)data, len, 0, (sockaddr*)&address, &l );
		if (addr)
			*addr = AddrToIp( address );
		return (hr == len);
	}

	bool IpAddress_GetHostName(IpAddress_t addr, str_t name, int len)
	{
		sockaddr_in address = IpToAddr(addr);
		socklen_t l = sizeof(sockaddr_in);
		const int hr = getnameinfo((sockaddr*)&address, l, name, (DWORD)len, nullptr, 0, 0);
		return hr == 0;
	}

} }
