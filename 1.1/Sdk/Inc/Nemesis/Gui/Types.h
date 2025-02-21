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
#include <Nemesis/Core/ArrayTypes.h>
#include <Nemesis/Core/InputTypes.h>
#include <Nemesis/Core/Math.h>

//======================================================================================
namespace nemesis { namespace gui
{
	/// Types

	typedef uint32_t Id_t;
	typedef Handle_t Font_t;
	typedef Handle_t Texture_t;

	typedef struct Renderer_s		*Renderer_t;
	typedef struct Graphics_s		*Graphics_t;
	typedef struct Context_s		*Context_t;
	typedef struct Theme_s			*Theme_t;
	typedef struct FontCache_s		*FontCache_t;
	typedef struct StateCache_s		*StateCache_t;
	typedef struct ScratchBuffer_s	*ScratchBuffer_t;

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Draw

	typedef uint16_t DrawIdx_t;

	struct DrawVtx_s
	{
		Vec2_s Pos;
		Vec2_s UV;
		uint32_t Color;
	};

	struct DrawCmd
	{
		enum Enum
		{ None = 0
		, Clip = 1
		};
		typedef uint32_t Mask;
	};

	struct DrawCmd_s
	{
		Texture_t		Texture;
		Rect_s			Clip;
		DrawCmd::Mask	Flags;
		uint32_t		NumIdx;
	};

	struct DrawBatch_s
	{
		const DrawCmd_s* Cmd;
		const DrawIdx_t* Idx;
		const DrawVtx_s* Vtx;
		uint32_t NumCmd;
		uint32_t NumIdx;
		uint32_t NumVtx;
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Renderer

	struct TextureFormat
	{
		enum Enum
		{ A8
		, A8R8G8B8
		};
	};

	struct TextureLock
	{
		enum Mode
		{ Write
		, Read
		};
	};

	struct TextureInfo_s
	{
		int Width;
		int Height;
		TextureFormat::Enum Format;
	};

	struct TextureLock_s
	{
		void* Data;
		int Pitch;
	};

	struct Renderer_v
	{
		bool (NE_CALLBK *CreateTexture) ( Renderer_t renderer, const TextureInfo_s& info, Texture_t* texture );
		bool (NE_CALLBK *GetTextureInfo)( Renderer_t renderer, Texture_t texture, TextureInfo_s& info );
		bool (NE_CALLBK *LockTexture)	( Renderer_t renderer, Texture_t texture, TextureLock::Mode mode, TextureLock_s& info );
		void (NE_CALLBK *UnlockTexture) ( Renderer_t renderer, Texture_t texture );
		void (NE_CALLBK *ReleaseTexture)( Renderer_t renderer, Texture_t texture );
	};

	struct Renderer_s
	{
		const Renderer_v* Api;
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Input

	typedef MouseState_s	Mouse_s;
	typedef KeyboardState_s Keyboard_s;
	typedef KeyState_s		KeyInfo_s;

	struct DragInfo_s
	{
		Id_t				Id;
		Vec2_s				Pos;
		MouseButton::Enum	Button;
	};

	struct CaretInfo_s
	{
		bool	Show;
		int64_t Tick;
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Font

	struct GlyphInfo_s
	{
		Vec2_s Pos[2];
		Vec2_s UV [2];
		float  Advance;
		float  Offset;
	};

	struct CharInfo_s
	{
		wchar_t	 Char;
		uint16_t Glyph;
	};

	struct FontInfo_s
	{
		const CharInfo_s*	Char;
		const GlyphInfo_s*	Glyph;
		int32_t				NumChars;
		int32_t				NumGlyphs;
		Texture_t			Texture;
		Vec2_s				WhiteUV;
		float				FontHeight;
		float				LineHeight;
		float				MaxCharWidth;
		wchar_t				BreakChar;
		wchar_t				DefaultChar;
	};

	struct FontStyle
	{
		enum Flags
		{
			Default		= 0x0000,
			Bold		= 0x0001,
			SemiBold	= 0x0002,
			IconSet		= 0x1000,
		};

		typedef uint32_t Mask;
	};

	struct FontMapper
	{
		enum Enum
		{ Gdi
		, Stb
		};
	};

