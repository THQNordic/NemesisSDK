#pragma once

//======================================================================================
#include <Nemesis/Gui/Dock.h>

//======================================================================================
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/Logging.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Gui/System.h>
#include <Nemesis/Gui/Controls.h>

//======================================================================================
#if NE_PLATFORM_IS_WINDOWS
#	include <Windows.h>
#endif

//======================================================================================
#if NE_PLATFORM_IS_WINDOWS
#	include <dwmapi.h>
	NE_LINK("Dwmapi.lib")		// DwmExtendFrameIntoClientArea
	NE_LINK("Comctl32.lib")		// SetWindowSubclass
#endif

//======================================================================================
#if NE_PLATFORM_IS_WINDOWS
#	include <malloc.h>
#else
#	include <stdlib.h>
#endif

//======================================================================================
#define IMPLEMENT_ME NeAssertOut(false, "Implement me!")
