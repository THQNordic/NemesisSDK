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
#include "Assert.h"
#include "Memory.h"

//======================================================================================
namespace nemesis
{
	struct BitSet_s
	{
		uint32_t* Block;
		int32_t NumBlocks;
		int32_t NumBits;
	};

	inline int32_t BitSet_BitToBlock( int bit )
	{
		return bit >> 5;
	}

	inline uint32_t BitSet_BitToMask( int bit )
	{
		return (0x80000000 >> (bit & 31l));
	}

	inline bool BitSet_IsBitSet( const BitSet_s& set, int bit )
	{
		NeAssert((bit >= 0) && (bit < set.NumBits));
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		return (set.Block[block] & mask);
	}

	inline void BitSet_ClearBit( const BitSet_s& set, int bit )
	{
		NeAssert((bit >= 0) && (bit < set.NumBits));
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		set.Block[block] &= ~mask;
	}

	inline void BitSet_SetBit( const BitSet_s& set, int bit )
	{
		NeAssert((bit >= 0) && (bit < set.NumBits));
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		set.Block[block] |= mask;
	}

	inline void BitSet_SetBit( const BitSet_s& set, int bit, bool on )
	{
		return on 
			? BitSet_SetBit	 ( set, bit )
			: BitSet_ClearBit( set, bit );
	}

	inline void BitSet_Set( const BitSet_s& mask )
	{
		for ( int i = 0; i < mask.NumBits; ++i )
			BitSet_SetBit( mask, i );
	}

	inline void BitSet_Clear( const BitSet_s& mask )
	{
		Mem_Set( mask.Block, 0, mask.NumBlocks * sizeof(mask.Block[0]) );
	}
}
