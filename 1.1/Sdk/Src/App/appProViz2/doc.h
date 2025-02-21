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
struct ServerInfo_s
{
	ne::system::IpAddress_t Ip;
	ne::system::Tick_t		Ack;
	bool					Alive;
};

struct ServerList_s
{
	ne::gui::List_s					   Ctrl;
	ne::system::IpAddress_t			   DefaultIp;
	ne::BoundedArray<ServerInfo_s, 64> Hosts;
	int32_t							   RefreshDelay;
	bool							   RefreshEnabled;
	ne::system::StopWatch_c			   RefreshElapsed;
	ne::system::Socket_t			   RefreshSocket;
};

void NE_API ServerList_AutoRefresh(ServerList_s& list);
void NE_API ServerList_Refresh	  (ServerList_s& list);
void NE_API ServerList_Refresh	  (ServerList_s& list, ne::system::IpPort_t port);

//======================================================================================
typedef struct Doc_s* Doc_t;

struct Doc_s
{
	ne::Allocator_t					Alloc;
	ne::profiling::Database_t		Db;
	ne::profiling::Parser_t			Parser;
	ne::profiling::Receiver_t		Receiver;
	ne::profiling::ReceiverCallback ReceiverCallback;
	ne::gui::ZonePanel_s			ZoneState;
	ne::Rect_s						ZoneRect;
	ne::Vec2_s						ZoneScroll;
	ne::Vec2_s						ZoneSize;
	ne::gui::Timeline_s				FrameTimeline;
	ServerList_s					ServerList;
};

void NE_API Doc_Initialize(Doc_t doc, ne::Allocator_t alloc);
void NE_API Doc_Shutdown  (Doc_t doc);
