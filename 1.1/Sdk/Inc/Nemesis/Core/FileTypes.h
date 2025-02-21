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

//======================================================================================
namespace nemesis {	namespace system
{
	typedef struct File_s *File_t;

	struct FileCreate
	{
		enum Mode
		{ CreateAlways
		, CreateNew
		, OpenAlways
		, OpenExisting
		, TruncateExisting
		};
	};

	struct FileAccess
	{
		enum Flags
		{ Read		= 0x01 
		, Write		= 0x02
		, ReadWrite = Read | Write
		};

		typedef uint32_t Mask_t;
	};

	struct FileSeek
	{
		enum Mode
		{ Current
		, Begin
		, End
		};
	};

} }

//======================================================================================
namespace nemesis {	namespace system
{
	typedef char Path_t[256];

} }