	struct FontKey_s
	{
		FontMapper::Enum	Mapper;
		FontStyle::Mask		Style;
		int32_t				Height;
		int32_t				_pad0_;
		wchar_t				Name[64];
	};

	struct ScaledFont_s
	{
		Font_t Font;
		float  DrawScale;
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Graphics

	struct LinearGradient
	{
		enum Mode 
		{
			Vertical,
			Horizontal,
			ForwardDiagonal,
			BackwardDiagonal,
		};
	};

	struct TextFormat
	{
		enum Flags
		{
			Default	= 0x0000,
			Left	= 0x0000,
			Center	= 0x0010,
			Right	= 0x0020,
			Top		= 0x0000,
			Middle	= 0x0100,
			Bottom	= 0x0200,
			NoWrap	= 0x1000,
			NoClip	= 0x2000,
		};

		enum Align
		{
			Horz = Left | Center | Right,
			Vert = Top | Middle | Bottom,
		};

		typedef uint32_t Mask_t;
	};

	struct Text_s
	{
		cstr_t  Utf8;
		wcstr_t Utf16;
		int NumChars;

		Text_s()						 : Utf8( nullptr ), Utf16( nullptr ), NumChars( 0 )   {}
		Text_s( cstr_t  text )			 : Utf8( text )	  , Utf16( nullptr ), NumChars( -1 )  {}
		Text_s( wcstr_t text )			 : Utf8( nullptr ), Utf16( text )   , NumChars( -1 )  {}
		Text_s( cstr_t  text , int len ) : Utf8( text )	  , Utf16( nullptr ), NumChars( len ) {}
		Text_s( wcstr_t text , int len ) : Utf8( nullptr ), Utf16( text )   , NumChars( len ) {}
	};

	struct TextList_s
	{
		const cstr_t* Utf8;
		const wcstr_t* Utf16;
		int NumItems;

		TextList_s()								 { Utf8 = nullptr	; Utf16 = nullptr; NumItems = 0;	 }
		TextList_s( const cstr_t*  item, int count ) { Utf8 = item		; Utf16 = nullptr; NumItems = count; }
		TextList_s( const wcstr_t* item, int count ) { Utf8 = nullptr	; Utf16 = item	 ; NumItems = count; }

		Text_s operator [] ( int index ) const 
		{ 
			if ( (index < 0) || (index > NumItems) )
				return Text_s();
			if (Utf8)
				return Utf8[index];
			if (Utf16)
				return Utf16[index];
			return Text_s();
		}
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Generic

	struct CtrlState
	{ 
		enum Enum
		{ Normal
		, Hot
		, Pushed
		, Disabled
		, COUNT
		};
	};

	struct Direction
	{
		enum Enum
		{ Horizontal
		, Vertical
		};
	};

	struct ScrollInfo_s
	{
		Vec2_s Size;
		Vec2_s Offset;
	};

	struct Transform_s 
	{
		Vec2_s Offset;
	};

	struct ScratchBuffer_s
	{
		wchar_t Data[1024];
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Theme

