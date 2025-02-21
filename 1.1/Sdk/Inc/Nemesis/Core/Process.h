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
#include "ProcessTypes.h"
#include "Atomic.h"
#include "Map.h"

//======================================================================================
namespace nemesis {	namespace system
{
	/// Cpu

	Tick_t	NE_API Clock_GetFreq();
	Tick_t	NE_API Clock_GetTick();

	CpuId_t	NE_API Cpu_GetIndex();
	void	NE_API Cpu_Yield();

	/// Thread

	Thread_t	NE_API Thread_Create ( const ThreadSetup_s& setup );
	void		NE_API Thread_Wait   ( Thread_t thread );
	void		NE_API Thread_SleepMs( uint32_t ms );
	ThreadId_t	NE_API Thread_GetId  ();

	/// Thread Local Storage

	TlsId_t	NE_API Tls_Alloc	();
	void	NE_API Tls_Free		( TlsId_t id );
	void	NE_API Tls_SetValue	( TlsId_t id, void* data );
	void*	NE_API Tls_GetValue	( TlsId_t id );

	/// Synchronization Objects

	Event_t	NE_API Event_Create  ( bool signaled );
	void	NE_API Event_Close   ( Event_t ev );
	void	NE_API Event_Signal  ( Event_t ev );
	void	NE_API Event_Unsignal( Event_t ev );
	void	NE_API Event_Wait	 ( Event_t ev );

	Semaphore_t NE_API Semaphore_Create ( int initial, int maximum );
	void		NE_API Semaphore_Destroy  ( Semaphore_t semaphore );
	void		NE_API Semaphore_Signal ( Semaphore_t semaphore, int count );
	void		NE_API Semaphore_Wait	( Semaphore_t semaphore );

	void NE_API CriticalSection_Create( CriticalSection_t& cs );
	void NE_API CriticalSection_Destroy	  ( CriticalSection_t& cs );
	void NE_API CriticalSection_Enter	  ( CriticalSection_t& cs );
	void NE_API CriticalSection_Leave	  ( CriticalSection_t& cs );

} }

//======================================================================================
namespace nemesis {	namespace system 
{
	class StopWatch_c
	{
	public:
		StopWatch_c()
			: Freq(Clock_GetFreq())
			, Tick(Clock_GetTick())
		{}

	public:
		void Start()
		{ Tick = system::Clock_GetTick(); }

		Tick_t ElapsedTicks() const
		{ return system::Clock_GetTick()-Tick; }

		int64_t ElapsedMs64() const
		{ return (ElapsedTicks() * 1000) / Freq; }

		uint32_t ElapsedMs() const
		{ return (uint32_t)ElapsedMs64(); }

		float ElapsedMsF() const
		{ return (float)(ElapsedTicks() * 1000) / (float)Freq; }

		float ElapsedSec() const
		{ return (float)ElapsedTicks() / (float)Freq; }

	private:
		Tick_t Freq;
		Tick_t Tick;
	};

	class ScopedLock_c
	{
		NE_NO_COPY(ScopedLock_c);

	public:
		explicit ScopedLock_c( CriticalSection_t& cs )
			: Mutex( cs )
		{ CriticalSection_Enter( Mutex ); }

		~ScopedLock_c()
		{ CriticalSection_Leave( Mutex ); }
		
	private:
		CriticalSection_t& Mutex;
	};

} } 

//======================================================================================
#define NeLock	::nemesis::system::ScopedLock_c	NeUnique(lock)

//======================================================================================
namespace nemesis {	namespace system 
{
	class SlimThread_c
	{
		NE_NO_COPY(SlimThread_c);

	public:
		SlimThread_c()
			: Instance(0)
			, Quit(0)
		{}

	public:
		bool Start( const ThreadSetup_s& setup )
		{
			if (Instance)
				return false;
			Quit.Set( 0 );
			Instance = Thread_Create( setup );
			return (Instance != 0);
		}

		void Stop()
		{
			Quit.Set( 1 );
			Thread_Wait( Instance );
			Instance = 0;
		}

		bool Continue() const
		{ return Quit ? false : true; }

	private:
		Thread_t Instance;
		atomic::Int32 Quit;
	};

} }
