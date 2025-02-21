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
#include "DockCtrl.h"

//======================================================================================
namespace nemesis
{
	void DockCtrl_OnWrap	( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t container );
	void DockCtrl_OnUnwrap	( DockCtrl_t ctrl, DockCtrl_t container, DockCtrl_t remainder );
	void DockCtrl_OnInsert	( DockCtrl_t ctrl, DockCtrl_t insert );
	void DockCtrl_OnRemove	( DockCtrl_t ctrl, DockCtrl_t remove );
	void DockCtrl_OnReorder ( DockCtrl_t ctrl, DockCtrl_t reorder );
	void DockCtrl_OnResize	( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	Rect_s DockCtrl_CalcInsertRect( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area );
}
