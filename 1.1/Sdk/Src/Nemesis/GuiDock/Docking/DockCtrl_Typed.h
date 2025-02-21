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
	Pad_s	NE_API UserCtrl_CalcNonClient	( DockCtrl_t ctrl );
	Rect_s	NE_API UserCtrl_CalcInsertRect	( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side );
}

//======================================================================================
namespace nemesis
{
	void	NE_API TabCtrl_UpdateActivePage	( DockCtrl_t ctrl, DockCtrl_t remove );
	Pad_s	NE_API TabCtrl_CalcNonClient	( DockCtrl_t ctrl );
	Rect_s	NE_API TabCtrl_CalcInsertRect	( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side );
	void	NE_API TabCtrl_Layout			( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove );
	void	NE_API TabCtrl_DoTabs			( DockCtrl_t ctrl );
	void	NE_API TabCtrl_DoChildren		( DockCtrl_t ctrl );
	void	NE_API TabCtrl_DoClient			( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	Pad_s	NE_API SplitterCtrl_CalcNonClient	( DockCtrl_t ctrl );
	Rect_s	NE_API SplitterCtrl_CalcInsertRect	( DockCtrl_t ctrl, DockCtrl_t child, DockCtrl_t insert, DockSide::Enum side );
	void	NE_API SplitterCtrl_Layout			( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove );
	void	NE_API SplitterCtrl_DoSplitter		( DockCtrl_t ctrl, DockCtrl_t child );
	void	NE_API SplitterCtrl_DoSplitters		( DockCtrl_t ctrl );
	void	NE_API SplitterCtrl_DoChildren		( DockCtrl_t ctrl );
	void	NE_API SplitterCtrl_DoClient		( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	Pad_s	NE_API MainCtrl_CalcNonClient	( DockCtrl_t ctrl );
	Rect_s	NE_API MainCtrl_CalcInsertRect	( DockCtrl_t ctrl, DockCtrl_t insert, DockSide::Enum side, DockArea::Enum area );
	void	NE_API MainCtrl_Layout			( DockCtrl_t ctrl, DockCtrl_t insert, DockCtrl_t remove );
	void	NE_API MainCtrl_DoSplitter		( DockCtrl_t ctrl, DockCtrl_t child, bool* recalc_layout );
	void	NE_API MainCtrl_DoSplitters		( DockCtrl_t ctrl, bool* recalc_layout );
	void	NE_API MainCtrl_DoChildren		( DockCtrl_t ctrl );
	void	NE_API MainCtrl_DoClient		( DockCtrl_t ctrl );
}

//======================================================================================
namespace nemesis
{
	Rect_s	NE_API OverlayCtrl_CalcScreenRect	( DockCtrl_t drop );
	void	NE_API OverlayCtrl_Do				( DockCtrl_t ctrl );
}
