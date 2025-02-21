//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Core/Math.h>
#include <Nemesis/Core/Assert.h>
#include <math.h>
#include <float.h>

//======================================================================================
namespace nemesis
{
	/// Float

	float Flt_Nice( float x, bool round )
	{
		/* Nice Numbers for Graph Labels
		 * by Paul Heckbert
		 * from "Graphics Gems", Academic Press, 1990 */

		float expv;						/* exponent of x */
		float f;						/* fractional part of x */
		float nf;						/* nice, rounded fraction */
		expv = floorf(log10f(x));
		f = x/powf(10.0f, expv);		/* between 1 and 10 */
		if (round)
			if (f<1.5f) nf = 1.0f;
			else if (f<3.0f) nf = 2.0f;
			else if (f<7.0f) nf = 5.0f;
			else nf = 10.0f;
		else
			if (f<=1.0f) nf = 1.0f;
			else if (f<=2.0f) nf = 2.0f;
			else if (f<=5.0f) nf = 5.0f;
			else nf = 10.0f;
		return nf*powf(10.0f, expv);
	}

	Vec2_s Flt_SolveQuad( float a, float b, float c )
	{
		NeAssert(a * a + b * b + c * c > 0);
		const float coeff1sqr = (b * b - 4.0f * a * c);
		const float coeff1 = sqrtf( NeMax( coeff1sqr, 0.0f ) );
		const float sign_b = Flt_Select(b, 1.0f, -1.0f);
		const float t = -0.5f * (b + sign_b * coeff1);
		const float t_a = t / a;
		const float c_t = c / t;
		return Vec2_s
			{ Flt_Select( fabsf(a) - FLT_TINY, t_a, c_t )
			, Flt_Select( fabsf(t) - FLT_TINY, c_t, t_a )
			};
	}

}

//======================================================================================
namespace nemesis
{
	/// Mat4

	Mat4_s Mat4_LookAtLH( const Pos3_s& eye, const Pos3_s& at, const Pos3_s& up )
	{
		const Pos3_s z = Pos3_Norm( at - eye );
		const Pos3_s x = Pos3_Norm( Pos3_Cross( up, z ) );
		const Pos3_s y = Pos3_Cross( z, x );
		const Pos3_s d = 
		{ -Pos3_Dot( x, eye )
		, -Pos3_Dot( y, eye )
		, -Pos3_Dot( z, eye )
		};
		return Mat4_s
			{ /* Pos3_s { */ x.x, y.x, z.x, 0.0f /* } */
			, /* Pos3_s { */ x.y, y.y, z.y, 0.0f /* } */
			, /* Pos3_s { */ x.z, y.z, z.z, 0.0f /* } */
			, /* Pos3_s { */ d.x, d.y, d.z, 1.0f /* } */
			};
	}

