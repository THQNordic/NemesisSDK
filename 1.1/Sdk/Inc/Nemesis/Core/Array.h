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
#include "ArrayTypes.h"
#include "Alloc.h"
#include "Assert.h"
#include "Memory.h"

//======================================================================================
namespace nemesis
{
	template < typename T >
	Array<T>::Array( const Array<T>& rhs )
	: Data(nullptr)
	, Count(0)
	, Capacity(0)
	, Alloc(nullptr)
	{
		this->operator = ( rhs );
	}

	template < typename T >
	Array<T>::Array( Allocator_t alloc )
	: Data(nullptr)
	, Count(0)
	, Capacity(0)
	, Alloc(alloc)
	{}

	template < typename T >
	Array<T>::Array()
	: Data(nullptr)
	, Count(0)
	, Capacity(0)
	, Alloc(nullptr)
	{}

	template < typename T >
	Array<T>::~Array()
	{ Clear(); }

	template < typename T >
	Array<T>& Array<T>::operator = ( const Array<T>& rhs )
	{
		if (this != &rhs)
		{
			Resize( rhs.Count );
			BitwiseCopier::Copy(Data, rhs.Data, rhs.Count);
		}
		return *this;
	}

	template < typename T >
	inline T& Array<T>::operator [] ( int index )
	{
		NeAssertOut((index >= 0) && (index < Count), "Invalid index: %i", index);
		return Data[index];
	}

	template < typename T >
	inline const T& Array<T>::operator [] ( int index ) const
	{
		NeAssertOut((index >= 0) && (index < Count), "Invalid index: %i", index);
		return Data[index];
	}

	template < typename T >
	void Array<T>::Init( Allocator_t alloc )
	{ 
		Alloc = alloc;
	}

	template < typename T >
	void Array<T>::Clear()
	{ 
		if (!Data)
			return;
		Mem_Free(Alloc, Data);
		Data = nullptr;
		Count = 0;
		Capacity = 0;
	}

	template < typename T >
	inline void Array<T>::Reset()
	{ Resize( 0 ); }

	template < typename T >
	void Array<T>::GrowBy( int count )
	{
		NeAssertOut(count >= 0, "Invalid count: %d", count);
		if (Count + count > Capacity)
		{
			const int growBy = NeMax(Capacity >> 3, 4/*, 1024*/);
			const int newCapacity = Capacity + growBy + count;
			Reserve( newCapacity );
		}
		Count += count;
	}

	template < typename T >
	void Array<T>::Resize( int count )
	{
		NeAssertOut(count >= 0, "Invalid count: %d", count);
		Reserve( count );
		if ( count < Count )
			ZeroInitializer::Initialize( Data + count, Count - count );
		Count = count;
	}

	template < typename T >
	void Array<T>::Reserve( int capacity )
	{
		NeAssertOut(capacity >= 0, "Invalid capacity: %d", capacity);
		if (Capacity >= capacity)
			return;
		Data = (T*)Mem_Realloc(Alloc, Data, capacity * sizeof(T));
		ZeroInitializer::Initialize(Data + Capacity, capacity - Capacity);
		Capacity = capacity;
	}

	template < typename T >
	inline T& Array<T>::Append()
	{ 
		GrowBy(1);
		return Data[Count-1];
	}

	template < typename T >
	inline void Array<T>::Append( const T& value )
	{ Append( &value, 1 ); }

	template < typename T >
	void Array<T>::Append( const T* data, int count )
	{
		NeAssertOut(count >= 0, "Invalid count: %d", count);
		NeAssertOut((count == 0) || (data != nullptr), "Invalid data");
		if (data && count)
		{
			const int at = Count;
			GrowBy( count );
			BitwiseCopier::Copy( Data + at, data, count );
		}
	}

	template < typename T >
	inline void Array<T>::Append( const Array<T>& data )
	{ Append( data.Data, data.Count ); }

	template < typename T >
	inline void Array<T>::InsertAt( int index, const T& value )
	{ InsertAt (index, &value, 1 ); }

	template < typename T >
	void Array<T>::InsertAt( int index, const T* data, int count )
	{
		NeAssertOut(data, "Invalid data");
		NeAssertOut(count >= 0, "Invalid count: %d", count);
		NeAssertOut((index >= 0) && (index <= Count), "Invalid index: %d", index);
		if (index == Count)	
			return Append( data, count );

		if (data && count)
		{
			GrowBy( count );
			const int end = index + count;
			Mem_Mov( Data + end, Data + index, (Count - end) * sizeof(T) );
			BitwiseCopier::Copy( Data + index, data, count );
		}
	}

