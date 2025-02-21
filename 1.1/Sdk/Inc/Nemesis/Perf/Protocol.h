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
#include <Nemesis/Core/Memory.h>

//======================================================================================
namespace nemesis {	namespace profiling
{
	/// A chunk of data.
	struct Chunk
	{
		uint32_t id;
		uint32_t size;
	};

	/// A packet of data.
	struct Packet
	{
		enum Flag
		{
			None		= 0x00000000,
			BigEndian	= 0x00000001
		};

		uint32_t DataSize() const 
		{ return header.size - sizeof(*this); }

		Chunk header;
		uint32_t flags;
		uint32_t reserved;
	};

} }

//======================================================================================
namespace nemesis {	namespace profiling
{
	namespace chunk
	{
		struct Type 
		{ 
			enum Enum 
			{ EnterScope			= 0x00a0
			, EnterIdleScope		= 0x00a1
			, EnterLockScope		= 0x00a2
			, LeaveScope			= 0x00b0
			, EnterLock				= 0x0030
			, LeaveLock				= 0x0031
			, EndFrame				= 0x0041
			, NameList				= 0x0102
			, LocationList			= 0x0103
			, ThreadInfo			= 0x2002
			, MutexInfo				= 0x2004
			, Counter_U32_32		= 0x2010
			, Counter_U32_64		= 0x2011
			, Counter_Float_32		= 0x2012
			, Counter_Float_64		= 0x2013
			, Counter_Path_U32_32	= 0x2020
			, Counter_Path_U32_64	= 0x2021
			, Counter_Path_Float_32	= 0x2022
			, Counter_Path_Float_64	= 0x2023
			, Log					= 0x3001
			, Packet				= 0x4002
			, Connect				= 0xf001
			};
		};

#pragma pack ( push, 8 )

		struct EnterScope
		{
			Chunk header;
			uint8_t threadId;
			uint8_t cpuId;
			uint8_t _pad_[2];
			uint32_t location;
			int64_t timeStamp;
		};

		struct LeaveScope
		{
			Chunk header;
			uint8_t threadId;
			uint8_t cpuId;
			uint8_t _pad_[2];
			uint32_t reserved;
			int64_t timeStamp;
		};

		struct EnterLock
		{
			Chunk header;
			uint8_t threadId;
			uint8_t cpuId;
			uint8_t _pad_[2];
			int64_t timeStamp;
			uint64_t lockId;
		};

		struct LeaveLock
		{
			Chunk header;
			uint8_t threadId;
			uint8_t cpuId;
			uint8_t _pad_[2];
			int64_t timeStamp;
			uint64_t lockId;
		};

		struct NameList
		{
			Chunk header;
			uint32_t numItems;
			uint32_t _pad_;
		};

		struct LocationItem
		{
			uint64_t name;
			uint64_t func;
			uint64_t file;
			uint32_t line;
			uint32_t id;
		};

		struct LocationList
		{
			Chunk header;
			uint32_t numItems;
			uint8_t threadId;
			uint8_t _pad_[3];
			LocationItem item[0];
		};

		struct ThreadInfo
		{
			Chunk header;
			uint8_t threadId;
			uint8_t _pad_[3];
			uint32_t _pad2_;
			uint64_t name;
		};

		struct MutexInfo
		{
			Chunk header;
			uint64_t handle;
			uint64_t name;
		};

		struct Counter_U32_32
		{
			Chunk header;
			uint32_t value;
			uint32_t name;
		};

		struct Counter_U32_64
		{
			Chunk header;
			uint32_t value;
			uint64_t name;
		};

		struct Counter_Float_32
		{
			Chunk header;
			float value;
			uint32_t name;
		};

		struct Counter_Float_64
		{
			Chunk header;
			float value;
			uint64_t name;
		};

		struct Counter_Path_U32_32
		{
			Chunk header;
			uint32_t value;
			uint32_t name;
			uint32_t path;
		};

		struct Counter_Path_U32_64
		{
			Chunk header;
			uint32_t value;
			uint64_t name;
			uint64_t path;
		};

		struct Counter_Path_Float_32
		{
			Chunk header;
			float value;
			uint32_t name;
			uint32_t path;
		};

		struct Counter_Path_Float_64
		{
			Chunk header;
			float value;
			uint64_t name;
			uint64_t path;
		};

		struct EndFrame
		{
			Chunk header;
			int64_t beginTick;
			int64_t endTick;
			int64_t tickRate;
			uint32_t frameNumber;
			uint32_t reserved;
		};

		struct Log
		{
			Chunk header;
			int64_t timeStamp;
			uint8_t threadId;
			uint8_t _pad_[3];
			uint32_t location;
			char text[0];
		};

		struct Connect
		{
			Chunk header;
			int64_t timeStamp;
		};
	}

#pragma pack ( pop )

} } 

