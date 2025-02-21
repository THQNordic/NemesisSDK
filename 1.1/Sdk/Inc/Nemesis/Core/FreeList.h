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
#include "Assert.h"

//======================================================================================
namespace nemesis
{
	struct FreeLink_s
	{
		FreeLink_s* Next;
	};

	struct FreeList_s
	{
		FreeLink_s* Free;
		uint8_t*	Data;
		uint8_t*	End;
		int32_t		Count;
		uint32_t	ItemSize;
	};
}

//======================================================================================
namespace nemesis
{
	inline void FreeList_Init( FreeList_s* list, uint32_t item_size, int32_t num_items, void* data )
	{
		NeAssert(item_size >= sizeof(FreeLink_s*));
		list->Free		= (FreeLink_s*)data;
		list->Data		=  (uint8_t*) data;
		list->End		= ((uint8_t*) data) + item_size * num_items;
		list->ItemSize	= item_size;
		list->Count		= 0;

		uint8_t* p = list->Data;
		uint8_t* n = list->Data + item_size;
		for ( int i = 0; i < (num_items-1); ++i, p += item_size, n += item_size )
			((FreeLink_s*)(p))->Next = (FreeLink_s*)(n);
		((FreeLink_s*)(p))->Next = nullptr;
		NeAssert(n == list->End);
	}

	inline bool FreeList_Contains( FreeList_s* list, void* ptr )
	{
		return (ptr >= list->Data) && (ptr < list->End);
	}

	inline void* FreeList_Alloc( FreeList_s* list )
	{
		FreeLink_s* item = list->Free;
		if (item)
		{
			list->Free = list->Free->Next;
			list->Count++;
		}
		return item;
	}

	inline void FreeList_Free( FreeList_s* list, void* ptr )
	{
		if (!ptr)
			return;
		NeAssert(FreeList_Contains( list, ptr ));
		FreeLink_s* link = (FreeLink_s*)ptr;
		link->Next = list->Free;
		list->Free = link;
		list->Count--;
		NeAssert(list->Count >= 0);
	}
} 