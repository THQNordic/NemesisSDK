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
namespace nemesis
{
	DockMgrApi_Os_s	NE_API DockOs_Null_GetApi();

	DockOs_t	NE_CALLBK DockOs_Null_Create	( ptr_t user );
	void		NE_CALLBK DockOs_Null_Release	( DockOs_t os );
	int			NE_CALLBK DockOs_Null_OrderZ	( DockOs_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts );

	DockHost_t	NE_CALLBK DockHost_Null_Create	 ( DockOs_t os, DockCtrl_t ctrl );
	bool		NE_CALLBK DockHost_Null_IsActive ( DockHost_t host );
	void	  	NE_CALLBK DockHost_Null_Move	 ( DockHost_t host );
	void		NE_CALLBK DockHost_Null_Move_Rect( DockHost_t host, const Rect_s& r );
	void		NE_CALLBK DockHost_Null_Show	 ( DockHost_t host );
	void		NE_CALLBK DockHost_Null_Render	 ( DockHost_t host );
	void		NE_CALLBK DockHost_Null_Release	 ( DockHost_t host );

}