	template < typename T >
	inline void Array<T>::InsertAt( int index, const Array<T>& data )
	{ InsertAt( index, data.Data, data.Count ); }

	template < typename T >
	inline void Array<T>::RemoveAt( int index )
	{ RemoveAt(index, 1); }

	template < typename T >
	void Array<T>::RemoveAt( int index, int count )
	{
		const int end = index + count;
		if (count)
		{
			NeAssertOut((index >= 0) && (index < Count), "Invalid index: %d", index);
			NeAssertOut(end <= Count, "Invalid count: %d", count);
			Mem_Mov( Data + index, Data + end, (Count - end) * sizeof(T) );
			Resize( Count-count );
		}
	}

	template < typename T >
	void Array<T>::Swap( Array<T>& rhs )
	{ 
		T* data = Data;
		int count = Count;
		int capacity = Capacity;

		Data = rhs.Data;
		Count = rhs.Count;
		Capacity = rhs.Capacity;

		rhs.Data = data;
		rhs.Count = count;
		rhs.Capacity = capacity;
	}
}

//======================================================================================
namespace nemesis
{
	template < typename T, int N >
	void BoundedArray<T,N>::Reset()
	{
		Count = 0;
	}

	template < typename T, int N >
	int BoundedArray<T,N>::Remain() const
	{
		return N-Count;
	}

	template < typename T, int N >
	T& BoundedArray<T,N>::Append()
	{
		NeAssert(Remain() > 0);
		return Data[Count++];
	}

	template < typename T, int N >
	void BoundedArray<T,N>::Append( const T& item )
	{
		Append() = item;
	}

	template < typename T, int N >
	bool BoundedArray<T,N>::TryAppend( const T& item )
	{
		if (Remain())
		{
			Append() = item;
			return true;
		}
		return false;
	}

	template < typename T, int N >
	T& BoundedArray<T,N>::operator [] ( int index )
	{
		NeAssert(index >= 0);
		NeAssert(index < Count);
		return Data[index];
	}

	template < typename T, int N >
	const T& BoundedArray<T,N>::operator [] ( int index ) const
	{
		NeAssert(index >= 0);
		NeAssert(index < Count);
		return Data[index];
	}
}

//======================================================================================
namespace nemesis
{
	//------------------------------------------------------------------------------

	/// Test whether the index is within the bounds of the array.
	template < typename A >
	inline bool Array_IsValidIndex( const A& a, int index )
	{ return (index >= 0) && (index < a.Count); }

	/// Determines the number of bytes occupied by a single item in the given array.
	template < typename A >
	inline size_t Array_GetItemSize( const A& a )
	{ return sizeof(a.Data[0]);	}

	/// Determines the number of bytes occupied by used items in the array.
	template < typename A >
	inline size_t Array_GetCountSize( const A& a )
	{ return a.Count * Array_GetItemSize(a); }

	/// Determines the number of bytes occupied by allocated items in the array.
	template < typename A >
	inline size_t Array_GetCapacitySize( const A& a )
	{ return a.Capacity * Array_GetItemSize(a); }

	//------------------------------------------------------------------------------

