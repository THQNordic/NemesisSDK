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
#include "Assert.h"
#include "Memory.h"

//======================================================================================
namespace nemesis
{
	struct HashTableStats_s
	{
		uint32_t SetCalls;	
		uint32_t SetHits;	// # of times set call executed in O(1)
		uint32_t SetMisses;	// # of keys compared looking for insert position
		uint32_t GetCalls;	
		uint32_t GetHits;	// # of times get call executed in O(1)
		uint32_t GetMisses;	// # of keys compared looking for existing key
		uint32_t Overflows;	// # of times table was re-allocated because the load factor became too high
	};

	template < typename K, typename V >
	struct HashTable
	{
		K* Key;
		V* Val;
		int32_t Count;
		int32_t Capacity;
		Allocator_t Alloc;
	#if (NE_CONFIG < NE_CONFIG_MASTER)
		mutable HashTableStats_s Stats;
	#endif
	};

}

//======================================================================================
namespace nemesis
{
	typedef HashTable<uint64_t, uint32_t> HashTable_64_32_s;

}

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline void Array_Fill( T* a, int count, T v )
	{
		for ( int i = 0; i < count; ++i )
			a[i] = v;
	}

	template < typename T >
	inline int Array_Probe_Linear( const T* a, int count, int start, T v, T stop )
	{
		NeAssert(start < count);
		for ( int i = start; i < count; ++i )
		{
			const T k = a[i];
			if ((k == v) || (k == stop))
				return i;
		}
		for ( int i = 0; i < start; ++i )
		{
			const T k = a[i];
			if ((k == v) || (k == stop))
				return i;
		}
		return -1;
	}
}

//======================================================================================
namespace nemesis
{
	template < typename K, typename V >
	inline void HashTable_Init( HashTable<K,V>& table, Allocator_t alloc )
	{
		NeZero(table);
		table.Alloc = alloc;
	}

	template < typename K, typename V >
	inline void HashTable_Clear( HashTable<K,V>& table )
	{
		Mem_Free( table.Alloc, table.Key );
		Mem_Free( table.Alloc, table.Val );
		table.Count = 0;
		table.Capacity = 0;
	#if NE_CONFIG < NE_CONFIG_MASTER
		NeZero( table.Stats );
	#endif
	}

	template < typename K, typename V >
	inline void HashTable_Reset( HashTable<K,V>& table )
	{
		Array_Fill( table.Key, table.Capacity, ~(K(0)) );
		table.Count = 0;
	#if NE_CONFIG < NE_CONFIG_MASTER
		NeZero( table.Stats );
	#endif
	}

	template < typename K, typename V >
	inline void HashTable_Set_NoGrow( HashTable<K,V>& table, K key, V val )
	{
		NeAssert(key != ~(K(0)));
		const int slot = (key % table.Capacity);
		const int pos = Array_Probe_Linear( table.Key, table.Capacity, slot, key, ~(K(0)) );
		NeAssert(pos >= 0);
		if (table.Key[pos] == ~(K(0)))
			++table.Count;
		table.Key[pos] = key;
		table.Val[pos] = val;
	#if NE_CONFIG < NE_CONFIG_MASTER
		table.Stats.SetCalls  += 1;
		table.Stats.SetHits   += (slot == pos) ? 1 : 0;
		table.Stats.SetMisses += (slot <= pos) ? (pos-slot) : (pos + table.Capacity) - slot;
	#endif
	}

	template < typename K, typename V >
	inline void HashTable_Grow( HashTable<K,V>& table )
	{
		// don't grow until we reach a load factor of 0.5 
		if (table.Capacity > (2*table.Count))
			return;

		// calculate new capacity
		const int new_capacity = table.Capacity ? 2*table.Capacity : 64;

		// create a new table
		HashTable<K,V> new_table = {};
		new_table.Key = Mem_Alloc<K>( table.Alloc, new_capacity ); 
		new_table.Val = Mem_Alloc<V>( table.Alloc, new_capacity );
		new_table.Alloc = table.Alloc;
		new_table.Capacity = new_capacity;
	#if (NE_CONFIG < NE_CONFIG_MASTER)
		new_table.Stats = table.Stats;
		new_table.Stats.Overflows = table.Stats.Overflows+1;
	#endif
		Array_Fill( new_table.Key, new_table.Capacity, ~(K(0)) );
		Array_Fill( new_table.Val, new_table.Capacity,  (V(0)) );

		// copy existing keys from old table to new table
		for ( int i = 0; i < table.Capacity; ++i )
		{
			const K key = table.Key[i];
			if (key == ~(K(0)))
				continue;
			HashTable_Set_NoGrow( new_table, key, table.Val[i] );
		}

		// swap tables 
		HashTable<K,V> old_table = table;
		table = new_table;

		// clear old table
		HashTable_Clear( old_table );
	}

	template < typename K, typename V >
	inline void HashTable_Set( HashTable<K,V>& table, K key, V val )
	{
		HashTable_Grow( table );
		HashTable_Set_NoGrow( table, key, val );
	}

	template < typename K, typename V >
	inline V HashTable_Get( const HashTable<K,V>& table, K key, V def )
	{
		if (table.Capacity)
		{
			const int slot = (key % table.Capacity);
			const int pos = Array_Probe_Linear( table.Key, table.Capacity, slot, key, ~(K(0)) );
			NeAssert(pos >= 0);
		#if (NE_CONFIG < NE_CONFIG_MASTER)
			table.Stats.GetCalls  += 1;
			table.Stats.GetHits   += (slot == pos) ? 1 : 0;
			table.Stats.GetMisses += (slot <= pos) ? (pos-slot) : (pos + table.Capacity) - slot;
		#endif
			if (table.Key[pos] == key) 
				return table.Val[pos];
		}
		return def;
	}

	template < typename K, typename V >
	inline bool HashTable_Has( const HashTable<K,V>& table, K key )
	{
		if (table.Capacity)
		{
			const int slot = (key % table.Capacity);
			const int pos = Array_Probe_Linear( table.Key, table.Capacity, slot, key, ~(K(0)) );
			NeAssert(pos >= 0);
		#if NE_CONFIG < NE_CONFIG_MASTER
			table.Stats.GetCalls  += 1;
			table.Stats.GetHits   += (slot == pos) ? 1 : 0;
			table.Stats.GetMisses += (slot <= pos) ? (pos-slot) : (pos + table.Capacity) - slot;
		#endif
			return (table.Key[pos] == key);
		}
		return false;
	}
}
