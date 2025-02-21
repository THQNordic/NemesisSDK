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

//======================================================================================
namespace nemesis
{
	/// A pair consisting of key and value
	///
	///	K: The key type
	/// T: The value type
	template < typename K, typename T >
	struct KeyValuePair
	{
		K Key;
		T Value;
	};

	/// A set of unique keys
	/// 
	/// K: The key type
	///
	///	This version always appends to the end of the arrays and 
	/// uses linear search to find existing keys.
	template < typename K >
	class LinearArraySet
	{
	public:
		LinearArraySet();
		~LinearArraySet();

	public:
		bool Contains( const K& key ) const;
		int IndexOf( const K& key ) const;

	public:
		int Register( const K& key );

	public:
		void Remove( const K& key );

	public:
		void Reset();
		void Clear();

	public:
		Array<K> Keys;
	};

	//----------------------------------------------------------------------------------
	template < typename K >
	LinearArraySet<K>::LinearArraySet()
	{}

	template < typename K >
	LinearArraySet<K>::~LinearArraySet()
	{}

	template < typename K >
	inline bool LinearArraySet<K>::Contains( const K& key ) const
	{ return IndexOf(key) >= 0; }

	template < typename K >
	inline int LinearArraySet<K>::IndexOf( const K& key ) const
	{ return Array_LinearFind( Keys, key ); }

	template < typename K >
	int LinearArraySet<K>::Register( const K& key )
	{
		const int index = IndexOf( key );
		if ( index >= 0 )
			return index;
		Keys.Append(key);
		return Keys.Count-1;
	}

	template < typename K >
	void LinearArraySet<K>::Remove( const K& key )
	{
		const int index = IndexOf( key );
		Keys.RemoveAt( index );
	}

	template < typename K >
	inline void LinearArraySet<K>::Reset()
	{ 
		Keys.Reset(); 
	}

	template < typename K >
	inline void LinearArraySet<K>::Clear()
	{ 
		Keys.Clear(); 
	}

	/// Maps keys to values
	/// 
	/// K: The key type
	/// T: The value type
	///
	///	This version always appends to the end of the arrays and 
	/// uses linear search to find existing keys.
	///
	/// In contrast, BinaryArrayMap sorts keys on inserts and 
	/// uses binary search to find existing keys.
	template < typename K, typename T >
	class LinearArrayMap
	{
	public:
		explicit LinearArrayMap( Allocator_t alloc );
		LinearArrayMap();
		~LinearArrayMap();

	public:
		bool Contains( K key ) const;
		int IndexOf( K key ) const;

	public:
		bool Lookup( K key, T& value ) const;
		int Register( K key, const T& value );

	public:
		void Remove( K key );
		void Remove( K key, T& value );

	public:
		void Reset();
		void Clear();

	public:
		const T& operator [] ( K key ) const;

	public:
		Array<K> Keys;
		Array<T> Values;
	};

	//----------------------------------------------------------------------------------
	template < typename K, typename T >
	LinearArrayMap<K,T>::LinearArrayMap( Allocator_t alloc )
		: Keys(alloc)
		, Values(alloc)
	{}

	template < typename K, typename T >
	LinearArrayMap<K,T>::LinearArrayMap()
	{}

	template < typename K, typename T >
	LinearArrayMap<K,T>::~LinearArrayMap()
	{}

	template < typename K, typename T >
	inline bool LinearArrayMap<K,T>::Contains( K key ) const
	{ return IndexOf(key) >= 0; }

	template < typename K, typename T >
	inline int LinearArrayMap<K,T>::IndexOf( K key ) const
	{ return Array_LinearFind( Keys, key ); }

	template < typename K, typename T >
	bool LinearArrayMap<K,T>::Lookup( K key, T& value ) const
	{
		const int index = IndexOf( key );
		if ( index >= 0 )
		{
			value = Values[index];
			return true;
		}
		return false;
	}

	template < typename K, typename T >
	int LinearArrayMap<K,T>::Register( K key, const T& value )
	{
		const int index = IndexOf( key );
		if ( index >= 0 )
			return index;

		Keys.Append(key);
		Values.Append(value);
		return Keys.Count-1;
	}

	template < typename K, typename T >
	void LinearArrayMap<K,T>::Remove( K key )
	{
		const int index = IndexOf( key );
		Keys.RemoveAt( index );
		Values.RemoveAt( index );
	}

	template < typename K, typename T >
	void LinearArrayMap<K,T>::Remove( K key, T& value )
	{
		const int index = IndexOf( key );
		value = Values[index];
		Keys.RemoveAt( index );
		Values.RemoveAt( index );
	}

