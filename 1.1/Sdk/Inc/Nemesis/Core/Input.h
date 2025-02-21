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
#include "InputTypes.h"

//======================================================================================
namespace nemesis
{
	void NE_API Input_Notify( Handle_t wnd, uint32_t msg, uint64_t w, uint64_t l );

	Result_t NE_API Mouse_Poll( MouseState_s& mouse, Handle_t wnd );
	Result_t NE_API Keyboard_Poll( KeyboardState_s& kb );
}
