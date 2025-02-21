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
//																			  Extensions
//======================================================================================
inline __m128 _mm_cvtepu32_ps_sse2(const __m128i v)
{
	// https://stackoverflow.com/questions/34066228/how-to-perform-uint32-float-conversion-with-sse

    __m128i msk_lo    = _mm_set1_epi32(0xFFFF);
    __m128  cnst65536f= _mm_set1_ps(65536.0f);

    __m128i v_lo      = _mm_and_si128(v,msk_lo);          /* extract the 16 lowest significant bits of v                                   */
    __m128i v_hi      = _mm_srli_epi32(v,16);             /* 16 most significant bits of v                                                 */
    __m128  v_lo_flt  = _mm_cvtepi32_ps(v_lo);            /* No rounding                                                                   */
    __m128  v_hi_flt  = _mm_cvtepi32_ps(v_hi);            /* No rounding                                                                   */
            v_hi_flt  = _mm_mul_ps(cnst65536f,v_hi_flt);  /* No rounding                                                                   */
    return              _mm_add_ps(v_hi_flt,v_lo_flt);    /* Rounding may occur here, mul and add may fuse to fma for haswell and newer    */
} 

//======================================================================================
//																			   Constants
//======================================================================================
namespace nemesis
{
	constexpr uint32_t NE_U32_SELECT_0	= 0x00000000;
	constexpr uint32_t NE_U32_SELECT_1	= 0xffffffff;
	constexpr uint32_t NE_U32_SIGN_MASK	= 0x80000000;

	extern const NeWeak vec4f_u32_s NE_VEC4_MASK_3		= { NE_U32_SELECT_1, NE_U32_SELECT_1, NE_U32_SELECT_1, NE_U32_SELECT_0 };
    extern const NeWeak vec4f_u32_s NE_VEC4_SELECT_1110	= { NE_U32_SELECT_1, NE_U32_SELECT_1, NE_U32_SELECT_1, NE_U32_SELECT_0 };
	extern const NeWeak vec4f_u32_s NE_VEC4_INFINITY	= { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 };
	extern const NeWeak vec4f_u32_s NE_VEC4_QNAN		= { 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 };
	extern const NeWeak vec4f_f32_s NE_VEC4_ONE			= {  1.0f,  1.0f,  1.0f, 1.0f };
	extern const NeWeak vec4f_f32_s NE_VEC4_NEG_3		= { -1.0f, -1.0f, -1.0f, 1.0f };
    extern const NeWeak vec4f_f32_s NE_MAT4_ID_R0		= {  1.0f,  0.0f,  0.0f, 0.0f };
    extern const NeWeak vec4f_f32_s NE_MAT4_ID_R1		= {  0.0f,  1.0f,  0.0f, 0.0f };
    extern const NeWeak vec4f_f32_s NE_MAT4_ID_R2		= {  0.0f,  0.0f,  1.0f, 0.0f };
    extern const NeWeak vec4f_f32_s NE_MAT4_ID_R3		= {  0.0f,  0.0f,  0.0f, 1.0f };
}

//======================================================================================
//																			   Functions
//======================================================================================
namespace nemesis
{
	//---------------------------------------------------------------------------- vec4i

	inline vec4f_t NE_VEC vec4i_asf( vec4i_t v )
	{
		return _mm_castsi128_ps( v );
	}

	inline vec4f_t NE_VEC vec4i_itof( vec4i_t v )
	{
		return _mm_cvtepi32_ps( v );
	}

	inline vec4f_t NE_VEC vec4i_utof( vec4i_t v )
	{
		return _mm_cvtepu32_ps_sse2( v );
	}

	inline vec4i_t NE_VEC vec4i_set( uint32_t v )
	{
		return _mm_set1_epi32( v );
	}

	inline vec4i_t NE_VEC vec4i_and( vec4i_t v, vec4i_t mask )
	{
		return _mm_and_si128( v, mask );
	}

	inline vec4i_t NE_VEC vec4i_and( vec4i_t v, uint32_t mask )
	{
		return vec4i_and( v, vec4i_set( mask ) );
	}

