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
	void Initialize( Allocator_t alloc, Renderer_t renderer );
	void Shutdown();

	void BeginFrame( HWND hWnd );
	void HandleMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	const Mouse_s& GetMouse();
	const Keyboard_s& GetKeyboard();

	const DragInfo_s& GetDrag();

	bool IsHot( Id_t id );
	bool IsActive( Id_t id );
	void SetHot( Id_t id );
	void SetActive( Id_t id );

	void BeginDrag( Id_t id, const Vec2_s& offset );
	void BeginDrag( Id_t id );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	TextBuffer& GetTextBuffer();

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	void PushClip( const Rect_s& r );
	void PopClip ();

	void DrawLine	  ( const Color_c& c, float x0, float y0, float x1, float y1 );
	void DrawLine	  ( const Color_c& c, float x0, float y0, float x1, float y1, float thickness );
	void DrawRectangle( const Rect_s& r, const Color_c& c );
	void FillRectangle( const Rect_s& r, const Color_c& c );
	void FillRectangle( const Rect_s& r, const Color_c& c1, const Color_c& c2, LinearGradient::Mode m );
	void DrawString	  ( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Rect_s& r, const Color_c& c );
	void DrawString	  ( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Vec2_s& p, const Color_c& c );
	void MeasureString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Rect_s& r, Rect_s* b );
	void MeasureString( const wchar_t* t, int l, Font_t f, TextFormat::Mask_t fmt, const Vec2_s& p, Rect_s* b );

	float GetLineHeight( Font_t f );
	float GetMaxCharWidth( Font_t f );

	Font_t EnsureFont( const wchar_t* name, float size, FontStyle::Mask style );

} }