	struct Metric
	{
		enum Enum
		{ Button_Margin
		, CheckBox_Margin
		, CheckBox_Box_Size
		, CheckBox_Check_Margin
		, TextEdit_Margin
		, Progress_Track_Size
		, Progress_Bar_Margin
		, Slider_Track_Size
		, Slider_Thumb_Size
		, ScrollBar_Arrow_Size
		, ScrollBar_Arrow_Margin
		, ScrollBar_Thumb_Margin
		, ScrollBar_Thumb_Min_Size
		, ScrollBar_Wheel_Lines
		, ListHeader_Item_Margin
		, ListHeader_Item_Min_Size
		, ListHeader_Item_Divider_Size
		, ListHeader_Item_Icon_Size
		, ListHeader_Item_Icon_Margin
		, ListColumn_Item_Margin
		, Tree_Item_Margin
		, Tree_Item_Indent
		, Tree_Item_Icon_Size
		, Tree_Item_Icon_Margin
		, Tree_Item_Text_Indent
		, Tab_Item_Margin
		, Tab_Item_Indent
		, Tab_Item_Enlarge
		, Menu_Main_Item_Margin
		, Menu_PopUp_Item_Margin
		, Menu_PopUp_Item_Icon_Size
		, Menu_PopUp_Item_Icon_Margin
		, Menu_PopUp_Item_Text_Indent
		, Menu_PopUp_Item_Expander_Size
		, Menu_PopUp_Item_Expander_Margin
		, Menu_PopUp_Separator_Size
		, DropDown_Border
		, DropDown_Item_Margin
		, DropDown_Box_Button_Size
		, DropDown_Box_Button_Margin
		, Caption_Margin
		, Expander_Margin
		, Expander_Spacing
		, Expander_Button_Size
		, Expander_Button_Margin
		, Window_Client_Margin
		, Window_Min_Size
		, ToolBar_Margin
		, ToolBar_Spacing
		, Splitter_Size
		, Frame_Tab_Item_Margin
		, Frame_Tab_Item_Indent
		, Frame_Tab_Item_Enlarge
		, Frame_Border_Size
		, Frame_Border_Thin_Size
		, TearOff_Threshold
		, DockIndicator_Button_Size
		, DockIndicator_Button_Spacing
		, Panel_Min_Size
		, COUNT
		};
	};

	struct Visual
	{
		enum Enum
		{ Label
		, Button
		, CheckBox_Box
		, CheckBox_Check
		, TextEdit
		, TextEdit_Caret
		, TextEdit_Selection
		, Progress_Track
		, Progress_Bar
		, Slider_Track
		, Slider_Thumb
		, ScrollBar_Track
		, ScrollBar_Thumb
		, ScrollBar_Arrow
		, ListHeader_Item
		, ListColumn_Item
		, ListColumn_Selection
		, List
		, List_Grid
		, Tree
		, Tree_Item
		, Tree_Item_Arrow
		, Tree_Selection
		, Tab
		, Tab_Item
		, Menu_Main
		, Menu_Main_Item
		, Menu_PopUp
		, Menu_PopUp_Icon
		, Menu_PopUp_Item
		, Menu_PopUp_Item_Arrow
		, Menu_PopUp_Separator
		, DropDown
		, DropDown_Item
		, DropDown_Box
		, Caption
		, Expander
		, Expander_Arrow
		, Window
		, ToolBar
		, Splitter
		, Frame_Tab
		, Frame_Tab_Item
		, Frame_Border
		, DockIndicator
		, DockIndicator_Target
		, COUNT
		};
	};

	struct Visual_s
	{
		uint32_t Front[ CtrlState::COUNT ];
		uint32_t Back [ CtrlState::COUNT ];
		uint32_t Text [ CtrlState::COUNT ];
	};

	struct Theme_s
	{
		Font_t	 Font;
		Visual_s Visual[ Visual::COUNT ];
		Vec2_s	 Metric[ Metric::COUNT ];
	};

	struct ThemePreset
	{
		enum Enum
		{ Default
		, Dark
		};
	};

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Color