	inline vec4i_t NE_VEC vec4i_shift_r( vec4i_t v, uint32_t shift )
	{
		return _mm_srli_epi32( v, shift );
	}

	//---------------------------------------------------------------------------- vec4f

	inline vec4i_t NE_VEC vec4f_asi( vec4f_t v )
	{
		return _mm_castps_si128( v );
	}

	inline vec4f_t NE_VEC vec4f_set( float v )
	{
		return _mm_set1_ps( v );
	}

	inline vec4f_t NE_VEC vec4f_set( float x, float y, float z, float w )
	{
		return _mm_setr_ps( x, y, z, w );
	}

	inline vec4f_t NE_VEC vec4f_splat_x( vec4f_t v )
	{
		return _mm_permute_ps( v, _MM_SHUFFLE(0,0,0,0) );
	}

	inline vec4f_t NE_VEC vec4f_splat_y( vec4f_t v )
	{
		return _mm_permute_ps( v, _MM_SHUFFLE(1,1,1,1) );
	}

	inline vec4f_t NE_VEC vec4f_splat_z( vec4f_t v )
	{
		return _mm_permute_ps( v, _MM_SHUFFLE(2,2,2,2) );
	}

	inline vec4f_t NE_VEC vec4f_splat_w( vec4f_t v )
	{
		return _mm_permute_ps( v, _MM_SHUFFLE(3,3,3,3) );
	}

	inline vec4f_t NE_VEC vec4f_abs( vec4f_t v )
	{
		const vec4f_u32_s mask = { NE_U32_SIGN_MASK, NE_U32_SIGN_MASK, NE_U32_SIGN_MASK, NE_U32_SIGN_MASK };
		return _mm_andnot_ps( mask.v, v );
	}

	inline vec4f_t NE_VEC vec4f_neg( vec4f_t v )
	{
		return _mm_sub_ps( _mm_setzero_ps(), v );
	}

	inline vec4f_t NE_VEC vec4f_rcp( vec4f_t v )
	{
		return _mm_rcp_ps( v );
	}

	inline vec4f_t NE_VEC vec4f_add( vec4f_t lhs, vec4f_t rhs )
	{
		return _mm_add_ps( lhs, rhs );
	}

	inline vec4f_t NE_VEC vec4f_sub( vec4f_t lhs, vec4f_t rhs )
	{
		return _mm_sub_ps( lhs, rhs );
	}

	inline vec4f_t NE_VEC vec4f_mul( vec4f_t lhs, vec4f_t rhs )
	{
		return _mm_mul_ps( lhs, rhs );
	}

	inline vec4f_t NE_VEC vec4f_div( vec4f_t lhs, vec4f_t rhs )
	{
	#if USE_SSE2_DIV
		return _mm_div_ps( lhs, rhs );
	#else
		return vec4f_mul( lhs, vec4f_rcp( rhs ) );
	#endif
	}

	inline vec4f_t NE_VEC vec4f_add( vec4f_t lhs, float rhs )
	{
		return vec4f_add( lhs, vec4f_set( rhs ) );
	}

	inline vec4f_t NE_VEC vec4f_sub( vec4f_t lhs, float rhs )
	{
		return vec4f_sub( lhs, vec4f_set( rhs ) );
	}

	inline vec4f_t NE_VEC vec4f_mul( vec4f_t lhs, float rhs )
	{
		return vec4f_mul( lhs, vec4f_set( rhs ) );
	}

	inline vec4f_t NE_VEC vec4f_div( vec4f_t lhs, float rhs )
	{
		return vec4f_div( lhs, vec4f_set( rhs ) );
	}

	inline vec4f_t NE_VEC vec4f_neg( vec4f_t v, vec4f_t mask )
	{
		const vec4f_u32_s sign = { NE_U32_SIGN_MASK, NE_U32_SIGN_MASK, NE_U32_SIGN_MASK, NE_U32_SIGN_MASK };
		return _mm_xor_ps( _mm_and_ps( sign.v, mask), v );
	}

	inline vec4f_t NE_VEC vec4f_sel( vec4f_t lhs, vec4f_t rhs, vec4f_t sel )
	{
		vec4f_t t1 = _mm_andnot_ps( sel, lhs );
		vec4f_t t2 = _mm_and_ps	 ( rhs, sel );
		return _mm_or_ps( t1, t2 );
	}

