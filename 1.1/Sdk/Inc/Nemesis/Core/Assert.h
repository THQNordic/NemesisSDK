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
#include "AssertTypes.h"

//======================================================================================
namespace nemesis 
{
	AssertHook_s		NE_API Assert_GetHook();
	void				NE_API Assert_SetHook( const AssertHook_s& hook );
	AssertAction::Enum	NE_API Assert_Failure( const AssertInfo_s& info, cstr_t msg, ... );
}

//======================================================================================
#define NeAssertBreak() \
	NeDebugBreak()

#define NeAssertFailed(expr, ...) \
	( \
		(::nemesis::Assert_Failure( ::nemesis::AssertInfo_s( #expr, __FUNCTION__, __FILE__, __LINE__ ), __VA_ARGS__ )) \
			== ::nemesis::AssertAction::Break \
			? (void)NeAssertBreak() \
			: (void)(0) \
	)

#define NeAssertEval(expr, ...) \
	( \
		(void)												/* strip off the result since we don't use it (clang will emit a warning)*/ \
			(	(expr)										/* either the expression evaluates to 'true' and the assertion holds */ \
			||  (											/* or the assetion failed */ \
				  NeAssertFailed(expr,__VA_ARGS__)			/*		and we call the 'failed assertion handler' */ \
				, false										/*		while evaluating to a boolean */ \
				) \
			) \
	)

#define NeAssertOutImpl(expr, ...) \
	do \
	{ \
		NeAssertEval(expr, __VA_ARGS__); \
		/*__analysis_assume(expr);*/ \
	} \
	while(false)

#define NeAssertOutOnceImpl(expr, ...) \
	do \
	{ \
		static bool once = true; \
		if ( once ) \
		{ \
			const bool result = NeAssertEval(expr, __VA_ARGS__); \
			once &= result; \
		} \
		/*__analysis_assume(expr);*/ \
	} \
	while(false)

//======================================================================================
#if !defined NE_ENABLE_ASSERT
#	define	 NE_ENABLE_ASSERT (NE_CONFIG < NE_CONFIG_MASTER)
#endif

//======================================================================================
#if NE_ENABLE_ASSERT
#	define NeAssert(expr)				NeAssertOutImpl(expr, "")
#	define NeAssertOut(expr, ...)		NeAssertOutImpl(expr, __VA_ARGS__)
#	define NeAssertOutOnce(expr, ...)	NeAssertOutOnceImpl(expr, __VA_ARGS__)
#else
#	define NeAssert(expr)				//__noop(expr)
#	define NeAssertOut(expr, ...)		//__noop(expr, __VA_ARGS__)
#	define NeAssertOutOnce(expr, ...)	//__noop(expr, __VA_ARGS__)
#endif

//======================================================================================
#if NE_ENABLE_ASSERT
#	define NeVerify(expr)			NeAssert(expr)
#	define NeVerifyOut(expr, ...)	NeAssertOut(expr, __VA_ARGS__)
#else
#	define NeVerify(expr)			expr
#	define NeVerifyOut(expr, ...)	expr
#endif

//======================================================================================
#define NeAssertBounds( i, n ) NeAssertOut( (i >= 0) && (i < n), "Index out of bounds: %d. Count: %d.", i, n )
