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
#include <Nemesis/Core/Types.h>

//======================================================================================
namespace nemesis { namespace gui 
{
	struct Palette_s
	{
		uint32_t Text;
		uint32_t Normal_Bright;
		uint32_t Normal_Medium;
		uint32_t Normal_Dark;
		uint32_t Active_Bright;
		uint32_t Active_Medium;
		uint32_t Active_Dark;
	};

	void Theme_ResetMetrics( Theme_t theme );
	void Theme_ResetVisuals( Theme_t theme, const Palette_s& palette, float alpha );

	void Theme_ApplyPreset_Default( Theme_t theme );
	void Theme_ApplyPreset_Dark	  ( Theme_t theme );

} }