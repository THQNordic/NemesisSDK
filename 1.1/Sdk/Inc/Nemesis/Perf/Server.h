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
#include <Nemesis/Core/AllocTypes.h>

//======================================================================================
namespace nemesis { namespace profiling
{
	struct ScopeType
	{
		enum Enum
		{ Regular
		, Idle
		, Lock
		};
	};

	struct ServerStat
	{
		enum Enum
		{ NumThreads
		, MaxThreads
		, NumNames
		, NumScopes
		, NumLocks
		, BacklogSize
		, BacklogCapacity
		, COUNT			 
		};
	};

	struct ServerStats_s
	{
		uint32_t Value[ ServerStat::COUNT ];
	};

	struct Consumer_s
	{
		void (NE_CALLBK *Consume)( void* context, const uint8_t* data, uint32_t size );
		void* Context;
	};

	typedef struct Server_s* Server_t;

	Result_t Server_Create			( Allocator_t alloc, Server_t* server );
	void	 Server_Release			( Server_t server );
	void	 Server_NextFrame		( Server_t server );
	void	 Server_GetStats		( Server_t server, ServerStats_s& stats );
	void	 Server_SetConsumer		( Server_t server, const Consumer_s& consumer );
	void	 Server_SetThreadInfo	( Server_t server, const char* name );
	void	 Server_EnterScopeEx	( Server_t server, const NamedLocation& scope, ScopeType::Enum type, int64_t tick );
	void	 Server_LeaveScopeEx	( Server_t server, const NamedLocation& scope, int64_t tick );
	void	 Server_EnterScope		( Server_t server, const NamedLocation& scope, ScopeType::Enum type );
	void	 Server_LeaveScope		( Server_t server, const NamedLocation& scope );
	void	 Server_SetMutexInfo	( Server_t server, const void* handle, const char* name );
	void	 Server_EnterMutex		( Server_t server, const void* handle );
	void	 Server_LeaveMutex		( Server_t server, const void* handle );
	void	 Server_RecordValue		( Server_t server, const char* name, uint32_t v );
	void	 Server_RecordValue		( Server_t server, const char* name, float v );
	void	 Server_RecordValue		( Server_t server, const char* path, const char* name, uint32_t v );
	void	 Server_RecordValue		( Server_t server, const char* path, const char* name, float v );
	void	 Server_RecordLog		( Server_t server, const NamedLocation& scope, const char* text );
	void	 Server_StartSender		( Server_t server, uint16_t port );
	void	 Server_StopSender		( Server_t server );

	Result_t Server_Initialize		( Allocator_t alloc );
	void	 Server_Shutdown		();
	void	 Server_NextFrame		();
	void	 Server_GetStats		( ServerStats_s& stats );
	void	 Server_SetConsumer		( const Consumer_s& consumer );
	void	 Server_SetThreadInfo	( const char* name );
	void	 Server_EnterScopeEx	( const NamedLocation& scope, ScopeType::Enum type, int64_t tick );
	void	 Server_LeaveScopeEx	( const NamedLocation& scope, int64_t tick );
	void	 Server_EnterScope		( const NamedLocation& scope, ScopeType::Enum type );
	void	 Server_LeaveScope		( const NamedLocation& scope );
	void	 Server_SetMutexInfo	( const void* handle, const char* name );
	void	 Server_EnterMutex		( const void* handle );
	void	 Server_LeaveMutex		( const void* handle );
	void	 Server_RecordValue		( const char* name, uint32_t v );
	void	 Server_RecordValue		( const char* name, float v );
	void	 Server_RecordValue		( const char* path, const char* name, uint32_t v );
	void	 Server_RecordValue		( const char* path, const char* name, float v );
	void	 Server_RecordLog		( const NamedLocation& scope, const char* text );
	void	 Server_StartSender		( uint16_t port );
	void	 Server_StopSender		();

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct Scope_s : public NamedLocation
	{
		Scope_s( const char* function, const char* file, unsigned long line, const char* name, ScopeType::Enum type )
			: NamedLocation(name, function, file, line)
		{ Server_EnterScope(*this, type); }

		Scope_s( const char* function, const char* file, unsigned long line, const char* name )
			: NamedLocation(name, function, file, line)
		{ Server_EnterScope(*this, ScopeType::Regular); }

		~Scope_s()
		{ Server_LeaveScope(*this); }
	};

} }

//======================================================================================
#ifndef NE_ENABLE_PROFILING
#	define NE_ENABLE_PROFILING		(NE_CONFIG <= NE_CONFIG_PROFILE)
#endif

//======================================================================================
#if NE_ENABLE_PROFILING
#	define NePerfInit							::nemesis::profiling::Server_Initialize
#	define NePerfShutdown						::nemesis::profiling::Server_Shutdown
#	define NePerfNextFrame						::nemesis::profiling::Server_NextFrame
#	define NePerfEnterEx( scope, type, tick )	::nemesis::profiling::Server_EnterScopeEx( scope, type, tick )
#	define NePerfLeaveEx( scope, tick )			::nemesis::profiling::Server_LeaveScopeEx( scope, tick )
#	define NePerfEnter( scope, type )			::nemesis::profiling::Server_EnterScope( scope, type )
#	define NePerfLeave( scope )					::nemesis::profiling::Server_LeaveScope( scope )
#	define NePerfThread( name )					::nemesis::profiling::Server_SetThreadInfo( name )
#	define NePerfMutex( handle, name )			::nemesis::profiling::Server_SetMutexInfo( handle, name )
#	define NePerfLock( handle )					::nemesis::profiling::Server_EnterMutex( handle )
#	define NePerfUnlock( handle )				::nemesis::profiling::Server_LeaveMutex( handle )
#	define NePerfCounter( ... )					::nemesis::profiling::Server_RecordValue( __VA_ARGS__ )
#	define NePerfLog( text )					::nemesis::profiling::Server_RecordLog( NamedLocation( __FUNCTION__, __FUNCTION__, __FILE__, __LINE__ ), text )
#	define NePerfStartSender( ... )				::nemesis::profiling::Server_StartSender( __VA_ARGS__ )
#	define NePerfStopSender						::nemesis::profiling::Server_StopSender
#	define NePerfScope( ... )					::nemesis::profiling::Scope_s NeUnique(scope)( __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#else
#	define NePerfInit(...)						//__noop
#	define NePerfShutdown(...)					//__noop
#	define NePerfNextFrame(...)					//__noop
#	define NePerfEnterEx( scope, type, tick )	//__noop( scope, type )
#	define NePerfLeaveEx( scope, tick )			//__noop( scope )
#	define NePerfEnter( scope, type )			//__noop( scope, type )
#	define NePerfLeave( scope )					//__noop( scope )
#	define NePerfThread( name )					//__noop( name )
#	define NePerfMutex( handle, name )			//__noop( handle, name )
#	define NePerfLock( handle )					//__noop( handle )
#	define NePerfUnlock( handle )				//__noop( handle )
#	define NePerfCounter( ... )					//__noop( __VA_ARGS__ )
#	define NePerfLog( text )					//__noop( text )
#	define NePerfStartSender( ... )				//__noop( __VA_ARGS__ )
#	define NePerfStopSender						//__noop
#	define NePerfScope( ... )					//__noop( __VA_ARGS__ )
#endif

#define NePerfFunc NePerfScope( __FUNCTION__ )
