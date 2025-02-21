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
namespace nemesis
{
	static const float FLT_SMALL  = 1e-4f;
	static const float FLT_TINY   = 1e-6f;
	static const float FLT_TOL    = 1e-6f;
	static const float FLT_TOL_SQ = FLT_TOL * FLT_TOL;

	static const uint32_t Discrete_16 = 0xffff;
	static const uint32_t Discrete_11 = 0x7ff;
	static const uint32_t Discrete_10 = 0x3ff;
}

//======================================================================================
//																				   Float
//======================================================================================
namespace nemesis
{
	float	NE_API Flt_Asin		( float v );
	float	NE_API Flt_Acos		( float v );
	float	NE_API Flt_AcosFast	( float v );
	float	NE_API Flt_Square	( float x );
	float	NE_API Flt_Fract	( float a );
	float	NE_API Flt_Step		( float a, float x );
	float	NE_API Flt_Nice		( float v ) ;
	float	NE_API Flt_Nice		( float x, bool round );
	float	NE_API Flt_Dist		( float s, float d );
	float	NE_API Flt_Lerp		( float s, float d, float t );
	float	NE_API Flt_Lerp		( const float* v, int count, int s, float t );
	float	NE_API Flt_Smooth	( float s, float d, float x );
	float	NE_API Flt_Rerange	( float x, float in_min, float in_max, float out_min, float out_max );
	float   NE_API Flt_Mad		( float v, float scale, float mean );
	float   NE_API Flt_Smu		( float v, float mean, float scale );
	void	NE_API Flt_MinMax	( const float* item, int count, float& min, float& max );
	float	NE_API Flt_Sqrt		( float v, float def );
	float	NE_API Flt_Div		( float a, float b, float def );
	float	NE_API Flt_Mod		( float a, float b, float def );
	float	NE_API Flt_Select	( float cmp, float ge, float lt );
	float	NE_API Flt_Wrap		( float v, float min, float max );
	Vec2_s	NE_API Flt_SolveQuad( float a, float b, float c );
	Vec2_s	NE_API Flt_ThaSinCos( float tha );
	bool	NE_API Flt_IsTiny	( float v );
	bool	NE_API Flt_Eq		( float lhs, float rhs );
	bool	NE_API Flt_Eq		( float lhs, float rhs, float tolerance );
	float	NE_API Flt_NormRad	( float v );
	float	NE_API Flt_Pow2		( float v );
	float	NE_API Flt_Pow3		( float v );
	float	NE_API Flt_Pow5		( float v );
	void	NE_API Flt_SinCos	( float v, float& s, float& c );

}

