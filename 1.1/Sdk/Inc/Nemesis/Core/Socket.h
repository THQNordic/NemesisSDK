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
#include "SocketTypes.h"

//======================================================================================
namespace nemesis {	namespace system
{
	/// Generic Socket

	void		Socket_Initialize	();
	void		Socket_Shutdown		();
	Socket_t	Socket_Open			( SocketArg::Protocol proto );
	void		Socket_Close		( Socket_t socket );
	void		Socket_Stop			( Socket_t socket );
	bool		Socket_Bind			( Socket_t socket, IpAddress_t port );
	bool		Socket_Listen		( Socket_t socket );
	Socket_t	Socket_Accept		( Socket_t socket );
	bool		Socket_Connect		( Socket_t socket, IpAddress_t addr );
	IpAddress_t	Socket_GetAddress	( Socket_t socket );
	IpAddress_t	Socket_GetPeer		( Socket_t socket );
	void		Socket_SetOption	( Socket_t socket, SocketArg::Option option, bool enable );
	bool		Socket_Send			( Socket_t socket, const void* data, size_t size );
	bool		Socket_Receive		( Socket_t socket,	     void* data, size_t size );
	bool		Socket_SendTo		( Socket_t socket, IpAddress_t  addr, const void* data, size_t size );
	bool		Socket_ReceiveFrom	( Socket_t socket, IpAddress_t* addr,	    void* data, size_t size );

	/// TCP / UDP

	Socket_t	Tcp_Listen		( IpPort_t port );
	Socket_t	Tcp_Connect		( IpAddress_t addr );
	Socket_t	Tcp_Accept		( Socket_t socket );
	IpAddress_t	Tcp_GetPeer		( Socket_t socket );
	bool		Tcp_Send		( Socket_t socket, const void* buffer, size_t size );
	bool		Tcp_Receive		( Socket_t socket,		 void* buffer, size_t size );
	void		Tcp_Close		( Socket_t socket );

	Socket_t	Udp_Open		( IpPort_t port, SocketOption::Mask opt );
	bool		Udp_Send		( Socket_t socket, IpAddress_t  to  , const void* buffer, size_t size );
	bool		Udp_Receive		( Socket_t socket, IpAddress_t* from,	    void* buffer, size_t size );
	void		Udp_Close		( Socket_t socket );

	/// Address

	bool		IpAddress_GetHostName(IpAddress_t addr, str_t name, int len);

} }