	inline vec4f_t NE_VEC vec4f_norm( vec4f_t v )
	{
		const vec4f_t d = _mm_dp_ps( v, v, 0xff );
		const vec4f_t s = _mm_sqrt_ps( d );
		const vec4f_t r = _mm_div_ps( v, s );
		return r;
	}

	//----------------------------------------------------------------------------- pos3

	inline vec4f_t NE_VEC pos3_zero()
	{ 
		return vec4f_set( 0.0f, 0.0f, 0.0f, 0.0f );
	}

	inline vec4f_t NE_VEC pos3_set( float v )
	{
		return vec4f_set( v, v, v, 0.0f );
	}

	inline vec4f_t NE_VEC pos3_set( float x, float y, float z )
	{
		return vec4f_set( x, y, z, 0.0f );
	}

	inline vec4f_t NE_VEC pos3_add( vec4f_t lhs, vec4f_t rhs )
	{
		return _mm_add_ps( lhs, rhs );
	}

	inline vec4f_t NE_VEC pos3_mul( vec4f_t lhs, float v )
	{
		return _mm_mul_ps( lhs, pos3_set( v ) );
	}

	inline vec4f_t NE_VEC pos3_mul( vec4f_t lhs, vec4f_t rhs )
	{
		return _mm_mul_ps( lhs, rhs );
	}

	inline vec4f_t NE_VEC pos3_cross( vec4f_t lhs, vec4f_t rhs )
	{
	#if USE_SSE2_DX
		// https://github.com/microsoft/DirectXMath

		 vec4f_t t1 = _mm_permute_ps(lhs, _MM_SHUFFLE(3, 0, 2, 1));
		 vec4f_t t2 = _mm_permute_ps(rhs, _MM_SHUFFLE(3, 1, 0, 2));
		 vec4f_t r0 = _mm_mul_ps(t1, t2);
		 t1 = _mm_permute_ps(t1, _MM_SHUFFLE(3, 0, 2, 1));
		 t2 = _mm_permute_ps(t2, _MM_SHUFFLE(3, 1, 0, 2));
		 r0 = _mm_fnmadd_ps(t1, t2, r0);
		 return _mm_and_ps(r0, VEC4_MASK_3.v);
	#else
		// http://fastcpp.blogspot.com/2011/04/vector-cross-product-using-sse-code.html

		return _mm_sub_ps
			( _mm_mul_ps(_mm_permute_ps(lhs, _MM_SHUFFLE(3, 0, 2, 1)), _mm_permute_ps(rhs, _MM_SHUFFLE(3, 1, 0, 2)))
			, _mm_mul_ps(_mm_permute_ps(lhs, _MM_SHUFFLE(3, 1, 0, 2)), _mm_permute_ps(rhs, _MM_SHUFFLE(3, 0, 2, 1)))
			);
	#endif
	}

	//----------------------------------------------------------------------------- quat

