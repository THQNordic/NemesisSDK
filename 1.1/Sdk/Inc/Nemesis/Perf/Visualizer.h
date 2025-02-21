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
#include <Nemesis/Core/ArrayTypes.h>
#include <Nemesis/Core/SocketTypes.h>
#include "VisualizerTypes.h"
#include "Protocol.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	struct Connect
	{
		enum Result
		{ Ok
		, Failed
		, AlreadyConnected
		};
	};

	struct ReceiverCallback
	{
		void (NE_CALLBK *PacketReceived)( void* context, system::Socket_t sender, const Packet& packet, const Chunk* head );
		void* UserContext;
	};

	Receiver_t			Receiver_Create( Allocator_t alloc );
	void				Receiver_Destroy( Receiver_t receiver );
	bool				Receiver_IsConnected( Receiver_t receiver );
	Connect::Result		Receiver_Connect( Receiver_t receiver, system::IpAddress_t addr, const ReceiverCallback& callback );
	void				Receiver_Disconnect( Receiver_t receiver );
	system::Socket_t	Receiver_GetSocket( Receiver_t receiver );
	bool				Receiver_IsPaused( Receiver_t receiver );
	void				Receiver_Pause( Receiver_t receiver, bool pause );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct Parse
	{
		enum Mode
		{ Immediate
		, Buffered
		};
	};

	struct ParserSetup 
	{
		Database_t	Database;
		size_t		BufferSize;
	};

	Parser_t	Parser_Create		( Allocator_t alloc, const ParserSetup& setup );
	void		Parser_Destroy		( Parser_t parser );
	void		Parser_ParseData	( Parser_t parser, const Packet& packet, const Chunk* head, Parse::Mode mode );
	void		Parser_JoinData		( Parser_t parser );
	void		Parser_ResetData	( Parser_t parser );
	bool		Parser_IsPaused		( Parser_t parser );
	void		Parser_Pause		( Parser_t parser, bool pause );
	void		Parser_SetDebugDelay( Parser_t parser, int ms );

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	struct DatabaseSetup_s
	{
		uint32_t MaxNumBytes;
		uint32_t MaxNumFrames;
	};

	typedef void (*EnumFrameGroupsFunc)	( void* context, const viz::FrameGroup& item );
	typedef void (*EnumZoneGroupsFunc)	( void* context, const viz::ZoneGroup& item );
	typedef void (*EnumCpuGroupsFunc)	( void* context, const viz::CpuGroup& item );
	typedef void (*EnumLockEventFunc)	( void* context, const viz::LockEvent& ev, int event_index );

	Database_t			Database_Create					( Allocator_t alloc, const DatabaseSetup_s& setup );
	void				Database_Destroy				( Database_t db );
	size_t				Database_GetSize				( Database_t db );
	size_t				Database_GetCapacity			( Database_t db );
	void				Database_SetCapacity			( Database_t db, size_t size );
	const Clock&		Database_GetClock				( Database_t db );
	Tick				Database_GetFirstTick			( Database_t db );
	Tick				Database_GetLastEndTick			( Database_t db );
	Tick				Database_GetLastBeginTick		( Database_t db );
	Tick				Database_GetMaxFrameDuration	( Database_t db );
	uint32_t			Database_GetFirstFrameNumber	( Database_t db );
	uint32_t			Database_GetLastFrameNumber		( Database_t db );
	Tick				Database_GotoFrameIndex			( Database_t db, int frame_index );
	Tick				Database_GotoFrameNumber		( Database_t db, uint32_t frame_number );
	Tick				Database_GotoFirstFrame			( Database_t db );
	Tick				Database_GotoLastFrame			( Database_t db );
	Tick				Database_GotoBegin				( Database_t db );
	Tick				Database_GotoEnd				( Database_t db, Tick ticks_in_view );
	int					Database_TickToFrame			( Database_t db, Tick tick );
	int					Database_GetNumCpus				( Database_t db );
	int					Database_GetNumThreads			( Database_t db );
	void				Database_GetThread				( Database_t db, int index, viz::Thread& item );
	int					Database_GetNumFrames			( Database_t db );
	void				Database_GetFrame				( Database_t db, int index, viz::Frame& item );
	const viz::Frame*	Database_GetFrames				( Database_t db );
	int					Database_GetNumLocks			( Database_t db );
	void				Database_GetLock				( Database_t db, int index, viz::Lock& item );
	int					Database_GetNumCounters			( Database_t db );
	void				Database_GetCounter				( Database_t db, int index, viz::Counter& item );
	int					Database_GetNumCounterGroups	( Database_t db );
	void				Database_GetCounterGroup		( Database_t db, int index, viz::CounterGroup& item );
	int					Database_GetNumCounterValues	( Database_t db );
	void				Database_GetCounterValue		( Database_t db, int index, viz::CounterValue& item );
	int  				Database_GetNumLocations		( Database_t db );
	void 				Database_GetLocation			( Database_t db, int index, NamedLocation& item );
	void 				Database_GetLocationByZone		( Database_t db, int zone, NamedLocation& item );
	int  				Database_GetNumScopes			( Database_t db );
	void 				Database_EnumFrameGroups		( Database_t db, const viz::FrameRange& cull, const viz::FrameGroupSetup& setup, EnumFrameGroupsFunc func, void* context );
	void 				Database_EnumZoneGroups 		( Database_t db, const viz::FrameRange& cull, const viz::ZoneGroupSetup& setup, int thread_index, EnumZoneGroupsFunc func, void* context );
	void 				Database_EnumCpuGroups  		( Database_t db, const viz::FrameRange& cull, const viz::CpuGroupSetup& setup, int thread_index, EnumCpuGroupsFunc func, void* context );
	void 				Database_EnumLockEvents 		( Database_t db, int lock_index, int first_frame, int num_frames, EnumLockEventFunc func, void* context );
	void 				Database_BuildHotSpots  		( Database_t db, const viz::HotSpotRange& range, viz::HotSpotGroup& group );

} }
