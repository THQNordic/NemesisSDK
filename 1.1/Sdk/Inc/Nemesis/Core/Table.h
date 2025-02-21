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
	struct ReBase
	{
		enum Op
		{ Attach	// file -> memory
		, Detach	// memory -> file
		};
	};

	struct TableSetup
	{
		enum Mode
		{ CopyData
		, CalcSize
		};
	};

	template < typename T >
	struct Table
	{
		union
		{
			T*		 Item;
			uint8_t* Data;
			int64_t	 Offset;
		};
		int32_t Count;
		int32_t User;

		uint8_t* Bind( uint8_t* data )
		{
			Data = Count ? data : nullptr;
			return data + Mem_Align_16( sizeof(T) * Count );
		}

		void Copy( const T* item, int count )
		{
			NeAssert( Count == count );
			Mem_Cpy( Item, item, count * sizeof(T) ); 
		}

		void ReBase( ReBase::Op op )
		{
			switch (op)
			{
			case ReBase::Attach:
				NeAssertOut(!Data || (Ptr_Tell( this, Data ) < 0), "Trying to attach a table that is already located.");
				if (Data)
					Data += (ptrdiff_t)this;
				NeAssert(Offset % 16 == 0);
				break;

			case ReBase::Detach:
				NeAssertOut(!Data || (Ptr_Tell( this, Data ) > 0), "Trying to detach a table that is already dislocated.");
				if (Data)
					Data -= (ptrdiff_t)this;
				NeAssert(Offset % 16 == 0);
				break;

			default:
				break;
			}
		}

		const T& operator [] ( int index ) const
		{
			NeAssertBounds( index, Count );
			return Item[ index ];
		}

		T& operator [] ( int index )
		{
			NeAssertBounds( index, Count );
			return Item[ index ];
		}
	};

	template < typename T >
	inline void Table_Init( Table<T>* table, int count )
	{ table->Count = count; }

	template < typename T >
	inline uint8_t* Table_Bind( Table<T>* table, uint8_t* data )
	{ return table->Bind( data ); }

	template < typename T >
	inline void Table_CopyData( Table<T>* table, const T* item, int count )
	{ table->Copy( item, count ); }

	template < typename T >
	inline void Table_ReBase( Table<T>* table, ReBase::Op op )
	{ table->ReBase( op ); }

	template < typename T >
	inline uint8_t* Table_Setup( Table<T>* table, uint8_t* pos, const T* item, int count, TableSetup::Mode mode )
	{ 
		Table_Init( table, count );
		pos = Table_Bind( table, pos );
		if (mode == TableSetup::CalcSize)
			return pos;
		Table_CopyData( table, item, count );
		return pos;
	}

}

//======================================================================================
namespace nemesis { namespace anim
{
	/// Bit Table

	struct BitTable_s
	{
		Table<uint32_t> Blocks;
	};

	void	 BitTable_Init ( BitTable_s* table, int num_bits );
	uint8_t* BitTable_Bind ( BitTable_s* table, uint8_t* pos );
	uint8_t* BitTable_Setup( BitTable_s* table, uint8_t* pos, int num_bits );

	bool BitTable_GetBit( BitTable_s* table, int bit );
	void BitTable_SetBit( BitTable_s* table, int bit );
	void BitTable_SetBit( BitTable_s* table, int bit, bool state );

	void BitTable_ClearBits	( BitTable_s* table, const BitTable_s* mask );
	void BitTable_ClearAll	( BitTable_s* table );

} }

//======================================================================================
namespace nemesis
{
	struct NameTable_s
	{
		Table<int32_t> Offsets;
		Table<char>	   Strings;
	};

	uint8_t* NameTable_Bind		( NameTable_s* table, uint8_t* data );
	void	 NameTable_ReBase	( NameTable_s* table, ReBase::Op op );
	void	 NameTable_Init		( NameTable_s* table, const cstr_t* item, int count );
	void	 NameTable_CopyData	( NameTable_s* table, const cstr_t* item, int count );
	uint8_t* NameTable_Setup	( NameTable_s* table, uint8_t* pos, const cstr_t* item, int count, TableSetup::Mode mode );
	cstr_t	 NameTable_GetName	( NameTable_s* table, int index );

}

//======================================================================================
namespace nemesis
{
	struct BlobTable_s
	{
		Table<int32_t> Offsets;
		Table<uint8_t> Items;
	};

	uint8_t* BlobTable_Bind		  ( BlobTable_s* table, uint8_t* data );
	void	 BlobTable_ReBase	  ( BlobTable_s* table, ReBase::Op op );
	int		 BlobTable_GetNumItems( BlobTable_s* table );
	uint8_t* BlobTable_GetItem	  ( BlobTable_s* table, int index );

}
