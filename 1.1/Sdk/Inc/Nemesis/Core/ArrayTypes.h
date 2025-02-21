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
#include "AllocTypes.h"

//======================================================================================
namespace nemesis
{
	template < typename T >
	class Array
	{
	public:
		explicit Array( Allocator_t alloc );
		Array( const Array<T>& rhs );
		Array();
		~Array();

	public:
		void Init( Allocator_t alloc );

	public:
		void Clear();
		void Reset();
		void GrowBy( int count );
		void Resize( int count );
		void Reserve( int capacity );

	public:
		T& Append();
		void Append( const T& value );
		void Append( const T* data, int count );
		void Append( const Array<T>& data );
		void InsertAt( int index, const T& value );
		void InsertAt( int index, const T* data, int count );
		void InsertAt( int index, const Array<T>& data );
		void RemoveAt( int index );
		void RemoveAt( int index, int count );

	public:
		void Swap( Array<T>& rhs );

	public:
			  T& operator [] ( int index );
		const T& operator [] ( int index ) const;

		Array<T>& operator = ( const Array<T>& rhs );

	public:
		T* Data;
		int Count;
		int Capacity;
		Allocator_t Alloc;
	};

	template < typename T, int N >
	class BoundedArray
	{
	public:
		void Reset();

	public:
		int Remain() const; 

		T& Append();
		void Append( const T& item );
		bool TryAppend( const T& item );

	public:
			  T& operator [] ( int index );
		const T& operator [] ( int index ) const;

	public:
		T Data[ N ];
		int Count;
	};
}
