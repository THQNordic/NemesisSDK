//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "profiling_viz.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz 
{
	//==================================================================================

	/// Initializes data member for first use.
	void Initialize( Session& session )
	{
		NeZero(session);
	}

	void LoadSettings( Session& session )
	{
		LSTATUS hr;
		char sz[256] = "";
		DWORD len = sizeof(sz);
		DWORD type = REG_SZ;
		hr = RegGetValueA( HKEY_CURRENT_USER, "Software\\Nemesis\\ProViz\\1.0\\Settings", "DefaultIp", RRF_RT_REG_SZ, &type, sz, &len );
		if (hr)
			return;

		uint32_t ip[5] = {};
		sscanf_s( sz, "%u.%u.%u.%u:%u", ip+0, ip+1, ip+2, ip+3, ip+4 );
		system::IpAddress_t addr = { (uint8_t)ip[0], (uint8_t)ip[1], (uint8_t)ip[2], (uint8_t)ip[3], (uint16_t)ip[4] };
		session.DefaultIp = addr;
		session.KnownServers.Ensure(addr);
	}

	void SaveSettings( Session& session )
	{
		system::IpAddress_t ip = session.DefaultIp;

		LSTATUS hr;
		char sz[256] = "";
		int len = sprintf_s( sz, "%u.%u.%u.%u:%u", (uint32_t)ip.Ip[0], (uint32_t)ip.Ip[1], (uint32_t)ip.Ip[2], (uint32_t)ip.Ip[3], ip.Port );
		hr = RegSetKeyValueA( HKEY_CURRENT_USER, "Software\\Nemesis\\ProViz\\1.0\\Settings", "DefaultIp", REG_SZ, sz, len );
		NeUnused(hr);
	}

	//==================================================================================

	/// Converts the given duration to a string representation.
	int FormatDurationW( int64_t num_ticks, const Clock& clock, wchar_t* buffer, size_t len )
	{
		if ( num_ticks >= clock.TicksPerSecond )
			return swprintf_s( buffer, len, L"%.3f sec", clock.TickToSec( num_ticks ) );

		if ( (num_ticks * 1000) >= clock.TicksPerSecond )
			return swprintf_s( buffer, len, L"%.3f ms", clock.TickToMs( num_ticks ) );

		if ( (num_ticks * 1000 * 1000) >= clock.TicksPerSecond )
			return swprintf_s( buffer, len, L"%.3f \u00b5s", clock.TickToMs( 1000 * num_ticks ) );

		return swprintf_s( buffer, len, L"%llu cycles", num_ticks );
	}

	//==================================================================================

	void MakeDarkSkin( profiling::viz::Skin& skin )
	{
		using namespace gui;

		const Color_c btn_bkg1 = 0xffeeeeee;
		const Color_c btn_bkg2 = 0xffaaaaaa;
		const Color_c btn_lit  = Color::Gold;
		const Color_c tb_bkgnd = 0xff333333;

		// common controls
		{
			const gui::tab::Visual_s v = 
			{ { EnsureFont( 12.0f, FontStyle::SemiBold ), Color::Black } 
			}; 
			skin.TabBar = v;
		}
		{
			const toolbar::Visual_s v =
			{ tb_bkgnd
			,   { EnsureFont( 11.0f, FontStyle::Default ), Color::White }
			, { { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, btn_bkg1, btn_bkg2, btn_lit }
			};
			skin.ToolBar = v;
		}
		{
			const optionbar::Visual_s v = 
			{   { EnsureFont( 11.0f, FontStyle::Default ), Color::White }
			, { { EnsureFont(  9.0f, FontStyle::Default ), Color::Black }, btn_bkg1, btn_bkg2, btn_lit }
			};
			skin.OptionBar = v;
		}
		{
			const gui::collapsebar::Visual_s v = 
			{   { EnsureFont( 11.0f, FontStyle::Default ), Color::Black }
			, { { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, btn_bkg1, btn_bkg2, btn_lit }
			};
			skin.CollapseBar = v;
		}

		// profiling controls
		static Color_c THREAD_COLORS[] =
		{
			Color_Desaturate( Color::Gold   , 0.2f ),
			Color_Desaturate( Color::Green  , 0.2f ),
			Color_Desaturate( Color::Violet , 0.2f ),
			Color_Desaturate( Color::Teal   , 0.2f ),
			Color_Desaturate( Color::Cyan   , 0.2f ),
			Color_Desaturate( Color::Magenta, 0.2f ),
		};

		using namespace gui::profiler;
		{
			const Color_c bkgnd_color	 = tb_bkgnd;
			const Color_c bar_color    = 0xff3c3c50;
			const Color_c fps_color	 = Color_c( Color::Gray, 0.25f );
			const Color_c range_color  = Color_c( Color::Green, 0.25f );
			const frame_bar::Visual_s v = 
			{ bkgnd_color
			, bar_color
			, fps_color
			, range_color
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }
			};
			skin.FrameBar = v;
		}
		{
			static const float CPU_SPACING	= 1.0f;
			static const float CPU_HEIGHT	= 30.0f;

			const cpu_bar::Visual_s v = 
			{ { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }
			, 0xffbfbfbf, 0xff737373
			, Color::DarkGray
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, CPU_HEIGHT
			, CPU_SPACING
			};
			skin.CpuBar = v;
		}
		{
			const time_bar::Visual_s v = 
			{ EnsureFont( 12.0f, FontStyle::Default )
			, EnsureFont( 11.0f, FontStyle::Default )
			, EnsureFont( 10.0f, FontStyle::Default )
			, 0xffd3d3e1
			, 0xff000000
			, { 0xffffffff, 0xffc3c3c3, 0xffeeeeee, 0xffa3a3a3 }
			, { 0xff000000, 0xff000000, 0xff000000, 0xff000000 }
			};
			skin.TimeBar = v;
		}
		{
			static const float LOCK_HEIGHT	= 30.0f;
			static const float LOCK_SPACING	= 1.0f;

			const lock_bar::Visual_s v = 
			{ 0xff586858, 0xff343f34
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::PaleGoldenrod }
			, 0xff717d5e
			, Color::PaleGoldenrod, Color::OrangeRed, Color::OliveDrab 
			, LOCK_HEIGHT
			, LOCK_SPACING
			};
			skin.LockBar = v;
		}
		{
			static const float PROFILE_SPACING	= 1.0f;
			static const float PROFILE_HEIGHT	= 12.0f;

			const Color_c base_color = Color::White;
			const profile_bar::Visual_s v = 
			{ { Color::DarkGray, 0xff787890 }
			, { base_color * 0.4f, base_color * 0.3f }
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::White }
			, PROFILE_HEIGHT
			, PROFILE_SPACING
			};
			skin.ProfileBar = v;
		}
		{
			const summary_bar::Visual_s v =
			{ EnsureFont( 12.0f, FontStyle::Default )
			, EnsureFont( 11.0f, FontStyle::Default )
			, 0xffc5c5d8, 0xff646464
			, 0xff53535a, 0xff646464
			};
			skin.SummaryBar = v;
		}
		{
			static const float COLLAPSED_HEIGHT	= 32.0f;
			static const float ZONE_SPACING		= 1.0f;

			const zone_bar_list::Visual_s v = 
			{ EnsureFont( 11.0f, FontStyle::Default )
			, 0xffbfbfbf, 0xff737373
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, COLLAPSED_HEIGHT
			, 24.0f			// item.ExpandedHeight
			, ZONE_SPACING
			, true			// settings.ShowIdleZones
			};
			skin.ZoneBarList = v;
		}
	}

	void MakeLightSkin( profiling::viz::Skin& skin )
	{
		using namespace gui;

		// common controls
		{
			const gui::tab::Visual_s v = 
			{ { EnsureFont( 12.0f, FontStyle::SemiBold ), Color::Black } 
			}; 
			skin.TabBar = v;
		}
		{
			const toolbar::Visual_s v =
			{ Color::SlateBlue
			,   { EnsureFont( 11.0f, FontStyle::Default ), Color::White }
			, { { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, 0xffeeeeee, 0xffaaaaaa, Color_Desaturate(Color::Gold, 0.4f) }
			};
			skin.ToolBar = v;
		}
		{
			const optionbar::Visual_s v = 
			{   { EnsureFont( 11.0f, FontStyle::Default ), Color::White }											// label
			, { { EnsureFont(  9.0f, FontStyle::Default ), Color::Black }, 0xffeeeeee, 0xffaaaaaa, Color::Gold }	// button
			};
			skin.OptionBar = v;
		}
		{
			const gui::collapsebar::Visual_s v = 
			{   { EnsureFont( 11.0f, FontStyle::Default ), Color::Black }											// label
			, { { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }, 0xffeeeeee, 0xffaaaaaa, Color::White }	// button
			};
			skin.CollapseBar = v;
		}

		// profiling controls
		static Color_c THREAD_COLORS[] =
		{
			Color_Desaturate( Color::Gold   , 0.2f ),
			Color_Desaturate( Color::Green  , 0.2f ),
			Color_Desaturate( Color::Violet , 0.2f ),
			Color_Desaturate( Color::Teal   , 0.2f ),
			Color_Desaturate( Color::Cyan   , 0.2f ),
			Color_Desaturate( Color::Magenta, 0.2f ),
		};

		using namespace gui::profiler;
		{
			const Color_c bkgnd_color	 = 0xff9090b0;
			const Color_c bar_color    = 0xff3c3c50;
			const Color_c fps_color	 = Color_c( 0xff3c3c50, 0.1f );
			const Color_c range_color  = Color_c( Color::Green, 0.25f );
			const frame_bar::Visual_s v = 
			{ bkgnd_color
			, bar_color
			, fps_color
			, range_color
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }
			};
			skin.FrameBar = v;
		}

		{
			static const float CPU_SPACING	= 1.0f;
			static const float CPU_HEIGHT	= 30.0f;

			const cpu_bar::Visual_s v = 
			{ { EnsureFont( 10.0f, FontStyle::Default ), Color::Black }
			, 0xffbfbfbf, 0xff737373
			, Color::DarkGray
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, CPU_HEIGHT
			, CPU_SPACING
			};
			skin.CpuBar = v;
		}
		{
			const time_bar::Visual_s v = 
			{ EnsureFont( 12.0f, FontStyle::Default )
			, EnsureFont( 11.0f, FontStyle::Default )
			, EnsureFont( 10.0f, FontStyle::Default )
			, 0xffd3d3e1
			, 0xff000000
			, { 0xffffffff, 0xffc3c3c3, 0xffeeeeee, 0xffa3a3a3 }
			, { 0xff000000, 0xff000000, 0xff000000, 0xff000000 }
			};
			skin.TimeBar = v;
		}
		{
			static const float LOCK_HEIGHT	= 30.0f;
			static const float LOCK_SPACING	= 1.0f;

			const lock_bar::Visual_s v = 
			{ 0xff586858, 0xff343f34
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::PaleGoldenrod }
			, 0xff717d5e
			, Color::PaleGoldenrod, Color::OrangeRed, Color::OliveDrab 
			, LOCK_HEIGHT
			, LOCK_SPACING
			};
			skin.LockBar = v;
		}
		{
			static const float PROFILE_SPACING	= 1.0f;
			static const float PROFILE_HEIGHT	= 12.0f;

			const Color_c base_color = Color::White;
			const profile_bar::Visual_s v = 
			{ { Color::DarkGray, 0xff787890 }
			, { base_color * 0.4f, base_color * 0.3f }
			, { EnsureFont( 10.0f, FontStyle::Default ), Color::White }
			, PROFILE_HEIGHT
			, PROFILE_SPACING
			};
			skin.ProfileBar = v;
		}
		{
			const summary_bar::Visual_s v =
			{ EnsureFont( 12.0f, FontStyle::Default )
			, EnsureFont( 11.0f, FontStyle::Default )
			, 0xffc5c5d8, 0xff646464
			, 0xff53535a, 0xff646464
			};
			skin.SummaryBar = v;
		}
		{
			static const float COLLAPSED_HEIGHT	= 32.0f;
			static const float ZONE_SPACING		= 1.0f;

			const zone_bar_list::Visual_s v = 
			{ EnsureFont( 11.0f, FontStyle::Default )
			, 0xffbfbfbf, 0xff737373
			, THREAD_COLORS
			, NeCountOf(THREAD_COLORS)
			, COLLAPSED_HEIGHT
			, 24.0f			// item.ExpandedHeight
			, ZONE_SPACING
			, true			// settings.ShowIdleZones
			};
			skin.ZoneBarList = v;
		}
	}

	void MakeDefaultSkin( profiling::viz::Skin& skin )
	{
		return MakeDarkSkin( skin );
	}

} } }
