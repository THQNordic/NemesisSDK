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
#include "Target.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

//======================================================================================
namespace nemesis
{
	typedef unsigned int	uint_t;

	typedef void*			ptr_t;
	typedef const void*		cptr_t;

	typedef char*			str_t;
	typedef const char*		cstr_t;

	typedef wchar_t*		wstr_t;
	typedef const wchar_t*	wcstr_t;

	typedef void*			Handle_t;
	typedef int				Result_t;
}

//======================================================================================

template < typename T >
inline T NeMin( const T& a, const T& b )
{ return a < b ? a : b; }

template < typename T >
inline T NeMax( const T& a, const T& b )
{ return a > b ? a : b; }

template < typename T >
inline T NeClamp( const T& value, const T& minimum, const T& maximum )
{ return NeMin( NeMax( value, minimum ), maximum ); }

template < typename T >
inline void NeSwap( T& a, T& b )
{ 
	const T temp = b;
	b = a;
	a = temp;
}

template < typename T >
inline T* NeSkip( T* ptr, ptrdiff_t bytes )
{ return (T*) (((char*)ptr) + bytes); }

//======================================================================================
namespace nemesis
{
	/// Address

	inline bool Size_IsPow2( size_t size )
	{
		return ((size != 0) && (size & (size-1)) == 0);
	}

	inline size_t Size_Align( size_t size, size_t align )
	{
		return ((size + (align - 1)) / align) * align;
	}

	inline size_t Size_Align_Pow2( size_t size, size_t align )
	{
		return (size + (align - 1)) & (~(align - 1));
	}

	inline bool Size_IsAligned_Pow2( size_t size, size_t align )
	{
		return (size & (align-1)) == 0;
	}

	inline ptrdiff_t PtrDiff_Align_Pow2( ptrdiff_t pos, size_t align )
	{
		return (pos + (align-1)) & ~(align-1);
	}

	inline uint32_t Mem_Align_Pow2( uint32_t pos, uint32_t align  )
	{
		return (pos + (align-1)) & ~(align-1);
	}

	inline uint32_t Mem_Align_16( uint32_t pos )
	{
		return (pos + (16-1)) & ~(16-1);
	}

	// Pointer

	inline ptrdiff_t Ptr_Tell( cptr_t src, cptr_t dst )
	{
		return ((uint8_t*)dst) - ((uint8_t*)src);
	}

	inline ptr_t Ptr_Seek( ptr_t ptr, ptrdiff_t offset )
	{
		return ((uint8_t*)ptr) + offset;
	}

	inline cptr_t Ptr_Seek( cptr_t ptr, ptrdiff_t offset )
	{
		return ((const uint8_t*)ptr) + offset;
	}

	inline ptr_t Ptr_Align_Pow2( ptr_t pos, size_t align )
	{
		return (ptr_t)PtrDiff_Align_Pow2( (ptrdiff_t)pos, align );
	}

	inline cptr_t Ptr_Align_Pow2( cptr_t pos, size_t align )
	{
		return (ptr_t)PtrDiff_Align_Pow2( (ptrdiff_t)pos, align );
	}

	inline bool Ptr_IsAligned_Pow2( cptr_t pos, size_t align )
	{
		return Size_IsAligned_Pow2( (size_t)pos, align );
	}

}

//======================================================================================
namespace nemesis
{
	/// Comparer for intrinsic types

	struct IntrinsicComparer
	{
		template < typename T >
		static bool Equals( const T& lhs, const T& rhs )
		{ return lhs == rhs; }

		template < typename T >
		static int Compare( const T& lhs, const T& rhs )
		{
			if (lhs == rhs)
				return 0;
			if (lhs > rhs)
				return 1;
			return -1;
		}
	};
}

//======================================================================================
namespace nemesis
{ 
	/// Union for packing/extracting low and high fields

	template < typename U, typename H, typename L = H >
	struct LowHigh
	{
		union
		{
			struct
			{
			#if (NE_ENDIAN == NE_ENDIAN_INTEL)
				L Low;
				H High;
			#else
				H High;
				L Low;
			#endif
			};
			U Value;
		};
	};

	typedef LowHigh<int16_t, int8_t,  uint8_t>	S16_LH;
	typedef LowHigh<int32_t, int16_t, uint16_t>	S32_LH;
	typedef LowHigh<int64_t, int32_t, uint32_t>	S64_LH;
	typedef LowHigh<uint16_t, uint8_t >			U16_LH;
	typedef LowHigh<uint32_t, uint16_t>			U32_LH;
	typedef LowHigh<uint64_t, uint32_t>			U64_LH;

	/// Information on a line of source code
	struct Location
	{
		Location() {};
		Location( const char* function, const char* file, unsigned long line )
			: Function( function )
			, File( file )
			, Line( line )
		{};

		operator Location* ()			  { return this; }
		operator const Location* () const { return this; }

		const char* Function;
		const char* File;
		unsigned long Line;
	};

	/// Information on a line of source code with additional user-provided name
	struct NamedLocation
	{
		NamedLocation() {};
		NamedLocation( const char* name, const char* function, const char* file, unsigned long line )
			: Name( name )
			, Location( function, file, line )
		{};

		operator NamedLocation* ()			   { return this; }
		operator const NamedLocation* () const { return this; }

		const char* Name;
		Location Location;
	};
}

//======================================================================================
#define NeUnused(x)			((void)(x))
#define NeCountOf(a)		(sizeof(a)/sizeof(a[0]))
#define NeUnique(prefix)	NeConcat(prefix, __LINE__)

#define NeHasFlag(style, flag)		(((style) & (flag)) == (flag))
#define NeSetFlag(style, flag, on)	((style) = (on) ? ((style)|(flag)) : ((style)&(~(flag))))

#define NeMakeFourCc( a, b, c, d )  ( ((d) << 24) | ((c) << 16) | ((b) << 8) | (a) )

#define NE_NO_COPY(...)\
	private:\
		__VA_ARGS__( const __VA_ARGS__& );\
		__VA_ARGS__& operator = ( const __VA_ARGS__& )

#define NE_DECL_INTERFACE(x)\
	protected:\
		virtual ~x() {}\
	public:

//======================================================================================
#if (NE_COMPILER == NE_COMPILER_MSVC)
#	define NeWeak __declspec(selectany) 
#else
#	define NeWeak __attribute__((weak))
#endif

//======================================================================================
#define NE_OK					0
#define NE_ERROR				0x8000000
#define NE_ERR_CORE				0x0010000
#define NE_ERR_INVALID_ARG		(NE_ERROR | NE_ERR_CORE | 0x1)
#define NE_ERR_INVALID_CALL		(NE_ERROR | NE_ERR_CORE | 0x2)
#define NE_ERR_OUT_OF_MEMORY	(NE_ERROR | NE_ERR_CORE | 0x3)
#define NE_ERR_NOT_SUPPORTED	(NE_ERROR | NE_ERR_CORE | 0x4)
#define NE_ERR_NOT_FOUND		(NE_ERROR | NE_ERR_CORE | 0x5)

#define NeFailed( hr )		((hr & NE_ERROR) == NE_ERROR)
#define NeSucceeded( hr )	((hr & NE_ERROR) == 0)

//======================================================================================
namespace ne = nemesis;
