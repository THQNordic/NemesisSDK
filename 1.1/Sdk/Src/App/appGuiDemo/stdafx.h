#pragma once

//======================================================================================
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//======================================================================================
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//======================================================================================
#include <Nemesis/Core/All.h>
#include <Nemesis/Perf/All.h>
#include <Nemesis/Gui/All.h>
#include <Nemesis/Gfx/DriverTypes.h>
NE_LINK("libNeCore.lib")
NE_LINK("libNeFont.lib")
NE_LINK("libNeGui.lib")
NE_LINK("libNePerf.lib")

//======================================================================================
#define  NE_GFX_DRIVER NE_GFX_DRIVER_D3D11
#include <Nemesis/Gfx/Driver.h>
