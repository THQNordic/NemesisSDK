//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Table.h>
#include <Nemesis/Core/String.h>
#include <Nemesis/Core/Memory.h>

//======================================================================================
namespace nemesis
{
	/// String

	static int StrTable_Size( const cstr_t* item, int count )
	{
		int sum = 0;
		for ( int i = 0; i < count; ++i )
			sum += item[i] ? (1+Str_Len( item[i] )) : 0;
		return sum;
	}
}

//======================================================================================
namespace nemesis { namespace anim
{
	/// Bit Table

	static int32_t BitSet_BitToBlock( int bit )
	{
		return bit >> 5;
	}

	static uint32_t BitSet_BitToMask( int bit )
	{
		return (0x80000000 >> (bit & 31l));
	}

	void BitTable_Init( BitTable_s* table, int num_bits )
	{
		table->Blocks.Count =  (num_bits / 32)			// full
							+ ((num_bits % 32) ? 1 : 0)	// partial
							;
	}

	uint8_t* BitTable_Bind( BitTable_s* table, uint8_t* pos )
	{
		return table->Blocks.Bind( pos );
	}

	uint8_t* BitTable_Setup( BitTable_s* table, uint8_t* pos, int num_bits )
	{
		BitTable_Init( table, num_bits );
		return BitTable_Bind( table, pos );
	}

	bool BitTable_GetBit( BitTable_s* table, int bit )
	{
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		NeAssert( (block >= 0) && (block < table->Blocks.Count) );
		return (mask == (table->Blocks.Item[ block ] & mask));
	}

	void BitTable_SetBit( BitTable_s* table, int bit )
	{
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		table->Blocks.Item[ block ] |= mask;
	}

	void BitTable_ClearBit( BitTable_s* table, int bit )
	{
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		table->Blocks.Item[ block ] &= ~mask;
	}

	void BitTable_ToggleBit( BitTable_s* table, int bit )
	{
		const int32_t block = BitSet_BitToBlock( bit );
		const uint32_t mask = BitSet_BitToMask ( bit );
		table->Blocks.Item[ block ] ^= mask;
	}

	void BitTable_SetBit( BitTable_s* table, int bit, bool state )
	{
		return state 
			 ? BitTable_SetBit  ( table, bit ) 
			 : BitTable_ClearBit( table, bit );
	}

	void BitTable_ClearBits( BitTable_s* table, const BitTable_s* mask )
	{
		NeAssert( mask->Blocks.Count == table->Blocks.Count );
		for ( int i = 0; i < mask->Blocks.Count; ++i )
			table->Blocks.Item[i] &= ~(mask->Blocks.Item[i]);
	}

	void BitTable_ClearAll( BitTable_s* table )
	{
		Mem_Set( table->Blocks.Item, 0, table->Blocks.Count * sizeof(table->Blocks.Item[0]) );
	}

} }

//======================================================================================
namespace nemesis
{
	/// Name Table

	uint8_t* NameTable_Bind( NameTable_s* table, uint8_t* data )
	{
		data = table->Offsets.Bind( data );
		data = table->Strings.Bind( data );
		return data;
	}

	void NameTable_ReBase( NameTable_s* table, ReBase::Op op )
	{
		table->Offsets.ReBase( op );
		table->Strings.ReBase( op );
	}

	void NameTable_Init( NameTable_s* table, const cstr_t* item, int count )
	{
		const int32_t str_size = (item && count) ? StrTable_Size( item, count ) : 0;

		table->Offsets.Count = str_size ? count					   : 0;
		table->Strings.Count = str_size ? Mem_Align_16( str_size ) : 0;
	}

	void NameTable_CopyData( NameTable_s* table, const cstr_t* item, int count )
	{
		if (!table->Offsets.Count)
			return;

		NeAssert(table->Offsets.Count == count);

		int len;
		char* begin = table->Strings.Item;
		char* pos = begin;
		for ( int i = 0; i < table->Offsets.Count; ++i )
		{
			// offset
			table->Offsets.Item[i] = (int)(pos-begin);

			// string
			len = item[i] ? 1+Str_Len( item[i] ) : 0;
			Mem_Cpy( pos, item[i], len );

			// next
			pos = pos + len;
		}
	}

	uint8_t* NameTable_Setup( NameTable_s* table, uint8_t* pos, const cstr_t* item, int count, TableSetup::Mode mode )
	{
		NameTable_Init( table, item, count );
		pos = NameTable_Bind( table, pos );
		if (mode == TableSetup::CalcSize)
			return pos;
		NameTable_CopyData( table, item, count );
		return pos;
	}

	cstr_t NameTable_GetName( NameTable_s* table, int index )
	{
		if (!table->Offsets.Data)
			return nullptr;
		if ((index < 0) || (index >= table->Offsets.Count))
			return nullptr;
		return table->Strings.Item + table->Offsets.Item[ index ];
	}

}

//======================================================================================
namespace nemesis
{
	uint8_t* BlobTable_Bind( BlobTable_s* table, uint8_t* data )
	{
		data = Table_Bind( &table->Offsets, data );
		data = Table_Bind( &table->Items  , data );
		return data;
	}

	void BlobTable_ReBase( BlobTable_s* table, ReBase::Op op )
	{
		Table_ReBase( &table->Offsets, op );
		Table_ReBase( &table->Items  , op );
	}

	int BlobTable_GetNumItems( BlobTable_s* table )
	{
		return table->Offsets.Count;
	}

	uint8_t* BlobTable_GetItem( BlobTable_s* table, int index )
	{
		if (!table->Offsets.Data)
			return nullptr;
		if ((index < 0) || (index >= table->Offsets.Count))
			return nullptr;
		return table->Items.Item + table->Offsets.Item[ index ];
	}
}
