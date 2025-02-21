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
#include "Array.h"
#include "Stack.h"

//======================================================================================
namespace nemesis
{
	typedef struct NameSet_s *NameSet_t;

	struct NameSetItem_s
	{
		size_t Size;
		cstr_t Text;
	};

	struct NameSet_s
	{
		Array<NameSetItem_s> Items;
		Stack_s				 Stack;
	};

	void NE_API NameSet_Initialize( NameSet_t map, Allocator_t alloc );
	void NE_API NameSet_Shutdown( NameSet_t map );
	void NE_API NameSet_Reset( NameSet_t map );
	bool NE_API NameSet_IsEmpty( NameSet_t map );

	cstr_t NE_API NameSet_Intern( NameSet_t map, cstr_t s, cstr_t e );
	cstr_t NE_API NameSet_Intern( NameSet_t map, cstr_t s );

}
