//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Database.h"
#include "Private/Parser.h"

//======================================================================================
namespace nemesis { namespace profiling
{
	void Database_Initialize( Database_t db, Allocator_t alloc, const DatabaseSetup_s& setup )
	{
		db->Alloc = alloc;
		db->Setup = setup;
		db->NameTable.Alloc = alloc;
		db->StringPool.Pages.Alloc = alloc;
		ParsedData_Initialize( db->Data, alloc );

		if (!setup.MaxNumBytes)
			 db->Setup.MaxNumBytes = 500 * 1024 * 1024;

		const uint32_t large = db->Setup.MaxNumBytes / 2;
		const uint32_t small = db->Setup.MaxNumBytes / 10;

		db->Data.Frames			.Reserve( small / sizeof(db->Data.Frames		[0]) );
		db->Data.Scopes			.Reserve( large / sizeof(db->Data.Scopes		[0]) );
		db->Data.LockEvents		.Reserve( small / sizeof(db->Data.LockEvents	[0]) );
		db->Data.CounterValues	.Reserve( small / sizeof(db->Data.CounterValues	[0]) );
		db->Data.LogItems		.Reserve( small / sizeof(db->Data.LogItems		[0]) );
	}

	size_t Database_TotalSize( Database_t db )
	{
		return db->Data.TotalSize()
			 + Array_GetCountSize( db->NameTable )
			 + Stack_GetSize	( db->StringPool );
	}

	void Database_ResetStrings( Database_t db )
	{
		db->NameTable.Reset();
		db->StringPool.Reset();
	}

	void Database_Shutdown( Database_t db )
	{
		db->NameTable.Clear();
		db->StringPool.Clear();
		ParsedData_Shutdown( db->Data );
	}

} }
