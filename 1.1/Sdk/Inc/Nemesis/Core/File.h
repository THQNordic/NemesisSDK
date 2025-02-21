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
#include "FileTypes.h"

//======================================================================================
namespace nemesis {	namespace system
{
	Result_t NE_API File_Create	( cstr_t path, FileCreate::Mode create, FileAccess::Mask_t access, File_t* file );
	Result_t NE_API File_GetTime( File_t file, uint64_t* create, uint64_t* access, uint64_t* write );
	Result_t NE_API File_GetSize( File_t file, uint32_t* size );
	Result_t NE_API File_GetPos	( File_t file, uint32_t* pos );
	Result_t NE_API File_Seek	( File_t file, FileSeek::Mode seek, int64_t pos );
	Result_t NE_API File_Read	( File_t file,  ptr_t data, uint32_t size, uint32_t* num_read );
	Result_t NE_API File_Write	( File_t file, cptr_t data, uint32_t size, uint32_t* num_written );
	Result_t NE_API File_Close	( File_t file );

	int NE_API FileTime_Compare( uint64_t lhs, uint64_t rhs );

} }

//======================================================================================
namespace nemesis {	namespace system
{
	Path_t& NE_API Path_Parent	 ( Path_t& path );
	Path_t& NE_API Path_Normalize( Path_t& path );
	Path_t& NE_API Path_Copy	 ( Path_t& path, cstr_t src );
	Path_t& NE_API Path_Init	 ( Path_t& path, cstr_t src );
	Path_t& NE_API Path_Concat	 ( Path_t& path, cstr_t src );
	Path_t& NE_API Path_Concat	 ( Path_t& path, cstr_t src, cstr_t dst );
	Path_t& NE_API Path_SetExt	 ( Path_t& path, cstr_t ext );

} }
