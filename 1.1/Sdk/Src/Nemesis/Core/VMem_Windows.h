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
#include <Nemesis/Core/VMem.h>

//======================================================================================
namespace nemesis
{
	ptr_t NE_API VMem_Alloc( size_t size )
	{
		return VirtualAlloc( nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	}

	void NE_API VMem_Free( ptr_t ptr, size_t size )
	{
		// @note:
		//	C6331: Invalid parameter:  passing MEM_RELEASE and MEM_DECOMMIT in conjunction to 'VirtualFree' is not allowed.  This causes the call to fail.
		VirtualFree( ptr, 0, MEM_RELEASE );
	}

	uint32_t NE_API VMem_GetPageSize()
	{
		SYSTEM_INFO sys_info = {};
		GetSystemInfo( &sys_info );
		return sys_info.dwPageSize;
	}
}

