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
#include "Alloc.h"
#include <new>

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline void Mem_Destruct( T* p )
	{ if (p) p->~T(); }
}

//======================================================================================
#define NE_ALLOW_DESTRUCT\
	template < typename T > friend void ::nemesis::Mem_Destruct( T* ptr )

//======================================================================================
#define NE_ALLOC_NEW(	 alloc, T ) new ( Mem_Alloc( alloc, sizeof(T) ) ) T
#define NE_ALLOC_DELETE( alloc, p ) do { if( p ) { ::nemesis::Mem_Destruct( p ); Mem_Free( alloc, p ); } } while(false)

//======================================================================================
#define NE_NEW(    T ) NE_ALLOC_NEW(    nullptr, T )
#define NE_DELETE( p ) NE_ALLOC_DELETE( nullptr, p )
