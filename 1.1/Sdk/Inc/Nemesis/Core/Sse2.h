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
#include "Sse2Types.h"

//======================================================================================
namespace nemesis
{
	//---------------------------------------------------------------------------- vec4i

	vec4f_t NE_VEC vec4i_asf( vec4i_t v );
	vec4f_t NE_VEC vec4i_itof( vec4i_t v );
	vec4f_t NE_VEC vec4i_utof( vec4i_t v );
	vec4i_t NE_VEC vec4i_set( uint32_t v );
	vec4i_t NE_VEC vec4i_and( vec4i_t v, vec4i_t mask );
	vec4i_t NE_VEC vec4i_and( vec4i_t v, uint32_t mask );
	vec4i_t NE_VEC vec4i_shift_r( vec4i_t v, uint32_t shift );

	//---------------------------------------------------------------------------- vec4f

	vec4i_t NE_VEC vec4f_asi( vec4f_t v );
	vec4f_t NE_VEC vec4f_set( float v );
	vec4f_t NE_VEC vec4f_set( float x, float y, float z, float w );
	vec4f_t NE_VEC vec4f_splat_x( vec4f_t v );
	vec4f_t NE_VEC vec4f_splat_y( vec4f_t v );
	vec4f_t NE_VEC vec4f_splat_z( vec4f_t v );
	vec4f_t NE_VEC vec4f_splat_w( vec4f_t v );
	vec4f_t NE_VEC vec4f_abs( vec4f_t v );
	vec4f_t NE_VEC vec4f_neg( vec4f_t v );
	vec4f_t NE_VEC vec4f_rcp( vec4f_t v );
	vec4f_t NE_VEC vec4f_add( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC vec4f_sub( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC vec4f_mul( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC vec4f_div( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC vec4f_add( vec4f_t lhs, float rhs );
	vec4f_t NE_VEC vec4f_sub( vec4f_t lhs, float rhs );
	vec4f_t NE_VEC vec4f_mul( vec4f_t lhs, float rhs );
	vec4f_t NE_VEC vec4f_div( vec4f_t lhs, float rhs );
	vec4f_t NE_VEC vec4f_neg( vec4f_t v, vec4f_t mask );
	vec4f_t NE_VEC vec4f_sel( vec4f_t lhs, vec4f_t rhs, vec4f_t sel );
	vec4f_t NE_VEC vec4f_norm( vec4f_t v );

	//----------------------------------------------------------------------------- pos3

	vec4f_t NE_VEC pos3_zero();
	vec4f_t NE_VEC pos3_set( float x, float y, float z );
	vec4f_t NE_VEC pos3_add( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC pos3_mul( vec4f_t lhs, float rhs );
	vec4f_t NE_VEC pos3_mul( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC pos3_cross( vec4f_t lhs, vec4f_t rhs );

	//----------------------------------------------------------------------------- quat

	vec4f_t NE_VEC quat_id();
	vec4f_t NE_VEC quat_inv( vec4f_t q );
	vec4f_t NE_VEC quat_mul_qq( vec4f_t lhs, vec4f_t rhs );
	vec4f_t NE_VEC quat_mul_qv( vec4f_t q, vec4f_t v );

	//----------------------------------------------------------------------------- mat4

	mat4_t NE_VEC mat4_rotation_q( vec4f_t q );
	mat4_t NE_VEC mat4_transform_vq( vec4f_t v, vec4f_t q );
	mat4_t NE_VEC mat4_transpose( const mat4_t& m );

	//----------------------------------------------------------------------------- soa4

	soa4_t NE_VEC soa4_splat( vec4f_t v );
	soa4_t NE_VEC soa4_transpose( const soa4_t& v );
	soa4_t NE_VEC soa4_pack( vec4f_t a, vec4f_t b, vec4f_t c, vec4f_t d );
	soa4_t NE_VEC soa4_pack_strided_2( const vec4f_t* v );
	soa4_t NE_VEC soa4_neg( const soa4_t& v, vec4f_t mask );
	soa4_t NE_VEC soa4_add( const soa4_t& lhs, const soa4_t& rhs );
	soa4_t NE_VEC soa4_sub( const soa4_t& lhs, const soa4_t& rhs );
	soa4_t NE_VEC soa4_mul( const soa4_t& lhs, const soa4_t& rhs );
	soa4_t NE_VEC soa4_add( const soa4_t& lhs, vec4f_t rhs );
	soa4_t NE_VEC soa4_sub( const soa4_t& lhs, vec4f_t rhs );
	soa4_t NE_VEC soa4_mul( const soa4_t& lhs, vec4f_t rhs );
	soa4_t NE_VEC soa4_mad( const soa4_t& v, const soa4_t& mul, const soa4_t& add );
	soa4_t NE_VEC soa4_decode_11_11_10( vec4i_t v );
	soa4_t NE_VEC soa4_decode_11_11_10( vec4i_t v, const soa4_t& mul, const soa4_t& add );
	vec4f_t NE_VEC soa4_dot( const soa4_t& lhs, const soa4_t& rhs );
	vec4f_t NE_VEC soa4_dot3( const soa4_t& lhs, const soa4_t& rhs );
	vec4f_t NE_VEC soa4_len_sq( const soa4_t& v );
	vec4f_t NE_VEC soa4_len_sq3( const soa4_t& v );
	soa4_t NE_VEC soa4_tqa_to_quat( const soa4_t& v );
	soa4_t NE_VEC soa4_quat_id();
	soa4_t NE_VEC soa4_quat_inv( const soa4_t& q );
	soa4_t NE_VEC soa4_quat_norm( const soa4_t& q );
	soa4_t NE_VEC soa4_quat_mul_qq( const soa4_t& a, const soa4_t& b );
	soa4_t NE_VEC soa4_quat_slerp_fast( const soa4_t& s, const soa4_t& d, vec4f_t t, vec4f_t dot );
	soa4_t NE_VEC soa4_quat_slerp( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_pos3_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_pos3_blend_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_pos3_blend_add( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_pos3_blend_sub( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_quat_blend_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_quat_blend_add( const soa4_t& s, const soa4_t& d, vec4f_t t );
	soa4_t NE_VEC soa4_quat_blend_sub( const soa4_t& s, const soa4_t& d, vec4f_t t );

	//------------------------------------------------------------------------ operators

	inline vec4f_t NE_VEC operator - ( vec4f_t rhs )							{ return vec4f_neg( rhs ); }
	inline vec4f_t NE_VEC operator + ( vec4f_t lhs, vec4f_t rhs )				{ return vec4f_add( lhs, rhs ); }
	inline vec4f_t NE_VEC operator - ( vec4f_t lhs, vec4f_t rhs )				{ return vec4f_sub( lhs, rhs ); }
	inline vec4f_t NE_VEC operator * ( vec4f_t lhs, vec4f_t rhs )				{ return vec4f_mul( lhs, rhs ); }
	inline vec4f_t NE_VEC operator / ( vec4f_t lhs, vec4f_t rhs )				{ return vec4f_div( lhs, rhs ); }
	inline vec4f_t NE_VEC operator + ( vec4f_t lhs, float rhs )					{ return vec4f_add( lhs, rhs ); }
	inline vec4f_t NE_VEC operator - ( vec4f_t lhs, float rhs )					{ return vec4f_sub( lhs, rhs ); }
	inline vec4f_t NE_VEC operator * ( vec4f_t lhs, float rhs )					{ return vec4f_mul( lhs, rhs ); }
	inline vec4f_t NE_VEC operator / ( vec4f_t lhs, float rhs )					{ return vec4f_div( lhs, rhs ); }
	inline vec4f_t NE_VEC operator + ( float lhs, vec4f_t rhs )					{ return vec4f_add( rhs, lhs ); }
	inline vec4f_t NE_VEC operator * ( float lhs, vec4f_t rhs )					{ return vec4f_mul( rhs, lhs ); }
	inline vec4f_t NE_VEC operator - ( float lhs, vec4f_t rhs )					{ return vec4f_set( lhs ) - rhs; }
	inline vec4f_t NE_VEC operator / ( float lhs, vec4f_t rhs )					{ return vec4f_set( lhs ) / rhs; }

	inline soa4_t NE_VEC operator + ( const soa4_t& lhs, const soa4_t& rhs )	{ return soa4_add( lhs, rhs ); }
	inline soa4_t NE_VEC operator - ( const soa4_t& lhs, const soa4_t& rhs )	{ return soa4_sub( lhs, rhs ); }
	inline soa4_t NE_VEC operator * ( const soa4_t& lhs, const soa4_t& rhs )	{ return soa4_mul( lhs, rhs ); }
	inline soa4_t NE_VEC operator + ( const soa4_t& lhs, vec4f_t rhs )			{ return soa4_add( lhs, rhs ); }
	inline soa4_t NE_VEC operator - ( const soa4_t& lhs, vec4f_t rhs )			{ return soa4_sub( lhs, rhs ); }
	inline soa4_t NE_VEC operator * ( const soa4_t& lhs, vec4f_t rhs )			{ return soa4_mul( lhs, rhs ); }
	inline soa4_t NE_VEC operator + ( vec4f_t lhs, const soa4_t& rhs )			{ return soa4_add( rhs, lhs ); }
	inline soa4_t NE_VEC operator - ( vec4f_t lhs, const soa4_t& rhs )			{ return soa4_sub( rhs, lhs ); }
	inline soa4_t NE_VEC operator * ( vec4f_t lhs, const soa4_t& rhs )			{ return soa4_mul( rhs, lhs ); }

}

//======================================================================================
#include "Sse2.inl"