	struct Color
	{
        static const uint32_t AliceBlue				= 0xFFF0F8FF;
        static const uint32_t AntiqueWhite			= 0xFFFAEBD7;
        static const uint32_t Aqua					= 0xFF00FFFF;
        static const uint32_t Aquamarine			= 0xFF7FFFD4;
        static const uint32_t Azure					= 0xFFF0FFFF;
        static const uint32_t Beige					= 0xFFF5F5DC;
        static const uint32_t Bisque				= 0xFFFFE4C4;
        static const uint32_t Black					= 0xFF000000;
        static const uint32_t BlanchedAlmond		= 0xFFFFEBCD;
        static const uint32_t Blue					= 0xFF0000FF;
        static const uint32_t BlueViolet			= 0xFF8A2BE2;
        static const uint32_t Brown					= 0xFFA52A2A;
        static const uint32_t BurlyWood				= 0xFFDEB887;
        static const uint32_t CadetBlue				= 0xFF5F9EA0;
        static const uint32_t Chartreuse			= 0xFF7FFF00;
        static const uint32_t Chocolate				= 0xFFD2691E;
        static const uint32_t Coral					= 0xFFFF7F50;
        static const uint32_t CornflowerBlue		= 0xFF6495ED;
        static const uint32_t Cornsilk				= 0xFFFFF8DC;
        static const uint32_t Crimson				= 0xFFDC143C;
        static const uint32_t Cyan					= 0xFF00FFFF;
        static const uint32_t DarkBlue 				= 0xFF00008B;
        static const uint32_t DarkCyan 				= 0xFF008B8B;
        static const uint32_t DarkGoldenrod			= 0xFFB8860B;
        static const uint32_t DarkGray				= 0xFFA9A9A9;
        static const uint32_t DarkGreen				= 0xFF006400;
        static const uint32_t DarkKhaki				= 0xFFBDB76B;
        static const uint32_t DarkMagenta 			= 0xFF8B008B;
        static const uint32_t DarkOliveGreen		= 0xFF556B2F;
        static const uint32_t DarkOrange 			= 0xFFFF8C00;
        static const uint32_t DarkOrchid 			= 0xFF9932CC;
        static const uint32_t DarkRed				= 0xFF8B0000;
        static const uint32_t DarkSalmon			= 0xFFE9967A;
        static const uint32_t DarkSeaGreen			= 0xFF8FBC8F;
        static const uint32_t DarkSlateBlue 		= 0xFF483D8B;
        static const uint32_t DarkSlateGray 		= 0xFF2F4F4F;
        static const uint32_t DarkTurquoise 		= 0xFF00CED1;
        static const uint32_t DarkViolet			= 0xFF9400D3;
        static const uint32_t DeepPink				= 0xFFFF1493;
        static const uint32_t DeepSkyBlue			= 0xFF00BFFF;
        static const uint32_t DimGray				= 0xFF696969;
        static const uint32_t DodgerBlue			= 0xFF1E90FF;
        static const uint32_t Firebrick				= 0xFFB22222;
        static const uint32_t FloralWhite			= 0xFFFFFAF0;
        static const uint32_t ForestGreen			= 0xFF228B22;
        static const uint32_t Fuchsia				= 0xFFFF00FF;
        static const uint32_t Gainsboro				= 0xFFDCDCDC;
        static const uint32_t GhostWhite			= 0xFFF8F8FF;
        static const uint32_t Gold					= 0xFFFFD700;
        static const uint32_t Goldenrod				= 0xFFDAA520;
        static const uint32_t Gray					= 0xFF808080;
        static const uint32_t Green					= 0xFF008000;
        static const uint32_t GreenYellow			= 0xFFADFF2F;
        static const uint32_t Honeydew				= 0xFFF0FFF0;
        static const uint32_t HotPink				= 0xFFFF69B4;
        static const uint32_t IndianRed				= 0xFFCD5C5C;
        static const uint32_t Indigo				= 0xFF4B0082;
        static const uint32_t Ivory					= 0xFFFFFFF0;
        static const uint32_t Khaki					= 0xFFF0E68C;
        static const uint32_t Lavender				= 0xFFE6E6FA;
        static const uint32_t LavenderBlush			= 0xFFFFF0F5;
        static const uint32_t LawnGreen				= 0xFF7CFC00;
        static const uint32_t LemonChiffon			= 0xFFFFFACD;
        static const uint32_t LightBlue				= 0xFFADD8E6;
        static const uint32_t LightCoral			= 0xFFF08080;
        static const uint32_t LightCyan				= 0xFFE0FFFF;
        static const uint32_t LightGoldenrodYellow	= 0xFFFAFAD2;
        static const uint32_t LightGreen			= 0xFF90EE90;
        static const uint32_t LightGray				= 0xFFD3D3D3;
        static const uint32_t LightPink				= 0xFFFFB6C1;
        static const uint32_t LightSalmon			= 0xFFFFA07A;
        static const uint32_t LightSeaGreen			= 0xFF20B2AA;
        static const uint32_t LightSkyBlue			= 0xFF87CEFA;
        static const uint32_t LightSlateGray		= 0xFF778899;
        static const uint32_t LightSteelBlue		= 0xFFB0C4DE;
        static const uint32_t LightYellow			= 0xFFFFFFE0;
        static const uint32_t Lime					= 0xFF00FF00;
        static const uint32_t LimeGreen				= 0xFF32CD32;
        static const uint32_t Linen					= 0xFFFAF0E6;
        static const uint32_t Magenta				= 0xFFFF00FF;
        static const uint32_t Maroon				= 0xFF800000;
        static const uint32_t MediumAquamarine		= 0xFF66CDAA;
        static const uint32_t MediumBlue			= 0xFF0000CD;
        static const uint32_t MediumOrchid			= 0xFFBA55D3;
        static const uint32_t MediumPurple			= 0xFF9370DB;
        static const uint32_t MediumSeaGreen		= 0xFF3CB371;
        static const uint32_t MediumSlateBlue		= 0xFF7B68EE;
        static const uint32_t MediumSpringGreen		= 0xFF00FA9A;
        static const uint32_t MediumTurquoise		= 0xFF48D1CC;
        static const uint32_t MediumVioletRed		= 0xFFC71585;
        static const uint32_t MidnightBlue			= 0xFF191970;
        static const uint32_t MintCream				= 0xFFF5FFFA;
        static const uint32_t MistyRose				= 0xFFFFE4E1;
        static const uint32_t Moccasin				= 0xFFFFE4B5;
        static const uint32_t NavajoWhite			= 0xFFFFDEAD;
        static const uint32_t Navy					= 0xFF000080;
        static const uint32_t OldLace				= 0xFFFDF5E6;
        static const uint32_t Olive					= 0xFF808000;
        static const uint32_t OliveDrab				= 0xFF6B8E23;
        static const uint32_t Orange				= 0xFFFFA500;
        static const uint32_t OrangeRed				= 0xFFFF4500;
        static const uint32_t Orchid				= 0xFFDA70D6;
        static const uint32_t PaleGoldenrod			= 0xFFEEE8AA;
        static const uint32_t PaleGreen				= 0xFF98FB98;
        static const uint32_t PaleTurquoise			= 0xFFAFEEEE;
        static const uint32_t PaleVioletRed			= 0xFFDB7093;
        static const uint32_t PapayaWhip			= 0xFFFFEFD5;
        static const uint32_t PeachPuff				= 0xFFFFDAB9;
        static const uint32_t Peru					= 0xFFCD853F;
        static const uint32_t Pink					= 0xFFFFC0CB;
        static const uint32_t Plum					= 0xFFDDA0DD;
        static const uint32_t PowderBlue			= 0xFFB0E0E6;
        static const uint32_t Purple				= 0xFF800080;
        static const uint32_t Red					= 0xFFFF0000;
        static const uint32_t RosyBrown				= 0xFFBC8F8F;
        static const uint32_t RoyalBlue				= 0xFF4169E1;
        static const uint32_t SaddleBrown			= 0xFF8B4513;
        static const uint32_t Salmon				= 0xFFFA8072;
        static const uint32_t SandyBrown			= 0xFFF4A460;
        static const uint32_t SeaGreen				= 0xFF2E8B57;
        static const uint32_t SeaShell				= 0xFFFFF5EE;
        static const uint32_t Sienna				= 0xFFA0522D;
        static const uint32_t Silver				= 0xFFC0C0C0;
        static const uint32_t SkyBlue				= 0xFF87CEEB;
        static const uint32_t SlateBlue				= 0xFF6A5ACD;
        static const uint32_t SlateGray				= 0xFF708090;
        static const uint32_t Snow					= 0xFFFFFAFA;
        static const uint32_t SpringGreen			= 0xFF00FF7F;
        static const uint32_t SteelBlue				= 0xFF4682B4;
        static const uint32_t Tan					= 0xFFD2B48C;
        static const uint32_t Teal					= 0xFF008080;
        static const uint32_t Thistle				= 0xFFD8BFD8;
        static const uint32_t Tomato				= 0xFFFF6347;
        static const uint32_t Turquoise				= 0xFF40E0D0;
        static const uint32_t Violet				= 0xFFEE82EE;
        static const uint32_t Wheat					= 0xFFF5DEB3;
        static const uint32_t White					= 0xFFFFFFFF;
        static const uint32_t WhiteSmoke			= 0xFFF5F5F5;
        static const uint32_t Yellow				= 0xFFFFFF00;
        static const uint32_t YellowGreen			= 0xFF9ACD32;
	};

} }