	template < typename K, typename T >
	inline void LinearArrayMap<K,T>::Reset()
	{ 
		Keys.Reset(); 
		Values.Reset();
	}

	template < typename K, typename T >
	inline void LinearArrayMap<K,T>::Clear()
	{ 
		Keys.Clear(); 
		Values.Clear();
	}

	template < typename K, typename T >
	const T& LinearArrayMap<K,T>::operator [] ( K key ) const
	{
		const int index = IndexOf(key);
		NeAssertOut(index >= 0, "[Access violation] Invalid key: %u", key);
		return Values[index];
	}

	/// Maps keys to values
	/// 
	/// K: The key type
	/// T: The value type
	///
	/// This version sorts keys on inserts and 
	/// uses binary search to find existing keys.
	///
	///	In contrast, LinearArrayMap always appends to the end of
	/// the arrays and uses linear search to find existing keys.
	template 
		< typename K
		, typename T
		, typename C = IntrinsicComparer
		>
	class BinaryArrayMap
	{
	public:
		explicit BinaryArrayMap( Allocator_t alloc );
		BinaryArrayMap();
		~BinaryArrayMap();

	public:
		void Init( Allocator_t alloc );

	public:
		bool Contains( K key ) const;
		int IndexOf( K key ) const;

	public:
		bool Lookup( K key, T& value ) const;
		int Register( K key, const T& value );

	public:
		void Remove( K key );
		void Remove( K key, T& value );

	public:
		void Reset();
		void Clear();

	public:
		const T& operator [] ( K key ) const;

	public:
		Array<K> Keys;
		Array<T> Values;
	};

	//----------------------------------------------------------------------------------
	template < typename K, typename T, typename C >
	BinaryArrayMap<K,T,C>::BinaryArrayMap( Allocator_t alloc )
		: Keys	( alloc )
		, Values( alloc )
	{}

	template < typename K, typename T, typename C >
	BinaryArrayMap<K,T,C>::BinaryArrayMap()
	{}

	template < typename K, typename T, typename C >
	BinaryArrayMap<K,T,C>::~BinaryArrayMap()
	{}

	template < typename K, typename T, typename C >
	inline void BinaryArrayMap<K,T,C>::Init( Allocator_t alloc )
	{ 
		Keys.Alloc = alloc;
		Values.Alloc = alloc;
	}

	template < typename K, typename T, typename C >
	inline bool BinaryArrayMap<K,T,C>::Contains( K key ) const
	{ return IndexOf(key) >= 0; }

	template < typename K, typename T, typename C >
	inline int BinaryArrayMap<K,T,C>::IndexOf( K key ) const
	{ 
		const int pos = Array_BinaryFind<C>( Keys, key );
		return (pos >= 0) ? pos : -1;
	}

	template < typename K, typename T, typename C >
	bool BinaryArrayMap<K,T,C>::Lookup( K key, T& value ) const
	{
		const int pos = Array_BinaryFind<C>( Keys, key );
		if ( pos >= 0 )
		{
			value = Values[pos];
			return true;
		}
		return false;
	}

	template < typename K, typename T, typename C >
	int BinaryArrayMap<K,T,C>::Register( K key, const T& value )
	{
		const int pos = Array_BinaryFind<C>( Keys, key );
		if ( pos >= 0 )
			return pos;
		const int idx = ~pos;
		Keys.InsertAt(idx, key);
		Values.InsertAt(idx, value);
		return idx;
	}

	template < typename K, typename T, typename C >
	void BinaryArrayMap<K,T,C>::Remove( K key )
	{
		const int index = IndexOf( key );
		Keys.RemoveAt( index );
		Values.RemoveAt( index );
	}

	template < typename K, typename T, typename C >
	void BinaryArrayMap<K,T,C>::Remove( K key, T& value )
	{
		const int index = IndexOf( key );
		value = Values[index];
		Keys.RemoveAt( index );
		Values.RemoveAt( index );
	}

	template < typename K, typename T, typename C >
	inline void BinaryArrayMap<K,T,C>::Reset()
	{ 
		Keys.Reset(); 
		Values.Reset();
	}

	template < typename K, typename T, typename C >
	inline void BinaryArrayMap<K,T,C>::Clear()
	{ 
		Keys.Clear(); 
		Values.Clear();
	}

	template < typename K, typename T, typename C >
	const T& BinaryArrayMap<K,T,C>::operator [] ( K key ) const
	{
		const int index = IndexOf(key);
		NeAssertOut(index >= 0, "[Access violation] Invalid key: %u", key);
		return Values[index];
	}
}
