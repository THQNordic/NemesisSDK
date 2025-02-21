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
#include <Nemesis/Core/VMem.h>
#include "Platform.h"

//======================================================================================
#if ((NE_PLATFORM == NE_PLATFORM_WIN32) || (NE_PLATFORM == NE_PLATFORM_WIN64))
#	include "VMem_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_DURANGO)
#	include "VMem_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_SCARLETT)
#	include "VMem_Windows.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS4)
#	include "VMem_Orbis.h"
#elif (NE_PLATFORM == NE_PLATFORM_PS5)
#	include "VMem_Orbis.h"
#else
#	error Unrecognized platform...
#endif

//======================================================================================
namespace nemesis
{
	namespace
	{
		struct EopHeader_s
		{
			ptr_t	Base;
			size_t	Size;
		};
	}

	ptr_t NE_API Eop_Alloc( size_t size )
	{
		const size_t full_size = size + sizeof(EopHeader_s);
		const size_t page_size = VMem_GetPageSize();
		const size_t num_pages = (full_size + (page_size - 1)) / page_size;
		const size_t alloc_size = num_pages * page_size;
		uint8_t* base = (uint8_t*)VMem_Alloc( alloc_size );
		if (!base)
			return nullptr;
		const ptrdiff_t offset = alloc_size - full_size;
		EopHeader_s* header = (EopHeader_s*)(base + offset);
		header->Base = base;
		header->Size = size;
		return header+1;
	}

	void NE_API Eop_Free( ptr_t ptr )
	{
		if (!ptr)
			return;
		EopHeader_s* header = ((EopHeader_s*)ptr)-1;
		const size_t full_size = header->Size + sizeof(EopHeader_s);
		const size_t page_size = VMem_GetPageSize();
		const size_t num_pages = (full_size + (page_size - 1)) / page_size;
		const size_t alloc_size = num_pages * page_size;
		VMem_Free( header->Base, alloc_size );
	}

	size_t NE_API Eop_SizeOf( ptr_t ptr )
	{
		if (!ptr)
			return 0;
		EopHeader_s* header = ((EopHeader_s*)ptr)-1;
		return header->Size;
	}
}