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
namespace nemesis { namespace profiling
{
	typedef struct Parser_s		*Parser_t;
	typedef struct Receiver_s	*Receiver_t;
	typedef struct Database_s	*Database_t;

} }

//======================================================================================
namespace nemesis { namespace profiling
{
	/// A quantum of time
	typedef int64_t Tick;

	/// A closed time interval
	struct TickInterval
	{
		Tick Begin;
		Tick End;
		Tick Duration()						       const { return End-Begin; }
		bool Contains( Tick tick )			       const { return (tick >= Begin) && (tick < End); }
		bool Intersects( Tick first, Tick last )   const { return (End > first) && (Begin < last); }
		bool Intersects( const TickInterval& rhs ) const { return Intersects( rhs.Begin, rhs.End ); }
	};
	
	/// A range of indices
	struct IndexRange
	{
		int First;
		int Count;
		int Begin() const { return First; }
		int Last () const { return End() - 1; }
		int End  () const { return First + Count; }
	};

	/// Clock settings
	struct Clock
	{
		Tick TicksPerSecond;
		float OneOverTicksPerSecond;

		float TicksPerMs()			 const { return TicksPerSecond * 0.001f; }
		float TickToMs( Tick tick )  const { return OneOverTicksPerSecond * (1000 * tick); }
		float TickToSec( Tick tick ) const { return OneOverTicksPerSecond * tick; }
		float TickToFps( Tick tick ) const { return (float)TicksPerSecond / (float)tick; }
		Tick MsToTick( float ms )    const { return (Tick)(ms * TicksPerMs()); }

		void Initialize( Tick ticks_per_second )
		{
			if (TicksPerSecond == ticks_per_second)
				return;
			NeAssertOut( TicksPerSecond == 0, "Tick rate changed!" );
			TicksPerSecond = ticks_per_second;
			OneOverTicksPerSecond = 1.0f/float(ticks_per_second);
		}
	};
} }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz 
{
	struct ScopeEvent
	{
		Tick Time;
		uint32_t Location;
		uint8_t Thread;
		uint8_t Cpu;
		uint8_t Type;
		uint8_t Enter;
	};

	struct Lock
	{
		uint64_t Handle_t;
		const char* Name;
	};

	struct LockEvent
	{
		int64_t Tick;
		uint8_t Lock;
		uint8_t Thread;
		uint8_t Enter;
		uint8_t _padding_;
	};

	struct Counter
	{
		const char* Name;
		uint32_t Group; 
		float Minimum;
		float Maximum;
	};

	struct CounterGroup
	{
		const char* Name;
		uint32_t	Length;
		int32_t	Parent;
	};

	struct CounterValue
	{
		const char* Name;
		float Value;
	};

	/// A profiling frame
	struct Frame
	{
		TickInterval Time;
		uint32_t FirstScopeEvent;
		uint32_t NumScopeEvents;
		uint32_t FirstLockEvent;
		uint32_t NumLockEvents;
		uint32_t FirstCounterValue;
		uint32_t NumCounterValues;
		uint32_t ParsedBytes;
	};

	/// A profiling thread
	struct Thread
	{
		uint8_t NumLevels;
		uint8_t _padding_[3];
		const char* Name;
	};

	struct LogItem
	{
		const char* Text;
		uint32_t Location;
		uint8_t Thread;
		uint8_t Reserved[3];
	};

} } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz 
{
	struct HotSpot
	{
		uint32_t Location;
		uint32_t NumSamples;
		Tick Duration;
	};

	struct HotSpotGroup
	{
		Array<HotSpot>	Data;
	};

	struct HotSpotRange
	{
		int FirstFrame;
		int NumFrames;
		int NumHotSpotsPerFrame;
	};

} } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz
{
	struct FrameRange
	{
		TickInterval Time;
		IndexRange Frames;
	};

	struct FrameGroup
	{
		IndexRange Frames;
	};

	struct FrameGroupSetup
	{
		Tick MinFrameTicks;
	};

	struct ZoneGroup
	{
		TickInterval Time;
		uint32_t	Location;
		uint8_t	Thread;
		uint8_t	Level;
		uint8_t	Type: 2;
		uint8_t	Cpu0: 6;
		uint8_t	Cpu1: 6;
		uint8_t	Flags: 2;
		uint32_t	Index;
		uint32_t	NumZones;
	};

	struct ZoneGroupSetup
	{
		Tick MinFrameTicks;
		Tick MinZoneTicks;
		Tick MinGroupTicks;
		Tick MaxGroupSpacingTicks;
		uint8_t   MaxZoneLevel;
		uint8_t	 Clip;
		uint8_t	 _padding_[2];
	};

	struct CpuGroup
	{
		TickInterval Time;
		uint8_t Thread;
		uint8_t Cpu;
		uint8_t Cpu2;
		uint8_t _pad_[1];
	};

	struct CpuGroupSetup
	{
	};

} } }
