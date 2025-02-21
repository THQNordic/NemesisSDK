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
#include "Private/ParserData.h"

//======================================================================================
namespace nemesis { namespace profiling
{ 
	struct Database_s
	{
		Allocator_t		Alloc;
		DatabaseSetup_s Setup;
		ParsedData_s	Data;
		Array<cstr_t>	NameTable;
		Stack_s			StringPool;
	};

	void	Database_Initialize	 ( Database_t db, Allocator_t alloc, const DatabaseSetup_s& setup );
	size_t	Database_TotalSize	 ( Database_t db );
	void	Database_ResetStrings( Database_t db );
	void	Database_Shutdown	 ( Database_t db );

} }