//======================================================================================
namespace nemesis 
{
	using namespace profiling;

	inline void EndianSwap( const Chunk& in, Chunk& out )
	{
		out.id	 = nemesis::EndianSwap( in.id );
		out.size = nemesis::EndianSwap( in.size );
	}

	inline void EndianSwap( const chunk::EnterScope& in, chunk::EnterScope& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId	= nemesis::EndianSwap( in.threadId );
		out.cpuId		= nemesis::EndianSwap( in.cpuId );
		out.location	= nemesis::EndianSwap( in.location );
		out.timeStamp	= nemesis::EndianSwap( in.timeStamp );
	}

	inline void EndianSwap( const chunk::LeaveScope& in, chunk::LeaveScope& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId	= nemesis::EndianSwap( in.threadId );
		out.cpuId		= nemesis::EndianSwap( in.cpuId );
		out.reserved	= nemesis::EndianSwap( in.reserved );
		out.timeStamp	= nemesis::EndianSwap( in.timeStamp );
	}

	inline void EndianSwap( const chunk::EnterLock& in, chunk::EnterLock& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId	= nemesis::EndianSwap( in.threadId );
		out.cpuId		= nemesis::EndianSwap( in.cpuId );
		out.timeStamp	= nemesis::EndianSwap( in.timeStamp );
		out.lockId		= nemesis::EndianSwap( in.lockId );
	}

	inline void EndianSwap( const chunk::LeaveLock& in, chunk::LeaveLock& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId	= nemesis::EndianSwap( in.threadId );
		out.cpuId		= nemesis::EndianSwap( in.cpuId );
		out.timeStamp	= nemesis::EndianSwap( in.timeStamp );
		out.lockId		= nemesis::EndianSwap( in.lockId );
	}

	inline void EndianSwap( const chunk::NameList& in, chunk::NameList& out )
	{
		EndianSwap( in.header, out.header );
		out.numItems = nemesis::EndianSwap( in.numItems );
	}

	inline void EndianSwap( const chunk::ThreadInfo& in, chunk::ThreadInfo& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId = nemesis::EndianSwap( in.threadId );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::MutexInfo& in, chunk::MutexInfo& out )
	{
		EndianSwap( in.header, out.header );
		out.handle = nemesis::EndianSwap( in.handle );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::Counter_U32_32& in, chunk::Counter_U32_32& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::Counter_U32_64& in, chunk::Counter_U32_64& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::Counter_Float_32& in, chunk::Counter_Float_32& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::Counter_Float_64& in, chunk::Counter_Float_64& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
	}

	inline void EndianSwap( const chunk::Counter_Path_U32_32& in, chunk::Counter_Path_U32_32& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
		out.path = nemesis::EndianSwap( in.path );
	}

	inline void EndianSwap( const chunk::Counter_Path_U32_64& in, chunk::Counter_Path_U32_64& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
		out.path = nemesis::EndianSwap( in.path );
	}

	inline void EndianSwap( const chunk::Counter_Path_Float_32& in, chunk::Counter_Path_Float_32& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
		out.path = nemesis::EndianSwap( in.path );
	}

	inline void EndianSwap( const chunk::Counter_Path_Float_64& in, chunk::Counter_Path_Float_64& out )
	{
		EndianSwap( in.header, out.header );
		out.value = nemesis::EndianSwap( in.value );
		out.name = nemesis::EndianSwap( in.name );
		out.path = nemesis::EndianSwap( in.path );
	}

	inline void EndianSwap( const chunk::EndFrame& in, chunk::EndFrame& out )
	{
		EndianSwap( in.header, out.header );
		out.beginTick = nemesis::EndianSwap( in.beginTick );
		out.endTick = nemesis::EndianSwap( in.endTick );
		out.tickRate = nemesis::EndianSwap( in.tickRate );
		out.frameNumber = nemesis::EndianSwap( in.frameNumber );
		out.reserved = nemesis::EndianSwap( in.reserved );
	}

	inline void EndianSwap( const chunk::LocationList& in, chunk::LocationList& out )
	{
		EndianSwap( in.header, out.header );
		out.threadId = EndianSwap(in.threadId);
		out.numItems = EndianSwap(in.numItems);
	}
}

//======================================================================================
namespace nemesis { namespace profiling
{
	namespace ping
	{
		struct Header
		{
			static const uint32_t ID   = NeMakeFourCc( 'N', 'E', 'M', ' ' );
			static const uint32_t TYPE = NeMakeFourCc( 'P', 'E', 'R', 'F' );

			uint32_t Id;
			uint32_t Type;

			bool IsValid() const
			{ return (Id == ID) && (Type == TYPE); }

			void Init()
			{
				Id = ID;
				Type = TYPE;
			}
		};

		struct Response
		{
			Header Header;
			uint32_t	   Data[4];
		};
	}

} }
