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
#include "Types.h"

//======================================================================================
namespace nemesis
{
	struct FreeListLink_s
	{
		FreeListLink_s* Next;
	};

	template < typename T, int NUM_ITEMS >
	struct FreeList
	{
		FreeListLink_s* Free;
		T				Item[ NUM_ITEMS ];

		NeStaticAssert(sizeof(T) >= (sizeof(T*)));
	};

	template < typename T, int N >
	inline void FreeList_Initialize( FreeList<T,N>& list )
	{
		list.Free = (FreeListLink_s*)(list.Item);
		for ( int i = 0; i < (N-1); ++i )
		{
			FreeListLink_s* link = (FreeListLink_s*)(list.Item+i  );
			FreeListLink_s* next = (FreeListLink_s*)(list.Item+i+1);
			link->Next = next;
		}
	}

	template < typename T, int N >
	inline T* FreeList_Alloc( FreeList<T,N>& list )
	{
		T* item = (T*)list.Free;
		if (item)
			list.Free = list.Free->Next;
		return item;
	}

	template < typename T, int N >
	inline void FreeList_Free( FreeList<T,N>& list, T* item )
	{
		NeAssert( (item >= list.Item) && (item < (list.Item+N)) );
		FreeListLink_s* link = (FreeListLink_s*)item;
		link->Next = list.Free;
		list.Free = link;
	}

}
