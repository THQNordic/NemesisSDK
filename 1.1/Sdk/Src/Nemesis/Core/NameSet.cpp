//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/NameSet.h>
#include <Nemesis/Core/String.h>

//======================================================================================
//																				 Private
//======================================================================================
namespace nemesis
{
	namespace 
	{
		struct Comparer
		{
			static int Compare( const NameSetItem_s& lhs, const NameSetItem_s& rhs )
			{
				if (lhs.Size < rhs.Size)
					return -1;
				if (lhs.Size > rhs.Size)
					return 1;
				return Str_Cmp(lhs.Text, rhs.Text, rhs.Size);
			}
		};
	}

	static int NameSet_Find( NameSet_t map, cstr_t text, size_t size )
	{
		const NameSetItem_s find = { size, text };
		return Array_BinaryFind<Comparer>(map->Items, find);
	}

}

//======================================================================================
//																				  Public
//======================================================================================
namespace nemesis
{
	void NameSet_Initialize( NameSet_t map, Allocator_t alloc )
	{
		map->Items.Init( alloc );
		map->Stack.Init( alloc );
	}

	void NameSet_Shutdown( NameSet_t map )
	{
		map->Items.Clear();
		map->Stack.Clear();
	}

	void NameSet_Reset( NameSet_t map )
	{
		map->Items.Reset();
		map->Stack.Reset();
	}

	bool NameSet_IsEmpty( NameSet_t map )
	{
		return map->Items.Count == 0;
	}

	cstr_t NameSet_Intern( NameSet_t map, cstr_t s, cstr_t e )
	{
		const ptrdiff_t len = e-s;
		if (len < 0)
			return nullptr;

		const int found = NameSet_Find( map, s, len );
		if (found >= 0)
			return map->Items[found].Text;

		NeAssert((len+1) < UINT32_MAX);
		const uint32_t size = (uint32_t)(len+1);
		str_t clone = (str_t)(map->Stack.Alloc( size ));
		Mem_Cpy(clone, s, len);
		clone[len] = 0;

		const int idx = ~found;
		map->Items.InsertAt( idx, { (size_t)len, clone } );

		return clone;
	}

	cstr_t NameSet_Intern( NameSet_t map, cstr_t s )
	{
		return NameSet_Intern( map, s, s + Str_Len(s) );
	}

}