//======================================================================================
//																				    Vec2
//======================================================================================
namespace nemesis
{
	Vec2_s	NE_API Vec2_Zero	();
	Vec2_s	NE_API Vec2_Unit	();
	Vec2_s	NE_API Vec2_UnitX	();
	Vec2_s	NE_API Vec2_UnitY	();
	float	NE_API Vec2_LenSq	( const Vec2_s& a );
	float	NE_API Vec2_Len		( const Vec2_s& a );
	Vec2_s	NE_API Vec2_Norm	( const Vec2_s& a );
	float	NE_API Vec2_Dot		( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Neg		( const Vec2_s& a );
	Vec2_s	NE_API Vec2_Floor	( const Vec2_s& a );
	float	NE_API Vec2_Cross	( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Min		( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Max		( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Add		( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Sub		( const Vec2_s& a, const Vec2_s& b );
	Vec2_s	NE_API Vec2_Mul		( const Vec2_s& a, float s );
	Vec2_s	NE_API Vec2_Dir		( const Vec2_s& s, const Vec2_s& d );
	float	NE_API Vec2_InvLen	( const Vec2_s& a, float fail );
}

//======================================================================================
//																				    Vec3
//======================================================================================
namespace nemesis
{
	float	NE_API Vec3_LenSq	( const Vec3_s& a );
	float	NE_API Vec3_Len		( const Vec3_s& a );
	float	NE_API Vec3_Dot		( const Vec3_s& a, const Vec3_s& b );
	Vec3_s	NE_API Vec3_Neg		( const Vec3_s& a );
	Vec3_s	NE_API Vec3_Add		( const Vec3_s& a, const Vec3_s& b );
	Vec3_s	NE_API Vec3_Sub		( const Vec3_s& a, const Vec3_s& b );
	Vec3_s	NE_API Vec3_Mul		( const Vec3_s& a, const Vec3_s& b );
	Vec3_s	NE_API Vec3_Mul		( const Vec3_s& a, float s );
}

//======================================================================================
//																				    Pos3
//======================================================================================
namespace nemesis
{
	Pos3_s	NE_API Pos3_Zero			();
	Pos3_s	NE_API Pos3_Unit			();
	Pos3_s	NE_API Pos3_UnitX			();
	Pos3_s	NE_API Pos3_UnitY			();
	Pos3_s	NE_API Pos3_UnitZ			();
	float	NE_API Pos3_LenSq			( const Pos3_s& a );
	float	NE_API Pos3_Len				( const Pos3_s& a );
	Pos3_s	NE_API Pos3_Norm			( const Pos3_s& a );
	Pos3_s	NE_API Pos3_Neg				( const Pos3_s& a );
	float	NE_API Pos3_Dot				( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_Add				( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_Sub				( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_Mul				( const Pos3_s& a, float s );
	Pos3_s	NE_API Pos3_Mul				( const Pos3_s& v, const Quat_s& q );
	Pos3_s	NE_API Pos3_Div				( const Pos3_s& v, float s, const Pos3_s& def );
	Pos3_s	NE_API Pos3_Cross			( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_Ortho			( const Pos3_s& v );
	void	NE_API Pos3_Add				( const Pos3_s* a, const Pos3_s* b, int count, Pos3_s* out );
	Pos3_s	NE_API Pos3_Mad				( const Pos3_s& v, const Pos3_s& scale, const Pos3_s& mean );
	Pos3_s	NE_API Pos3_Smu				( const Pos3_s& v, const Pos3_s& mean, const Pos3_s& scale );
	Pos3_s	NE_API Pos3_Lerp			( const Pos3_s& s, const Pos3_s& d, float t );
	Pos3_s	NE_API Pos3_Lerp			( const Pos3_s* v, int count, int s, float t );
	void	NE_API Pos3_Lerp			( const Pos3_s* a, const Pos3_s* b, int count, float t, Pos3_s* out );
	void	NE_API Pos3_Lerp			( const Pos3_s* a, const Pos3_s* b, const float* t, int count, Pos3_s* out );
	Pos3_s	NE_API Pos3_MinEach			( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_MaxEach			( const Pos3_s& a, const Pos3_s& b );
	void	NE_API Pos3_MinMaxEach		( const Pos3_s* item, int count, Pos3_s& min, Pos3_s& max );
	float	NE_API Pos3_Angle			( const Pos3_s& from, const Pos3_s& to );
	float	NE_API Pos3_Dist			( const Pos3_s& from, const Pos3_s& to );
	Pos3_s	NE_API Pos3_Project			( const Pos3_s& pos, const Pos3_s& dir );
	bool	NE_API Pos3_IsNorm			( const Pos3_s& v, float tolerance );
	bool	NE_API Pos3_IsNorm			( const Pos3_s& v );
	bool	NE_API Pos3_IsZeroLen		( const Pos3_s& v, float tolerance );
	bool	NE_API Pos3_IsZeroLen		( const Pos3_s& v );
	bool	NE_API Pos3_IsOrtho			( const Pos3_s& a, const Pos3_s& b, float tolerance );
	bool	NE_API Pos3_IsOrtho			( const Pos3_s& a, const Pos3_s& b );
	Pos3_s	NE_API Pos3_TransformCoord	( const Pos3_s& v, const Mat4_s& m );
	Pos3_s	NE_API Pos3_TransformNormal	( const Pos3_s& v, const Mat4_s& m );
	Pos3_s	NE_API Pos3_MirrorX			( const Pos3_s& v );
	Pos3_s	NE_API Pos3_MirrorY			( const Pos3_s& v );
	Pos3_s	NE_API Pos3_MirrorZ			( const Pos3_s& v );
	void	NE_API Pos3_Basis			( const Pos3_s& e0, Pos3_s* e1, Pos3_s* e2 );
}

//======================================================================================
//																				    Quat
//======================================================================================
namespace nemesis
{
	Quat_s	NE_API Quat_Id						();
	bool	NE_API Quat_IsNorm					( const Quat_s& q );
	bool	NE_API Quat_IsNorm					( const Quat_s& q, float tol_sq );
	float	NE_API Quat_LenSq					( const Quat_s& q );
	float	NE_API Quat_Len						( const Quat_s& q );
	float	NE_API Quat_Dot						( const Quat_s& a, const Quat_s& b );
	Pos3_s	NE_API Quat_Vec						( const Quat_s& q );
	Quat_s	NE_API Quat_Norm					( const Quat_s& q );
	Quat_s	NE_API Quat_Inv						( const Quat_s& q );
	Quat_s	NE_API Quat_Neg						( const Quat_s& q );
	Quat_s	NE_API Quat_Sel						( const Quat_s& a, float sel );
	Quat_s	NE_API Quat_Add						( const Quat_s& a, const Quat_s& b );
	void	NE_API Quat_Add						( const Quat_s* a, const Quat_s* b, int count, Quat_s* out );
	Quat_s	NE_API Quat_Sub						( const Quat_s& a, const Quat_s& b );
	Quat_s	NE_API Quat_Mul						( const Quat_s& a, float b );
	Quat_s	NE_API Quat_Mul						( const Quat_s& a, const Quat_s& b );
	Pos3_s	NE_API Quat_Mul						( const Quat_s& a, const Pos3_s& b );
	Pos3_s	NE_API Quat_InvMul					( const Quat_s& a, const Pos3_s& b );
	Quat_s	NE_API Quat_Lerp					( const Quat_s& s, const Quat_s& d, float t );
	Quat_s	NE_API Quat_SlerpPrecise			( const Quat_s& s, const Quat_s& d, float t, float dot );
	Quat_s	NE_API Quat_SlerpPrecise			( const Quat_s& s, const Quat_s& d, float t );
	Quat_s	NE_API Quat_SlerpFast				( const Quat_s& s, const Quat_s& d, float t, float dot );
	Quat_s	NE_API Quat_SlerpFast				( const Quat_s& s, const Quat_s& d, float t );
	Quat_s	NE_API Quat_Slerp					( const Quat_s& s, const Quat_s& d, float t );
	Quat_s	NE_API Quat_Slerp					( const Quat_s* q, int count, int s, float t );
	void	NE_API Quat_Slerp					( const Quat_s* a, const Quat_s* b, int count, float t, Quat_s* out );
	void	NE_API Quat_Slerp					( const Quat_s* a, const Quat_s* b, const float* t, int count, Quat_s* out );
	Quat_s	NE_API Quat_FromTha					( const Pos3_s& v );
	Quat_s	NE_API Quat_FromTqa					( const Pos3_s& v );
	Pos3_s	NE_API Quat_ToTqa					( const Quat_s& q );
	void	NE_API Quat_ToTqa					( const Quat_s* src, int count, Pos3_s* dst );
	Quat_s	NE_API Quat_FromVec					( const Pos3_s& v );
	Pos3_s	NE_API Quat_ToVec					( const Quat_s& q );
	Quat_s	NE_API Quat_FromTo					( const Pos3_s& from, const Pos3_s& to );
	Quat_s	NE_API Quat_FromTo					( const Quat_s& from, const Quat_s& to );
	Quat_s	NE_API Quat_FromAxisAngle			( const Pos3_s& axis, float angle );
	Quat_s	NE_API Quat_FromEulerAngles			( const Pos3_s& euler );
	Quat_s	NE_API Quat_FromEulerAngles_Deg		( const Pos3_s& euler );
	Quat_s	NE_API Quat_FromEulerAngles			( float x, float y, float z );
	Quat_s	NE_API Quat_FromEulerAngles_Deg		( float x, float y, float z );
	Quat_s	NE_API Quat_FromTwistSwing			( float t, float s1, float s2 );
	Quat_s	NE_API Quat_FromTwistSwing			( const Pos3_s& v );
	Pos3_s	NE_API Quat_ToTwistSwing			( const Quat_s& q );
	void	NE_API Quat_ToSwingTwist			( const Quat_s& q, const Pos3_s& axis, Quat_s& swing, Quat_s& twist );
	Pos3_s	NE_API Quat_ToEulerAngles			( const Quat_s& q );
	Pos3_s	NE_API Quat_ToEulerAngles_Deg		( const Quat_s& q );
	Pos3_s	NE_API Quat_ToAxisAngle				( const Quat_s& q );
	float	NE_API Quat_ToAngle					( const Quat_s& q );
	Quat_s	NE_API Quat_Select					( const Quat_s& q, float cmp );
	Pos3_s  NE_API Quat_Rotate					( const Quat_s& q, const Pos3_s& v );
	Pos3_s  NE_API Quat_InvRotate				( const Quat_s& q, const Pos3_s& v );
	Quat_s	NE_API Quat_ForRotation				( const Pos3_s& v1, const Pos3_s& v2 );
	Quat_s	NE_API Quat_ForRotationAxis			( const Pos3_s& v1, const Pos3_s& v2, const Pos3_s& axis );
	Quat_s	NE_API Quat_ForRotationAxis			( const Pos3_s& v1, const Pos3_s& v2, const Pos3_s& axis, float* condition );
	Quat_s	NE_API Quat_ClosestRotationAxis		( const Quat_s& q, const Pos3_s& axis );
	Quat_s	NE_API Quat_ClosestRotationAxis		( const Quat_s& q, const Pos3_s& axis, float* condition );
	Quat_s	NE_API Quat_ClosestRotationAxisSoft	( const Quat_s& q, const Pos3_s& axis, float softness );
	Quat_s	NE_API Quat_MirrorX					( const Quat_s& v );
	Quat_s	NE_API Quat_MirrorY					( const Quat_s& v );
	Quat_s	NE_API Quat_MirrorZ					( const Quat_s& v );
	Quat_s	NE_API Quat_Fade					( const Quat_s& q0, const Quat_s& q1, float weight );
	Quat_s	NE_API Quat_Scale					( const Quat_s& q, float dt );
	Pos3_s	NE_API Quat_GetAxisX				( const Quat_s& q );
	Pos3_s	NE_API Quat_GetAxisY				( const Quat_s& q );
	Pos3_s	NE_API Quat_GetAxisZ				( const Quat_s& q );
	Quat_s	NE_API Quat_GetSwing				( const Quat_s& q );
	float	NE_API Quat_GetTwistAngle			( const Quat_s& q, const Pos3_s& axis );
	float	NE_API Quat_Dist					( const Quat_s& a, const Quat_s& b );
	Quat_s	NE_API Quat_Offset					( const Quat_s& a, const Quat_s& b );					// r = a +		   sign_correct(b)
	Quat_s	NE_API Quat_Offset					( const Quat_s& q, const Quat_s& add, float scale );	// r = a + scale * sign_correct(b)
	float	NE_API Quat_CosHalfTheta			( const Quat_s& q );
	float	NE_API Quat_SinHalfTheta			( const Quat_s& q );
	float	NE_API Quat_CosTheta				( const Quat_s& q );
	Pos3_s	NE_API Quat_Log						( const Quat_s& q );
	Quat_s	NE_API Quat_Exp						( const Pos3_s& v );
	float	NE_API Quat_SignedAngleY			( const Quat_s& q );
}

//======================================================================================
//																				   XForm
//======================================================================================
namespace nemesis
{
	XForm_s NE_API XForm_Id				();
	XForm_s NE_API XForm_Inv			( const XForm_s& v );
	XForm_s NE_API XForm_Mul			( const XForm_s& x, float s );
	XForm_s	NE_API XForm_Mul			( const XForm_s& lhs, const XForm_s& rhs );
	XForm_s	NE_API XForm_Mul			( const XForm_s& lhs, const Pos3_s& p, const Quat_s& q );
	XForm_s NE_API XForm_InvMul			( const XForm_s& lhs, const XForm_s& rhs );
	XForm_s NE_API XForm_Delta			( const XForm_s& prev, const XForm_s& next );
	XForm_s NE_API XForm_Concat			( const XForm_s& prev, const XForm_s& delta );
	XForm_s NE_API XForm_Blend			( const XForm_s& src, const XForm_s& dst, float t );
	Pos3_s  NE_API XForm_Transform		( const XForm_s& a, const Pos3_s& b );
	Pos3_s  NE_API XForm_InvTransform	( const XForm_s& a, const Pos3_s& b );
	XForm_s NE_API XForm_Retransform	( const XForm_s& a, const XForm_s& b );
	XForm_s NE_API XForm_Offset			( const XForm_s& t, const XForm_s& add, float scale );		// r = a + scale * sign_correct(b)
}

//======================================================================================
//																				    Mat4
//======================================================================================
namespace nemesis
{
	Mat4_s	NE_API Mat4_Id					();
	float	NE_API Mat4_Det					( const Mat4_s& m );
	Mat4_s	NE_API Mat4_Transpose			( const Mat4_s& m );
	Mat4_s	NE_API Mat4_Inv					( const Mat4_s& m );
	Mat4_s	NE_API Mat4_Mul					( const Mat4_s& a, const Mat4_s& b );
	Mat4_s	NE_API Mat4_BasisX				( const Pos3_s& v );
	Mat4_s	NE_API Mat4_BasisY				( const Pos3_s& v );
	Mat4_s	NE_API Mat4_BasisZ				( const Pos3_s& v );
	Mat4_s	NE_API Mat4_RotationX			( float angle );
	Mat4_s	NE_API Mat4_RotationY			( float angle );
	Mat4_s	NE_API Mat4_RotationZ			( float angle );
	Mat4_s	NE_API Mat4_RotationAxis		( const Pos3_s& axis, float angle );
	Mat4_s	NE_API Mat4_RotationQuat		( const Quat_s& q );
	Mat4_s	NE_API Mat4_Scale				( float x, float y, float z );
	Mat4_s	NE_API Mat4_Translation			( float x, float y, float z );
	Mat4_s	NE_API Mat4_Transform			( const Pos3_s& p, const Quat_s& q );
	Mat4_s	NE_API Mat4_LookAtLH			( const Pos3_s& eye, const Pos3_s& at, const Pos3_s& up );
	Mat4_s	NE_API Mat4_PerspectiveFovLH	( float fov_y, float w_by_h, float z_near, float z_far );
	Mat4_s	NE_API Mat4_OrthoLH				( float w, float h, float z_near, float z_far );
	Mat4_s	NE_API Mat4_OrthoOffCenterLH	( float l, float r, float b, float t, float z_near, float z_far );
	Quat_s	NE_API Mat4_GetRotation_Quat	( const Mat4_s& m );
	Pos3_s	NE_API Mat4_GetTranslation_Pos3	( const Mat4_s& m );
}

//======================================================================================
//																				    Rect
//======================================================================================
namespace nemesis
{
	Vec2_s	NE_API Rect_Pos			( const Rect_s& r );
	Vec2_s	NE_API Rect_Size		( const Rect_s& r );
	Vec2_s	NE_API Rect_Center		( const Rect_s& r );
	bool	NE_API Rect_Contains	( const Rect_s& r, const Vec2_s& p ); 
	bool	NE_API Rect_Intersects	( const Rect_s& a, const Rect_s& b );
	Rect_s	NE_API Rect_Intersect	( const Rect_s& a, const Rect_s& b );
	Rect_s	NE_API Rect_Offset		( const Rect_s& a, const Vec2_s& b );
	Rect_s	NE_API Rect_Inflate		( const Rect_s& v, float l, float t, float r, float b );
	Rect_s	NE_API Rect_Inflate		( const Rect_s& r, const Vec2_s& size );
	Rect_s	NE_API Rect_Inflate		( const Rect_s& r, float x, float y );
	Rect_s	NE_API Rect_Inflate		( const Rect_s& r, float size );
	Rect_s	NE_API Rect_Margin		( const Rect_s& v, float l, float t, float r, float b );
	Rect_s	NE_API Rect_Margin		( const Rect_s& r, const Vec2_s& margin );
	Rect_s	NE_API Rect_Margin		( const Rect_s& r, float margin );
	void	NE_API Rect_SplitH		( const Rect_s& r, float x, Rect_s* left, Rect_s* right );
	void	NE_API Rect_SplitV		( const Rect_s& r, float y, Rect_s* top, Rect_s* bottom );
	Rect2_s NE_API Rect_SplitT		( const Rect_s& r, float v );
	Rect2_s NE_API Rect_SplitB		( const Rect_s& r, float v );
	Rect2_s NE_API Rect_SplitL		( const Rect_s& r, float v );
	Rect2_s NE_API Rect_SplitR		( const Rect_s& r, float v );
	Rect2_s	NE_API Rect_SplitH		( const Rect_s& r, float x );
	Rect2_s	NE_API Rect_SplitV		( const Rect_s& r, float y );
	Rect_s  NE_API Rect_Ctor		( const Vec2_s& pos, const Vec2_s& size );
	Vec2_s  NE_API Rect_Min			( const Rect_s& r );
	Vec2_s  NE_API Rect_Max			( const Rect_s& r );
}

//======================================================================================
//																				   Color
//======================================================================================
namespace nemesis
{
	uint8_t	 NE_API Color_GetAlpha	( uint32_t argb );
	uint32_t NE_API Color_SetAlpha	( uint32_t argb, uint8_t alpha );
	Color_s	 NE_API Color_FromArgb	( uint32_t argb );
	Color_s	 NE_API Color_FromArgb	( uint32_t argb, float a );
	uint32_t NE_API Color_ToArgb	( const Color_s& c );
	uint32_t NE_API Color_ToArgb	( const Color_s& c, float alpha );
	Color_s	 NE_API Color_Blend		( const Color_c& s, const Color_c& d, float a );
	Color_s	 NE_API Color_Desaturate( const Color_c& c, float s );
	Color_s	 NE_API Color_Brighten	( const Color_c& c, float s );
	Color_s	 NE_API Color_Modulate	( const Color_s& c, float s );
	Color_s	 NE_API Color_Modulate	( const Color_s& c, float rgb, float a );
}

//======================================================================================
//																			Quantization
//======================================================================================
namespace nemesis
{
	uint16_t NE_API Flt_Encode_16( float v );
	void	 NE_API Flt_Encode_16( const float* src, int count, uint16_t* dst, float& scale, float& mean );
	float	 NE_API Flt_Decode_16( uint16_t v );
	float	 NE_API Flt_Decode_16( uint16_t src, float scale, float mean );
	uint32_t NE_API Pos3_Encode3_11_11_10( const Pos3_s& v );
	Pos3_s	 NE_API Pos3_Decode3_11_11_10( uint32_t v );
	void	 NE_API Pos3_Encode3_11_11_10( const Pos3_s* src, int count, uint32_t* dst, Pos3_s& scale, Pos3_s& mean );
	Pos3_s	 NE_API Pos3_Decode3_11_11_10( uint32_t src, const Pos3_s& scale, const Pos3_s& mean );
	void	 NE_API Pos3_Decode3_11_11_10( const uint32_t* src, int count, const Pos3_s& scale, const Pos3_s& mean, Pos3_s* dst );
	void	 NE_API Quat_DecodeTqa3_11_11_10( const uint32_t* src, int count, const Pos3_s& scale, const Pos3_s& mean, Quat_s* dst );
}

//======================================================================================
//																				Blending
//======================================================================================
namespace nemesis
{
	float  NE_API Flt_Blend_Lerp( float src, float dst, float t );
	float  NE_API Flt_Blend_Add( float src, float dst, float t );
	float  NE_API Flt_Blend_Sub( float src, float dst, float t );
	Pos3_s NE_API Pos3_Blend_Lerp( const Pos3_s& src, const Pos3_s& dst, float t );
	Pos3_s NE_API Pos3_Blend_Add( const Pos3_s& src, const Pos3_s& dst, float t );
	Pos3_s NE_API Pos3_Blend_Sub( const Pos3_s& src, const Pos3_s& dst, float t );
	Quat_s NE_API Quat_Blend_Lerp( const Quat_s& src, const Quat_s& dst, float t );
	Quat_s NE_API Quat_Blend_Add( const Quat_s& src, const Quat_s& dst, float t );
	Quat_s NE_API Quat_Blend_Sub( const Quat_s& src, const Quat_s& dst, float t );
}

//======================================================================================
namespace nemesis
{
	template < typename T >
	inline T NeLerp( const T& min, const T& max, float alpha )
	{ return min + alpha * (max-min); }

	template < typename T >
	inline T NeLerpClamped( const T& min, const T& max, float alpha )
	{ return NeLerp( min, max, NeClamp( alpha, 0.0f, 1.0f ) ); }
}

//======================================================================================
//																			   Operators
//======================================================================================
namespace nemesis
{
	inline Vec2_s	operator -	( const Vec2_s& a )						{ return Vec2_Neg( a ); }
	inline Vec2_s	operator +	( const Vec2_s& a, const Vec2_s& b )	{ return Vec2_Add( a, b ); }
	inline Vec2_s	operator -	( const Vec2_s& a, const Vec2_s& b )	{ return Vec2_Sub( a, b ); }
	inline Vec2_s	operator *	( const Vec2_s& a, float s )			{ return Vec2_Mul( a, s ); }
	inline Vec2_s	operator *	( float s, const Vec2_s& a )			{ return Vec2_Mul( a, s ); }

	inline Vec3_s	operator -	( const Vec3_s& a )						{ return Vec3_Neg( a ); }
	inline Vec3_s	operator +	( const Vec3_s& a, const Vec3_s& b )	{ return Vec3_Add( a, b ); }
	inline Vec3_s	operator -	( const Vec3_s& a, const Vec3_s& b )	{ return Vec3_Sub( a, b ); }
	inline Vec3_s	operator *	( const Vec3_s& a, const Vec3_s& b )	{ return Vec3_Mul( a, b ); }
	inline Vec3_s	operator *	( const Vec3_s& a, float s )			{ return Vec3_Mul( a, s ); }
	inline Vec3_s	operator *	( float s, const Vec3_s& a )			{ return Vec3_Mul( a, s ); }

	inline Pos3_s	operator -	( const Pos3_s& a )						{ return Pos3_Neg( a ); }
	inline Pos3_s	operator +	( const Pos3_s& a, const Pos3_s& b )	{ return Pos3_Add( a, b ); }
	inline Pos3_s	operator -	( const Pos3_s& a, const Pos3_s& b )	{ return Pos3_Sub( a, b ); }
	inline Pos3_s	operator *	( const Pos3_s& a, float s )			{ return Pos3_Mul( a, s ); }
	inline Pos3_s	operator *	( float s, const Pos3_s& a )			{ return Pos3_Mul( a, s ); }

	inline Quat_s	operator ~	( const Quat_s& a )						{ return Quat_Inv( a ); }
	inline Quat_s	operator -	( const Quat_s& a )						{ return Quat_Neg( a ); }
	inline Quat_s	operator +	( const Quat_s& a, const Quat_s& b )	{ return Quat_Add( a, b ); }
	inline Quat_s	operator -	( const Quat_s& a, const Quat_s& b )	{ return Quat_Sub( a, b ); }
	inline Quat_s	operator *	( const Quat_s& a, float b )			{ return Quat_Mul( a, b ); }
	inline Quat_s	operator *	( float b, const Quat_s& a )			{ return Quat_Mul( a, b ); }
	inline Quat_s	operator *	( const Quat_s& a, const Quat_s& b )	{ return Quat_Mul( a, b ); }

	inline Pos3_s	operator *	( const Quat_s& a, const Pos3_s& b )	{ return Quat_Mul( a, b ); }
	inline Pos3_s	operator *	( const Pos3_s& b, const Quat_s& a )	{ return Quat_Mul( a, b ); }

	inline XForm_s	operator ~	( const XForm_s& x )					{ return XForm_Inv( x ); }
	inline XForm_s	operator *	( float s, const XForm_s& x )			{ return XForm_Mul( x, s ); }
	inline XForm_s	operator *	( const XForm_s& x, float s )			{ return XForm_Mul( x, s ); }
	inline XForm_s	operator *	( const XForm_s& a, const XForm_s& b )	{ return XForm_Mul( a, b ); }

	inline Mat4_s	operator *	( const Mat4_s& a, const Mat4_s& b )	{ return Mat4_Mul( a, b ); }

	inline Rect_s	operator +	( const Rect_s& a, const Vec2_s& b )	{ return Rect_Offset( a,  b ); }
	inline Rect_s	operator -	( const Rect_s& a, const Vec2_s& b )	{ return Rect_Offset( a, -b ); }
	inline Rect_s	operator +	( const Vec2_s& a, const Rect_s& b )	{ return Rect_Offset( b,  a ); }

	inline Color_s	operator *	( const Color_s& a, const Color_s& b )	{ return Color_c( a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a ); }
	inline Color_s	operator *	( const Color_s& c, float s )			{ return Color_c( c.r * s, c.g * s, c.b * s, c.a ); }
	inline Color_s	operator +	( const Color_s& a, const Color_s& b )	{ return Color_c( a.r + b.r, a.g + b.g, a.b + b.b, 0.5f * (a.a + b.a) ); }
	inline Color_s	operator +	( const Color_s& c, float s )			{ return Color_c( c.r + s, c.g + s, c.b + s, c.a ); }
	inline Color_s	operator -	( const Color_s& c, float s )			{ return Color_c( c.r - s, c.g - s, c.b - s, c.a ); }
	inline Color_s	operator *	( float s, const Color_s& c )			{ return c * s; }
	inline Color_s	operator +	( float s, const Color_s& c )			{ return c + s; }

	inline Color_c::Color_c( uint32_t rgb, float _a )					{ *this = Color_FromArgb( rgb, _a ); }
	inline Color_c::Color_c( uint32_t argb )							{ *this = Color_FromArgb( argb ); }
}

//======================================================================================
//																				FractInt
//======================================================================================
namespace nemesis
{
	FractInt_s	NE_API FractInt_Set		( float v );
	FractInt_s	NE_API FractInt_Add		( const FractInt_s& a, float b );
	FractInt_s	NE_API FractInt_Sub		( const FractInt_s& a, const FractInt_s& b );
	float		NE_API FractInt_ToFloat	( const FractInt_s& v );
	FractInt_s	NE_API FractInt_Norm	( const FractInt_s& v );
	float		NE_API FractInt_Diff	( const FractInt_s& from, const FractInt_s& to );
}

//======================================================================================
//																				 Ellipse
//======================================================================================
namespace nemesis
{
	bool	NE_API Ellipse_IsPointOutside( float a, float b, float x, float y );
	Vec2_s	NE_API Ellipse_ClosestPoint  ( float a, float b, float x, float y );
}

//======================================================================================
#include "Math.inl"
