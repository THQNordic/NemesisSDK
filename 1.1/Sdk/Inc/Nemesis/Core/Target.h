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
//                                      macros
//======================================================================================
#define NeStringifyPlain(a)	#a
#define NeStringify(a)		NeStringifyPlain(a)

#define NeConcatPlain(a,b)	a ## b
#define NeConcat(a,b)		NeConcatPlain(a, b)

#define NeConcatStr(x,y)	x y
#define NeConcatStr3(x,y,z) x y z

//======================================================================================
//                                      compiler
//======================================================================================
#define NE_COMPILER_MSVC	1
#define NE_COMPILER_SNC		2
#define NE_COMPILER_CLANG	3

//--------------------------------------------------------------------------------------
#if defined _MSC_VER
#	define NE_COMPILER			NE_COMPILER_MSVC
#	define NE_COMPILER_VERSION	_MSC_VER
#	define NE_COMPILER_NAME		"Microsoft Visual C++"
#elif defined __SNC__
#	define NE_COMPILER			NE_COMPILER_SNC
#	define NE_COMPILER_VERSION	__SN_VER__
#	define NE_COMPILER_NAME		"SNC"
#elif defined __clang__
#	define NE_COMPILER			NE_COMPILER_CLANG
#	define NE_COMPILER_VERSION	__clang_version__
#	define NE_COMPILER_NAME		"LLVM C/C++"
#else
#	error Unrecognized compiler...
#endif

//======================================================================================
#define NE_COMPILER_IS_C11	(!((NE_COMPILER == NE_COMPILER_SNC) || ((NE_COMPILER == NE_COMPILER_MSVC) && (_MSC_VER < 1600))))

//======================================================================================
//                                    configuration
//======================================================================================
#define NE_CONFIG_DEBUG		1
#define NE_CONFIG_RELEASE	2
#define NE_CONFIG_PROFILE	3
#define NE_CONFIG_MASTER	4

//--------------------------------------------------------------------------------------
#if defined _DEBUG
#	define NE_CONFIG		NE_CONFIG_DEBUG
#	define NE_CONFIG_NAME	"Debug"
#elif defined _MASTER
#	define NE_CONFIG		NE_CONFIG_MASTER
#	define NE_CONFIG_NAME	"Master"
#elif defined _PROFILE
#	define NE_CONFIG		NE_CONFIG_PROFILE
#	define NE_CONFIG_NAME	"Profile"
#else
#	define NE_CONFIG		NE_CONFIG_RELEASE
#	define NE_CONFIG_NAME	"Release"
#endif

//======================================================================================
//										platform
//======================================================================================
#define NE_PLATFORM_WIN32		1
#define NE_PLATFORM_WIN64		2
#define NE_PLATFORM_X360		3
#define NE_PLATFORM_DURANGO		4
#define NE_PLATFORM_SCARLETT	5
#define NE_PLATFORM_PS3			6
#define NE_PLATFORM_PS4			7
#define NE_PLATFORM_PS5			8
#define NE_PLATFORM_NX32		9
#define NE_PLATFORM_NX64		10

//--------------------------------------------------------------------------------------
#define NE_PROCESSOR_X86	1
#define NE_PROCESSOR_X64	2

//--------------------------------------------------------------------------------------
#define NE_ENDIAN_INTEL		1
#define NE_ENDIAN_MOTOROLA	2
#define NE_ENDIAN_LITTLE	NE_ENDIAN_INTEL
#define NE_ENDIAN_BIG		NE_ENDIAN_MOTOROLA

//======================================================================================
#if (defined _XBOX || defined XBOX)
	// Microsoft Xbox 360 - Codename Xenon
	// XDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_MOTOROLA
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_X360
#	define NE_PLATFORM_NAME		"Microsoft XBox 360"
#elif (defined _DURANGO || defined DURANGO)
	// Microsoft XBox One - Codename Durango
	// XDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_DURANGO
#	define NE_PLATFORM_NAME		"Microsoft XBox (Durango)"
#elif (defined _GAMING_XBOX_XBOXONE)
	// Microsoft XBox One - Codename Durango
	// GDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_DURANGO
#	define NE_PLATFORM_NAME		"Microsoft XBox (Durango) [GDK]"
#elif (defined _GAMING_XBOX_SCARLETT)
	// Microsoft XBox One X - Codename Scarlett
	// GDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_SCARLETT
#	define NE_PLATFORM_NAME		"Microsoft XBox (Scarlett)"
#elif (defined _GAMING_DESKTOP)
	// Microsoft Gaming Desktop App - 64 bit
	// GDX Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_WIN64
#	define NE_PLATFORM_NAME		"Microsoft Windows (64bit)"
#elif (defined _WIN64 || defined WIN64)
	// Microsoft Windows Desktop App - 64 bit
	// Windows SDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_WIN64
#	define NE_PLATFORM_NAME		"Microsoft Windows (64bit)"
#elif defined _WIN32
	// Microsoft Windows Desktop App - 32 bit
	// Windows SDK Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X86
#	define NE_PLATFORM			NE_PLATFORM_WIN32
#	define NE_PLATFORM_NAME		"Microsoft Windows (32bit)"
#elif (defined _PS3 || defined __SNC__)
	// Sony PlayStation 3
	// GCC Toolchain
