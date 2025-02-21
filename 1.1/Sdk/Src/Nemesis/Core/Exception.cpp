//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"

//======================================================================================
#include <Nemesis/Core/Exception.h>
#include "Platform.h"

//======================================================================================
#if ((NE_PLATFORM == NE_PLATFORM_WIN32) || (NE_PLATFORM == NE_PLATFORM_WIN64))
#	include "Exception_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_DURANGO)
#	include "Exception_Null.h"
#elif (NE_PLATFORM == NE_PLATFORM_SCARLETT)
#	include "Exception_Null.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS4)
#	include "Exception_Null.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS5)
#	include "Exception_Null.h"
#else
#	error Unrecognized platform...
#endif
