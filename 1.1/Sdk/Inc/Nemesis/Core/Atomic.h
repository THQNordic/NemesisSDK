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
#include "AtomicTypes.h"

//======================================================================================
namespace nemesis
{	
	void NE_API Interlocked_Exchange( Atomic32* p, int32_t v );
}

//======================================================================================
namespace nemesis {	namespace atomic 
{
	struct Int32
	{
	public:
		Int32()
			: Value(0)
		{}

		Int32( int32_t value )
			: Value( value )
		{}

	public:
		void Set( int32_t value )
		{ Interlocked_Exchange( &Value, value ); }

	public:
		operator int32_t () const
		{ return Value; }

		void operator = ( int32_t value ) 
		{ Set( value ); }

	private:
		Atomic32 Value;
	};

} }
