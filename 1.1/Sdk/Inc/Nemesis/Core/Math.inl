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
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Math.h>
#include <math.h>
#include <float.h>

//======================================================================================
//																				   Float
//======================================================================================
namespace nemesis
{
	inline float Flt_Asin( float v )
	{
		return asinf( NeClamp( v, -1.0f, 1.0f ) );
	}

	inline float Flt_Acos( float v )
	{
		return acosf( NeClamp( v, -1.0f, 1.0f ) );
	}

	inline float Flt_AcosFast( float x )
	{
		return sqrtf( 2.0f * NeMax( 1.0f - x, 0.0f ) );
	}

	inline float Flt_Square( float x )
	{
		return x * x;
	}

	inline float Flt_Fract( float v )
	{
	#if 0
		return v - (int)v;
	#else
		return fmodf(v, 1.0f);
	#endif
	}

	inline float Flt_Step( float a, float x )
	{
		return (x > a) ? 1.0f : 0.0f;
	}

	inline float Flt_Nice( float v ) 
	{ 
		return (fabsf(v) < FLT_SMALL) ? 0.0f : v; 
	}

	inline float Flt_Dist( float a, float b )
	{
		return b - a;
	}

	inline float Flt_Lerp_Precise( float s, float d, float t )
	{
		return (1.0f-t) * s + t * d;
	}

	inline float Flt_Lerp_Fast( float s, float d, float t )
	{
		// https://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
		// for floats, in general
		//	s + (d-s) != d
		return s + t * (d-s);	
	}

	inline float Flt_Lerp( float s, float d, float t )
	{
		return Flt_Lerp_Precise( s, d, t );
	}

	inline float Flt_Lerp( const float* v, int count, int s, float t )
	{
		NeAssert( v && (count > 1) && (s < (count-1)) );
		return t ? Flt_Lerp( v[s], v[s+1], t ) : v[s];
	}

	inline float Flt_Smooth( float s, float d, float t )
	{
		const float x = NeClamp( (t-s) / (d-s), 0.0f, 1.0f );
		return x * x * (3.0f - 2.0f * x);
	}

	inline float Flt_Rerange( float x, float in_min, float in_max, float out_min, float out_max )
	{
		return (x-in_min) / (in_max-in_min) * (out_max-out_min) + out_min;
	}

	inline float Flt_Mad( float v, float scale, float mean )
	{
		return v * scale + mean;
	}

	inline float Flt_Smu( float v, float mean, float scale )
	{
		return (v - mean) * scale;
	}

	inline void Flt_MinMax( const float* item, int count, float& min, float& max )
	{
		if (!item || !count)
		{
			min = max = 0.0f;
			return;
		}
		min = max = item[0];
		if (count < 2)
			return;
		for ( int i = 1; i < count; ++i )
		{
			min = NeMin( min, item[i] );
			max = NeMax( max, item[i] );
		}
	}

	inline float Flt_Sqrt( float v, float def )	  
	{ 
		return (v > 0.0f) ? sqrtf(v) : def;
	}

	inline float Flt_Div( float a, float b, float def )	  
	{ 
		return Flt_Select( fabsf(b) - FLT_TINY, a / b, def ); 
	}

	inline float Flt_Mod( float a, float b, float def )	  
	{ 
		return Flt_Select( fabsf(b) - FLT_TINY, fmodf(a, b), def ); 
	}

	inline float Flt_Select( float cmp, float ge, float lt ) 
	{ 
		return (cmp >= 0.0f) ? ge : lt; 
	}

	inline bool Flt_IsTiny( float v )
	{
		return fabsf(v) < FLT_TINY;
	}

	inline bool Flt_Eq( float lhs, float rhs, float tolerance )
	{
		return fabsf(lhs-rhs) <= tolerance;
	}

	inline bool Flt_Eq( float lhs, float rhs )
	{
		return Flt_Eq( lhs, rhs, FLT_SMALL );
	}

	inline float Flt_Wrap( float v, float min, float max )
	{
		if (min == max)
			return min;
		const float diff = max-min;
		const float u1 = (v - min) / diff;
		const float u2 = u1 - floorf(u1);
		return diff * u2 + min;
	}

	inline Vec2_s Flt_ThaSinCos( float tha )
	{
	#if 1
		// https://en.wikipedia.org/wiki/Tangent_half-angle_formula
		//
		// sin(theta) = (2*tha)  /(1+tha^2)
		// cos(theta) = (1-tha^2)/(1+tha^2)
		//
		const float tha_sq		= tha * tha;
		const float scale		= 1.0f/(1.0f + tha_sq);
		const float sin_angle	= scale * (2.0f * tha);
		const float cos_angle	= scale * (1.0f - tha_sq);
		return Vec2_s { sin_angle, cos_angle };
	#else
		const float angle = 2.0f * atanf( tha );
		const float sin_angle = sinf( angle );
		const float cos_angle = cosf( angle );
		return Vec2_s { sin_angle, cos_angle };
	#endif
	}

	inline float Flt_NormRad( float v )
	{
		while ( v > NE_PI )
			v -= 2.0f * NE_PI;
		while ( v < -NE_PI )
			v += 2.0f * NE_PI;
		return v;
	}

	inline float Flt_Pow2( float v )
	{
		return v * v;
	}

	inline float Flt_Pow3( float v )
	{
		return v * v * v;
	}

	inline float Flt_Pow5( float v )
	{
		return v * v * v * v * v;
	}

	inline void Flt_SinCos( float v, float& s, float& c )
	{
		s = sinf(v);
		c = cosf(v);
	}
}

//======================================================================================
//																				    Vec2
//======================================================================================
namespace nemesis
{
	inline Vec2_s Vec2_Zero()
	{
		return Vec2_s { 0.0f, 0.0f };
	}

	inline Vec2_s Vec2_Unit()
	{
		return Vec2_s { 1.0f, 1.0f };
	}

	inline Vec2_s Vec2_UnitX()
	{
		return Vec2_s { 1.0f, 0.0f };
	}

	inline Vec2_s Vec2_UnitY()
	{
		return Vec2_s { 0.0f, 1.0f };
	}

	inline float Vec2_Dot( const Vec2_s& a, const Vec2_s& b )
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float Vec2_LenSq( const Vec2_s& a )
	{
		return Vec2_Dot( a, a );
	}

	inline float Vec2_Len( const Vec2_s& a )
	{
		return sqrtf( Vec2_LenSq( a ) );
	}

	inline Vec2_s Vec2_Norm( const Vec2_s& a )
	{
		const float len = Vec2_Len( a );
		return len ? Vec2_Mul( a, 1.0f / len ) : Vec2_s {};
	}

	inline Vec2_s Vec2_Neg( const Vec2_s& a )
	{
		return Vec2_s { -a.x, -a.y };
	}

	inline Vec2_s Vec2_Floor( const Vec2_s& a )
	{
		return Vec2_s { floorf( a.x ), floorf( a.y ) };
	}

	inline float Vec2_Cross( const Vec2_s& a, const Vec2_s& b )
	{
		return a.x * b.y - a.y * b.x;
	}

	inline Vec2_s Vec2_Min( const Vec2_s& a, const Vec2_s& b )
	{
		return Vec2_s
			{ NeMin( a.x, b.x )
			, NeMin( a.y, b.y )
			};
	}

	inline Vec2_s Vec2_Max( const Vec2_s& a, const Vec2_s& b )
	{
		return Vec2_s
			{ NeMax( a.x, b.x )
			, NeMax( a.y, b.y )
		};
	}

	inline Vec2_s Vec2_Add( const Vec2_s& a, const Vec2_s& b )
	{
		return Vec2_s { a.x + b.x, a.y + b.y };
	}

	inline Vec2_s Vec2_Sub( const Vec2_s& a, const Vec2_s& b )
	{
		return Vec2_s { a.x - b.x, a.y - b.y };
	}

	inline Vec2_s Vec2_Mul( const Vec2_s& a, float s )
	{
		return Vec2_s { a.x * s, a.y * s };
	}

	inline Vec2_s Vec2_Dir( const Vec2_s& from, const Vec2_s& to )
	{
		const Vec2_s diff = Vec2_Sub( to, from );
		const float inv_len = Vec2_InvLen( diff, 1.0f );
		return Vec2_Mul( diff, inv_len );
	}

	inline float Vec2_InvLen( const Vec2_s& a, float fail )
	{
		const float d = a.x * a.x + a.y * a.y; 
		return (d > 0.0f) ? 1.0f / sqrtf(d) : fail; 
	}
}

//======================================================================================
//																				    Vec3
//======================================================================================
namespace nemesis
{
	inline float Vec3_Dot( const Vec3_s& a, const Vec3_s& b )
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float Vec3_LenSq( const Vec3_s& a )
	{
		return Vec3_Dot( a, a );
	}

	inline float Vec3_Len( const Vec3_s& a )
	{
		return sqrtf( Vec3_LenSq( a ) );
	}

	inline Vec3_s Vec3_Neg( const Vec3_s& a )
	{
		return Vec3_s { -a.x, -a.y, -a.z };
	}

	inline Vec3_s Vec3_Add( const Vec3_s& a, const Vec3_s& b )
	{
		return Vec3_s { a.x + b.x, a.y + b.y, a.z + b.z };
	}

	inline Vec3_s Vec3_Sub( const Vec3_s& a, const Vec3_s& b )
	{
		return Vec3_s { a.x - b.x, a.y - b.y, a.z - b.z };
	}

	inline Vec3_s Vec3_Mul( const Vec3_s& a, const Vec3_s& b )
	{
		return Vec3_s { a.x * b.x, a.y * b.y, a.z * b.z };
	}

	inline Vec3_s Vec3_Mul( const Vec3_s& a, float s )
	{
		return Vec3_s { a.x * s, a.y * s, a.z * s };
	}
}

//======================================================================================
//																				    Pos3
//======================================================================================
namespace nemesis
{
	inline Pos3_s Pos3_Zero()
	{
		return Pos3_s { 0.0f, 0.0f, 0.0f, 0.0f };
	}

	inline Pos3_s Pos3_Unit()
	{
		return Pos3_s { 1.0f, 1.0f, 1.0f, 0.0f };
	}

	inline Pos3_s Pos3_UnitX()
	{
		return Pos3_s { 1.0f, 0.0f, 0.0f, 0.0f };
	}

