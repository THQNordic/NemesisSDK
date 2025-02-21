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
namespace nemesis { namespace profiling { namespace viz 
{
	struct ServerTable
	{
		struct Item
		{
			system::IpAddress_t Address;
			uint32_t			Status;
		};

		ServerTable()
		{
			NeZero(Item);
		}

		int Find( system::IpAddress_t addr )
		{
			int found = -1;
			for ( int i = 0; i < NumItems; ++i )
			{
				if (BitwiseComparer::Equals( addr, Item[i].Address ))
					return i;
			}
			return -1;
		}

		int Ensure( system::IpAddress_t addr )
		{
			int found = Find( addr );
			if (found < 0)
			{
				if (NumItems == 256)
					return -1;
				found = NumItems++;
			}

			Item[found].Address = addr;
			return found;
		}

		Item Item[256];
		int	NumItems;
	};

	/// Session information
	struct Session
	{
		//Limit Limit;
		Parser_t Parser;
		Receiver_t Receiver;
		Database_t Database;
		system::IpAddress_t DefaultIp;
		ReceiverCallback ReceiverCallback;
		ServerTable KnownServers;
	};

	struct Skin
	{
		gui::tab::Visual_s						TabBar;
		gui::toolbar::Visual_s					ToolBar;
		gui::optionbar::Visual_s				OptionBar;
		gui::collapsebar::Visual_s				CollapseBar;
		gui::profiler::frame_bar::Visual_s		FrameBar;
		gui::profiler::cpu_bar::Visual_s		CpuBar;
		gui::profiler::time_bar::Visual_s		TimeBar;
		gui::profiler::lock_bar::Visual_s		LockBar;
		gui::profiler::profile_bar::Visual_s	ProfileBar;
		gui::profiler::summary_bar::Visual_s	SummaryBar;
		gui::profiler::zone_bar_list::Visual_s	ZoneBarList;
	};

} } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz 
{
	void Initialize( Session& session );
	void LoadSettings( Session& session );
	void SaveSettings( Session& session );

	int FormatDurationW( int64_t num_ticks, const Clock& clock, wchar_t* buffer, size_t len );

	void MakeDarkSkin	( Skin& skin );
	void MakeLightSkin	( Skin& skin );
	void MakeDefaultSkin( Skin& skin );

} } }