	Mat4_s Mat4_PerspectiveFovLH( float fov_y, float w_by_h, float z_near, float z_far )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		const float tan = tanf( 0.5f * fov_y );
		o[0][0] = 1.0f / (w_by_h * tan);
		o[1][1] = 1.0f /		   tan;
		o[2][2] = z_far / (z_far - z_near);
		o[2][3] = 1.0f;
		o[3][2] = (z_far * z_near) / (z_near - z_far);
		o[3][3] = 0.0f;
		return out;
	}

	Mat4_s Mat4_OrthoLH( float w, float h, float z_near, float z_far )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		o[0][0] = 2.0f/w;
		o[1][1] = 2.0f/h;
		o[2][2] = 1.0f/(z_far-z_near);
		o[3][2] = -z_near/(z_far-z_near);
		return out;
	}

	Mat4_s Mat4_OrthoOffCenterLH( float l, float r, float b, float t, float z_near, float z_far )
	{
		Mat4_s out = Mat4_Id();
		float4x4& o = (float4x4&)out;
		o[0][0] = 2.0f/(r-l);
		o[1][1] = 2.0f/(t-b);
		o[2][2] = 1.0f/(z_far-z_near);
		o[3][0] = (l+r)/(l-r);
		o[3][1] = (t+b)/(b-t);
		o[3][2] = z_near/(z_near-z_far);
		return out;
	}

	Quat_s Mat4_GetRotation_Quat( const Mat4_s& m )
	{
		const float* const t = (float*)&m.r[0].x;

		float tr, s;

		tr = 1.0f + t[0] + t[5] + t[10];

		Quat_s quat;

		if (tr > 1.0f)
		{
			s = sqrtf (tr) * 2.0f;

			quat.w = s * 0.25f;

			quat.x = (t[6] - t[9]) / s;
			quat.y = (t[8] - t[2]) / s;
			quat.z = (t[1] - t[4]) / s;
		}
		else
		{
			if ((t [0] > t [5]) && (t [0] > t [10]))
			{
				s  = sqrtf (1.0f + t [0] - t [5] - t [10]) * 2.0f;
				quat.x = 0.25f * s;
				quat.y = (t [1] + t [4]) / s;
				quat.z = (t [8] + t [2]) / s;
				quat.w = (t [6] - t [9]) / s;
			}
			else if (t [5] > t [10])
			{
				s  = sqrtf (1.0f + t [5] - t [0] - t [10]) * 2.0f;
				quat.x = (t [1] + t[4]) / s;
				quat.y = 0.25f * s;
				quat.z = (t[6] + t[9]) / s;
				quat.w = (t[8] - t[2]) / s;
			}
			else
			{
				s  = sqrtf (1.0f + t [10] - t [0] - t [5]) * 2.0f;
				quat.x = (t [8] + t [2]) / s;
				quat.y = (t [6] + t [9]) / s;
				quat.z = 0.25f * s;
				quat.w = (t [1] - t [4]) / s;
			}
		}

		return quat;
	}

	Pos3_s Mat4_GetTranslation_Pos3( const Mat4_s& m )
	{
		return Pos3_s { m.r[3].x, m.r[3].y, m.r[3].z };
	}
}

//======================================================================================
//																				 Ellipse
//======================================================================================
namespace nemesis
{
	bool Ellipse_IsPointOutside( float a, float b, float x, float y )
	{
		const float x_over_a = (x*x) / (a*a);
		const float y_over_b = (y*y) / (b*b);
		const float coord = x_over_a + y_over_b;
		return (coord > 1.0f);
	}

	Vec2_s Ellipse_ClosestPoint( float a, float b, float x, float y )
	{
		// https://en.wikipedia.org/wiki/Newton%27s_method

		NeAssert(a > 0.0f);
		NeAssert(b > 0.0f);

		const int max_iter = 100;

		const float x_switch = Flt_Select( x, 1.0f, -1.0f );
		const float y_switch = Flt_Select( y, 1.0f, -1.0f );

		const float a_over_b = a/b;
		const float b_over_a = b/a;
		const float aa_over_bb = a_over_b * a_over_b;
		const float bb_over_aa = b_over_a * b_over_a;

		const float x_clamped = NeMin(x, a);
		const float y_clamped = NeMin(y, b);
		const float norm = sqrtf( (x_clamped * x_clamped) / (a * a) + (y_clamped * y_clamped) / (b * b) );

		float cx = x_clamped / norm;
		float cy = y_clamped / norm;

		float cos_theta = cx / a;
		float t2 = (1.0f - cos_theta) / (1.0f + cos_theta);
		float t = sqrtf( Flt_Select( t2, t2, 0.0f ) );

		int iter = 0;
		float delta_t;
		float sin_theta;
		do 
		{
			++iter;

			const float x_sub_cx = x-cx;
			const float y_sub_cy = y-cy;
			const float d0 = 1.0f + cx / a;
			const float d1 = -cy / b;
			const float f0 = 2.0f * (a_over_b * cy - b_over_a * y_sub_cy * cx);
			const float f1 = 2.0f * (bb_over_aa * cx * cx + aa_over_bb * cy * cy + x_sub_cx * cx + y_sub_cy * cy);
			const float f2 = f1 * d0 + f0 * d1;

			delta_t = f0 / f2;
			NeAssert(isfinite(delta_t));

			t -= delta_t;
			t = NeClamp(t, 0.0f, 1.0f);

			const float tt = t*t;
			const float tt_plus_1 = 1.0f + tt;

			cos_theta = (1.0f - tt) / tt_plus_1;
			sin_theta =  2.0f * t   / tt_plus_1;

			cx = a * cos_theta;
			cy = b * sin_theta;
			NeAssert(isfinite(cx));
			NeAssert(isfinite(cy));
		}
		while ((fabsf(delta_t) > 1e-7f) && (iter < max_iter));

		return Vec2_s { cx * x_switch, cy * y_switch };
	}
}

