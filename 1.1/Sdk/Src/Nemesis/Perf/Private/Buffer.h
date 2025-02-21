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
#include "Types.h"
#include "Constants.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	struct BufferType
	{
		enum Enum
		{ Data
		, Meta
		};
	};

	struct Buffer_s
	{
		uint32_t Type;
		uint32_t Count;
		uint8_t  Data[ BUFFER_SIZE ];
	};

	inline void Buffer_Initialize( Buffer_t buffer, BufferType::Enum type )
	{
		buffer->Type = type;
		buffer->Count = 0;
	}

} }
