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
#include "Types.h"

//======================================================================================
namespace nemesis { namespace gui
{
	/// Id

	Id_t NE_API Id_Gen( cstr_t name );
	Id_t NE_API Id_Cat( Id_t id, int child );
	Id_t NE_API Id_Cat( Id_t id, cstr_t child );

	#define NE_UI_ID			::nemesis::gui::Id_Cat( ::nemesis::gui::Id_Gen( __FILE__ ), __LINE__ )
	#define NE_UI_ID_CHILD( n ) ::nemesis::gui::Id_Cat( NE_UI_ID, n )

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// System

	Result_t	NE_API System_Initialize	( Allocator_t alloc, Renderer_t renderer );
	void		NE_API System_Shutdown		();
	Renderer_t	NE_API System_GetRenderer	();
	FontCache_t	NE_API System_GetFontCache	();
	Context_t	NE_API System_GetContext	();
	
} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Context

	void				NE_API Context_BeginFrame		( Context_t dc, const Rect_s& clip, const Mouse_s& mouse, const Keyboard_s& kb );
	void				NE_API Context_EndFrame			( Context_t dc, DrawBatch_s* batch );
	void				NE_API Context_BeginFlush		( Context_t dc, DrawBatch_s* batch );
	void				NE_API Context_EndFlush			( Context_t dc );
	Theme_t				NE_API Context_GetTheme			( Context_t dc );
	StateCache_t		NE_API Context_GetCache			( Context_t dc );
	ScratchBuffer_t		NE_API Context_GetScratch		( Context_t dc );
	Graphics_t			NE_API Context_GetGraphics		( Context_t dc );
	Rect_s				NE_API Context_GetDektop		( Context_t dc );
	Rect_s				NE_API Context_GetChild			( Context_t dc );
	Vec2_s				NE_API Context_GetView			( Context_t dc );
	const CaretInfo_s&	NE_API Context_GetCaret			( Context_t dc );
	const DragInfo_s&	NE_API Context_GetDrag			( Context_t dc );
	const Mouse_s&		NE_API Context_GetMouse			( Context_t dc );
	const Keyboard_s&	NE_API Context_GetKeyboard		( Context_t dc );
	bool				NE_API Context_IsWnd			( Context_t dc, Id_t id );
	bool				NE_API Context_IsHot			( Context_t dc, Id_t id );
	bool				NE_API Context_IsPushed			( Context_t dc, Id_t id );
	bool				NE_API Context_IsDragging		( Context_t dc, Id_t id );
	bool				NE_API Context_HasFocus			( Context_t dc, Id_t id );
	Id_t				NE_API Context_GetWnd			( Context_t dc );
	Id_t				NE_API Context_GetHot			( Context_t dc );
	Id_t				NE_API Context_GetPushed		( Context_t dc );
	Id_t				NE_API Context_GetFocus			( Context_t dc );
	void				NE_API Context_SetWnd			( Context_t dc, Id_t id );
	void				NE_API Context_SetHot			( Context_t dc, Id_t id );
	void				NE_API Context_SetFocus			( Context_t dc, Id_t id );
	void				NE_API Context_SetPushed		( Context_t dc, Id_t id );
	void				NE_API Context_BeginDrag		( Context_t dc, Id_t id, const Vec2_s& pos );
	void				NE_API Context_BeginDrag		( Context_t dc, Id_t id, const Vec2_s& pos, MouseButton::Enum button );
	void				NE_API Context_BeginModal		( Context_t dc );
	void				NE_API Context_EndModal			( Context_t dc );
	void				NE_API Context_BeginChild		( Context_t dc, const Rect_s& r );
	void				NE_API Context_EndChild			( Context_t dc );
	void				NE_API Context_BeginView		( Context_t dc, const Vec2_s& offset );
	void				NE_API Context_EndView			( Context_t dc );
	void				NE_API Context_BeginHost		( Context_t dc, const Rect_s& clip, const Vec2_s& mouse );
	void				NE_API Context_EndHost			( Context_t dc, DrawBatch_s* batch );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Context Extensions

