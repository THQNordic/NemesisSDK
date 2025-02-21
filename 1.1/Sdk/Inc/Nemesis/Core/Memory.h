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

//======================================================================================
namespace nemesis
{
	void* NE_API Mem_Zero( void* dst, size_t size );
	void* NE_API Mem_Set ( void* dst, int val, size_t size );
	void* NE_API Mem_Cpy ( void* dst, const void* src, size_t size );
	void* NE_API Mem_Cpy ( void* dst, size_t dst_size, const void* src, size_t src_size );
	void* NE_API Mem_Mov ( void* dst, const void* src, size_t size );
	int	  NE_API Mem_Cmp ( const void* lhs, const void* rhs, size_t size );

	template < typename T >	inline T* Arr_Zero( T* dst				, int count ) { return (T*)Mem_Zero( dst	 , count * sizeof(T) ); }
	template < typename T >	inline T* Arr_Cpy ( T* dst, const T* src, int count ) { return (T*)Mem_Cpy ( dst, src, count * sizeof(T) ); }
	template < typename T >	inline T* Arr_Mov ( T* dst, const T* src, int count ) { return (T*)Mem_Mov ( dst, src, count * sizeof(T) ); }
}

//======================================================================================
namespace nemesis
{
	template < size_t S, typename T > inline void* Mem_Cpy( T (&dst)[S], const void* src, size_t src_size ) { return Mem_Cpy( dst, S * sizeof(T), src, src_size ); }
}

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline void NeZero( T& v )
	{ Mem_Zero( &v, sizeof(v) ); }
}

//======================================================================================
namespace nemesis
{
	inline uint8_t EndianSwap( uint8_t v )
	{ return v; }

	inline uint16_t EndianSwap( uint16_t v )
	{ 
		return	(v & 0x00ff) << 8
			|	(v & 0xff00) >> 8
			; 
	}

	inline uint32_t EndianSwap( uint32_t v )
	{
		return	(v & 0x000000ff) << 24
			 |	(v & 0x0000ff00) << 8
			 |	(v & 0x00ff0000) >> 8
			 |	(v & 0xff000000) >> 24
			 ;
	}

	inline uint64_t EndianSwap( uint64_t v )
	{
		return	(v & 0x00000000000000ffll) << 56
			 |	(v & 0x000000000000ff00ll) << 40
			 |	(v & 0x0000000000ff0000ll) << 24
			 |	(v & 0x00000000ff000000ll) << 8
			 |	(v & 0x000000ff00000000ll) >> 8
			 |	(v & 0x0000ff0000000000ll) >> 24
			 |	(v & 0x00ff000000000000ll) >> 40
			 |	(v & 0xff00000000000000ll) >> 56;
	}

	inline int64_t EndianSwap( int64_t v )
	{
		return (int64_t)EndianSwap((uint64_t)v);
	}

	inline float EndianSwap( float v )
	{
		const uint32_t s = EndianSwap( *reinterpret_cast<const uint32_t*>(&v) );
		return *reinterpret_cast<const float*>(&s);
	}
}

//======================================================================================
namespace nemesis
{
	/// Comparer for raw memory.
	struct BitwiseComparer
	{
		template < typename T >
		static bool Equals( const T& lhs, const T& rhs )
		{ return 0 == Compare( lhs, rhs ); }

		template < typename T >
		static int Compare( const T& lhs, const T& rhs )
		{ return Mem_Cmp( &lhs, &rhs, sizeof(T) ); }
	};

	/// Copier for raw memory.
	struct BitwiseCopier
	{
		template < typename T >
		static void Copy( T* dst, const T* src, int count )
		{ Mem_Cpy( dst, src, count * sizeof(T) ); }
	};

	/// Initializer for pod types.
	struct ZeroInitializer
	{
		template < typename T >
		static void Initialize( T* p, int count )
		{ Mem_Set( p, 0, count * sizeof(T) ); }

		template < typename T >
		static void Deinitialize( T* p, int count )
		{ Mem_Set( p, 0, count * sizeof(T) ); }
	};

	/// Copier for intrinsic types.
	struct IntrinsicCopier
	{
		template < typename T >
		static void Copy( T* dst, const T* src, int count )
		{ 
			for ( int i = 0; i < count; ++i )
				dst[i] = src[i];
		}
	};

	/// Hash value generator for pointers.
	struct PointerHasher
	{
		static size_t Hash( const void* p )
		{ return (size_t)p; }
	};
}
