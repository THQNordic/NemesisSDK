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
	/// PDB

	struct PdbSymbolInfo_s
	{
		enum { MAX_NAME_CHARS = 256 };
		uint32_t	Private;
		uint32_t	TypeIndex;
		uint64_t	Reserved[2];
		uint32_t	Index;
		uint32_t	Size;
		uint64_t	ModBase;
		uint32_t	Flags;
		uint64_t	Value;
		uint64_t	Address;
		uint32_t	Register;
		uint32_t	Scope;
		uint32_t	Tag;
		uint32_t	NameLen;
		uint32_t	MaxNameLen;
		char		Name[1 + MAX_NAME_CHARS];
	};

	struct PdbSymbolLineInfo_s
	{
		uint32_t	Private;
		const void*	Key;
		uint32_t	LineNumber;
		const char*	FileName;
		uint64_t	Address;
	};

	/// PopUp

	struct DebugPopUp
	{
		enum Mode
		{ None
		, OkCancel
		, RetryCancelContinue
		};

		enum Result
		{ Ok
		, Retry
		, Cancel
		, Continue
		};
	};

} }