	inline Pos3_s Pos3_UnitY()
	{
		return Pos3_s { 0.0f, 1.0f, 0.0f, 0.0f };
	}

	inline Pos3_s Pos3_UnitZ()
	{
		return Pos3_s { 0.0f, 0.0f, 1.0f, 0.0f };
	}

	inline float Pos3_Dot( const Pos3_s& a, const Pos3_s& b )
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float Pos3_LenSq( const Pos3_s& a )
	{
		return Pos3_Dot( a, a );
	}

	inline float Pos3_Len( const Pos3_s& a )
	{
		const float len_sq = Pos3_LenSq( a );
		return len_sq ? sqrtf( len_sq ) : 0.0f;
	}

	inline Pos3_s Pos3_Norm( const Pos3_s& a )
	{
		const float len = Pos3_Len( a );
		return len ? Pos3_Mul( a, 1.0f / len ) : Pos3_Zero();
	}

	inline Pos3_s Pos3_Neg( const Pos3_s& a )
	{
		return Pos3_s { -a.x, -a.y, -a.z, 0.0f };
	}

	inline Pos3_s Pos3_Add( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_s { a.x + b.x, a.y + b.y, a.z + b.z, 0.0f };
	}

	inline Pos3_s Pos3_Sub( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_s { a.x - b.x, a.y - b.y, a.z - b.z, 0.0f };
	}

	inline Pos3_s Pos3_Mul( const Pos3_s& a, float s )
	{
		return Pos3_s { a.x * s, a.y * s, a.z * s, 0.0f };
	}

	inline Pos3_s Pos3_Mul( const Pos3_s& v, const Quat_s& q )
	{
		// https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
		const Pos3_s r = { q.x, q.y, q.z, 0.0f };
		const Pos3_s t = 2.0f * Pos3_Cross( r, v );
		return v + q.w * t + Pos3_Cross( r, t );
	}

	inline Pos3_s Pos3_Div( const Pos3_s& v, float s, const Pos3_s& def )
	{
		return (fabsf(s) > FLT_TINY)
				? (v * (1.0f/s))
				: def;
	}

	inline Pos3_s Pos3_Cross( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_s
			{ a.y * b.z - a.z * b.y 
			, a.z * b.x - a.x * b.z
			, a.x * b.y - a.y * b.x
			, 0.0f
			};
	}

	inline Pos3_s Pos3_Ortho( const Pos3_s& v )
	{
		const Pos3_s n = Pos3_Norm( v );
		const Pos3_s r = ((n.z < 0.5f) && (n.z > -0.5f))
				? Pos3_s { -n.y, n.x, 0.0f, 0.0f } 
				: Pos3_s { -n.z, 0.0f, n.x, 0.0f }
				;
		return Pos3_Norm( r );
	}

	inline Pos3_s Pos3_Lerp( const Pos3_s& s, const Pos3_s& d, float t )
	{
		return Pos3_s
			{ Flt_Lerp( s.x, d.x, t )
			, Flt_Lerp( s.y, d.y, t )
			, Flt_Lerp( s.z, d.z, t )
			, 0.0f
			};
	}