	inline vec4f_t NE_VEC quat_id()
	{
		return vec4f_set( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	inline vec4f_t NE_VEC quat_inv( vec4f_t q )
	{
	#if 1
		return _mm_mul_ps( q, NE_VEC4_NEG_3.v );
	#else
		return _mm_mul_ps( q, vec4f_set( -1.0f, -1.0f, -1.0f, 1.0f ) );
	#endif
	}

	inline vec4f_t NE_VEC quat_norm( vec4f_t q )
	{
		return vec4f_norm( q );
	}

	inline vec4f_t NE_VEC quat_mul_qq( vec4f_t lhs, vec4f_t rhs )
	{
	#if USE_SSE2_DX
		// https://github.com/microsoft/DirectXMath

		static const vec4f_f32_s ctrl_wzyx = {  1.0f, -1.0f,  1.0f, -1.0f };
		static const vec4f_f32_s ctrl_zwxy = {  1.0f,  1.0f, -1.0f, -1.0f };
		static const vec4f_f32_s ctrl_yxwz = { -1.0f,  1.0f,  1.0f, -1.0f };

	    // copy to SSE registers and use as few as possible for x86
		vec4f_t rhs_x = rhs;
		vec4f_t rhs_y = rhs;
		vec4f_t rhs_z = rhs;
		vec4f_t out_q = rhs;

		// splat with one instruction
		out_q = _mm_permute_ps(out_q, _MM_SHUFFLE(3, 3, 3, 3));
		rhs_x = _mm_permute_ps(rhs_x, _MM_SHUFFLE(0, 0, 0, 0));
		rhs_y = _mm_permute_ps(rhs_y, _MM_SHUFFLE(1, 1, 1, 1));
		rhs_z = _mm_permute_ps(rhs_z, _MM_SHUFFLE(2, 2, 2, 2));

		// retire lhs and perform lhs * rhs.w
		out_q = _mm_mul_ps(out_q, lhs);
		vec4f_t lhs_shuffle = lhs;

		// shuffle the copies of lhs
		lhs_shuffle = _mm_permute_ps(lhs_shuffle, _MM_SHUFFLE(0, 1, 2, 3));

		// mul by lhs.wzyx
		rhs_x = _mm_mul_ps(rhs_x, lhs_shuffle);
		lhs_shuffle = _mm_permute_ps(lhs_shuffle, _MM_SHUFFLE(2, 3, 0, 1));

		// flip the signs on y and z
		out_q = _mm_fmadd_ps(rhs_x, ctrl_wzyx.v, out_q);

		// mul by lhs.zwxy
		rhs_y = _mm_mul_ps(rhs_y, lhs_shuffle);
		lhs_shuffle = _mm_permute_ps(lhs_shuffle, _MM_SHUFFLE(0, 1, 2, 3));

		// flip the signs on z and w
		rhs_y = _mm_mul_ps(rhs_y, ctrl_zwxy.v);

		// mul by lhs.yxwz
		rhs_z = _mm_mul_ps(rhs_z, lhs_shuffle);

		// flip the signs on x and w
		rhs_y = _mm_fmadd_ps(rhs_z, ctrl_yxwz.v, rhs_y);
		out_q = _mm_add_ps(out_q, rhs_y);

		return out_q;
	#else
		// https://nfrechette.github.io/page4/ Quaternion multiplication

		constexpr vec4f_f32_s control_wzyx = {  1.0f, -1.0f,  1.0f, -1.0f };
		constexpr vec4f_f32_s control_zwxy = {  1.0f,  1.0f, -1.0f, -1.0f };
		constexpr vec4f_f32_s control_yxwz = { -1.0f,  1.0f,  1.0f, -1.0f };

		vec4f_t r_xxxx = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(0, 0, 0, 0));
		vec4f_t r_yyyy = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(1, 1, 1, 1));
		vec4f_t r_zzzz = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(2, 2, 2, 2));
		vec4f_t r_wwww = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 3, 3, 3));
		
		vec4f_t lxrw_lyrw_lzrw_lwrw = _mm_mul_ps(r_wwww, lhs);
		vec4f_t l_wzyx = _mm_shuffle_ps(lhs, lhs,_MM_SHUFFLE(0, 1, 2, 3));
		
		vec4f_t lwrx_lzrx_lyrx_lxrx = _mm_mul_ps(r_xxxx, l_wzyx);
		vec4f_t l_zwxy = _mm_shuffle_ps(l_wzyx, l_wzyx,_MM_SHUFFLE(2, 3, 0, 1));
		
		vec4f_t lwrx_nlzrx_lyrx_nlxrx = _mm_mul_ps(lwrx_lzrx_lyrx_lxrx, control_wzyx.v); // flip!
		
		vec4f_t lzry_lwry_lxry_lyry = _mm_mul_ps(r_yyyy, l_zwxy);
		vec4f_t l_yxwz = _mm_shuffle_ps(l_zwxy, l_zwxy,_MM_SHUFFLE(0, 1, 2, 3));
		
		vec4f_t lzry_lwry_nlxry_nlyry = _mm_mul_ps(lzry_lwry_lxry_lyry, control_zwxy.v); // flip!
		
		vec4f_t lyrz_lxrz_lwrz_lzrz = _mm_mul_ps(r_zzzz, l_yxwz);
		vec4f_t result0 = _mm_add_ps(lxrw_lyrw_lzrw_lwrw, lwrx_nlzrx_lyrx_nlxrx);
		
		vec4f_t nlyrz_lxrz_lwrz_wlzrz = _mm_mul_ps(lyrz_lxrz_lwrz_lzrz, control_yxwz.v); // flip!
		vec4f_t result1 = _mm_add_ps(lzry_lwry_nlxry_nlyry, nlyrz_lxrz_lwrz_wlzrz);

		return _mm_add_ps(result0, result1);	
	#endif
	}

	inline vec4f_t NE_VEC quat_mul_qv( vec4f_t q, vec4f_t v )
	{
	#if USE_SSE2_DX
		// https://github.com/microsoft/DirectXMath

		vec4f_t v1 = vec4f_sel( VEC4_SELECT_1110.v, v, VEC4_SELECT_1110.v );
		vec4f_t q1 = quat_inv( q );
		vec4f_t r0 = quat_mul_qq( q1, v1 );
		vec4f_t r1 = quat_mul_qq( r0, q );
		return r1;
	#else
		// https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/

		vec4f_t q_cross_v = pos3_cross( q, v );
		vec4f_t t  = pos3_mul( q_cross_v, 2.0f );
		vec4f_t t0 = pos3_add( v, pos3_mul( t, vec4f_splat_w( q ) ) );
		vec4f_t t1 = pos3_add( t0, pos3_cross( q, t ) );
		return t1;
	#endif
	}

	//----------------------------------------------------------------------------- mat4

	inline mat4_t NE_VEC mat4_rotation_q( vec4f_t q )
	{
		static const vec4f_f32_s Constant1110 = { 1.0f, 1.0f, 1.0f, 0.0f };

		vec4f_t Q0 = _mm_add_ps(q, q);
		vec4f_t Q1 = _mm_mul_ps(q, Q0);

		vec4f_t V0 = _mm_permute_ps(Q1, _MM_SHUFFLE(3, 0, 0, 1));
		V0 = _mm_and_ps(V0, NE_VEC4_MASK_3.v);
		vec4f_t V1 = _mm_permute_ps(Q1, _MM_SHUFFLE(3, 1, 2, 2));
		V1 = _mm_and_ps(V1, NE_VEC4_MASK_3.v);
		vec4f_t R0 = _mm_sub_ps(Constant1110.v, V0);
		R0 = _mm_sub_ps(R0, V1);

		V0 = _mm_permute_ps(q, _MM_SHUFFLE(3, 1, 0, 0));
		V1 = _mm_permute_ps(Q0, _MM_SHUFFLE(3, 2, 1, 2));
		V0 = _mm_mul_ps(V0, V1);

		V1 = _mm_permute_ps(q, _MM_SHUFFLE(3, 3, 3, 3));
		vec4f_t V2 = _mm_permute_ps(Q0, _MM_SHUFFLE(3, 0, 2, 1));
		V1 = _mm_mul_ps(V1, V2);

		vec4f_t R1 = _mm_add_ps(V0, V1);
		vec4f_t R2 = _mm_sub_ps(V0, V1);

		V0 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(1, 0, 2, 1));
		V0 = _mm_permute_ps(V0, _MM_SHUFFLE(1, 3, 2, 0));
		V1 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 2, 0, 0));
		V1 = _mm_permute_ps(V1, _MM_SHUFFLE(2, 0, 2, 0));

		Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(1, 0, 3, 0));
		Q1 = _mm_permute_ps(Q1, _MM_SHUFFLE(1, 3, 2, 0));

		mat4_t M;
		M.r[0] = Q1;

		Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(3, 2, 3, 1));
		Q1 = _mm_permute_ps(Q1, _MM_SHUFFLE(1, 3, 0, 2));
		M.r[1] = Q1;

		Q1 = _mm_shuffle_ps(V1, R0, _MM_SHUFFLE(3, 2, 1, 0));
		M.r[2] = Q1;
		M.r[3] = NE_MAT4_ID_R3.v;
		return M;
	}

	inline mat4_t NE_VEC mat4_transform_vq( vec4f_t v, vec4f_t q )
	{
		mat4_t m = mat4_rotation_q( q );
		vec4f_t v1 = _mm_permute_ps(v				 , _MM_SHUFFLE(2, 2, 2, 2));	// v.z v.z  v.z v.z
		vec4f_t v2 = _mm_shuffle_ps(v1, NE_VEC4_ONE.v, _MM_SHUFFLE(0, 0, 0, 0));	// v.z v.z   1   1
		vec4f_t v3 = _mm_permute_ps(v2				 , _MM_SHUFFLE(3, 0, 0, 0));	// v.z v.z  v.z  1
		vec4f_t v4 = _mm_shuffle_ps(v , v3			 , _MM_SHUFFLE(3, 2, 1, 0));	// v.x v.y  v.z  1
		m.r[3] = v4;
		return m;
	}

	inline mat4_t NE_VEC mat4_transpose( const mat4_t& m )
	{
		vec4f_t t0 = _mm_shuffle_ps(m.r[0], m.r[1], 0x44);
		vec4f_t t2 = _mm_shuffle_ps(m.r[0], m.r[1], 0xEE);
		vec4f_t t1 = _mm_shuffle_ps(m.r[2], m.r[3], 0x44);
		vec4f_t t3 = _mm_shuffle_ps(m.r[2], m.r[3], 0xEE);

		return mat4_t
			{ _mm_shuffle_ps(t0, t1, 0x88)
			, _mm_shuffle_ps(t0, t1, 0xDD)
			, _mm_shuffle_ps(t2, t3, 0x88)
			, _mm_shuffle_ps(t2, t3, 0xDD)
			};
	}

	//----------------------------------------------------------------------------- soa4

	inline soa4_t NE_VEC soa4_splat( vec4f_t v )
	{
		return soa4_t 
			{ vec4f_splat_x(v)
			, vec4f_splat_y(v)
			, vec4f_splat_z(v)
			, vec4f_splat_w(v) 
			};
	}

	inline soa4_t NE_VEC soa4_transpose( const soa4_t& v )
	{
		vec4f_t t0 = _mm_shuffle_ps(v.x, v.y, 0x44);
		vec4f_t t2 = _mm_shuffle_ps(v.x, v.y, 0xEE);
		vec4f_t t1 = _mm_shuffle_ps(v.z, v.w, 0x44);
		vec4f_t t3 = _mm_shuffle_ps(v.z, v.w, 0xEE);

		return soa4_t
			{ _mm_shuffle_ps(t0, t1, 0x88)
			, _mm_shuffle_ps(t0, t1, 0xDD)
			, _mm_shuffle_ps(t2, t3, 0x88)
			, _mm_shuffle_ps(t2, t3, 0xDD)
			};
	}

	inline soa4_t NE_VEC soa4_pack( vec4f_t a, vec4f_t b, vec4f_t c, vec4f_t d )
	{
		return soa4_transpose( soa4_t { a, b, c, d } );
	}

	inline soa4_t NE_VEC soa4_pack_strided_2( const vec4f_t* v )
	{
		return soa4_pack( v[0], v[2], v[4], v[6] );
	}

	inline soa4_t NE_VEC soa4_neg( const soa4_t& v, vec4f_t mask )
	{
		return soa4_t
			{ vec4f_neg( v.x, mask )
			, vec4f_neg( v.y, mask )
			, vec4f_neg( v.z, mask )
			, vec4f_neg( v.w, mask )
			};
	}

	inline soa4_t NE_VEC soa4_add( const soa4_t& lhs, const soa4_t& rhs )
	{
		return soa4_t
			{ lhs.x + rhs.x
			, lhs.y + rhs.y
			, lhs.z + rhs.z
			, lhs.w + rhs.w
			};
	}

	inline soa4_t NE_VEC soa4_sub( const soa4_t& lhs, const soa4_t& rhs )
	{
		return soa4_t
			{ lhs.x - rhs.x
			, lhs.y - rhs.y
			, lhs.z - rhs.z
			, lhs.w - rhs.w
			};
	}

	inline soa4_t NE_VEC soa4_mul( const soa4_t& lhs, const soa4_t& rhs )
	{
		return soa4_t
			{ lhs.x * rhs.x
			, lhs.y * rhs.y
			, lhs.z * rhs.z
			, lhs.w * rhs.w
			};
	}

	inline soa4_t NE_VEC soa4_add( const soa4_t& lhs, vec4f_t rhs )
	{
		return soa4_t
			{ lhs.x + rhs
			, lhs.y + rhs
			, lhs.z + rhs
			, lhs.w + rhs
			};
	}

	inline soa4_t NE_VEC soa4_sub( const soa4_t& lhs, vec4f_t rhs )
	{
		return soa4_t
			{ lhs.x - rhs
			, lhs.y - rhs
			, lhs.z - rhs
			, lhs.w - rhs
			};
	}

	inline soa4_t NE_VEC soa4_mul( const soa4_t& lhs, vec4f_t rhs )
	{
		return soa4_t
			{ lhs.x * rhs
			, lhs.y * rhs
			, lhs.z * rhs
			, lhs.w * rhs
			};
	}

	inline soa4_t NE_VEC soa4_mad( const soa4_t& v, const soa4_t& mul, const soa4_t& add )
	{
		return v * mul + add;
	}

	inline soa4_t NE_VEC soa4_decode_11_11_10( vec4i_t v )
	{
		const vec4f_t x = vec4i_utof( vec4i_and( vec4i_shift_r( v, 21 ), Discrete_11 ) );
		const vec4f_t y = vec4i_utof( vec4i_and( vec4i_shift_r( v, 10 ), Discrete_11 ) );
		const vec4f_t z = vec4i_utof( vec4i_and( vec4i_shift_r( v,  0 ), Discrete_10 ) );
		const vec4f_t w = vec4f_set( 0.0f );
		return soa4_t { x, y, z, w };
	}

	inline soa4_t NE_VEC soa4_decode_11_11_10( vec4i_t v, const soa4_t& mul, const soa4_t& add )
	{
		return soa4_mad( soa4_decode_11_11_10( v ), mul, add );
	}

	inline vec4f_t NE_VEC soa4_dot( const soa4_t& lhs, const soa4_t& rhs )
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	inline vec4f_t NE_VEC soa4_dot3( const soa4_t& lhs, const soa4_t& rhs )
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline vec4f_t NE_VEC soa4_len_sq( const soa4_t& v )
	{
		return soa4_dot( v, v );
	}

	inline vec4f_t NE_VEC soa4_len_sq3( const soa4_t& v )
	{
		return soa4_dot3( v, v );
	}

	inline soa4_t NE_VEC soa4_tqa_to_quat( const soa4_t& v )
	{
		vec4f_t mag2 = soa4_len_sq3( v );
		vec4f_t opm2 = mag2 + 1.0f; 
		vec4f_t scale = 2.0f / opm2;
		vec4f_t w = (1.0f - mag2) /opm2;
		return soa4_t
			{ v.x * scale
			, v.y * scale
			, v.z * scale
			, w
			};
	}

	inline soa4_t NE_VEC soa4_quat_id()
	{
		return soa4_t 
			{ vec4f_set( 0.0f )
			, vec4f_set( 0.0f )
			, vec4f_set( 0.0f )
			, vec4f_set( 1.0f ) 
			};
	}

	inline soa4_t NE_VEC soa4_quat_inv( const soa4_t& q )
	{
		return soa4_t { -q.x, -q.y, -q.z,  q.w };
	}

	inline soa4_t NE_VEC soa4_quat_norm( const soa4_t& q )
	{
		// @todo: no transpose
		const soa4_t qt  = soa4_transpose( q );
		const soa4_t qtn = 
		{ quat_norm( qt.x )
		, quat_norm( qt.y )
		, quat_norm( qt.z )
		, quat_norm( qt.w )
		};
		return soa4_transpose( qtn );
	}

	inline soa4_t NE_VEC soa4_quat_mul_qq( const soa4_t& a, const soa4_t& b )
	{
	#if 1
		return soa4_t
			{ a.w * b.x + a.x * b.w + b.y * a.z - b.z * a.y
			, a.w * b.y + a.y * b.w + b.z * a.x - b.x * a.z
			, a.w * b.z + a.z * b.w + b.x * a.y - b.y * a.x
			, a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
			};
	#else
		return soa4_t
			{ b.w * a.x + b.x * a.w + b.y * a.z - b.z * a.y
			, b.w * a.y - b.x * a.z + b.y * a.w + b.z * a.x
			, b.w * a.z + b.x * a.y - b.y * a.x + b.z * a.w
			, b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z
			};
	#endif
	}

	inline soa4_t NE_VEC soa4_quat_slerp_fast( const soa4_t& s, const soa4_t& d, vec4f_t t, vec4f_t dot )
	{
		// c1 =  1.570994357000f + ( 0.56429298590f + (-0.17836577170f + 0.043199493520f * dot) * dot) * dot;
		// c3 = -0.646139638200f + ( 0.59456579360f + ( 0.08610323953f - 0.034651229280f * dot) * dot) * dot;
		// c5 =  0.079498235210f + (-0.17304369310f + ( 0.10792795990f - 0.014393978010f * dot) * dot) * dot;
		// c7 = -0.004354102836f + ( 0.01418962736f + (-0.01567189691f + 0.005848706227f * dot) * dot) * dot;

		const vec4f_t Ca = vec4f_set(  1.57099435700f, -0.64613963820f,  0.07949823521f, -0.004354102836f );
		const vec4f_t Cb = vec4f_set(  0.56429298590f,  0.59456579360f, -0.17304369310f,  0.014189627360f );
		const vec4f_t Cc = vec4f_set( -0.17836577170f,  0.08610323953f,  0.10792795990f, -0.015671896910f );
		const vec4f_t Cd = vec4f_set(  0.04319949352f, -0.03465122928f, -0.01439397801f,  0.005848706227f );

		const vec4f_t rcp = vec4f_rcp( dot + 1.0f );

		const vec4f_t T  = 1.0f - t;
		const vec4f_t t2 = t * t;
		const vec4f_t T2 = T * T;

		const soa4_t ff = soa4_splat( dot );
		const soa4_t cc = soa4_transpose( ff * (ff * (ff * Cd + Cc) + Cb) + Ca ); 

		const vec4f_t s_weights = ((cc.x + ((cc.y + ((cc.z + (cc.w * T2)) * T2)) * T2)) * (rcp * T));
		const vec4f_t d_weights = ((cc.x + ((cc.y + ((cc.z + (cc.w * t2)) * t2)) * t2)) * (rcp * t));

		const soa4_t q = s * s_weights + d * d_weights;

		return soa4_quat_norm( q );
	}

	inline soa4_t NE_VEC soa4_quat_slerp( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		const vec4f_t dot = soa4_dot( s, d );
		const vec4f_t abs = vec4f_abs( dot );
		const soa4_t neg = soa4_neg( d, dot );
		return soa4_quat_slerp_fast( s, neg, t, abs );
	}

	inline soa4_t NE_VEC soa4_pos3_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		return (1.0f-t) * s + t * d;
	}

	inline soa4_t NE_VEC soa4_pos3_blend_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		return soa4_pos3_lerp( s, d, t );
	}

	inline soa4_t NE_VEC soa4_pos3_blend_add( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		return s + t * d;
	}

	inline soa4_t NE_VEC soa4_pos3_blend_sub( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		return s - t * d;
	}

	inline soa4_t NE_VEC soa4_quat_blend_lerp( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		return soa4_quat_slerp( s, d, t );
	}

	inline soa4_t NE_VEC soa4_quat_blend_add( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		// s * slerp( id, d, t )
		return soa4_quat_mul_qq( s, soa4_quat_slerp( soa4_quat_id(), d, t ) );	
	}

	inline soa4_t NE_VEC soa4_quat_blend_sub( const soa4_t& s, const soa4_t& d, vec4f_t t )
	{
		// slerp( id, s, t ) * ~d
		return soa4_quat_mul_qq( soa4_quat_slerp( soa4_quat_id(), s, t ), soa4_quat_inv( d ) );
	}

}
