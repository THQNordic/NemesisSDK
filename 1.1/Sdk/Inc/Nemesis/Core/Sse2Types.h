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
#include "MathTypes.h"

//======================================================================================
#if NE_PLATFORM_IS_WINDOWS
#	include <intrin.h>
#	define NE_VEC	__vectorcall
#else
#	include <x86intrin.h>
#	define NE_VEC	
#endif

//======================================================================================
//																				   Types
//======================================================================================
namespace nemesis
{
#if (NE_COMPILER == NE_COMPILER_MSVC)
	using vec4f_t = __m128;
	using vec4i_t = __m128i;
#else
	// @note: 
	//	the C++ ISO does not allow operators for built-in types
	//	we either have to remove the operators or 
	//	wrap vector types in structs

	// @todo:
	//	check generated assembly and see it there's overhead
	//	if so, remove the operators instead
	struct vec4f_t
	{
		__m128 m128;

		vec4f_t() = default;
		vec4f_t(__m128 v) { m128 = v; }
		inline operator __m128() const { return m128; }
	};

	struct vec4i_t
	{
		__m128i m128i;

		vec4i_t() = default;
		vec4i_t(__m128i v) { m128i = v; }
		inline operator __m128i() const { return m128i; }
	};
#endif

	struct vec4i_u32_s
	{
		union
		{
			uint32_t u[4];
			vec4i_t	 v;
		};
	};

	struct vec4f_u32_s
	{
		union
		{
			uint32_t u[4];
			vec4f_t	 v;
		};
	};

	struct vec4f_f32_s
	{
		union
		{
			float  f[4];
			vec4f_t v;
		};
	};

	struct mat4_t
	{
		vec4f_t r[4];
	};

	struct soa4_t
	{
		vec4f_t x;
		vec4f_t y;
		vec4f_t z;
		vec4f_t w;
	};

	NeStaticAssert(NeAlignOf(vec4f_t) == 16);
	NeStaticAssert(NeAlignOf(vec4i_t) == 16);
	NeStaticAssert(NeAlignOf(mat4_t) == 16);
}
