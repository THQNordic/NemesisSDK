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
#define NE_PI			3.14159265358979323846f
#define NE_DEG_TO_RAD	0.01745329238f
#define NE_RAD_TO_DEG	57.2957795130f

//======================================================================================
namespace nemesis
{
	typedef float  float4  [4];
	typedef float4 float4x4[4];

	struct Vec2_s
	{
		float x, y;
	};

	struct Vec3_s
	{
		float x, y, z;
	};

	struct Vec4_s
	{
		float x, y, z, w;
	};

	struct Pos3_s
	{
		float x, y, z, _;
	};

	struct Quat_s
	{
		float x, y, z, w;
	};

	struct Mat4_s
	{
		Vec4_s r[4];
	};

	struct Rect_s
	{
		float x,y, w,h;
	};

	struct Pad_s
	{
		float l,t, r,b;
	};

	struct Rect2_s
	{
		Rect_s Rect[2];
	};

	struct Color_s
	{
		float r, g, b, a;
	};

	struct XForm_s
	{
		Pos3_s Pos;
		Quat_s Rot;
	};
}

//======================================================================================
namespace nemesis
{
	struct IntPoint_s
	{
		int x,y;
	};

	struct IntRect_s
	{
		int x,y;
		int w,h;
	};
}

//======================================================================================
namespace nemesis
{
	struct Color_c : public Color_s
	{
	public:
		Color_c()												 { r = b = g = 0.0f; a = 1.0f; }
		Color_c( const Color_s& rhs )							 { r = rhs.r; g = rhs.g; b = rhs.b; a = rhs.a; }
		Color_c( float _r, float _g, float _b, float _a = 1.0f ) { r = _r; g = _g; b = _b; a = _a; }
		Color_c( uint32_t rgb, float _a );
		Color_c( uint32_t argb );
	};
}

//======================================================================================
namespace nemesis
{
	struct FractInt_s
	{
		int32_t	Whole;
		float	Fract;
	};

}