#	define NE_ENDIAN			NE_ENDIAN_MOTOROLA
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_PS3
#	define NE_PLATFORM_NAME		"Sony PlayStation 3"
#elif (defined _PS4 || defined __ORBIS__)
	// Sony PlayStation 4 - Codename Orbis
	// Clang Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_PS4
#	define NE_PLATFORM_NAME		"Sony PlayStation 4"
#elif (defined _PS5 || defined __PROSPERO__)
	// Sony PlayStation 5 - Codename Prospero
	// Clang Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_PS5
#	define NE_PLATFORM_NAME		"Sony PlayStation 5"
#elif defined _NX32
	// Nintendo Switch - 32 bit
	// Clang Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X86
#	define NE_PLATFORM			NE_PLATFORM_NX32
#	define NE_PLATFORM_NAME		"Nintendo Switch (32bit)"
#elif defined _NX64
	// Nintendo Switch - 64 bit
	// Clang Toolchain
#	define NE_ENDIAN			NE_ENDIAN_INTEL
#	define NE_PROCESSOR			NE_PROCESSOR_X64
#	define NE_PLATFORM			NE_PLATFORM_NX64
#	define NE_PLATFORM_NAME		"Nintendo Switch (64bit)"
#else
#	error Unrecognized platform...
#endif

//======================================================================================
#define NE_PLATFORM_IS_DESKTOP	 ((NE_PLATFORM == NE_PLATFORM_WIN32) || (NE_PLATFORM == NE_PLATFORM_WIN64))
#define NE_PLATFORM_IS_WINDOWS	 ((NE_PLATFORM == NE_PLATFORM_WIN32) || (NE_PLATFORM == NE_PLATFORM_WIN64) || (NE_PLATFORM == NE_PLATFORM_DURANGO) || (NE_PLATFORM == NE_PLATFORM_SCARLETT))
#define NE_PLATFORM_IS_MICROSOFT ((NE_PLATFORM_IS_WINDOWS) || (NE_PLATFORM == NE_PLATFORM_X360))
#define NE_PLATFORM_IS_CONSOLE	 (!(NE_PLATFORM_IS_DESKTOP))
#define NE_PLATFORM_IS_NX		 ((NE_PLATFORM == NE_PLATFORM_NX32) || (NE_PLATFORM == NE_PLATFORM_NX64))

//======================================================================================
//							compiler/platform specifics
//======================================================================================

//
// ---	warning / error generation
//
#if (NE_COMPILER == NE_COMPILER_MSVC)
#	pragma warning ( disable : 4100 ) // 'name' : unreferenced formal parameter
#	pragma warning ( disable : 4127 ) // conditional expression is constant
#	pragma warning ( disable : 4189 ) // 'name' : unreferenced local var
#	pragma warning ( disable : 4200 ) // nonstandard extension used : zero-sized array in struct/union
#	pragma warning ( disable : 4201 ) // nonstandard extension used : nameless struct/union
#	pragma warning ( disable : 4505 ) // 'name' : unreferenced local function has been removed
#endif

#if (NE_COMPILER == NE_COMPILER_CLANG)
#	pragma clang diagnostic ignored "-Wunused-variable"
#	pragma clang diagnostic ignored "-Wunused-function"
#	pragma clang diagnostic ignored "-Wmissing-braces"
#endif

//
// ---	calling conventions
//
#if (NE_COMPILER == NE_COMPILER_MSVC)
#	define NE_API		__cdecl
#	define NE_CALLBK	__cdecl
#else
#	define NE_API
#	define NE_CALLBK
#endif

//
// ---	c++11 built-ins
//
#if (NE_COMPILER_IS_C11)
#	define NeStaticAssert(expr) static_assert(expr, #expr)
#else
#	define nullptr 0l

	template < bool expression > 
	struct NeCompileTimeAssert;

	template <> 
	struct NeCompileTimeAssert<true> 
	{};

#	define	NeStaticAssert(expr) NeCompileTimeAssert<(bool)(expr)> NeUnique(static_assert)
#endif

//
// ---	debug break
//
#if (NE_COMPILER == NE_COMPILER_SNC)
	extern void snPause();
#	define NeDebugBreak()	snPause()
#elif (NE_COMPILER == NE_COMPILER_CLANG)
#	define NeDebugBreak()	__builtin_trap()
#else
#	define NeDebugBreak()	__debugbreak()
#endif

//
//	--- compiler
//
#if (NE_COMPILER == NE_COMPILER_MSVC)
#	define NE_THREAD_LOCAL		__declspec(thread) 
#	define NE_NO_INLINE			__declspec(noinline)
#	define NeAlign(x)			__declspec(align(x))
#	define NeAlignOf(T)			__alignof(T)
#else
#	define NE_THREAD_LOCAL		__thread
#	define NE_NO_INLINE			__attribute__((noinline))
#	define NeAlign(x)			__attribute__((align_value(x)))
#	define NeAlignOf(T)			__alignof(T)
#endif

//
//	--- linker
//
#if (NE_COMPILER == NE_COMPILER_MSVC)
#	define NE_LINK(X) __pragma( comment( lib, X ) )
#else
#	define NE_LINK(X) _Pragma( NeStringify( comment( lib, X ) ) )
#endif