	/// Locates the first matching item within the given range.
	/// Returns -1 if no match was found.
	template < typename C, typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v, int start, int count )
	{
		const int end = start+count;
		NeAssert(IsValidIndex(a, start));
		NeAssert(end <= a.Count);
		for ( int i = start; i < end; ++i )
			if (C::Equals(v,a[i]))
				return i;
		return -1;
	}

	/// Locates the first matching item from the starting index onward.
	/// Returns -1 if no match was found.
	template < typename C, typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v, int start )
	{
		for ( int i = start; i < a.Count; ++i )
			if (C::Equals(v,a[i]))
				return i;
		return -1;
	}

	/// Locates the first matching item.
	/// Returns -1 if no match was found.
	template < typename C, typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v )
	{ return Array_LinearFind<C>( a, v, 0 ); }

	/// Locates the first matching item.
	/// Returns -1 if no match was found.
	template < typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v )
	{ return Array_LinearFind<IntrinsicComparer>( a, v ); }

	/// Locates the first matching item from the starting index onward.
	/// Returns -1 if no match was found.
	template < typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v, int start )
	{ return Array_LinearFind<IntrinsicComparer>( a, v, start ); }

	/// Locates the first matching item within the given range.
	/// Returns -1 if no match was found.
	template < typename A, typename T >
	inline int Array_LinearFind( const A& a, const T& v, int start, int count )
	{ return Array_LinearFind<IntrinsicComparer>( a, v, start, count ); }

	//------------------------------------------------------------------------------

	/// Locates first matching item from the starting index onward while wrapping 
	/// around at the end of the array.
	/// Returns -1 if no match was found.
	template < typename A, typename T >
	int Array_LinearProbe( const A& a, const T& v, int start )
	{
		const int index = Array_LinearFind( a, v, start, a.Count-start );
		if (index >= 0)
			return index;
		return Array_LinearFind( a, v, 0, start );
	}

	//------------------------------------------------------------------------------

	/// Locate a matching item within the given array by performing a binary search.
	/// The array must be sorted. 
	/// If the item was found, the function returns its index
	///	otherwise the two's-compliment of the index where the value should be inserted.
	template < typename C, typename A, typename T >
	int Array_BinaryFind( const A& a, const T& v )
	{
		int cmp;
		int pivot;
		int start = 0;
		int end = a.Count-1;
		for ( ; start <= end; )
		{
			pivot = (start+end)/2;
			cmp = C::Compare( a[pivot], v );
			if ( 0 == cmp )
				return pivot;
			if ( cmp > 0 )
				end = pivot-1;
			else
				start = pivot+1;
		}
		return ~start;
	}

	/// Locate a matching item within the given array by performing a binary search.
	/// The array must be sorted.
	template < typename A, typename T >
	int Array_BinaryFind( const A& a, const T& v )
	{ return Array_BinaryFind<IntrinsicComparer>( a, v ); }

}

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline void Array_Append( T* Data, int& Count, int Capacity, const T* data, int count )
	{
		NeAssert( (Count + count) <= Capacity );
		if (data && count)
		{
			const int at = Count;
			Count += count;
			BitwiseCopier::Copy( Data + at, data, count );
		}
	}

	template < typename T >
	inline void Array_Append( T* Data, int& Count, int Capacity, const T& v )
	{
		return Array_Append( Data, Count, Capacity, &v, 1 );
	}

	template < typename T >
	inline void Array_InsertAt( T* Data, int& Count, int Capacity, int index, const T* data, int count )
	{
		NeAssert( index <= Count );
		NeAssert( (Count + count) <= Capacity );
		if (index == Count)	
			return Array_Append( Data, Count, Capacity, data, count );
		if (data && count)
		{
			Count += count;
			const int end = index + count;
			Mem_Mov( Data + end, Data + index, (Count - end) * sizeof(T) );
			BitwiseCopier::Copy( Data + index, data, count );
		}
	}

	template < typename T >
	inline void Array_InsertAt( T* Data, int& Count, int Capacity, int index, const T& v )
	{ 
		return Array_InsertAt( Data, Count, Capacity, index, &v, 1 );
	}

}

//======================================================================================
namespace nemesis
{
	template < typename C, typename T, typename K >
	inline int Array_BinaryFind( const T* item, int count, const K& key )
	{
		int cmp;
		int pivot;
		int start = 0;
		int end = count-1;
		NeAssert(item);
		for ( ; start <= end; )
		{
			pivot = (start+end)/2;
			NeAssertBounds(pivot, count);
			cmp = C::Compare( item[pivot], key );
			if ( 0 == cmp )
				return pivot;
			if ( cmp > 0 )
				end = pivot-1;
			else
				start = pivot+1;
		}
		return ~start;
	}

	template < typename T, typename K >
	inline int Array_BinaryFind( const T* item, int count, const K& key )
	{ 
		return Array_BinaryFind< IntrinsicComparer, T, K >( item, count, key );
	}

	template < typename C, typename A, typename I, typename T >
	inline int32_t Array_BinaryFind( const A& a, const I& i, const T& v )
	{
		int cmp;
		int idx;
		int pivot;
		int start = 0;
		int end = i.Count-1;
		for ( ; start <= end; )
		{
			pivot = (start+end)/2;
			idx = i[pivot];
			cmp = C::Compare( a[idx] , v );
			if ( 0 == cmp )
				return pivot;
			if ( cmp > 0 )
				end = pivot-1;
			else
				start = pivot+1;
		}
		return ~start;
	}

	template < typename A, typename I, typename T >
	inline int32_t Array_BinaryFind( const A& a, const I& i, const T& v )
	{
		return Array_BinaryFind<IntrinsicComparer>( a, i, v );
	}
}

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline void Array_Swap( Array<T>& src, Array<T>& dst )
	{
		Allocator_t alloc = src.Alloc;
		T* data = src.Data;
		int count = src.Count;
		int capacity = src.Capacity;

		src.Alloc = dst.Alloc;
		src.Data = dst.Data;
		src.Count = dst.Count;
		src.Capacity = dst.Capacity;

		dst.Alloc = alloc;
		dst.Data = data;
		dst.Count = count;
		dst.Capacity = capacity;
	}
}