	bool	NE_API Context_Cull			( Context_t dc, const Rect_s& r );										/// local clip rect intersection
	Font_t	NE_API Context_GetFont		( Context_t dc );														/// current font (or theme's main font)
	float	NE_API Context_GetLineHeight( Context_t dc );														/// line height of current theme's main font
	Vec2_s	NE_API Context_GetMousePos	( Context_t dc );														/// local mouse pos
	Rect_s	NE_API Context_GetClipRect	( Context_t dc );														/// local clip rect
	void	NE_API Context_BeginDragOnce( Context_t dc, Id_t id, const Vec2_s& pos );							/// begin drag without override
	void	NE_API Context_BeginDragOnce( Context_t dc, Id_t id, const Vec2_s& pos, MouseButton::Enum button );	/// begin drag without override
	Text_s	NE_API Context_FormatStringV( Context_t dc, cstr_t  text, va_list args );							/// scratch string
	Text_s	NE_API Context_FormatStringV( Context_t dc, wcstr_t text, va_list args );							/// scratch string
	Text_s	NE_API Context_FormatString	( Context_t dc, cstr_t  text, ... );									/// scratch string
	Text_s	NE_API Context_FormatString	( Context_t dc, wcstr_t text, ... );									/// scratch string
	void*	NE_API Context_CacheIntern	( Context_t dc, Id_t id, size_t size );									/// cached ctrl state

	template < typename T >
	inline T& Context_CacheIntern( Context_t dc, Id_t id )
	{ return *((T*)Context_CacheIntern( dc, id, sizeof(T) )); }

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Graphics

