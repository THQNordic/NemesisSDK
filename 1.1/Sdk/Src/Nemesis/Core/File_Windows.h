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
	static File_t Handle_ToFile( HANDLE v )
	{
		if (v == INVALID_HANDLE_VALUE)
			return nullptr;
		if (v == 0)
			return (File_t)INVALID_HANDLE_VALUE;
		return (File_t)v;
	}

	static HANDLE File_ToHandle( File_t v )
	{
		if (v == nullptr)
			return INVALID_HANDLE_VALUE;
		if (v == INVALID_HANDLE_VALUE)
			return (HANDLE)0;
		return v;
	}

} }
//======================================================================================
namespace nemesis { namespace system
{
	Result_t File_Create( cstr_t path, FileCreate::Mode create, FileAccess::Mask_t access, File_t* file )
	{
		if (!file)
			return NE_ERR_INVALID_ARG;

		// convert args
		DWORD dwCreate = 0;
		DWORD dwAccess = 0;
		DWORD dwShare = 0;
		DWORD dwFlags = 0;
		switch (create)
		{
		case FileCreate::CreateAlways:		dwCreate = CREATE_ALWAYS;		break;
		case FileCreate::CreateNew:			dwCreate = CREATE_NEW;			break;
		case FileCreate::OpenAlways:		dwCreate = OPEN_ALWAYS;			break;
		case FileCreate::OpenExisting:		dwCreate = OPEN_EXISTING;		break;
		case FileCreate::TruncateExisting:	dwCreate = TRUNCATE_EXISTING;	break;
		default:
			return NE_ERR_INVALID_CALL;
		}
		if (NeHasFlag(access, FileAccess::Read))
			dwAccess |= GENERIC_READ;
		if (NeHasFlag(access, FileAccess::Write))
			dwAccess |= GENERIC_WRITE;
		if (!dwAccess)
			return NE_ERR_INVALID_ARG;
		dwShare = FILE_SHARE_READ | FILE_SHARE_DELETE;
		dwFlags = FILE_ATTRIBUTE_NORMAL;

		// convert path
		wchar_t wszPath[1024] = L"";
		MultiByteToWideChar( CP_UTF8, 0, path, -1, wszPath, NeCountOf(wszPath) );

		// create file
		HANDLE hFile = ::CreateFileW( wszPath, dwAccess, dwShare, nullptr, dwCreate, dwFlags, nullptr );
		*file = Handle_ToFile(hFile);
		return (hFile != INVALID_HANDLE_VALUE) ? NE_OK : NE_ERROR;
	}

	Result_t File_GetTime( File_t file, uint64_t* create, uint64_t* access, uint64_t* write )
	{
		HANDLE hFile = File_ToHandle( file );
		if (::GetFileTime( hFile, (FILETIME*)create, (FILETIME*)access, (FILETIME*)write))
			return NE_OK;
		return NE_ERROR;
	}

	Result_t File_GetSize( File_t file, uint32_t* size )
	{
		if (!size)
			return NE_ERR_INVALID_ARG;
		LARGE_INTEGER liSize = {};
		HANDLE hFile = File_ToHandle( file );
		if (!::GetFileSizeEx( hFile, &liSize ))
		{
			*size = 0;
			return NE_ERROR;
		}
		if (liSize.HighPart != 0)
		{
			*size = 0;
			return NE_ERROR;
		}
		*size = liSize.LowPart;
		return NE_OK;
	}

	Result_t File_GetPos( File_t file, uint32_t* pos )
	{
		if (!pos)
			return NE_ERR_INVALID_ARG;

		LARGE_INTEGER liPos = {};
		HANDLE hFile = File_ToHandle( file );
		if (!::SetFilePointerEx( hFile, {}, &liPos, FILE_CURRENT ))
		{
			*pos = 0;
			return NE_ERROR;
		}
		if (liPos.HighPart != 0)
		{
			*pos = 0;
			return NE_ERROR;
		}
		*pos = liPos.LowPart;
		return NE_OK;
	}

	Result_t File_Seek( File_t file, FileSeek::Mode seek, int64_t pos )
	{
		DWORD dwSeek = 0;
		switch (seek)
		{
		case FileSeek::Current: dwSeek = FILE_CURRENT;	break;
		case FileSeek::Begin:	dwSeek = FILE_BEGIN;	break;
		case FileSeek::End:		dwSeek = FILE_END;		break;
		default:
			return NE_ERR_INVALID_ARG;
		}
		LARGE_INTEGER liPos;
		liPos.QuadPart = pos;
		HANDLE hFile = File_ToHandle( file );
		if (::SetFilePointerEx( hFile, liPos, nullptr, dwSeek ))
			return NE_OK;
		return NE_ERROR;
	}

	Result_t File_Read( File_t file, ptr_t data, uint32_t size, uint32_t* num_read )
	{
		HANDLE hFile = File_ToHandle( file );
		if (::ReadFile( hFile, data, size, (DWORD*)num_read, nullptr ))
			return NE_OK;
		if (num_read)
			*num_read = 0;
		return NE_ERROR;
	}

	Result_t File_Write( File_t file, cptr_t data, uint32_t size, uint32_t* num_written )
	{
		HANDLE hFile = File_ToHandle( file );
		if (::WriteFile( hFile, data, size, (DWORD*)num_written, nullptr ))
			return NE_OK;
		if (num_written)
			*num_written = 0;
		return NE_ERROR;
	}

	Result_t File_Close( File_t file )
	{
		HANDLE hFile = File_ToHandle( file );
		if (::CloseHandle( hFile ))
			return NE_OK;
		return NE_ERROR;
	}

	int FileTime_Compare( uint64_t lhs, uint64_t rhs )
	{
		return ::CompareFileTime( (const FILETIME*)&lhs, (const FILETIME*)&rhs );
	}

} }

//======================================================================================
namespace nemesis { namespace system
{
	Path_t& Path_Copy( Path_t& path, cstr_t src )
	{
		Str_Cpy( path, src );
		return path;
	}

	Path_t& Path_Normalize( Path_t& path )
	{
		for ( int i = 0; (i < NeCountOf(path)) && path[i]; ++i )
			if (path[i] == '/')
				path[i] = '\\';
		return path;
	}

	Path_t& Path_Parent( Path_t& path )
	{
		str_t pos = Str_ChrR( path, '\\' );
		if (pos)
			pos[0] = 0;
		return path;
	}

	Path_t& Path_Init( Path_t& path, cstr_t src )
	{
		return Path_Normalize( Path_Copy( path, src ) );
	}

	Path_t& Path_Concat( Path_t& path, cstr_t src )
	{
		Str_Cat( path, "\\" );
		Str_Cat( path, src );
		return path;
	}

	Path_t& Path_Concat( Path_t& path, cstr_t src, cstr_t dst )
	{
		return Path_Concat( Path_Init( path, src ), dst );
	}

	Path_t& Path_SetExt( Path_t& path, cstr_t ext )
	{
		str_t pos = Str_ChrR( path, '.' );
		if (pos)
			Str_Cpy( pos, NeCountOf(path) - (pos-path), ext );
		else
			Str_Cat( path, ext );
		return path;
	}

} }