	inline void Pos3_Add( const Pos3_s* a, const Pos3_s* b, int count, Pos3_s* out )
	{
		NeAssert( a && b && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Pos3_Add( a[i], b[i] );
	}

	inline Pos3_s Pos3_Mad( const Pos3_s& v, const Pos3_s& scale, const Pos3_s& mean )
	{
		return Pos3_s
			{ v.x * scale.x + mean.x
			, v.y * scale.y + mean.y
			, v.z * scale.z + mean.z
			, 0.0f
			};
	}

	inline Pos3_s Pos3_Smu( const Pos3_s& v, const Pos3_s& mean, const Pos3_s& scale )
	{
		return Pos3_s
			{ (v.x - mean.x) * scale.x
			, (v.y - mean.y) * scale.y
			, (v.z - mean.z) * scale.z
			, 0.0f
			};
	}

	inline Pos3_s Pos3_Lerp( const Pos3_s* v, int count, int s, float t )
	{
		NeAssert( v && (count > 1) && (s < (count-1)) );
		return t ? Pos3_Lerp( v[s], v[s+1], t ) : v[s];
	}

	inline void Pos3_Lerp( const Pos3_s* a, const Pos3_s* b, int count, float t, Pos3_s* out )
	{
		NeAssert( a && b && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Pos3_Lerp( a[i], b[i], t );
	}

	inline void Pos3_Lerp( const Pos3_s* a, const Pos3_s* b, const float* t, int count, Pos3_s* out )
	{
		NeAssert( a && b && t && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Pos3_Lerp( a[i], b[i], t[i] );
	}

	inline Pos3_s Pos3_MinEach( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_s
			{ NeMin( a.x, b.x )
			, NeMin( a.y, b.y )
			, NeMin( a.z, b.z )
			, 0.0f
			};
	}

	inline Pos3_s Pos3_MaxEach( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_s
			{ NeMax( a.x, b.x )
			, NeMax( a.y, b.y )
			, NeMax( a.z, b.z )
			, 0.0f
			};
	}

	inline void Pos3_MinMaxEach( const Pos3_s* item, int count, Pos3_s& min, Pos3_s& max )
	{
		if (!item || !count)
		{
			min = max = Pos3_Zero();
			return;
		}
		min = max = item[0];
		if (count < 2)
			return;
		for ( int i = 1; i < count; ++i )
		{
			min = Pos3_MinEach( min, item[i] );
			max = Pos3_MaxEach( max, item[i] );
		}
	}

	inline float Pos3_Angle( const Pos3_s& a, const Pos3_s& b )
	{
		//		cos(theta)	= (a.b)/(|a||b|)
		//	=>	theta		= acos((a.b)/(|a||b|))
		const float dot_ab = Pos3_Dot( a, b );
		const float len_ab = Pos3_Len( a ) * Pos3_Len( b );
		const float cos_t = NeClamp( dot_ab / len_ab, 0.0f, 1.0f );
		const float theta = acosf( cos_t );
		return theta;
	}

	inline float Pos3_Dist( const Pos3_s& from, const Pos3_s& to )
	{
		return Pos3_Len( to - from );
	}

	inline Pos3_s Pos3_Project( const Pos3_s& pos, const Pos3_s& dir )
	{
		//		^
		//	D	|
		//	i	*	o Pos
		//	r 	|  /  
		//	 	| / 
		//	 	|/
		//		o
		NeAssert(Pos3_IsNorm( dir ));
		return Pos3_Dot( pos, dir ) * dir;
	}

	inline bool Pos3_IsNorm( const Pos3_s& v, float tolerance )
	{
		const float len = Pos3_Len( v );
		const float diff = 1.0f - len;
		return fabsf(diff) < tolerance;
	}

	inline bool Pos3_IsNorm( const Pos3_s& v )
	{
		return Pos3_IsNorm( v, FLT_TINY );
	}

	inline bool Pos3_IsZeroLen( const Pos3_s& v, float tolerance )
	{
		return Pos3_LenSq( v ) < (tolerance * tolerance);
	}

	inline bool Pos3_IsZeroLen( const Pos3_s& v )
	{
		return Pos3_IsZeroLen( v, FLT_SMALL );
	}

	inline bool Pos3_IsOrtho( const Pos3_s& a, const Pos3_s& b, float tolerance )
	{
		NeAssert(Pos3_IsNorm(a));
		NeAssert(Pos3_IsNorm(b));
		const float dot = Pos3_Dot(a, b);
		return fabsf(dot) < tolerance;
	}

	inline bool Pos3_IsOrtho( const Pos3_s& a, const Pos3_s& b )
	{
		return Pos3_IsOrtho( a,b, 1e-5f );
	}

	inline Pos3_s Pos3_TransformCoord( const Pos3_s& v, const Mat4_s& m )
	{
		const float4x4& m_ = (const float4x4&)m;
		const float x = v.x * m_[0][0] + v.y * m_[1][0] + v.z * m_[2][0] + m_[3][0];
		const float y = v.x * m_[0][1] + v.y * m_[1][1] + v.z * m_[2][1] + m_[3][1];
		const float z = v.x * m_[0][2] + v.y * m_[1][2] + v.z * m_[2][2] + m_[3][2];
		const float w = v.x * m_[0][3] + v.y * m_[1][3] + v.z * m_[2][3] + m_[3][3];
		return Pos3_s { x/w, y/w, z/w, 0.0f };
	}

	inline Pos3_s Pos3_TransformNormal( const Pos3_s& v, const Mat4_s& m )
	{
		const float4x4& m_ = (const float4x4&)m;
		const float x = v.x * m_[0][0] + v.y * m_[1][0] + v.z * m_[2][0];
		const float y = v.x * m_[0][1] + v.y * m_[1][1] + v.z * m_[2][1];
		const float z = v.x * m_[0][2] + v.y * m_[1][2] + v.z * m_[2][2];
		return Pos3_s { x, y, z, 0.0f };
	}

	inline Pos3_s Pos3_MirrorX( const Pos3_s& v )
	{
		return Pos3_s { -v.x,  v.y,  v.z };
	}

	inline Pos3_s Pos3_MirrorY( const Pos3_s& v )
	{
		return Pos3_s {  v.x, -v.y,  v.z };
	}

	inline Pos3_s Pos3_MirrorZ( const Pos3_s& v )
	{
		return Pos3_s {  v.x,  v.y, -v.z };
	}

	inline void Pos3_Basis( const Pos3_s& e0, Pos3_s* e1, Pos3_s* e2 )
	{
		NeAssert(e1 && e2);

		const float nx = fabsf(e0.x);
		const float ny = fabsf(e0.y);
		const float nz = fabsf(e0.z);

		Pos3_s n1;
		Pos3_s n2;

		if ( (nz > nx) && (nz > ny) )
			n1 = Pos3_s { 1.0f, 0.0f, 0.0f, 0.0f };
		else
			n1 = Pos3_s { 0.0f, 0.0f, 1.0f, 0.0f };

		n1 = Pos3_Norm( n1 - e0 * Pos3_Dot( n1, e0 ) );
		n2 = Pos3_Cross( n1, e0 );

		*e1 = n1;
		*e2 = n2;
	}
}

//======================================================================================
//																				    Quat
//======================================================================================
namespace nemesis
{
	inline Quat_s Quat_Id()
	{
		return Quat_s { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline bool	Quat_IsNorm( const Quat_s& q, float tol_sq )
	{
		return Quat_LenSq( q ) < (1.0f + tol_sq);
	}

	inline bool	Quat_IsNorm( const Quat_s& q )
	{
		return Quat_IsNorm( q, 1.e-5f );
	}

	inline float Quat_LenSq( const Quat_s& q )
	{
		return Quat_Dot( q, q );
	}

	inline float Quat_Len( const Quat_s& q )
	{
		return sqrtf( Quat_LenSq( q ) );
	}

	inline float Quat_Dot( const Quat_s& a, const Quat_s& b )
	{
		return a.x * b.x 
			 + a.y * b.y
			 + a.z * b.z
			 + a.w * b.w
			 ;
	}

	inline Pos3_s Quat_Vec( const Quat_s& q )
	{
		return { q.x, q.y, q.z };
	}

	inline Quat_s Quat_Norm( const Quat_s& q )
	{
		const float lsq = Quat_LenSq( q );
		return (lsq >= FLT_EPSILON)
			? q * (1.0f/sqrtf(lsq))
			: Quat_Id()
			;
	}

	inline Quat_s Quat_Inv( const Quat_s& q )
	{
		return Quat_s { -q.x, -q.y, -q.z, q.w };
	}

	inline Quat_s Quat_Neg( const Quat_s& q )
	{
		return Quat_s { -q.x, -q.y, -q.z, -q.w };
	}

	inline Quat_s Quat_Sel( const Quat_s& a, float sel )
	{
		return Quat_s 
			{ Flt_Select( sel, a.x, -a.x )
			, Flt_Select( sel, a.y, -a.y )
			, Flt_Select( sel, a.z, -a.z )
			, Flt_Select( sel, a.w, -a.w )
			};
	}

	inline Quat_s Quat_Add( const Quat_s& a, const Quat_s& b )
	{
		return Quat_s
			{ a.x + b.x 
			, a.y + b.y
			, a.z + b.z
			, a.w + b.w
		};
	}

	inline void Quat_Add( const Quat_s* a, const Quat_s* b, int count, Quat_s* out )
	{
		NeAssert( a && b && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Quat_Add( a[i], b[i] );
	}

	inline Quat_s Quat_Sub( const Quat_s& a, const Quat_s& b )
	{
		return Quat_s
			{ a.x - b.x 
			, a.y - b.y
			, a.z - b.z
			, a.w - b.w
		};
	}

	inline Quat_s Quat_Mul( const Quat_s& a, float b )
	{
		return Quat_s
			{ a.x * b
			, a.y * b
			, a.z * b
			, a.w * b
			};
	}

	inline Quat_s Quat_Mul( const Quat_s& a, const Quat_s& b )
	{
	#if 1
		return Quat_s
			{ a.w * b.x + a.x * b.w + b.y * a.z - b.z * a.y
			, a.w * b.y + a.y * b.w + b.z * a.x - b.x * a.z
			, a.w * b.z + a.z * b.w + b.x * a.y - b.y * a.x
			, a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
			};
	#else
		return Quat_s
			{ b.w * a.x + b.x * a.w + b.y * a.z - b.z * a.y
			, b.w * a.y - b.x * a.z + b.y * a.w + b.z * a.x
			, b.w * a.z + b.x * a.y - b.y * a.x + b.z * a.w
			, b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z
			};
	#endif
	}

	inline Pos3_s Quat_Mul( const Quat_s& q, const Pos3_s& v )
	{
	#if 1
		Pos3_s result;
		Pos3_s qv = { q.x, q.y, q.z };
		result = Pos3_Cross( qv, v );
		result = result * (q.w * 2.0f);
		result = result + (v * (2.0f * (q.w * q.w) - 1.0f));
		result = result + (qv * (Pos3_Dot( qv, v ) * 2.0f));
		return result;
	#else
		// https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
		// t = 2 * cross(q.xyz, v)
		// v' = v + q.w * t + cross(q.xyz, t)
		const Pos3_s t = 2.0f * Pos3_Cross( (const Pos3_s&)q, v );
		const Pos3_s r = v + q.w * t + Pos3_Cross( (const Pos3_s&)q, t );
		return r;
	#endif
	}

	inline Pos3_s Quat_InvMul( const Quat_s& q, const Pos3_s& v )
	{
		Pos3_s result;
		Pos3_s qv = { q.x, q.y, q.z };
		result = Pos3_Cross( qv, v );
		result = Pos3_Mul( result, -q.w * 2.0f );
		result = result + (v * (2.0f * (q.w * q.w) - 1.0f));
		result = result + (qv * (Pos3_Dot( qv, v ) * 2.0f));
		return result;
	}

	inline Quat_s Quat_Lerp( const Quat_s& s, const Quat_s& d, float t )
	{
		return Quat_s 
			{ Flt_Lerp( s.x, d.x, t )
			, Flt_Lerp( s.y, d.y, t )
			, Flt_Lerp( s.z, d.z, t )
			, Flt_Lerp( s.w, d.w, t )
			};
	}

	inline Quat_s Quat_SlerpPrecise( const Quat_s& s, const Quat_s& d, float t, float dot )
	{
		NeAssert((t >= 0) && (t <= 1.0f));
		const float sign = Flt_Select( dot, 1.0f, -1.0f );
		const Quat_s s0 = s;
		const Quat_s d0 = d * sign;
		const float dot0 = dot * sign;

		float s_weight, d_weight;
		if ((1.0f - fabsf(dot0)) > 0.05f)
		{
			const float theta		= acosf(dot);
			const float sin_theta	= sinf(theta);
			const float inv_sin		= 1.0f/sin_theta;
			s_weight = sinf((1.0f - t) * theta) * inv_sin;
			d_weight = sinf(		t  * theta) * inv_sin;
		}
		else
		{
			s_weight = 1.0f - t;
			d_weight =		  t;
		}

		const Quat_s q =
			{ s.x * s_weight + d.x * d_weight
			, s.y * s_weight + d.y * d_weight
			, s.z * s_weight + d.z * d_weight
			, s.w * s_weight + d.w * d_weight
			};

		return Quat_Norm( q );
	}

	inline Quat_s Quat_SlerpPrecise( const Quat_s& s, const Quat_s& d, float t )
	{
		const float dot = Quat_Dot( s, d );
		const float sign = (dot >= 0.0f) ? 1.0f : -1.0f;
		return Quat_SlerpPrecise( s, d * sign, t, dot * sign );
	}

	inline Quat_s Quat_SlerpFast( const Quat_s& s, const Quat_s& d, float t, float dot )
	{
		NeAssert( (t >= 0.0f) && (t <= 1.0f) );
		NeAssert( dot >= 0.0f );

		const float c1 =  1.570994357000f + ( 0.56429298590f + (-0.17836577170f + 0.043199493520f * dot) * dot) * dot;
		const float c3 = -0.646139638200f + ( 0.59456579360f + ( 0.08610323953f - 0.034651229280f * dot) * dot) * dot;
		const float c5 =  0.079498235210f + (-0.17304369310f + ( 0.10792795990f - 0.014393978010f * dot) * dot) * dot;
		const float c7 = -0.004354102836f + ( 0.01418962736f + (-0.01567189691f + 0.005848706227f * dot) * dot) * dot;

		const float T = 1 - t;
		const float t2 = t * t;
		const float T2 = T * T;

		const float recip = 1.0f / (1.0f + dot);
		const float s_weight = (c1 + (c3 + (c5 + c7 * T2) * T2) * T2) * T * recip;
		const float d_weight = (c1 + (c3 + (c5 + c7 * t2) * t2) * t2) * t * recip;

		const Quat_s q =
			{ s_weight * s.x + d_weight * d.x
			, s_weight * s.y + d_weight * d.y
			, s_weight * s.z + d_weight * d.z
			, s_weight * s.w + d_weight * d.w
			};

		return Quat_Norm( q );
	}

	inline Quat_s Quat_SlerpFast( const Quat_s& s, const Quat_s& d, float t )
	{
		const float dot = Quat_Dot( s, d );
		const float sign = (dot >= 0.0f) ? 1.0f : -1.0f;
		return Quat_SlerpFast( s, d * sign, t, dot * sign );
	}

	inline Quat_s Quat_Slerp( const Quat_s& s, const Quat_s& d, float t )
	{
		return Quat_SlerpFast( s, d, t );
	}

	inline Quat_s Quat_Slerp( const Quat_s* q, int count, int s, float t )
	{
		NeAssert( q && (count > 1) && (s < (count-1)) );
		return t ? Quat_Slerp( q[s], q[s+1], t ) : q[s];
	}

	inline void Quat_Slerp( const Quat_s* a, const Quat_s* b, int count, float t, Quat_s* out )
	{
		NeAssert( a && b && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Quat_Slerp( a[i], b[i], t );
	}

	inline void Quat_Slerp( const Quat_s* a, const Quat_s* b, const float* t, int count, Quat_s* out )
	{
		NeAssert( a && b && t && out );
		for ( int i = 0; i < count; ++i )
			out[i] = Quat_Slerp( a[i], b[i], t[i] );
	}

	inline Quat_s Quat_FromTha( const Pos3_s& v )
	{
		return Quat_Norm( Quat_s { v.x, v.y, v.z, 1.0f } );
	}

	inline Quat_s Quat_FromTqa( const Pos3_s& v )
	{
		const float mag2  = Pos3_LenSq( v );
		const float opm2  = 1.0f + mag2;
		const float scale = 2.0f / opm2;
		return Quat_s
				{ v.x * scale
				, v.y * scale
				, v.z * scale
				, (1.0f - mag2) / opm2
				};
	}

	inline Pos3_s Quat_ToTqa( const Quat_s& q )
	{
		const float sin_half_angle = sqrtf( q.x * q.x + q.y * q.y + q.z * q.z );
		if ((sin_half_angle < FLT_EPSILON) || (fabsf(q.w) >= 1.0f))
			return Pos3_Zero();

		const float theta
			= (q.w >= 0.0f)
			?  sqrtf((1.0f - q.w) / (1.0f + q.w))
			: -sqrtf((1.0f + q.w) / (1.0f - q.w));

		const float scale = theta / sin_half_angle;
		return Pos3_s
			{ q.x * scale
			, q.y * scale
			, q.z * scale
			, 0.0f
			};
	}

	inline void Quat_ToTqa( const Quat_s* src, int count, Pos3_s* dst )
	{
		for ( int i = 0; i < count; ++i )
			dst[i] = Quat_ToTqa( src[i] );
	}

	inline Quat_s Quat_FromVec( const Pos3_s& v )
	{
		const float theta = Pos3_Len( v );
		if (theta < FLT_EPSILON)
			return Quat_Id();
		const float half_angle = 0.5f * theta;
		const float scale = sinf( half_angle ) / theta;
		return Quat_s
			{ v.x * scale
			, v.y * scale
			, v.z * scale
			, cosf( half_angle )
			};
	}

	inline Pos3_s Quat_ToVec( const Quat_s& q )
	{
		const float sin_half_angle = sqrtf( q.x * q.x + q.y * q.y + q.z * q.z );
		if (sin_half_angle < FLT_EPSILON)
			return Pos3_Zero();

		const float a = (q.w > 0.0f) 
			?  2.0f * atan2f( sin_half_angle,  q.w )
			: -2.0f * atan2f( sin_half_angle, -q.w )
			;

		const float f = a / sin_half_angle;
		return Pos3_s
			{ f * q.x
			, f * q.y
			, f * q.z
			, 0.0f
			};
	}

	inline Quat_s Quat_FromTo( const Pos3_s& from, const Pos3_s& to )
	{
		const float tol = FLT_EPSILON;
		const float K = sqrtf( Pos3_LenSq( from ) * Pos3_LenSq( to ) );
		const float dot = NeClamp( Pos3_Dot( from, to ), -K, K );
		if (K < tol)
			return Quat_Id();

		if (fabsf(dot+K) < (tol*K))
		{
			const Pos3_s m = Pos3_Ortho( from );
			const Quat_s q1 = Quat_FromTo( m, to );
			const Quat_s q2 = Quat_FromTo( from, m );
			return Quat_Mul( q1, q2 );
		}

		const Pos3_s v = Pos3_Cross( from, to );
		const Quat_s q = { v.x, v.y, v.z, dot+K };
		return Quat_Norm( q );
	}

	inline Quat_s Quat_FromTo( const Quat_s& from, const Quat_s& to )
	{
		return to * Quat_Inv( from );
	}

	inline Quat_s Quat_FromAxisAngle( const Pos3_s& axis, float angle )
	{
		const Pos3_s norm = Pos3_Norm( axis );
		const float sin_theta = sinf( angle * 0.5f );
		const float cos_theta = cosf( angle * 0.5f );
		return Quat_s
			{ sin_theta * norm.x
			, sin_theta * norm.y
			, sin_theta * norm.z
			, cos_theta
			};
	}

	inline Quat_s Quat_FromEulerAngles( float x, float y, float z )
	{
		const float hx = x * 0.5f;
		const float hy = y * 0.5f;
		const float hz = z * 0.5f;
		const float chx = cosf(hx);
		const float chy = cosf(hy);
		const float chz = cosf(hz);
		const float shx = sinf(hx);
		const float shy = sinf(hy);
		const float shz = sinf(hz);
		return Quat_s
		{ shy * shz * chx + chy * chz * shx
		, shy * chz * chx + chy * shz * shx
		, chy * shz * chx - shy * chz * shx
		, chx * chy * chz - shx * shy * shz
		};
	}

	inline Quat_s Quat_FromEulerAngles_Deg( float x, float y, float z )
	{
		return Quat_FromEulerAngles
			( x * NE_DEG_TO_RAD
			, y * NE_DEG_TO_RAD
			, z * NE_DEG_TO_RAD
			);
	}

	inline Quat_s Quat_FromEulerAngles( const Pos3_s& euler )
	{
		return Quat_FromEulerAngles( euler.x, euler.y, euler.z );
	}

	inline Quat_s Quat_FromEulerAngles_Deg( const Pos3_s& euler )
	{
		return Quat_FromEulerAngles_Deg( euler.x, euler.y, euler.z );
	}

	inline Quat_s Quat_FromTwistSwing( float t, float s1, float s2 )
	{
		const float b = 2.0f / (1.0f + s1 * s1 + s2 * s2);
		const float c = 2.0f / (1.0f + t * t);
		const float bm1 = b - 1.0f;
		const float cm1 = c - 1.0f;
		const float ct = c * t;
		return Quat_s
			{ -t * bm1 * c
			, -b * (ct * s1 + cm1 * s2)
			, -b * (ct * s2 - cm1 * s1)
			, bm1 * cm1
			};
	}

	inline Quat_s Quat_FromTwistSwing( const Pos3_s& v )
	{
		return Quat_FromTwistSwing( v.x, v.y, v.z );
	}

	inline Pos3_s Quat_ToTwistSwing( const Quat_s& q )
	{
		const float chs2 = q.w * q.w + q.x * q.x;
		if (chs2 > 1.2e-7f)
		{
			float chs = sqrtf( chs2 );
			float mul = 1.0f / (chs * (chs + 1.0f));

			float denom = q.w + Flt_Select( -q.w, -chs, chs );
			return Pos3_s 
				{ -q.x / denom 
				, mul * ( q.x * q.y + q.w * q.z)
				, mul * (-q.w * q.y + q.x * q.z)
				};
		}
		const float rshs = 1.0f / sqrtf(1.0f - chs2);
		return Pos3_s { 0.0f, rshs * q.z, -rshs * q.y };
	}

	inline void Quat_ToSwingTwist( const Quat_s& q, const Pos3_s& axis, Quat_s& swing, Quat_s& twist )
	{
		const Pos3_s proj = Pos3_Project( Quat_Vec( q ), axis );
		const Quat_s quat = { proj.x, proj.y, proj.z, q.w };
		const float len_sq = Quat_LenSq( quat );
		twist = (len_sq > 0.0f) ? quat : Quat_Id();
		swing = ~twist * q;
	}

	inline Pos3_s Quat_ToEulerAngles( const Quat_s& q )
	{
		const float MARGIN = 0.999f;

		const float w2 = q.w * q.w;
		const float x2 = q.x * q.x;
		const float y2 = q.y * q.y;
		const float z2 = q.z * q.z;
		const float mag = w2 + x2 + y2 + z2;
		const float xy_wz = q.x * q.y + q.z * q.w;
		const float singular = 2.0f * (xy_wz) / (MARGIN * mag);

		if (singular > 1.0f)
			return Pos3_s { 0.0f, 2.0f * atan2f(q.x, q.w), NE_PI / 2.0f };

		if (singular < -1.0f)
			return Pos3_s { 0.0f, -2.0f * atan2f(q.x, q.w), -(NE_PI / 2.0f) };

		return Pos3_s 
			{ atan2f(2.0f * (q.x * q.w - q.y * q.z), w2 - x2 + y2 - z2)
			, atan2f(2.0f * (q.y * q.w - q.x * q.z), w2 + x2 - y2 - z2)
			, asinf(2.0f * xy_wz / mag)
			};

		/*
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		const float sinr_cosp =		   2.0f * (q.w * q.x + q.y * q.z);
		const float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		const float siny_cosp =		   2.0f * (q.w * q.z + q.x * q.y);
		const float cosy_cosp =	1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		const float sinp	  =		   2.0f * (q.w * q.y - q.z * q.x);
		const float x = atan2f( sinr_cosp, cosr_cosp );
		const float z = atan2f( siny_cosp, cosy_cosp );
		const float y = (fabsf(sinp) >= 1.0f)
			? copysignf(NE_PI/2.0f, sinp)
			: asinf(sinp);
		return { x, y, z };
		*/
	}

	inline Pos3_s Quat_ToEulerAngles_Deg( const Quat_s& q )
	{
		return Quat_ToEulerAngles( q ) * NE_RAD_TO_DEG;
	}

	inline Pos3_s Quat_ToAxisAngle( const Quat_s& q )
	{
		const Pos3_s v = { q.x, q.y, q.z };
		const float sin_half_angle = Pos3_Len( v );
		if (sin_half_angle < FLT_EPSILON)
			return Pos3_s { 1.0f, 0.0f, 0.0f, 0.0f };
		const float	angle = 2.0f * atan2f(sin_half_angle, q.w);
		const Pos3_s axis = (1.0f/sin_half_angle) * v;
		return Pos3_s { axis.x, axis.y, axis.z, angle };
	}

	inline float Quat_ToAngle( const Quat_s& q )
	{
		const float w_clamped = NeClamp( q.w, -1.0f, 1.0f );
		const float angle = 2.0f * acosf( w_clamped );
		return angle;
	}

	inline Quat_s Quat_Select( const Quat_s& q, float cmp )
	{
		return Quat_s
			{ Flt_Select( cmp, q.x, -q.x )
			, Flt_Select( cmp, q.y, -q.y )
			, Flt_Select( cmp, q.z, -q.z )
			, Flt_Select( cmp, q.w, -q.w )
			};
	}

	inline Pos3_s Quat_Rotate( const Quat_s& q, const Pos3_s& v )
	{
		return Quat_Mul( q, v );
	}

	inline Pos3_s Quat_InvRotate( const Quat_s& q, const Pos3_s& v )
	{
		return Quat_InvMul( q, v );
	}

	inline Quat_s Quat_ForRotation( const Pos3_s& v1, const Pos3_s& v2 )
	{
		const float tol = FLT_EPSILON;
		const float K = sqrtf( Pos3_LenSq(v1) * Pos3_LenSq(v2) );
		if (isinf(K))
			return Quat_Id();
		if (K < tol)
			return Quat_Id();

		const float dot = NeClamp( Pos3_Dot( v1, v2 ), -K, K );
		if (fabsf(dot + K) < (tol * K))
		{
			const Pos3_s mid = Pos3_Ortho( v1 );
			const Quat_s q1 = Quat_ForRotation( mid, v2 );
			const Quat_s q2 = Quat_ForRotation( v1, mid );
			return q2 * q1;
		}

		const Pos3_s xyz = Pos3_Cross( v1, v2 );
		const Quat_s q = { xyz.x, xyz.y, xyz.z, dot + K };
		return Quat_Norm( q );
	}

	inline Quat_s Quat_ForRotationAxis( const Pos3_s& v1, const Pos3_s& v2, const Pos3_s& axis )
	{
		const Pos3_s n1 = Pos3_Cross( axis, v1 );
		const Pos3_s n2 = Pos3_Cross( axis, n1 );
		const float n = -Pos3_Dot( v2, n1 );
		const float d =  Pos3_Dot( v2, n2 );
		const float K = -sqrtf( n * n + d * d );
		const Quat_s q = { n * axis.x, n * axis.y, n * axis.z, d + K };
		return Quat_Norm( q );
	}

	inline Quat_s Quat_ForRotationAxis( const Pos3_s& v1, const Pos3_s& v2, const Pos3_s& axis, float* condition )
	{
		const float d1 = 1.0f - fabsf(Pos3_Dot( v1, axis ));
		const float d2 = 1.0f - fabsf(Pos3_Dot( v2, axis ));
		*condition = d1 * d2;
		return Quat_ForRotationAxis( v1, v2, axis );
	}

	inline Quat_s Quat_ClosestRotationAxis( const Quat_s& q, const Pos3_s& axis )
	{
		const float dot = q.x * axis.x + q.y * axis.y + q.z * axis.z;
		const Pos3_s p = dot * axis;
		return Quat_Norm( Quat_s { p.x , p.y, p.z, q.w } );
	}

	inline Quat_s Quat_ClosestRotationAxis( const Quat_s& q, const Pos3_s& axis, float* condition )
	{
		*condition = ((1.0f - q.w) > FLT_TOL) 
			? fabsf( Pos3_Dot( Pos3_Norm( Quat_Vec(q) ), axis ) )
			: 1.0f;
		return Quat_ClosestRotationAxis( q, axis );
	}

	inline Quat_s Quat_ClosestRotationAxisSoft( const Quat_s& q, const Pos3_s& axis, float softness )
	{
		float condition;
		Quat_s result = Quat_ClosestRotationAxis( q, axis, &condition );
		if (softness > 0.0f)
		{
			condition /= softness;
			if (result.w > 0.0f)
				result.w += 1.0f - condition;
			else
				result.w -= 1.0f - condition;
			result = Quat_Norm( result );
		}
		return result;
	}

	inline Quat_s Quat_MirrorX( const Quat_s& v )
	{
		return Quat_s {  v.x, -v.y, -v.z, v.w };
	}

	inline Quat_s Quat_MirrorY( const Quat_s& v )
	{
		return Quat_s { -v.x,  v.y, -v.z, v.w };
	}

	inline Quat_s Quat_MirrorZ( const Quat_s& v )
	{
		return Quat_s { -v.x, -v.y,  v.z, v.w };
	}

	inline Quat_s Quat_Fade( const Quat_s& q0, const Quat_s& q1, float weight )
	{
		const float dot = Quat_Dot( q0, q1 );
		return (dot < 0.0f)
			? Quat_SlerpFast( q0, -q1, weight, -dot )
			: Quat_SlerpFast( q0,  q1, weight,  dot );
	}

	inline Quat_s Quat_Scale( const Quat_s& q, float dt )
	{
		const Pos3_s aa = Quat_ToAxisAngle( q );
		const float angle = fmodf( aa._ * dt, NE_PI * 2.0f );
		return Quat_FromAxisAngle( Pos3_s { aa.x, aa.y, aa.z }, angle );
	}

	inline Pos3_s Quat_GetAxisX( const Quat_s& q ) 
	{ 
		return q * Pos3_UnitX(); 
	}

	inline Pos3_s Quat_GetAxisY( const Quat_s& q ) 
	{	
		return q * Pos3_UnitY(); 
	}

	inline Pos3_s Quat_GetAxisZ( const Quat_s& q ) 
	{ 
		return q * Pos3_UnitZ();
	}

	inline Quat_s Quat_GetSwing( const Quat_s& q )
	{
		const float chs2 = q.w * q.w + q.x * q.x;
		if (chs2 > 1e-6f)
		{
			const float chs = sqrtf(chs2);
			return Quat_s 
				{ 0.0f
				, (q.w * q.y - q.x * q.z) / chs
				, (q.x * q.y + q.w * q.z) / chs
				, chs 
				};
		}
		const float rshs = 1.0f / sqrtf(1.0f - chs2);
		return Quat_s 
			{ 0.0f
			, q.y * rshs
			, q.z * rshs
			, 0.0f 
			};
	}

	inline float Quat_GetTwistAngle( const Quat_s& q, const Pos3_s& axis )
	{
		const float dot = Pos3_Dot( axis, Quat_Vec( q ) );
		const float theta = 2.0f * atan2f( dot, q.w );
		return Flt_NormRad( theta );
	}

	inline float Quat_Dist( const Quat_s& a, const Quat_s& b )
	{
		const float dot = Quat_Dot( a, b );
		const float cos = 2.0f * dot * dot - 1.0f;
		return acosf(cos);
	}

	inline Quat_s Quat_Offset( const Quat_s& q, const Quat_s& add )
	{
		const float dot  = Quat_Dot( q, add );
		const float sign = Flt_Select( dot, 1.0f, -1.0f );
		return q + add * sign;
	}

	inline Quat_s Quat_Offset( const Quat_s& q, const Quat_s& add, float factor )
	{
		return Quat_Offset( q, add * factor );
	}

	inline float Quat_CosHalfTheta( const Quat_s& q )
	{
		return q.w;
	}

	inline float Quat_SinHalfTheta( const Quat_s& q )
	{
		return Pos3_Len( Quat_Vec( q ) );
	}

	inline float Quat_CosTheta( const Quat_s& q )
	{
		// A quaternion represents an axis-angle rotation where:
		//	x = axis.x * sin(theta / 2)
		//	y = axis.y * sin(theta / 2)
		//	z = axis.z * sin(theta / 2)
		//	w =			 cos(theta / 2)

		// using the half angle formula: 
		//
		//	cos(theta/2) = sqrt(0.5*(1+cos(theta)))
		//
		//	=> (cos(theta/2))^2 = 0.5*(1+cos(theta))	| *2
		//	=> 2*(cos(theta/2))^2 = 1+cos(theta);		| -1
		//	=> 2*(cos(theta/2))^2-1 = cos(theta);		| ^2
		//	=> cos(theta) = 2 * q.w^2 - 1				| subst: cos(theta / 2) = w

		return 2.0f * Flt_Pow2( q.w ) - 1.0f;
	}

	inline Pos3_s Quat_Log( const Quat_s& q )
	{
		// check for a small rotation angle.
		const Pos3_s xyz = Quat_Vec( q );
		const float sin_half_angle = Pos3_Len( xyz );
		if (sin_half_angle < FLT_EPSILON)
			return Pos3_Zero();

		// @note: 
		//	this does not condition the quat to be in the 
		//	positive hemisphere, thus can encode shortest 
		//	or longest path rotation.
		const float scale = atan2f( sin_half_angle, q.w ) / sin_half_angle;
		return scale * xyz;
	}

	inline Quat_s Quat_Exp( const Pos3_s& v )
	{
		const float half_angle = Pos3_Len(v);
		if (half_angle < FLT_EPSILON)
			return Quat_Id();

		const float s = sinf(half_angle) / half_angle;
		const float w = cosf(half_angle);
		return Quat_s
			{ s * v.x
			, s * v.y
			, s * v.z
			, w
			};
	}

	inline float Quat_SignedAngleY( const Quat_s& s, const Quat_s& e )
	{
		const Pos3_s dir = { 0.0f, 0.0f, 1.0f };
		const Pos3_s p0 = dir * s;
		const Pos3_s p1 = dir * e;
		const float a0 = atan2f( p0.x, p0.z );
		const float a1 = atan2f( p1.x, p1.z );
		return a1-a0;
	}
}

//======================================================================================
//																				   XForm
//======================================================================================
namespace nemesis
{
	inline XForm_s XForm_Id()
	{
		return XForm_s 
			{ Pos3_Zero()
			, Quat_Id  () 
			};
	}

	inline XForm_s XForm_Inv( const XForm_s& v )
	{
		return XForm_s 
			{ Quat_InvRotate( v.Rot, -v.Pos )
			, Quat_Inv( v.Rot ) 
			};
	}

	inline XForm_s XForm_Mul( const XForm_s& x, float s )
	{
		return XForm_s { s * x.Pos, s * x.Rot };
	}

	inline XForm_s XForm_Mul( const XForm_s& lhs, const XForm_s& rhs )
	{
		return XForm_s
			{ lhs.Pos * rhs.Rot + rhs.Pos
			, lhs.Rot * rhs.Rot
			};
	}

	inline XForm_s XForm_Mul( const XForm_s& lhs, const Pos3_s& p, const Quat_s& q )
	{
		return XForm_s
			{ lhs.Pos * q + p
			, lhs.Rot * q
			};
	}

	inline XForm_s XForm_InvMul( const XForm_s& lhs, const XForm_s& rhs )
	{
		const Quat_s rhs_inv = ~rhs.Rot;
		return XForm_s
			{ rhs_inv * (lhs.Pos - rhs.Pos)
			, lhs.Rot * rhs_inv
			};
	}

	inline XForm_s XForm_Delta( const XForm_s& prev, const XForm_s& next )
	{
		const Pos3_s dx = next.Pos - prev.Pos;
		const Quat_s qc = Quat_Inv( prev.Rot );
		return XForm_s
			{			 qc * dx
			, Quat_Norm( qc * next.Rot )
			};
	}

	inline XForm_s XForm_Concat( const XForm_s& prev, const XForm_s& delta )
	{
		return XForm_s
			{ prev.Pos + prev.Rot * delta.Pos
			, Quat_Norm( prev.Rot * delta.Rot )
			};
	}

	inline XForm_s XForm_Blend( const XForm_s& src, const XForm_s& dst, float t )
	{
		return XForm_s
			{			 Pos3_Lerp ( src.Pos, dst.Pos, t )
			, Quat_Norm( Quat_Slerp( src.Rot, dst.Rot, t ) )
			};
	}

	inline Pos3_s XForm_Transform( const XForm_s& a, const Pos3_s& b )
	{
		return a.Pos + Quat_Rotate( a.Rot, b );
	}

	inline Pos3_s XForm_InvTransform( const XForm_s& a, const Pos3_s& b )
	{
		return Quat_InvRotate( a.Rot, b - a.Pos );
	}

	inline XForm_s XForm_Retransform( const XForm_s& lhs, const XForm_s& rhs )
	{
		const Quat_s q = ~lhs.Rot * rhs.Rot;
		const Pos3_s p = rhs.Pos - q * lhs.Pos;
		return XForm_s { p, q };
	}

	inline XForm_s XForm_Offset( const XForm_s& t, const XForm_s& add, float factor )
	{
		return XForm_s
			{ add.Pos * factor + t.Pos
			, Quat_Offset( t.Rot, add.Rot, factor )
			};
	}
}

//======================================================================================
//																				    Mat4
//======================================================================================
namespace nemesis
{
	inline Mat4_s Mat4_Id()
	{
		return Mat4_s
			{ Vec4_s { 1.0f, 0.0f, 0.0f, 0.0f }
			, Vec4_s { 0.0f, 1.0f, 0.0f, 0.0f }
			, Vec4_s { 0.0f, 0.0f, 1.0f, 0.0f }
			, Vec4_s { 0.0f, 0.0f, 0.0f, 1.0f }
		};
	}

	inline float Mat4_Det( const Mat4_s& m )
	{
		const float4x4& s = (const float4x4&)m;

		float t[3], v[4];

		t[0] =  s[2][2] * s[3][3] - s[2][3] * s[3][2];
		t[1] =  s[1][2] * s[3][3] - s[1][3] * s[3][2];
		t[2] =  s[1][2] * s[2][3] - s[1][3] * s[2][2];

		v[0] =  s[1][1] * t[0] - s[2][1] * t[1] + s[3][1] * t[2];
		v[1] = -s[1][0] * t[0] + s[2][0] * t[1] - s[3][0] * t[2];

		t[0] = s[1][0] * s[2][1] - s[2][0] * s[1][1];
		t[1] = s[1][0] * s[3][1] - s[3][0] * s[1][1];
		t[2] = s[2][0] * s[3][1] - s[3][0] * s[2][1];

		v[2] =  s[3][3] * t[0] - s[2][3] * t[1] + s[1][3] * t[2];
		v[3] = -s[3][2] * t[0] + s[2][2] * t[1] - s[1][2] * t[2];
		
		return s[0][0] * v[0] 
			 + s[0][1] * v[1] 
			 + s[0][2] * v[2] 
			 + s[0][3] * v[3];
	}

	inline Mat4_s Mat4_Transpose( const Mat4_s& m )
	{
		Mat4_s out;

			  float4x4& d = (float4x4&)out;
		const float4x4& s = (float4x4&)m;

		for ( int i = 0; i < 4; ++i )
			for ( int j = 0; j < 4; ++j )
				d[i][j] = s[j][i];

		return out;
	}

	inline Mat4_s Mat4_Inv( const Mat4_s& m )
	{
		const float4x4& s = (const float4x4&)m;

		float det, t[3], v[16];
		uint32_t i, j;
		
		t[0]  =  s[2][2] * s[3][3] - s[2][3] * s[3][2];
		t[1]  =  s[1][2] * s[3][3] - s[1][3] * s[3][2];
		t[2]  =  s[1][2] * s[2][3] - s[1][3] * s[2][2];

		v[0]  =  s[1][1] * t[0] - s[2][1] * t[1] + s[3][1] * t[2];
		v[4]  = -s[1][0] * t[0] + s[2][0] * t[1] - s[3][0] * t[2];
			  
		t[0]  =  s[1][0] * s[2][1] - s[2][0] * s[1][1];
		t[1]  =  s[1][0] * s[3][1] - s[3][0] * s[1][1];
		t[2]  =  s[2][0] * s[3][1] - s[3][0] * s[2][1];

		v[8]  =  s[3][3] * t[0] - s[2][3] * t[1] + s[1][3] * t[2];
		v[12] = -s[3][2] * t[0] + s[2][2] * t[1] - s[1][2] * t[2];
		
		det = s[0][0] * v[0] + s[0][1] * v[4] 
			+ s[0][2] * v[8] + s[0][3] * v[12];

		if (det == 0.0f)
		    return m;

		t[0] =  s[2][2] * s[3][3] - s[2][3] * s[3][2];
		t[1] =  s[0][2] * s[3][3] - s[0][3] * s[3][2];
		t[2] =  s[0][2] * s[2][3] - s[0][3] * s[2][2];

		v[1] = -s[0][1] * t[0] + s[2][1] * t[1] - s[3][1] * t[2];
		v[5] =  s[0][0] * t[0] - s[2][0] * t[1] + s[3][0] * t[2];
		
		t[0] =  s[0][0] * s[2][1] - s[2][0] * s[0][1];
		t[1] =  s[3][0] * s[0][1] - s[0][0] * s[3][1];
		t[2] =  s[2][0] * s[3][1] - s[3][0] * s[2][1];

		v[9] = -s[3][3] * t[0] - s[2][3] * t[1]- s[0][3] * t[2];
		v[13] = s[3][2] * t[0] + s[2][2] * t[1] + s[0][2] * t[2];
		
		t[0] =  s[1][2] * s[3][3] - s[1][3] * s[3][2];
		t[1] =  s[0][2] * s[3][3] - s[0][3] * s[3][2];
		t[2] =  s[0][2] * s[1][3] - s[0][3] * s[1][2];
			    
		v[2] =  s[0][1] * t[0] - s[1][1] * t[1] + s[3][1] * t[2];
		v[6] = -s[0][0] * t[0] + s[1][0] * t[1] - s[3][0] * t[2];
		
		t[0] = s[0][0] * s[1][1] - s[1][0] * s[0][1];
		t[1] = s[3][0] * s[0][1] - s[0][0] * s[3][1];
		t[2] = s[1][0] * s[3][1] - s[3][0] * s[1][1];

		v[10] =  s[3][3] * t[0] + s[1][3] * t[1] + s[0][3] * t[2];
		v[14] = -s[3][2] * t[0] - s[1][2] * t[1] - s[0][2] * t[2];
		
		t[0] =  s[1][2] * s[2][3] - s[1][3] * s[2][2];
		t[1] =  s[0][2] * s[2][3] - s[0][3] * s[2][2];
		t[2] =  s[0][2] * s[1][3] - s[0][3] * s[1][2];

		v[3] = -s[0][1] * t[0] + s[1][1] * t[1] - s[2][1] * t[2];
		v[7] =  s[0][0] * t[0] - s[1][0] * t[1] + s[2][0] * t[2];
		
		v[11] = -s[0][0] * (s[1][1] * s[2][3] - s[1][3] * s[2][1]) 
			  +  s[1][0] * (s[0][1] * s[2][3] - s[0][3] * s[2][1]) 
			  -  s[2][0] * (s[0][1] * s[1][3] - s[0][3] * s[1][1]);
		
		v[15] = s[0][0] * (s[1][1] * s[2][2] - s[1][2] * s[2][1]) 
			  - s[1][0] * (s[0][1] * s[2][2] - s[0][2] * s[2][1]) 
			  + s[2][0] * (s[0][1] * s[1][2] - s[0][2] * s[1][1]);
		
		det = 1.0f / det;
		
		float4x4 out;
		for (i = 0; i < 4; i++)
		    for (j = 0; j < 4; j++)
		        out[i][j] = v[4 * i + j] * det;
	
		return (const Mat4_s&)out;
	}

	inline Mat4_s Mat4_Mul( const Mat4_s& a, const Mat4_s& b )
	{
		float4x4 out;
		const float4x4& l = (float4x4&)a;
		const float4x4& r = (float4x4&)b;
		for ( int i = 0; i < 4; ++i )
		{
			for ( int j = 0; j < 4; ++j )
			{
				out[i][j] 
					= l[i][0] * r[0][j] 
					+ l[i][1] * r[1][j] 
					+ l[i][2] * r[2][j] 
					+ l[i][3] * r[3][j]
					;
			}
		}
		return (const Mat4_s&)out;
	}

	inline Mat4_s Mat4_BasisX( const Pos3_s& v )
	{
		const Pos3_s up = Pos3_s { 0.0f, 1.0f, 0.0f, 0.0f };
		const Pos3_s fw = Pos3_s { 0.0f, 0.0f, 1.0f, 0.0f };
		const float dot = Pos3_Dot( v, up );

		const Pos3_s a = (fabsf(dot) > 0.1f) ? up : fw;
		const Pos3_s x = Pos3_Norm( v );
		const Pos3_s y = Pos3_Norm( Pos3_Cross( x, a ) );
		const Pos3_s z = Pos3_Norm( Pos3_Cross( x, y ) );

		return Mat4_s 
			{ Vec4_s { x.x , x.y , x.z , 0.0f }
			, Vec4_s { y.x , y.y , y.z , 0.0f }
			, Vec4_s { z.x , z.y , z.z , 0.0f }
			, Vec4_s { 0.0f, 0.0f, 0.0f, 1.0f }
			};
	}

	inline Mat4_s Mat4_BasisY( const Pos3_s& v )
	{
		const Pos3_s up = Pos3_s { 0.0f, 1.0f, 0.0f, 0.0f };
		const Pos3_s fw = Pos3_s { 0.0f, 0.0f, 1.0f, 0.0f };
		const float dot = Pos3_Dot( v, up );

		const Pos3_s a = (fabsf(dot) > 0.1f) ? up : fw;
		const Pos3_s y = Pos3_Norm( v );
		const Pos3_s x = Pos3_Norm( Pos3_Cross( y, a ) );
		const Pos3_s z = Pos3_Norm( Pos3_Cross( y, x ) );

		return Mat4_s 
			{ Vec4_s { x.x , x.y , x.z , 0.0f }
			, Vec4_s { y.x , y.y , y.z , 0.0f }
			, Vec4_s { z.x , z.y , z.z , 0.0f }
			, Vec4_s { 0.0f, 0.0f, 0.0f, 1.0f }
			};
	}

	inline Mat4_s Mat4_BasisZ( const Pos3_s& v )
	{
		const Pos3_s up = Pos3_s { 0.0f, 1.0f, 0.0f, 0.0f };
		const Pos3_s fw = Pos3_s { 0.0f, 0.0f, 1.0f, 0.0f };
		const float dot = Pos3_Dot( v, up );

		const Pos3_s a = (fabsf(dot) > 0.1f) ? up : fw;
		const Pos3_s z = Pos3_Norm( v );
		const Pos3_s x = Pos3_Norm( Pos3_Cross( z, a ) );
		const Pos3_s y = Pos3_Norm( Pos3_Cross( z, x ) );

		return Mat4_s 
			{ Vec4_s { x.x , x.y , x.z , 0.0f }
			, Vec4_s { y.x , y.y , y.z , 0.0f }
			, Vec4_s { z.x , z.y , z.z , 0.0f }
			, Vec4_s { 0.0f, 0.0f, 0.0f, 1.0f }
			};
	}

	inline Mat4_s Mat4_RotationX( float angle )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		const float sin_theta = sinf(angle);
		const float cos_theta = cosf(angle);
		o[1][1] =  cos_theta;
		o[2][2] =  cos_theta;
		o[1][2] =  sin_theta;
		o[2][1] = -sin_theta;
		return out;
	}

	inline Mat4_s Mat4_RotationY( float angle )
	{ 
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		const float sin_theta = sinf(angle);
		const float cos_theta = cosf(angle);
		o[0][0] =  cos_theta;
		o[2][2] =  cos_theta;
		o[0][2] = -sin_theta;
		o[2][0] =  sin_theta;
		return out;
	}

	inline Mat4_s Mat4_RotationZ( float angle )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		const float sin_theta = sinf(angle);
		const float cos_theta = cosf(angle);
		o[0][0] =  cos_theta;
		o[1][1] =  cos_theta;
		o[0][1] =  sin_theta;
		o[1][0] = -sin_theta;
		return out;
	}

	inline Mat4_s Mat4_RotationAxis( const Pos3_s& axis, float angle )
	{
		float4x4 o;
		const Pos3_s norm = Pos3_Norm( axis );
		const float  sin_theta = sinf( angle );
		const float  cos_theta = cosf( angle );
		const float  cos_diff  = 1.0f-cos_theta;
		o[0][0] = cos_diff * norm.x * norm.x + cos_theta;
		o[1][0] = cos_diff * norm.x * norm.y - sin_theta * norm.z;
		o[2][0] = cos_diff * norm.x * norm.z + sin_theta * norm.y;
		o[3][0] = 0.0f;
		o[0][1] = cos_diff * norm.y * norm.x + sin_theta * norm.z;
		o[1][1] = cos_diff * norm.y * norm.y + cos_theta;
		o[2][1] = cos_diff * norm.y * norm.z - sin_theta * norm.x;
		o[3][1] = 0.0f;
		o[0][2] = cos_diff * norm.z * norm.x - sin_theta * norm.y;
		o[1][2] = cos_diff * norm.z * norm.y + sin_theta * norm.x;
		o[2][2] = cos_diff * norm.z * norm.z + cos_theta;
		o[3][2] = 0.0f;
		o[0][3] = 0.0f;
		o[1][3] = 0.0f;
		o[2][3] = 0.0f;
		o[3][3] = 1.0f;
		return (const Mat4_s&)o;
	}

	inline Mat4_s Mat4_RotationQuat( const Quat_s& q )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		o[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		o[0][1] =		 2.0f * (q.x * q.y + q.z * q.w);
		o[0][2] =		 2.0f * (q.x * q.z - q.y * q.w);
		o[1][0] =		 2.0f * (q.x * q.y - q.z * q.w);
		o[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		o[1][2] =		 2.0f * (q.y * q.z + q.x * q.w);
		o[2][0] =		 2.0f * (q.x * q.z + q.y * q.w);
		o[2][1] =		 2.0f * (q.y * q.z - q.x * q.w);
		o[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
  		return out;
	}

	inline Mat4_s Mat4_Scale( float x, float y, float z )
	{
		Mat4_s out = Mat4_Id();
		((float*)&out.r[0])[0] = x;
		((float*)&out.r[1])[1] = y;
		((float*)&out.r[2])[2] = z;
		return out;
	}

	inline Mat4_s Mat4_Translation( float x, float y, float z )
	{
		Mat4_s out = Mat4_Id();
		((float*)&out.r[3])[0] = x;
		((float*)&out.r[3])[1] = y;
		((float*)&out.r[3])[2] = z;
		return out;
	}

	inline Mat4_s Mat4_Transform( const Pos3_s& p, const Quat_s& q )
	{
		float4x4 out;
		out[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		out[0][1] =		   2.0f * (q.x * q.y + q.z * q.w);
		out[0][2] =		   2.0f * (q.x * q.z - q.y * q.w);
		out[0][3] = 0.0f;
		out[1][0] =		   2.0f * (q.x * q.y - q.z * q.w);
		out[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		out[1][2] =		   2.0f * (q.y * q.z + q.x * q.w);
		out[1][3] = 0.0f;
		out[2][0] =		   2.0f * (q.x * q.z + q.y * q.w);
		out[2][1] =		   2.0f * (q.y * q.z - q.x * q.w);
		out[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		out[2][3] = 0.0f;
		out[3][0] = p.x;
		out[3][1] = p.y;
		out[3][2] = p.z;
		out[3][3] = 1.0f;
  		return (const Mat4_s&)(out);
	}

}

//======================================================================================
//																				    Rect
//======================================================================================
namespace nemesis
{
	inline Vec2_s Rect_Pos( const Rect_s& r )
	{
		return Vec2_s { r.x, r.y };
	}

	inline Vec2_s Rect_Size( const Rect_s& r )
	{
		return Vec2_s { r.w, r.h };
	}

	inline Vec2_s Rect_Center( const Rect_s& r )
	{
		return Vec2_s { r.x + 0.5f * r.w, r.y + 0.5f * r.h };
	}

	inline float Rect_Right( const Rect_s& r )
	{
		return r.x + r.w;
	}

	inline float Rect_Bottom( const Rect_s& r )
	{
		return r.y + r.h;
	}

	inline bool Rect_Contains( const Rect_s& r, const Vec2_s& p )
	{
		return (p.x >= r.x) && p.x < (r.x + r.w)
			&& (p.y >= r.y) && p.y < (r.y + r.h);
	}

	inline bool Rect_Intersects( const Rect_s& a, const Rect_s& b )
	{
		return (a.x < Rect_Right (b)) 
			&& (a.y < Rect_Bottom(b)) 
			&& (Rect_Right (a) > b.x) 
			&& (Rect_Bottom(a) > b.y);
	}

	inline Rect_s Rect_Intersect( const Rect_s& a, const Rect_s& b )
	{
		const float x0 = NeMax( a.x, b.x );
		const float y0 = NeMax( a.y, b.y );
		const float x1 = NeMin( a.x + a.w, b.x + b.w );
		const float y1 = NeMin( a.y + a.h, b.y + b.h );
		const Rect_s result = { x0, y0, x1-x0, y1-y0 }; 
		return result;
	}

	inline Rect_s Rect_Offset( const Rect_s& a, const Vec2_s& b )
	{
		return Rect_s { a.x + b.x, a.y + b.y, a.w, a.h };
	}

	inline Rect_s Rect_Inflate( const Rect_s& v, float l, float t, float r, float b )
	{ 
		return Rect_s
			{ v.x -	 l	 , v.y -  t
			, v.w + (l+r), v.h + (t+b)
			};
	}

	inline Rect_s Rect_Inflate( const Rect_s& r, const Vec2_s& margin )
	{ 
		return Rect_Inflate( r, margin.x, margin.y, margin.x, margin.y );
	}

	inline Rect_s Rect_Inflate( const Rect_s& r, float x, float y )
	{
		return Rect_Inflate( r, Vec2_s { x, y } );
	}

	inline Rect_s Rect_Inflate( const Rect_s& r, float margin )
	{
		return Rect_Inflate( r, margin, margin );
	}

	inline Rect_s Rect_Margin( const Rect_s& v, float l, float t, float r, float b )
	{ 
		return Rect_Inflate( v, -l, -t, -r, -b );
	}

	inline Rect_s Rect_Margin( const Rect_s& r, const Vec2_s& margin )
	{ 
		return Rect_Margin( r, margin.x, margin.y, margin.x, margin.y );
	}

	inline Rect_s Rect_Margin( const Rect_s& r, float margin )
	{
		return Rect_Margin( r, Vec2_s { margin, margin } );
	}

	inline void Rect_SplitH( const Rect_s& r, float x, Rect_s* left, Rect_s* right )
	{
		if (left)
			*left = Rect_s { r.x, r.y, x, r.h };
		if (right)
			*right = Rect_s { r.x + x, r.y, r.w - x, r.h };
	}

	inline void Rect_SplitV( const Rect_s& r, float y, Rect_s* top, Rect_s* bottom )
	{
		if (top)
			*top = Rect_s { r.x, r.y, r.w, y };
		if (bottom)
			*bottom = Rect_s { r.x, r.y + y, r.w, r.h - y };
	}

	inline Rect_s Rect_Union( const Rect_s& a, const Rect_s& b )
	{
		const float x = NeMin( a.x, b.x );
		const float y = NeMin( a.y, b.y );
		return Rect_s 
			{ x
			, y
			, NeMax( a.x + a.w, b.x + b.w ) - x
			, NeMax( a.y + a.h, b.y + b.h ) - y 
			};
	}

	inline Rect2_s Rect_SplitT( const Rect_s& r, float v ) 
	{ 
		return Rect2_s 
			{ { { r.x, r.y	  , r.w,	   v }
			  , { r.x, r.y + v, r.w, r.h - v } 
			} }; 
	}

	inline Rect2_s Rect_SplitB( const Rect_s& r, float v ) 
	{ 
		return Rect2_s 
			{ { { r.x, r.y + r.h - v, r.w,	     v } 
			  , { r.x, r.y			, r.w, r.h - v }
			} }; 
	}

	inline Rect2_s Rect_SplitL( const Rect_s& r, float v ) 
	{ 
		return Rect2_s 
			{ { { r.x	 , r.y,		  v, r.h }
			  , { r.x + v, r.y, r.w - v, r.h } 
			} }; 
	}

	inline Rect2_s Rect_SplitR( const Rect_s& r, float v ) 
	{ 
		return Rect2_s 
			{ { { r.x + r.w - v, r.y,		v, r.h } 
			  , { r.x		   , r.y, r.w - v, r.h }
			} }; 
	}

	inline Rect2_s Rect_SplitH( const Rect_s& r, float x )
	{
		return Rect2_s 
			{ { { r.x	 , r.y,		  x, r.h }
			  , { r.x + x, r.y, r.w - x, r.h } 
			} };
	}

	inline Rect2_s Rect_SplitV( const Rect_s& r, float y )
	{
		return Rect2_s 
			{ { { r.x, r.y	  , r.w,	   y }
			  , { r.x, r.y + y, r.w, r.h - y }
			} };
	}

	inline Rect_s Rect_Ctor( const Vec2_s& pos, const Vec2_s& size )	
	{ 
		return Rect_s { pos.x, pos.y, size.x, size.y }; 
	}

	inline Vec2_s Rect_Min( const Rect_s& r )
	{ 
		return Vec2_s { r.x, r.y }; 
	}

	inline Vec2_s Rect_Max( const Rect_s& r )
	{ 
		return Vec2_s { r.x+r.w, r.y+r.h }; 
	}
}

//======================================================================================
//																			Quantization
//======================================================================================
namespace nemesis
{
	inline uint16_t Flt_Encode_16( float v )
	{
		NeAssert(((uint16_t)v) <= Discrete_16);
		return (uint16_t)v;
	}

	inline void Flt_Encode_16( const float* src, int count, uint16_t* dst, float& scale, float& mean )
	{
		if (!src || !count || !dst)
		{
			scale = mean = 0.0f;
			return;
		}

		// calc bounds for each axis 
		float min, max;
		Flt_MinMax( src, count, min, max );

		// discretize the range
		const float range = max-min;
		const float step = range / Discrete_16;
		const float inv_scale = step ? (1.0f / step) : 0.0f;

		// quantify items
		for ( int i = 0; i < count; ++i )
			dst[i] = Flt_Encode_16( Flt_Smu( src[i], min, inv_scale ) );

		// fill in scale and mean
		mean  = min;
		scale = step;
	}

	inline float Flt_Decode_16( uint16_t v )
	{
		return (float)(v & Discrete_16);
	}

	inline float Flt_Decode_16( uint16_t src, float scale, float mean )
	{
		return Flt_Mad( Flt_Decode_16( src ), scale, mean );
	}

	inline Pos3_s Pos3_Decode3_11_11_10( uint32_t v )
	{
		return Pos3_s
				{ (float)((v >> 21) & Discrete_11)
				, (float)((v >> 10) & Discrete_11)
				, (float)((v	  ) & Discrete_10)
				, 0.0f
				};
	}

	inline uint32_t Pos3_Encode3_11_11_10( const Pos3_s& v )
	{
		NeAssert(((uint32_t)v.x) <= Discrete_11);
		NeAssert(((uint32_t)v.y) <= Discrete_11);
		NeAssert(((uint32_t)v.z) <= Discrete_10);
		return ((uint32_t)(v.x) << 21)
			 | ((uint32_t)(v.y) << 10)
			 | ((uint32_t)(v.z)      )
			 ;
	}

	inline void Pos3_Encode3_11_11_10( const Pos3_s* src, int count, uint32_t* dst, Pos3_s& scale, Pos3_s& mean )
	{
		if (!src || !count || !dst)
		{
			scale = mean = Pos3_Zero();
			return;
		}

		// calc bounds for each axis 
		Pos3_s min, max;
		Pos3_MinMaxEach( src, count, min, max );

		// discretize the range
		const Pos3_s range = max-min;

		const Pos3_s step =
		{ range.x / Discrete_11
		, range.y / Discrete_11
		, range.z / Discrete_10
		};

		const Pos3_s inv_scale = 
		{ step.x ? 1.0f / step.x : 0.0f
		, step.y ? 1.0f / step.y : 0.0f
		, step.z ? 1.0f / step.z : 0.0f
		};

		// quantify items
		for ( int i = 0; i < count; ++i )
			dst[i] = Pos3_Encode3_11_11_10( Pos3_Smu( src[i], min, inv_scale ) );

		// fill in scale and mean
		mean  = min;
		scale = step;
	}
	
	inline Pos3_s Pos3_Decode3_11_11_10( uint32_t src, const Pos3_s& scale, const Pos3_s& mean )
	{
		return Pos3_Mad( Pos3_Decode3_11_11_10( src ), scale, mean );
	}

	inline void Pos3_Decode3_11_11_10( const uint32_t* src, int count, const Pos3_s& scale, const Pos3_s& mean, Pos3_s* dst )
	{
		if (!src || !count || !dst)
			return;

		for ( int i = 0; i < count; ++i )
			dst[i] = Pos3_Decode3_11_11_10( src[i], scale, mean );
	}

	inline void Quat_DecodeTqa3_11_11_10( const uint32_t* src, int count, const Pos3_s& scale, const Pos3_s& mean, Quat_s* dst )
	{
		if (!src || !count || !dst)
			return;

		for ( int i = 0; i < count; ++i )
			dst[i] = Quat_FromTqa( Pos3_Mad( Pos3_Decode3_11_11_10( src[i] ), scale, mean ) );
	}
}

//======================================================================================
//																				Blending
//======================================================================================
namespace nemesis
{
	inline float Flt_Blend_Lerp( float src, float dst, float t )
	{
		return src + t * (dst-src);
	}

	inline float Flt_Blend_Add( float src, float dst, float t )
	{
		return src + t * dst;
	}

	inline float Flt_Blend_Sub( float src, float dst, float t )
	{
		return src - t * dst;
	}

	inline Pos3_s Pos3_Blend_Lerp( const Pos3_s& src, const Pos3_s& dst, float t )
	{
		return src + t * (dst-src);
	}

	inline Pos3_s Pos3_Blend_Add( const Pos3_s& src, const Pos3_s& dst, float t )
	{
		return src + t * dst;
	}

	inline Pos3_s Pos3_Blend_Sub( const Pos3_s& src, const Pos3_s& dst, float t )
	{
		return src - t * dst;
	}

	inline Quat_s Quat_Blend_Lerp( const Quat_s& src, const Quat_s& dst, float t )
	{
		NeAssert( (t >= 0.0f) && (t <= 1.0f) );
		return Quat_Slerp( src, dst, t );
	}

	inline Quat_s Quat_Blend_Add( const Quat_s& src, const Quat_s& dst, float t )
	{
		return src * Quat_Slerp( Quat_Id(), dst, t );
	}

	inline Quat_s Quat_Blend_Sub( const Quat_s& src, const Quat_s& dst, float t )
	{
		return Quat_Slerp( Quat_Id(), src, t ) * Quat_Inv( dst );
	}
}

//======================================================================================
//																				   Color
//======================================================================================
namespace nemesis
{
	namespace
	{
		static const uint32_t AShift = 24;
		static const uint32_t RShift = 16;
		static const uint32_t GShift = 8;
		static const uint32_t BShift = 0;

		static const uint32_t AMask = 0xfful << AShift; 
		static const uint32_t RMask = 0xfful << RShift;
		static const uint32_t GMask = 0xfful << GShift;
		static const uint32_t BMask = 0xfful << BShift; 
	}

	inline uint8_t Color_GetAlpha( uint32_t argb )
	{
		return (argb & 0xff000000) >> 24;
	}

	inline uint32_t Color_SetAlpha( uint32_t argb, uint8_t a )
	{
		uint32_t a32 = a;
		return (argb & 0x00ffffff) | (a32 << 24);
	}

	inline Color_s Color_FromArgb( uint32_t argb )
	{
		return Color_c
			( ((argb & RMask) >> RShift) / 255.0f
			, ((argb & GMask) >> GShift) / 255.0f
			, ((argb & BMask) >> BShift) / 255.0f
			, ((argb & AMask) >> AShift) / 255.0f
			);
	}

	inline Color_s Color_FromArgb( uint32_t argb, float a )
	{
		return Color_c
			( ((argb & RMask) >> RShift) / 255.0f
			, ((argb & GMask) >> GShift) / 255.0f
			, ((argb & BMask) >> BShift) / 255.0f
			, a
			);
	}

	inline uint32_t Color_ToArgb( const Color_s& c )
	{
		return Color_ToArgb( c, c.a );
	}

	inline uint32_t Color_ToArgb( const Color_s& c, float alpha )
	{
		const uint8_t a_8 = (uint8_t)(NeClamp(alpha	, 0.0f, 1.0f) * 255.0f);
		const uint8_t r_8 = (uint8_t)(NeClamp(c.r	, 0.0f, 1.0f) * 255.0f);
		const uint8_t g_8 = (uint8_t)(NeClamp(c.g	, 0.0f, 1.0f) * 255.0f);
		const uint8_t b_8 = (uint8_t)(NeClamp(c.b	, 0.0f, 1.0f) * 255.0f);

		return (((uint32_t)a_8) << AShift)
			 | (((uint32_t)r_8) << RShift)
			 | (((uint32_t)g_8) << GShift)
			 | (((uint32_t)b_8) << BShift)
			 ;
	}

	inline Color_s Color_Blend( const Color_c& s, const Color_c& d, float a )
	{ 
		return ((1.0f-a) * s) + (a * d); 
	}

	inline Color_s	Color_Desaturate( const Color_c& c, float s )
	{ 
		return (1.0f-s) * c + s; 
	}

	inline Color_s	Color_Brighten( const Color_c& c, float s )
	{ 
		return c + s; 
	}

	inline Color_s Color_Modulate( const Color_s& c, float s )
	{ 
		return c * s; 
	}

	inline Color_s Color_Modulate( const Color_s& c, float rgb, float a )
	{
		return Color_c( c.r * rgb, c.g * rgb, c.b * rgb, c.a * a );	
	}
}

//======================================================================================
//																				FractInt
//======================================================================================
namespace nemesis
{
	inline FractInt_s FractInt_Set( float v )
	{
		const int   event = (int)v;
		const float phase = Flt_Fract(v);
		return FractInt_s { event, phase };
	}

	inline FractInt_s FractInt_Add( const FractInt_s& a, float b )
	{
		const float fract = a.Fract + b;
		const int	whole = (int)fract;
		return FractInt_s
			{ a.Whole + whole
			, fract	  - whole
			};
	}

	inline FractInt_s FractInt_Sub( const FractInt_s& a, const FractInt_s& b )
	{
		const float fract = a.Fract - b.Fract;
		const int	whole = (int)fract;
		return FractInt_s
			{ a.Whole - b.Whole + whole
			, fract				- whole
			};
	}

	inline float FractInt_ToFloat( const FractInt_s& v )
	{
		return v.Whole + v.Fract;
	}

	inline FractInt_s FractInt_Norm( const FractInt_s& v )
	{
		return FractInt_Set( FractInt_ToFloat( v ) );
	}

	inline float FractInt_Diff( const FractInt_s& from, const FractInt_s& to )
	{
		return FractInt_ToFloat( FractInt_Sub( to, from ) );
	}
}