	Vec2_s	NE_API Graphics_GetTransform		( Graphics_t g );
	void	NE_API Graphics_SetClipRect			( Graphics_t g, const Rect_s* r );
	void	NE_API Graphics_SetTexture			( Graphics_t g, Texture_t texture, const Vec2_s& white_uv );
	void	NE_API Graphics_DrawLine			( Graphics_t g, const Vec2_s& a, const Vec2_s& b, uint32_t argb );
	void	NE_API Graphics_DrawLine			( Graphics_t g, const Vec2_s& a, const Vec2_s& b, uint32_t argb, float thickness );
	void	NE_API Graphics_DrawRect			( Graphics_t g, const Rect_s& r, uint32_t argb );
	void	NE_API Graphics_FillRect			( Graphics_t g, const Rect_s& r, uint32_t argb );
	void	NE_API Graphics_FillRect			( Graphics_t g, const Rect_s& r, const Vec2_s& uv0, const Vec2_s& uv1, uint32_t argb );
	void	NE_API Graphics_FillRect			( Graphics_t g, const Rect_s& r, const Vec2_s& uv0, const Vec2_s& uv1, uint32_t argb );
	void	NE_API Graphics_FillRect			( Graphics_t g, const Rect_s& r, uint32_t argb0, uint32_t argb1, uint32_t argb2, uint32_t argb3 );
	void	NE_API Graphics_FillRect			( Graphics_t g, const Rect_s& r, uint32_t argb1, uint32_t argb2, LinearGradient::Mode m );
	void	NE_API Graphics_DrawCircle			( Graphics_t g, const Vec2_s& center, float radius, uint32_t argb );
	void	NE_API Graphics_DrawCircle			( Graphics_t g, const Vec2_s& center, float radius, int num_seg, uint32_t argb );
	void	NE_API Graphics_FillCircle			( Graphics_t g, const Vec2_s& center, float radius, uint32_t argb );
	void	NE_API Graphics_FillCircle			( Graphics_t g, const Vec2_s& center, float radius, int num_seg, uint32_t argb );
	void	NE_API Graphics_DrawTriangle		( Graphics_t g, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb );
	void	NE_API Graphics_FillTriangle		( Graphics_t g, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb );
	void	NE_API Graphics_DrawBezierCurve		( Graphics_t g, const Vec2_s& p0, const Vec2_s& cp0, const Vec2_s& cp1, const Vec2_s& p1, uint32_t argb );
	void	NE_API Graphics_DrawBezierCurve		( Graphics_t g, const Vec2_s& p0, const Vec2_s& cp0, const Vec2_s& cp1, const Vec2_s& p1, uint32_t argb, float thickness );
	Vec2_s	NE_API Graphics_DrawString			( Graphics_t g, const Vec2_s& p, cstr_t  t, int len, Font_t f, uint32_t argb );
	Vec2_s	NE_API Graphics_DrawString			( Graphics_t g, const Vec2_s& p, wcstr_t t, int len, Font_t f, uint32_t argb );
	Vec2_s	NE_API Graphics_DrawString			( Graphics_t g, const Vec2_s& p, const Text_s& text, Font_t f, uint32_t argb );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, cstr_t  t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt, uint32_t argb );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, cstr_t  t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale );
	void	NE_API Graphics_DrawString			( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, const Rect_s& r, cstr_t  t, int len, Font_t f, TextFormat::Mask_t fmt );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, cstr_t  t, int len, Font_t f );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, wcstr_t t, int len, Font_t f );
	Rect_s	NE_API Graphics_MeasureString		( Graphics_t g, const Text_s& text, Font_t f );
	float	NE_API Graphics_CaretToCoord		( Graphics_t g, cstr_t  t, int len, Font_t f, int caret );
	float	NE_API Graphics_CaretToCoord		( Graphics_t g, wcstr_t t, int len, Font_t f, int caret );
	float	NE_API Graphics_CaretToCoord		( Graphics_t g, const Text_s& text, Font_t f, int caret );
	int		NE_API Graphics_CoordToCaret		( Graphics_t g, cstr_t  t, int len, Font_t f, float coord );
	int		NE_API Graphics_CoordToCaret		( Graphics_t g, wcstr_t t, int len, Font_t f, float coord );
	int		NE_API Graphics_CoordToCaret		( Graphics_t g, const Text_s& text, Font_t f, float coord );

	/// v1

	void	NE_API Graphics_DrawBorder			( Graphics_t g, const Rect_s& r, uint32_t argb, const Vec2_s& size );
	void	NE_API Graphics_DrawBorder			( Graphics_t g, const Rect_s& r, uint32_t argb, float thickness );
	void	NE_API Graphics_DrawBox				( Graphics_t g, const Rect_s& r, uint32_t back, uint32_t front );
	void	NE_API Graphics_DrawRadio			( Graphics_t g, const Rect_s& r, uint32_t back, uint32_t front );
	float	NE_API Graphics_GetLineHeight		( Graphics_t g, Font_t f );
	float	NE_API Graphics_GetMaxCharWidth		( Graphics_t g, Font_t f );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// FontCache

	int			 NE_API FontCache_GetNumFonts	( FontCache_t cache );
	bool		 NE_API FontCache_GetFontKey	( FontCache_t cache, int idx, FontKey_s& key );
	Font_t		 NE_API FontCache_GetFont		( FontCache_t cache, int idx );
	Font_t		 NE_API FontCache_CreateFont	( FontCache_t cache, const FontKey_s& key );
	Font_t		 NE_API FontCache_CreateFont	( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style );
	Font_t		 NE_API FontCache_CreateGdiFont	( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style );
	Font_t		 NE_API FontCache_CreateStbFont	( FontCache_t cache, wcstr_t name, float size, FontStyle::Mask style );
	Result_t	 NE_API FontCache_ReplaceRange	( FontCache_t cache, Font_t f, wcstr_t name, FontStyle::Mask style, wchar_t first, wchar_t last );
	Result_t	 NE_API FontCache_PreloadChars	( FontCache_t cache, Font_t f, wchar_t s, wchar_t e, FontInfo_s& info );
	Result_t	 NE_API FontCache_PreloadText	( FontCache_t cache, Font_t f, wcstr_t t, int l, FontInfo_s& info );
	Result_t	 NE_API FontCache_PreloadText	( FontCache_t cache, Font_t f, cstr_t  t, int l, FontInfo_s& info );
	Result_t	 NE_API FontCache_GetFontInfo	( FontCache_t cache, Font_t f, FontInfo_s& info );
	ScaledFont_s NE_API FontCache_ScaleFont		( FontCache_t cache, Font_t font, float scale, float min_h, float max_h );
	int			 NE_API FontCache_FindFont		( FontCache_t cache, Font_t font );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Font

	const GlyphInfo_s* NE_API Font_FindGlyph( const FontInfo_s& font, wchar_t ch );

	Rect_s NE_API Font_MeasureString( const FontInfo_s& font, cstr_t  t, int l, TextFormat::Mask_t fmt, const Rect_s& r, float scale );
	Rect_s NE_API Font_MeasureString( const FontInfo_s& font, wcstr_t t, int l, TextFormat::Mask_t fmt, const Rect_s& r, float scale );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Scratch Buffer

	Text_s NE_API ScratchBuffer_FormatV( ScratchBuffer_t buffer, cstr_t  text, va_list args );
	Text_s NE_API ScratchBuffer_FormatV( ScratchBuffer_t buffer, wcstr_t text, va_list args );
	Text_s NE_API ScratchBuffer_Format( ScratchBuffer_t buffer, cstr_t  text, ... );
	Text_s NE_API ScratchBuffer_Format( ScratchBuffer_t buffer, wcstr_t text, ... );

} } 

//======================================================================================
namespace nemesis { namespace gui
{
	/// State Cache

	void* NE_API StateCache_Ensure( StateCache_t cache, Id_t id, size_t size );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Theme

	void NE_API Theme_ApplyPreset( Theme_t theme, ThemePreset::Enum preset );

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Transform

	Vec2_s NE_API Transform_Apply	( const Transform_s& t, const Vec2_s& v );
	Vec2_s NE_API Transform_Unapply	( const Transform_s& t, const Vec2_s& v );
	Rect_s NE_API Transform_Apply	( const Transform_s& t, const Rect_s& r );
	Rect_s NE_API Transform_Unapply	( const Transform_s& t, const Rect_s& r );

} }
