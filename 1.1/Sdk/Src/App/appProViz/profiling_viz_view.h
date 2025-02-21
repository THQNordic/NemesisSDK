//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once
#include "profiling_viz.h"

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	int64_t ClampScrollTick( Database_t db, const ZoomInfo& zoom, float view_width, int64_t tick );

	void EnumFrameGroups( Database_t db, const Rect_s& view, Tick scroll_tick, const ZoomInfo& zoom, FrameRange& frame_range );

	float CalculateMaxFps( const FrameRange& cull, Database_t db );

} } } }

//======================================================================================
namespace nemesis { namespace profiling { namespace viz { namespace view
{
	Vec2_s MouseMessageToPos( HWND hWnd, UINT msg, LPARAM lParam );

} } } }
