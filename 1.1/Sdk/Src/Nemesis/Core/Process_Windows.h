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
namespace nemesis { namespace system
{
	//==================================================================================

	Tick_t Clock_GetFreq()
	{
		LARGE_INTEGER freq;
		::QueryPerformanceFrequency( &freq );
		return freq.QuadPart;
	}

	Tick_t Clock_GetTick()
	{
		LARGE_INTEGER tick;
		::QueryPerformanceCounter( &tick );
		return tick.QuadPart;
	}

	//==================================================================================

	CpuId_t Cpu_GetIndex()
	{ return GetCurrentProcessorNumber(); }

	void Cpu_Yield()
	{ return YieldProcessor(); }

	//==================================================================================

	TlsId_t Tls_Alloc()
	{ return TlsAlloc(); }

	void Tls_Free( TlsId_t id )
	{ TlsFree( id ); }

	void Tls_SetValue( TlsId_t id, void* data )
	{ TlsSetValue( id, data ); }

	void* Tls_GetValue( TlsId_t id )
	{ return TlsGetValue( id ); }

	//==================================================================================

	Event_t Event_Create( bool signaled )
	{ return CreateEvent( nullptr, TRUE, signaled ? TRUE : FALSE, nullptr ); }

	void Event_Close( Event_t event )
	{ CloseHandle( event ); }

	void Event_Signal( Event_t event )
	{ SetEvent( event ); }

	void Event_Unsignal( Event_t event )
	{ ResetEvent( event ); }

	void Event_Wait( Event_t event )
	{ WaitForSingleObject( event, INFINITE ); }

	//==================================================================================

	Semaphore_t Semaphore_Create( int initial, int maximum )
	{ 
	#if (NE_PLATFORM == NE_PLATFORM_X360)
		return CreateSemaphore( nullptr, initial, maximum, nullptr ); 
	#else
		return CreateSemaphoreEx( nullptr, initial, maximum, nullptr, 0, SEMAPHORE_ALL_ACCESS ); 
	#endif
	}

	void Semaphore_Destroy( Semaphore_t semaphore )
	{ CloseHandle( semaphore ); }

	void Semaphore_Signal( Semaphore_t semaphore, int count )
	{ ReleaseSemaphore( semaphore, count, nullptr ); }

	void Semaphore_Wait( Semaphore_t semaphore )
	{ WaitForSingleObject( semaphore, INFINITE ); }

	//==================================================================================
	NeStaticAssert( sizeof(CriticalSection_t) >= sizeof(CRITICAL_SECTION) );

	void CriticalSection_Create( CriticalSection_t& cs )
	{ 
		NeZero( cs );
		InitializeCriticalSection( (CRITICAL_SECTION*)&cs );
	}

	void CriticalSection_Destroy( CriticalSection_t& cs )
	{ 
		DeleteCriticalSection( (CRITICAL_SECTION*)&cs ); 
	}

	void CriticalSection_Enter( CriticalSection_t& cs )
	{ 
		EnterCriticalSection( (CRITICAL_SECTION*)&cs ); 
	}

	void CriticalSection_Leave( CriticalSection_t& cs )
	{ 
		LeaveCriticalSection( (CRITICAL_SECTION*)&cs ); 
	}

	//==================================================================================

	namespace
	{
		static void SetThreadDebugName( DWORD thread_id, const char* name )
		{
		#if NE_CONFIG <= NE_CONFIG_MASTER
			if (!name || !name[0])
				name = "Nemesis";

			//NePerfThread( name );

			// http://msdn.microsoft.com/en-us/library/xcb2z8hs(VS.71).aspx
			#define MS_VC_EXCEPTION 0x406D1388
			typedef struct tagTHREADNAME_INFO
			{
			   DWORD dwType;		// Must be 0x1000.
			   LPCSTR szName;		// Pointer to name (in user addr space).
			   DWORD dwThreadID;	// Thread ID (-1=caller thread).
			   DWORD dwFlags;		// Reserved for future use, must be zero.
			} THREADNAME_INFO;

			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = name;
			info.dwThreadID = thread_id;
			info.dwFlags = 0;
			__try
			{
				RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		#endif
		}
	}

	namespace
	{
		struct ThreadArgs
		{
			ThreadProc Proc;
			void* Context;
			const char* Name;
		};

		static DWORD WINAPI RunThread( void* context )
		{
			// copy args to stack mem and delete them
			ThreadArgs* ptr = (ThreadArgs*)context;
			ThreadArgs args = *ptr;
			delete ptr;

			// set debug name
			SetThreadDebugName( GetCurrentThreadId(), args.Name );

			// run using the copy on the stack
			args.Proc( args.Context );

			// done
			return 0;
		}
	}

	Thread_t Thread_Create( const ThreadSetup_s& setup )
	{
		// copy args
		ThreadArgs* args = new ThreadArgs;
		args->Context = setup.Context;
		args->Proc = setup.Proc;
		args->Name = setup.Name;

		// create thread
		DWORD thread_id;
		Handle_t handle = CreateThread( nullptr, 0, &RunThread, args, 0, &thread_id );
		if (!handle)
		{
			delete args;
			return nullptr;
		}
		return (Thread_t)handle;
	}

	void Thread_Wait( Thread_t thread )
	{ WaitForSingleObject( (Handle_t)thread, INFINITE ); }

	void Thread_SleepMs( uint32_t ms )
	{ Sleep( ms ); }

	ThreadId_t Thread_GetId()
	{ return GetCurrentThreadId(); }

} }
