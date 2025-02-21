#pragma once

//======================================================================================
#include "targetver.h"
#include <windows.h>

//======================================================================================
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//======================================================================================
#include <Nemesis/Core/All.h>
#include <Nemesis/Gui/All.h>
#include <Nemesis/Gui/Dock.h>
#include <Nemesis/Gui/Profiling.h>
#include <Nemesis/Gfx/Gfx_d3d11.h>
#include <Nemesis/Perf/All.h>
NE_LINK("libNeCore.lib")
NE_LINK("libNeFont.lib")
NE_LINK("libNeGui.lib")
NE_LINK("libNeGuiDock.lib")
NE_LINK("libNeGfx_d3d11.lib")
NE_LINK("libNePerf.lib")

//======================================================================================
#define IMPLEMENT_ME NeAssertOut(false, "Not implemented!");
