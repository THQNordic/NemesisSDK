//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "doc.h"
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
static system::IpAddress_t GetPrimaryAdapterIp()
{
	ULONG sz;
	GetAdaptersInfo(nullptr, &sz);
	IP_ADAPTER_INFO* info = (IP_ADAPTER_INFO*)alloca(sz);
	GetAdaptersInfo(info, &sz);

	uint32_t ip[4];
	system::IpAddress_t addr = {};
	for (; info; info = info->Next)
	{
		if (StringComparer::Equals(info->IpAddressList.IpAddress.String, "0.0.0.0"))
			continue;
		sscanf_s(info->IpAddressList.IpAddress.String, "%u.%u.%u.%u", ip + 0, ip + 1, ip + 2, ip + 3);
		addr.Ip[0] = (uint8_t)ip[0];
		addr.Ip[1] = (uint8_t)ip[1];
		addr.Ip[2] = (uint8_t)ip[2];
		addr.Ip[3] = (uint8_t)ip[3];
		break;
	}
	return addr;
}

//======================================================================================
static void NE_CALLBK OnPacketReceived(void* context, system::Socket_t client, const profiling::Packet& packet, const profiling::Chunk* head)
{
	Doc_t doc = (Doc_t)context;
	Parser_ParseData(doc->Parser, packet, head, Parse::Buffered);
}

//======================================================================================
void ServerList_Initialize(ServerList_s& list)
{
	list.DefaultIp = { { 127, 0, 0, 1 }, 16001 };
	list.RefreshDelay = 1000;
	list.RefreshEnabled = true;
}

void ServerList_AutoRefresh(ServerList_s& list)
{
	if (list.RefreshElapsed.ElapsedMs64() < list.RefreshDelay)
	{
		return;
	}
	ServerList_Refresh(list);
	list.RefreshElapsed.Start();
}

void ServerList_Refresh(ServerList_s& list)
{
	ServerList_Refresh(list, 16001);
	ServerList_Refresh(list, 17001);
}

int ServerList_Refresh(ServerList_s& list, system::IpAddress_t from, Tick_t now)
{
	using namespace system;

	struct Comparer
	{
		static int Compare(const ServerInfo_s& lhs, const IpAddress_t addr)
		{
			return BitwiseComparer::Compare(lhs.Ip, addr);
		}
	};

	const int found = Array_BinaryFind<Comparer>(list.Hosts, from);
	const int index = (found >= 0) ? found : ~found;
	if (found < 0)
	{
		if (list.Hosts.Remain() == 0)
		{
			return -1;
		}

		const int end = index + 1;
		if (list.Hosts.Count > end)
			Arr_Mov(list.Hosts.Data + end, list.Hosts.Data + index, list.Hosts.Count - end);
		list.Hosts.Data[index].Ip = from;
		list.Hosts.Count++;
	}

	list.Hosts.Data[index].Ack = now;
	return index;
}

void ServerList_Refresh(ServerList_s& list, system::IpPort_t port)
{
	using namespace system;

	if (!list.RefreshSocket)
	{
		list.RefreshSocket = Udp_Open(0, SocketOption::Broadcast | SocketOption::NonBlocking);
	}

	IpAddress_t to = GetPrimaryAdapterIp();
	to.Port = port;
	to.Ip[3] = 255;

	ping::Header hdr;
	hdr.Init();
	Udp_Send(list.RefreshSocket, to, &hdr, sizeof(hdr));

	const Tick_t now = Clock_GetTick();

	IpAddress_t from = {};
	ping::Response ack = {};
	while (Udp_Receive(list.RefreshSocket, &from, &ack, sizeof(ack)))
	{
		if (!ack.Header.IsValid())
		{
			continue;
		}

		ServerList_Refresh(list, from, now);
	}

	const IpAddress_t local_host = { { 127,0,0,1 }, 16001 };
	ServerList_Refresh(list, local_host, now);

	const Tick_t freq = Clock_GetFreq();
	for (int i = 0; i < list.Hosts.Count; ++i)
	{
		const Tick_t latest_ack = list.Hosts[i].Ack;
		const Tick_t elapsed_tick = now - latest_ack;
		const uint64_t elapsed_ms = (elapsed_tick * 1000) / freq;
		list.Hosts[i].Alive = (elapsed_ms < 2000);
	}

	list.Ctrl.Data.NumRows = list.Hosts.Count;
}

//======================================================================================
void Doc_Initialize(Doc_t doc, Allocator_t alloc)
{
	doc->Alloc = alloc;

	const DatabaseSetup_s db_setup = {};
	doc->Db = Database_Create(alloc, db_setup);

	const ParserSetup parser_setup = { doc->Db, 16 * 1024 * 1024 };
	doc->Parser			   = Parser_Create(alloc, parser_setup);
	doc->Receiver		   = Receiver_Create(alloc);
	doc->ReceiverCallback = { OnPacketReceived, doc };

	ServerList_Initialize(doc->ServerList);

	doc->ZoneState.Cull		  = ZoneCull_Default();
	doc->ZoneState.AutoScroll = true;
}

void Doc_Shutdown(Doc_t doc)
{
	profiling::Receiver_Destroy(doc->Receiver);
	profiling::Parser_Destroy(doc->Parser);
	profiling::Database_Destroy(doc->Db);
	*doc = {};
}
