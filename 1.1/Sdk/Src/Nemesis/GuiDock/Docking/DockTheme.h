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
	typedef struct DockTheme_s *DockTheme_t;

	struct DockTheme_s
	{
		Vec2_s FrameBorder;
		Vec2_s MinFloatSize;
		Vec2_s MinDockedSize;
		Vec2_s TabItemSpacing;
		Vec2_s OverlayIconSize;
		Vec2_s OverlayIconSpace;
		uint32_t OverlayIconColor; 
		uint32_t OverlayIconColorHot; 
		uint32_t OverlayAreaColor; 
		float TearThreshold;
	};

	void NE_API DockTheme_Initialize( DockTheme_t theme );

}
