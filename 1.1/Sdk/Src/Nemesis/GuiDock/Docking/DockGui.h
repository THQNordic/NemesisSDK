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
namespace nemesis { namespace gui
{
	/// Context

	void NE_API Context_DrawRect( Context_t dc, const Rect_s& r, uint32_t c );
	void NE_API Context_FillRect( Context_t dc, const Rect_s& r, uint32_t c );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Ctrl

	void NE_API Ctrl_DrawBox( Context_t dc, const Rect_s& r, const Visual_s& v, CtrlState::Enum s );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// PosGrip

	struct PosGripAction
	{
		enum Enum
		{ None
		, Pushed
		, Dragged
		, Released
		};
	};

	PosGripAction::Enum NE_API PosGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, MouseButton::Enum button, Vec2_s& pos );
	PosGripAction::Enum NE_API PosGrip_Mouse( Context_t dc, Id_t id, const Rect_s& r, Vec2_s& pos );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Border

	bool NE_API Border_Draw ( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen );
	bool NE_API Border_Mouse( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen );
	bool NE_API Border_Do	( Context_t dc, Id_t id, const Rect_s& r, const Vec2_s& border, const Vec2_s& min_size, Rect_s& screen );
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Splitter2

	bool NE_API Splitter2_Draw	( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float  pos );
	bool NE_API Splitter2_Mouse	( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float& pos );
	bool NE_API Splitter2_Do	( Context_t dc, Id_t id, const Rect_s& r, Orientation::Enum dir, float splitter_size, float center_offset, float& pos );
	bool NE_API Splitter2_DoH	( Context_t dc, Id_t id, const Rect_s& r, float splitter_size, float center_offset, float& pos );
	bool NE_API Splitter2_DoV	( Context_t dc, Id_t id, const Rect_s& r, float splitter_size, float center_offset, float& pos );

} }
