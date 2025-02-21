#pragma once

//======================================================================================
#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0600
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

//======================================================================================
#include <Nemesis/Core/All.h>
#include <Nemesis/Font/All.h>
#include <Nemesis/Gui/All.h>
#include <Nemesis/Perf/All.h>
#include <Nemesis/Gfx/DriverTypes.h>
NE_LINK("libNeCore")
NE_LINK("libNeFont")
NE_LINK("libNeGui")
NE_LINK("libNePerf")

//======================================================================================
#define  NE_GFX_DRIVER NE_GFX_DRIVER_D3D11
#include <Nemesis/Gfx/Driver.h>

//======================================================================================
using namespace nemesis;

//======================================================================================
#include "ui/ui.h"
#include "profiling.ui/profiling_ui.h"
#include "framework/framework.h"

//======================================================================================
#include <tchar.h>
#include <process.h>
#include <windowsx.h>
#include <winsock.h>
#include <atlbase.h>
#include <shellapi.h>
#pragma comment (lib, "shell32.lib")

//======================================================================================
namespace nemesis { namespace gui
{
	inline Id_t ID( const char* name )
	{
		return Id_Gen( name );
	}

	inline Id_t ID( const char* name, uint32_t child )
	{
		return Id_Cat( ID( name ), child );
	}

	inline Id_t ID( const char* name, uint32_t child, uint32_t sub )
	{
		return Id_Cat( ID( name, child ), sub );
	}

} }
