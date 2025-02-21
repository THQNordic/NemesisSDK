//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Theme.h"

//======================================================================================
namespace nemesis { namespace gui 
{
	static uint32_t Color_ARGB( uint8_t a, uint8_t r, uint8_t g, uint8_t b )
	{
		const uint32_t a32 = a;
		const uint32_t r32 = r;
		const uint32_t g32 = g;
		const uint32_t b32 = b;
		return (a32 << 24) 
			 | (r32 << 16)
			 | (g32 <<  8)
			 | (b32 <<  0);
	}

	static uint32_t Color_RGB( uint8_t r, uint8_t g, uint8_t b )
	{
		return Color_ARGB( 0xff, r, g, b );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	void Theme_ResetMetrics( Theme_t theme )
	{
		theme->Metric[ Metric::Button_Margin					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::CheckBox_Margin					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::CheckBox_Box_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::CheckBox_Check_Margin			] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::TextEdit_Margin					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Progress_Track_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::Progress_Bar_Margin				] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Slider_Track_Size				] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Slider_Thumb_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::ScrollBar_Arrow_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::ScrollBar_Arrow_Margin			] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::ScrollBar_Thumb_Margin			] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::ScrollBar_Thumb_Min_Size			] = Vec2_s {  10.0f,  10.0f };
		theme->Metric[ Metric::ScrollBar_Wheel_Lines			] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::ListHeader_Item_Margin			] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::ListHeader_Item_Min_Size			] = Vec2_s {   8.0f,   8.0f };
		theme->Metric[ Metric::ListHeader_Item_Divider_Size		] = Vec2_s {   8.0f,   8.0f };
		theme->Metric[ Metric::ListHeader_Item_Icon_Size		] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::ListHeader_Item_Icon_Margin		] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::ListColumn_Item_Margin			] = Vec2_s {   4.0f,   2.0f };
		theme->Metric[ Metric::Tree_Item_Margin					] = Vec2_s {   4.0f,   2.0f };
		theme->Metric[ Metric::Tree_Item_Indent					] = Vec2_s {  10.0f,   0.0f };
		theme->Metric[ Metric::Tree_Item_Icon_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::Tree_Item_Icon_Margin			] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::Tree_Item_Text_Indent			] = Vec2_s {   4.0f,   0.0f };
		theme->Metric[ Metric::Tab_Item_Indent					] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Tab_Item_Margin					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Tab_Item_Enlarge					] = Vec2_s {   0.0f,   2.0f };
		theme->Metric[ Metric::Menu_Main_Item_Margin			] = Vec2_s {  10.0f,   2.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Margin			] = Vec2_s {   4.0f,   2.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Icon_Size		] = Vec2_s {  32.0f,  16.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Icon_Margin		] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Text_Indent		] = Vec2_s {   4.0f,   0.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Expander_Size	] = Vec2_s {  16.0f,  16.0f };
		theme->Metric[ Metric::Menu_PopUp_Item_Expander_Margin	] = Vec2_s {   4.0f,   2.0f };
		theme->Metric[ Metric::Menu_PopUp_Separator_Size		] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::DropDown_Border					] = Vec2_s {   1.0f,   1.0f };
		theme->Metric[ Metric::DropDown_Item_Margin				] = Vec2_s {   4.0f,   2.0f };
		theme->Metric[ Metric::DropDown_Box_Button_Size			] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::DropDown_Box_Button_Margin		] = Vec2_s {   3.0f,   8.0f };
		theme->Metric[ Metric::Caption_Margin					] = Vec2_s {   8.0f,   4.0f };
		theme->Metric[ Metric::Expander_Margin					] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Expander_Spacing					] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Expander_Button_Size				] = Vec2_s {  14.0f,  14.0f };
		theme->Metric[ Metric::Expander_Button_Margin			] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::Window_Client_Margin				] = Vec2_s {   1.0f,   1.0f };
		theme->Metric[ Metric::Window_Min_Size					] = Vec2_s {  40.0f,  40.0f };
		theme->Metric[ Metric::ToolBar_Margin					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::ToolBar_Spacing					] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Splitter_Size					] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Frame_Tab_Item_Margin			] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Frame_Tab_Item_Indent			] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::Frame_Tab_Item_Enlarge			] = Vec2_s {   0.0f,   2.0f };
		theme->Metric[ Metric::Frame_Border_Size				] = Vec2_s {   3.0f,   3.0f };
		theme->Metric[ Metric::Frame_Border_Thin_Size			] = Vec2_s {   1.0f,   1.0f };
		theme->Metric[ Metric::TearOff_Threshold				] = Vec2_s {   2.0f,   2.0f };
		theme->Metric[ Metric::DockIndicator_Button_Size		] = Vec2_s {  32.0f,  32.0f };
		theme->Metric[ Metric::DockIndicator_Button_Spacing		] = Vec2_s {   4.0f,   4.0f };
		theme->Metric[ Metric::Panel_Min_Size					] = Vec2_s {  24.0f,  24.0f };
	}
	
	void Theme_ResetVisuals( Theme_t theme, const Palette_s& palette, float alpha )
	{
		for ( int i = 0; i < NeCountOf(theme->Visual); ++i )
		{
			for ( int s = 0; s < CtrlState::COUNT; ++s )
			{
				theme->Visual[ i ].Text[ s ] = palette.Text;
			}
		}

		const uint32_t normal_front		= palette.Normal_Bright;
		const uint32_t normal_back		= palette.Normal_Dark;
		const uint32_t hot_front		= palette.Active_Bright;
		const uint32_t hot_back			= palette.Active_Medium;
		const uint32_t pushed_front		= palette.Active_Medium;
		const uint32_t pushed_back		= palette.Active_Dark;
		const uint32_t disabled_front	= palette.Normal_Medium;
		const uint32_t disabled_back	= palette.Normal_Dark;
		const uint8_t  opacity			= (uint8_t)(NeClamp(alpha, 0.0f, 1.0f) * 255.0f);

		theme->Visual[ Visual::Label ]					.Front[ CtrlState::Normal	] = normal_front	;	
		theme->Visual[ Visual::Label ]					.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Label ]					.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Label ]					.Back [ CtrlState::Hot		] = hot_back		;	
		theme->Visual[ Visual::Label ]					.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Label ]					.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Label ]					.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Label ]					.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Button ]					.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Button ]					.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Button ]					.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Button ]					.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Button ]					.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Button ]					.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Button ]					.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Button ]					.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::CheckBox_Box ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::CheckBox_Box ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::CheckBox_Box ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::CheckBox_Box ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::CheckBox_Box ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::CheckBox_Box ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::CheckBox_Box ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::CheckBox_Box ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::CheckBox_Check ]			.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::CheckBox_Check ]			.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::CheckBox_Check ]			.Front[ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::CheckBox_Check ]			.Back [ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::CheckBox_Check ]			.Front[ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::CheckBox_Check ]			.Back [ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::CheckBox_Check ]			.Front[ CtrlState::Disabled	] = disabled_back	;
		theme->Visual[ Visual::CheckBox_Check ]			.Back [ CtrlState::Disabled	] = disabled_front	;

		theme->Visual[ Visual::TextEdit ]				.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::TextEdit ]				.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::TextEdit ]				.Front[ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::TextEdit ]				.Back [ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::TextEdit ]				.Front[ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::TextEdit ]				.Back [ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::TextEdit ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::TextEdit ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::TextEdit_Caret ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::TextEdit_Caret ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::TextEdit_Caret ]			.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::TextEdit_Caret ]			.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::TextEdit_Caret ]			.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::TextEdit_Caret ]			.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::TextEdit_Caret ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::TextEdit_Caret ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::TextEdit_Selection ]		.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::TextEdit_Selection ]		.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::TextEdit_Selection ]		.Front[ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::TextEdit_Selection ]		.Back [ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::TextEdit_Selection ]		.Front[ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::TextEdit_Selection ]		.Back [ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::TextEdit_Selection ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::TextEdit_Selection ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Progress_Track ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Progress_Track ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Progress_Track ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Progress_Track ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Progress_Track ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Progress_Track ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Progress_Track ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Progress_Track ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Progress_Bar ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Progress_Bar ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Progress_Bar ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Progress_Bar ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Progress_Bar ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Progress_Bar ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Progress_Bar ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Progress_Bar ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Slider_Track ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Slider_Track ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Slider_Track ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Slider_Track ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Slider_Track ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Slider_Track ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Slider_Track ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Slider_Track ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Slider_Thumb ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Slider_Thumb ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Slider_Thumb ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Slider_Thumb ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Slider_Thumb ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Slider_Thumb ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Slider_Thumb ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Slider_Thumb ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::ScrollBar_Track ]		.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ScrollBar_Track ]		.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ScrollBar_Track ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ScrollBar_Track ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ScrollBar_Track ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ScrollBar_Track ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ScrollBar_Track ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ScrollBar_Track ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::ScrollBar_Thumb ]		.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ScrollBar_Thumb ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::ScrollBar_Arrow ]		.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ScrollBar_Arrow ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::ListHeader_Item ]		.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ListHeader_Item ]		.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ListHeader_Item ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ListHeader_Item ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ListHeader_Item ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ListHeader_Item ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ListHeader_Item ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ListHeader_Item ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::ListColumn_Item ]		.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ListColumn_Item ]		.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ListColumn_Item ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ListColumn_Item ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ListColumn_Item ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ListColumn_Item ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ListColumn_Item ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ListColumn_Item ]		.Back [ CtrlState::Disabled	] = disabled_back	;
	//	theme->Visual[ Visual::ListColumn_Item ]		.Text [ CtrlState::Hot		] = hot_front		;
	//	theme->Visual[ Visual::ListColumn_Item ]		.Text [ CtrlState::Pushed	] = pushed_front	;

		theme->Visual[ Visual::ListColumn_Selection ]	.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ListColumn_Selection ]	.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ListColumn_Selection ]	.Front[ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::ListColumn_Selection ]	.Back [ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::ListColumn_Selection ]	.Front[ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::ListColumn_Selection ]	.Back [ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::ListColumn_Selection ]	.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ListColumn_Selection ]	.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::List ]					.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::List ]					.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::List ]					.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::List ]					.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::List ]					.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::List ]					.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::List ]					.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::List ]					.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::List_Grid ]				.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::List_Grid ]				.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::List_Grid ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::List_Grid ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::List_Grid ]				.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::List_Grid ]				.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::List_Grid ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::List_Grid ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Tree ]					.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tree ]					.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tree ]					.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::Tree ]					.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::Tree ]					.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::Tree ]					.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::Tree ]					.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tree ]					.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Tree_Item ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tree_Item ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tree_Item ]				.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Tree_Item ]				.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Tree_Item ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Tree_Item ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Tree_Item ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tree_Item ]				.Back [ CtrlState::Disabled	] = disabled_back	;
								   
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tree_Item_Arrow ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Tree_Selection ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tree_Selection ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tree_Selection ]			.Front[ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Tree_Selection ]			.Back [ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Tree_Selection ]			.Front[ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Tree_Selection ]			.Back [ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Tree_Selection ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tree_Selection ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Tab ]					.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tab ]					.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tab ]					.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Tab ]					.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Tab ]					.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Tab ]					.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Tab ]					.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tab ]					.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Tab_Item ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Tab_Item ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Tab_Item ]				.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Tab_Item ]				.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Tab_Item ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Tab_Item ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Tab_Item ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Tab_Item ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Menu_Main ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Menu_Main ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Menu_Main ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::Menu_Main ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::Menu_Main ]				.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::Menu_Main ]				.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::Menu_Main ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_Main ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Menu_Main_Item ]			.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Menu_Main_Item ]			.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Menu_Main_Item ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Menu_Main_Item ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Menu_Main_Item ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Menu_Main_Item ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Menu_Main_Item ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_Main_Item ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Menu_PopUp ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp ]				.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp ]				.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_PopUp ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_PopUp_Icon ]		.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Menu_PopUp_Item ]		.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_PopUp_Item ]		.Back [ CtrlState::Disabled	] = disabled_back	;
								   
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Front[ CtrlState::Normal	] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Back [ CtrlState::Normal	] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Front[ CtrlState::Hot		] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Back [ CtrlState::Hot		] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Front[ CtrlState::Pushed	] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Back [ CtrlState::Pushed	] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Front[ CtrlState::Disabled	] = palette.Text	;
		theme->Visual[ Visual::Menu_PopUp_Item_Arrow ]	.Back [ CtrlState::Disabled	] = palette.Text	;

		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Front[ CtrlState::Normal	] = disabled_back	;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Back [ CtrlState::Normal	] = disabled_front	;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Menu_PopUp_Separator ]	.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::DropDown ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::DropDown ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::DropDown ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::DropDown ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::DropDown ]				.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::DropDown ]				.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::DropDown ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::DropDown ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::DropDown_Item ]			.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::DropDown_Item ]			.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::DropDown_Item ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::DropDown_Item ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::DropDown_Item ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::DropDown_Item ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::DropDown_Item ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::DropDown_Item ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::DropDown_Box ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::DropDown_Box ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::DropDown_Box ]			.Front[ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::DropDown_Box ]			.Back [ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::DropDown_Box ]			.Front[ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::DropDown_Box ]			.Back [ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::DropDown_Box ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::DropDown_Box ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Caption ]				.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Caption ]				.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Caption ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::Caption ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::Caption ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Caption ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Caption ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Caption ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Expander ]				.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Expander ]				.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Expander ]				.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Expander ]				.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Expander ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Expander ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Expander ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Expander ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Expander_Arrow ]			.Front[ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Expander_Arrow ]			.Back [ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Expander_Arrow ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Expander_Arrow ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Expander_Arrow ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Expander_Arrow ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Expander_Arrow ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Expander_Arrow ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Window ]					.Front[ CtrlState::Normal	] = palette.Normal_Bright;
		theme->Visual[ Visual::Window ]					.Back [ CtrlState::Normal	] = palette.Normal_Medium;
		theme->Visual[ Visual::Window ]					.Front[ CtrlState::Hot		] = palette.Active_Bright;
		theme->Visual[ Visual::Window ]					.Back [ CtrlState::Hot		] = palette.Normal_Medium;
		theme->Visual[ Visual::Window ]					.Front[ CtrlState::Pushed	] = palette.Normal_Bright;
		theme->Visual[ Visual::Window ]					.Back [ CtrlState::Pushed	] = palette.Normal_Medium;
		theme->Visual[ Visual::Window ]					.Front[ CtrlState::Disabled	] = palette.Normal_Bright;
		theme->Visual[ Visual::Window ]					.Back [ CtrlState::Disabled	] = palette.Normal_Medium;

		theme->Visual[ Visual::ToolBar ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::ToolBar ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::ToolBar ]				.Front[ CtrlState::Hot		] = normal_front	;
		theme->Visual[ Visual::ToolBar ]				.Back [ CtrlState::Hot		] = normal_back		;
		theme->Visual[ Visual::ToolBar ]				.Front[ CtrlState::Pushed	] = normal_front	;
		theme->Visual[ Visual::ToolBar ]				.Back [ CtrlState::Pushed	] = normal_back		;
		theme->Visual[ Visual::ToolBar ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::ToolBar ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Splitter ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Splitter ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Splitter ]				.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Splitter ]				.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Splitter ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Splitter ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Splitter ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Splitter ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Frame_Tab ]				.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Frame_Tab ]				.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Frame_Tab ]				.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Frame_Tab ]				.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Frame_Tab ]				.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Frame_Tab ]				.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Frame_Tab ]				.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Frame_Tab ]				.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Frame_Tab_Item ]			.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Frame_Tab_Item ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::Frame_Border ]			.Front[ CtrlState::Normal	] = disabled_back	;
		theme->Visual[ Visual::Frame_Border ]			.Back [ CtrlState::Normal	] = disabled_front	;
		theme->Visual[ Visual::Frame_Border ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::Frame_Border ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::Frame_Border ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::Frame_Border ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::Frame_Border ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::Frame_Border ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		for ( int i = 0; i < NeCountOf(theme->Visual); ++i )
		{
			for ( int s = 0; s < CtrlState::COUNT; ++s )
			{
				theme->Visual[ i ].Front[ s ] = Color_SetAlpha( theme->Visual[ i ].Front[ s ], opacity );
				theme->Visual[ i ].Back [ s ] = Color_SetAlpha( theme->Visual[ i ].Back [ s ], opacity );
			}
		}

		for ( int s = 0; s < CtrlState::COUNT; ++s )
		{
			theme->Visual[ Visual::Menu_Main_Item  ].Front[ s ] = Color_SetAlpha( theme->Visual[ Visual::Menu_Main_Item  ].Front[ s ], 0 );
			theme->Visual[ Visual::Menu_PopUp_Item ].Front[ s ] = Color_SetAlpha( theme->Visual[ Visual::Menu_PopUp_Item ].Front[ s ], 0 );
		}

		theme->Visual[ Visual::DockIndicator ]			.Front[ CtrlState::Normal	] = normal_back		;
		theme->Visual[ Visual::DockIndicator ]			.Back [ CtrlState::Normal	] = normal_front	;
		theme->Visual[ Visual::DockIndicator ]			.Front[ CtrlState::Hot		] = hot_front		;
		theme->Visual[ Visual::DockIndicator ]			.Back [ CtrlState::Hot		] = hot_back		;
		theme->Visual[ Visual::DockIndicator ]			.Front[ CtrlState::Pushed	] = pushed_front	;
		theme->Visual[ Visual::DockIndicator ]			.Back [ CtrlState::Pushed	] = pushed_back		;
		theme->Visual[ Visual::DockIndicator ]			.Front[ CtrlState::Disabled	] = disabled_front	;
		theme->Visual[ Visual::DockIndicator ]			.Back [ CtrlState::Disabled	] = disabled_back	;

		theme->Visual[ Visual::DockIndicator_Target ]	.Front[ CtrlState::Normal	] = Color_SetAlpha( normal_back		, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Back [ CtrlState::Normal	] = Color_SetAlpha( normal_front	, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Front[ CtrlState::Hot		] = Color_SetAlpha( hot_front		, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Back [ CtrlState::Hot		] = Color_SetAlpha( hot_back		, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Front[ CtrlState::Pushed	] = Color_SetAlpha( pushed_front	, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Back [ CtrlState::Pushed	] = Color_SetAlpha( pushed_back		, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Front[ CtrlState::Disabled	] = Color_SetAlpha( disabled_front	, (uint8_t)(0.7f * 255.0f) );
		theme->Visual[ Visual::DockIndicator_Target ]	.Back [ CtrlState::Disabled	] = Color_SetAlpha( disabled_back	, (uint8_t)(0.7f * 255.0f) );

	}

	static void Theme_Build( Theme_s& theme, Font_t font, const Palette_s& palette, float alpha )
	{
		theme.Font = font;
		Theme_ResetMetrics( &theme );
		Theme_ResetVisuals( &theme, palette, alpha );
	}

	static void Theme_Default( Theme_s& theme, FontCache_t font_cache, float alpha )
	{
		const Palette_s palette = 
		{ Color::Black
		, Color::LightGray
		, Color::Gray
		, Color::DarkGray
		, Color::PaleGoldenrod
		, Color::Goldenrod
		, Color::DarkGoldenrod
		};

		Font_t font = FontCache_CreateFont( font_cache, L"Consolas", 12.0f, FontStyle::Default );
		Theme_Build( theme, font, palette, alpha );
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// Theme Preset

	void Theme_ApplyPreset_Default( Theme_t theme )
	{
		Theme_Default( *theme, System_GetFontCache(), 0.7f );
	}

	void Theme_ApplyPreset_Dark( Theme_t theme )
	{
		FontCache_t atlas = System_GetFontCache();

		// default

		Theme_Default( *theme, atlas, 1.0f );

		// font

		theme->Font = FontCache_CreateFont( atlas, L"Segoe UI", 12.0f, FontStyle::Default );

		// label

		theme->Visual[Visual::Label].Text[CtrlState::Disabled]	 = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Label].Text[CtrlState::Normal]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Label].Text[CtrlState::Pushed]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Label].Text[CtrlState::Hot]		 = Color_RGB( 241, 241, 241 );

		// button

		theme->Visual[Visual::Button].Text[CtrlState::Disabled]	 = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Button].Text[CtrlState::Normal]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Button].Text[CtrlState::Pushed]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Button].Text[CtrlState::Hot]		 = Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::Button].Front[CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Button].Back [CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Button].Front[CtrlState::Normal]	 = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Button].Back [CtrlState::Normal]	 = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Button].Front[CtrlState::Pushed]	 = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Button].Back [CtrlState::Pushed]	 = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Button].Front[CtrlState::Hot]		 = Color_RGB(  62,  64,  64 );
		theme->Visual[Visual::Button].Back [CtrlState::Hot]		 = Color_RGB(  62,  64,  64 );

		// check box

		theme->Visual[Visual::CheckBox_Box].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::CheckBox_Box].Text[CtrlState::Normal]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::CheckBox_Box].Text[CtrlState::Pushed]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::CheckBox_Box].Text[CtrlState::Hot]		= Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::CheckBox_Box].Front[CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::CheckBox_Box].Back [CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::CheckBox_Box].Front[CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::CheckBox_Box].Back [CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::CheckBox_Box].Front[CtrlState::Pushed]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::CheckBox_Box].Back [CtrlState::Pushed]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::CheckBox_Box].Front[CtrlState::Hot]		= Color_RGB(  62,  64,  64 );
		theme->Visual[Visual::CheckBox_Box].Back [CtrlState::Hot]		= Color_RGB(  62,  64,  64 );
	
		theme->Visual[Visual::CheckBox_Check].Front[CtrlState::Disabled] = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Back [CtrlState::Disabled] = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Front[CtrlState::Normal]	 = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Back [CtrlState::Normal]	 = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Front[CtrlState::Pushed]	 = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Back [CtrlState::Pushed]	 = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Front[CtrlState::Hot]		 = Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::CheckBox_Check].Back [CtrlState::Hot]		 = Color_RGB( 215, 172, 106 );

		// text edit

		theme->Visual[Visual::TextEdit].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::TextEdit].Text[CtrlState::Normal]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::TextEdit].Text[CtrlState::Pushed]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::TextEdit].Text[CtrlState::Hot]		= Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::TextEdit].Front[CtrlState::Disabled]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::TextEdit].Back [CtrlState::Disabled]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::TextEdit].Front[CtrlState::Normal]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::TextEdit].Back [CtrlState::Normal]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::TextEdit].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::TextEdit].Back [CtrlState::Pushed]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::TextEdit].Front[CtrlState::Hot]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::TextEdit].Back [CtrlState::Hot]		= Color_RGB(  63,  63,  67 );

		theme->Visual[Visual::TextEdit_Caret].Text[CtrlState::Normal]	= Color_RGB( 241, 241, 241 );

	#if 0
		theme->Visual[Visual::TextEdit_Selection].Front[CtrlState::Pushed] = Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::TextEdit_Selection].Back [CtrlState::Pushed] = Color_RGB(  38,  87, 124 );
	#else
		theme->Visual[Visual::TextEdit_Selection].Front[CtrlState::Pushed] = Color_ARGB( 100, 0, 122, 204 );
		theme->Visual[Visual::TextEdit_Selection].Back [CtrlState::Pushed] = Color_ARGB( 100, 0, 122, 204 );
	#endif

		// progress

		theme->Visual[Visual::Progress_Track].Front[CtrlState::Disabled] = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Back [CtrlState::Disabled] = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Front[CtrlState::Normal]	 = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Back [CtrlState::Normal]	 = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Front[CtrlState::Pushed]	 = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Back [CtrlState::Pushed]	 = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Front[CtrlState::Hot]		 = Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Progress_Track].Back [CtrlState::Hot]		 = Color_RGB(  62,  62,  68 );

		theme->Visual[Visual::Progress_Bar].Front[CtrlState::Disabled]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Back [CtrlState::Disabled]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Front[CtrlState::Normal]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Back [CtrlState::Normal]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Front[CtrlState::Pushed]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Back [CtrlState::Pushed]	= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Front[CtrlState::Hot]		= Color_RGB( 215, 172, 106 );
		theme->Visual[Visual::Progress_Bar].Back [CtrlState::Hot]		= Color_RGB( 215, 172, 106 );

		// slider

		theme->Visual[Visual::Slider_Track].Front[CtrlState::Disabled]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Back [CtrlState::Disabled]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Front[CtrlState::Normal]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Back [CtrlState::Normal]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Front[CtrlState::Pushed]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Back [CtrlState::Pushed]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Front[CtrlState::Hot]		= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::Slider_Track].Back [CtrlState::Hot]		= Color_RGB(  62,  62,  68 );

		theme->Visual[Visual::Slider_Thumb].Front [CtrlState::Disabled]	= Color_RGB( 104, 104, 104 ); 
		theme->Visual[Visual::Slider_Thumb].Back  [CtrlState::Disabled]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::Slider_Thumb].Front [CtrlState::Normal]	= Color_RGB( 104, 104, 104 ); 
		theme->Visual[Visual::Slider_Thumb].Back  [CtrlState::Normal]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::Slider_Thumb].Front [CtrlState::Pushed]	= Color_RGB( 239, 235, 239 ); 
		theme->Visual[Visual::Slider_Thumb].Back  [CtrlState::Pushed]	= Color_RGB( 239, 235, 239 );
		theme->Visual[Visual::Slider_Thumb].Front [CtrlState::Hot]		= Color_RGB( 158, 158, 158 ); 
		theme->Visual[Visual::Slider_Thumb].Back  [CtrlState::Hot]		= Color_RGB( 158, 158, 158 );

		// scroll bar

		theme->Visual[Visual::ScrollBar_Arrow].Front [CtrlState::Disabled]	= Color_RGB(  85,  85,  88 );
		theme->Visual[Visual::ScrollBar_Arrow].Back	 [CtrlState::Disabled]	= Color_RGB(  85,  85,  88 );
		theme->Visual[Visual::ScrollBar_Arrow].Front [CtrlState::Normal]	= Color_RGB(  85,  85,  88 );
		theme->Visual[Visual::ScrollBar_Arrow].Back	 [CtrlState::Normal]	= Color_RGB(  85,  85,  88 );
		theme->Visual[Visual::ScrollBar_Arrow].Front [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ScrollBar_Arrow].Back	 [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ScrollBar_Arrow].Front [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::ScrollBar_Arrow].Back	 [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );

		theme->Visual[Visual::ScrollBar_Thumb].Front [CtrlState::Disabled]	= Color_RGB( 104, 104, 104 ); 
		theme->Visual[Visual::ScrollBar_Thumb].Back  [CtrlState::Disabled]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::ScrollBar_Thumb].Front [CtrlState::Normal]	= Color_RGB( 104, 104, 104 ); 
		theme->Visual[Visual::ScrollBar_Thumb].Back  [CtrlState::Normal]	= Color_RGB( 104, 104, 104 );
		theme->Visual[Visual::ScrollBar_Thumb].Front [CtrlState::Pushed]	= Color_RGB( 239, 235, 239 ); 
		theme->Visual[Visual::ScrollBar_Thumb].Back  [CtrlState::Pushed]	= Color_RGB( 239, 235, 239 );
		theme->Visual[Visual::ScrollBar_Thumb].Front [CtrlState::Hot]		= Color_RGB( 158, 158, 158 ); 
		theme->Visual[Visual::ScrollBar_Thumb].Back  [CtrlState::Hot]		= Color_RGB( 158, 158, 158 );

		theme->Visual[Visual::ScrollBar_Track].Front [CtrlState::Disabled]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Back	 [CtrlState::Disabled]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Front [CtrlState::Normal]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Back	 [CtrlState::Normal]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Front [CtrlState::Pushed]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Back	 [CtrlState::Pushed]	= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Front [CtrlState::Hot]		= Color_RGB(  62,  62,  68 );
		theme->Visual[Visual::ScrollBar_Track].Back	 [CtrlState::Hot]		= Color_RGB(  62,  62,  68 );

		// list header

		theme->Visual[Visual::ListHeader_Item].Text	[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::ListHeader_Item].Text	[CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::ListHeader_Item].Text	[CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::ListHeader_Item].Text	[CtrlState::Hot]		= Color_RGB( 255, 255, 255 );

		theme->Visual[Visual::ListHeader_Item].Front [CtrlState::Disabled]	= Color_RGB(  63,  63,  70 );	
		theme->Visual[Visual::ListHeader_Item].Back	 [CtrlState::Disabled]	= Color_RGB(  37,  37,  38 );	
		theme->Visual[Visual::ListHeader_Item].Front [CtrlState::Normal]	= Color_RGB(  63,  63,  70 );	// lines
		theme->Visual[Visual::ListHeader_Item].Back	 [CtrlState::Normal]	= Color_RGB(  37,  37,  38 );	// fill
		theme->Visual[Visual::ListHeader_Item].Front [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ListHeader_Item].Back	 [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ListHeader_Item].Front [CtrlState::Hot]		= Color_RGB(  62,  62,  64 );
		theme->Visual[Visual::ListHeader_Item].Back	 [CtrlState::Hot]		= Color_RGB(  62,  62,  64 );

		// list

		theme->Visual[Visual::ListColumn_Item].Text	[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::ListColumn_Item].Text	[CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::ListColumn_Item].Text	[CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::ListColumn_Item].Text	[CtrlState::Hot]		= Color_RGB( 255, 255, 255 );

		theme->Visual[Visual::ListColumn_Selection].Front[CtrlState::Normal] = Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::ListColumn_Selection].Back [CtrlState::Normal] = Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::ListColumn_Selection].Front[CtrlState::Pushed] = Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::ListColumn_Selection].Back [CtrlState::Pushed] = Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::ListColumn_Selection].Back [CtrlState::Hot]	 = 0;
		theme->Visual[Visual::ListColumn_Selection].Front[CtrlState::Hot]	 = 0;

		theme->Visual[Visual::List].Front[CtrlState::Disabled]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Back [CtrlState::Disabled]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Front[CtrlState::Normal]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Back [CtrlState::Normal]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Front[CtrlState::Pushed]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Back [CtrlState::Pushed]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Front[CtrlState::Hot]		= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::List].Back [CtrlState::Hot]		= Color_RGB( 37, 37, 38 );

		theme->Visual[Visual::List_Grid].Front[CtrlState::Normal] = Color_RGB(  28,  28,  30 );
		theme->Visual[Visual::List_Grid].Back [CtrlState::Normal] = Color_RGB(  28,  28,  30 );

		// tree

		theme->Visual[Visual::Tree].Front[CtrlState::Disabled]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Back [CtrlState::Disabled]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Front[CtrlState::Normal]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Back [CtrlState::Normal]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Front[CtrlState::Pushed]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Back [CtrlState::Pushed]	= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Front[CtrlState::Hot]		= Color_RGB( 37, 37, 38 );
		theme->Visual[Visual::Tree].Back [CtrlState::Hot]		= Color_RGB( 37, 37, 38 );

		theme->Visual[Visual::Tree_Item].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tree_Item].Text[CtrlState::Normal]	= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item].Text[CtrlState::Pushed]	= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item].Text[CtrlState::Hot]		= Color_RGB( 255, 255, 255 );

		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Pushed]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Pushed]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Hot]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Hot]		= Color_RGB(   0, 122, 204 );

		theme->Visual[Visual::Tree_Selection].Front[CtrlState::Normal]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Tree_Selection].Back [CtrlState::Normal]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Tree_Selection].Front[CtrlState::Pushed]	= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Tree_Selection].Back [CtrlState::Pushed]	= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Tree_Selection].Back [CtrlState::Hot]		= 0;
		theme->Visual[Visual::Tree_Selection].Front[CtrlState::Hot]		= 0;

		// tab 

		theme->Visual[Visual::Tab].Front[CtrlState::Disabled]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Back [CtrlState::Disabled]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Front[CtrlState::Normal]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Back [CtrlState::Normal]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Front[CtrlState::Pushed]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Back [CtrlState::Pushed]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Front[CtrlState::Hot]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Tab].Back [CtrlState::Hot]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );

		theme->Visual[Visual::Tab_Item].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tab_Item].Text[CtrlState::Normal]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Tab_Item].Text[CtrlState::Pushed]		= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Tab_Item].Text[CtrlState::Hot]		= Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::Tab_Item].Front[CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Tab_Item].Back [CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Tab_Item].Front[CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Tab_Item].Back [CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Tab_Item].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tab_Item].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tab_Item].Front[CtrlState::Hot]		= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Tab_Item].Back [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );

		theme->Visual[Visual::Frame_Tab].Front[CtrlState::Disabled]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Back [CtrlState::Disabled]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Front[CtrlState::Normal]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Back [CtrlState::Normal]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Front[CtrlState::Pushed]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Back [CtrlState::Pushed]	= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Front[CtrlState::Hot]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Frame_Tab].Back [CtrlState::Hot]		= Color_RGB(  42 + 10,  42 + 10,  44 + 10 );

		theme->Visual[Visual::Frame_Tab_Item].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Frame_Tab_Item].Text[CtrlState::Normal]	= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Frame_Tab_Item].Text[CtrlState::Pushed]	= Color_RGB(  77, 170, 228 );
		theme->Visual[Visual::Frame_Tab_Item].Text[CtrlState::Hot]		= Color_RGB(  77, 170, 228 );

		theme->Visual[Visual::Frame_Tab_Item].Front[CtrlState::Disabled] = Color_RGB(  45, 45, 48 );
		theme->Visual[Visual::Frame_Tab_Item].Back [CtrlState::Disabled] = Color_RGB(  45, 45, 48 );
		theme->Visual[Visual::Frame_Tab_Item].Front[CtrlState::Normal]	 = Color_RGB(  45, 45, 48 );
		theme->Visual[Visual::Frame_Tab_Item].Back [CtrlState::Normal]	 = Color_RGB(  45, 45, 48 );
		theme->Visual[Visual::Frame_Tab_Item].Front[CtrlState::Pushed]	 = Color_RGB(  63, 63, 70 );
		theme->Visual[Visual::Frame_Tab_Item].Back [CtrlState::Pushed]	 = Color_RGB(  37, 37, 38 );
		theme->Visual[Visual::Frame_Tab_Item].Front[CtrlState::Hot]		 = Color_RGB(  63, 63, 70 );
		theme->Visual[Visual::Frame_Tab_Item].Back [CtrlState::Hot]		 = Color_RGB(  62, 62, 64 );

		// menu

		theme->Visual[Visual::Menu_Main].Front[CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Back [CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Front[CtrlState::Normal]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Back [CtrlState::Normal]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Front[CtrlState::Pushed]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Back [CtrlState::Pushed]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Front[CtrlState::Hot]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_Main].Back [CtrlState::Hot]		= Color_RGB(  42,  42,  44 );

		theme->Visual[Visual::Menu_Main_Item].Text[CtrlState::Disabled]	 = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Menu_Main_Item].Text[CtrlState::Normal]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Menu_Main_Item].Text[CtrlState::Pushed]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Menu_Main_Item].Text[CtrlState::Hot]		 = Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::Menu_Main_Item].Front[CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_Main_Item].Back [CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_Main_Item].Front[CtrlState::Normal]	 = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_Main_Item].Back [CtrlState::Normal]	 = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_Main_Item].Front[CtrlState::Pushed]	 = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_Main_Item].Back [CtrlState::Pushed]	 = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_Main_Item].Front[CtrlState::Hot]		 = Color_RGB(  62,  64,  64 );
		theme->Visual[Visual::Menu_Main_Item].Back [CtrlState::Hot]		 = Color_RGB(  62,  64,  64 );

		// popup

		theme->Visual[Visual::Menu_PopUp].Front[CtrlState::Disabled] = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Back [CtrlState::Disabled] = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Front[CtrlState::Normal]	 = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Back [CtrlState::Normal]	 = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Front[CtrlState::Pushed]	 = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Back [CtrlState::Pushed]	 = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Front[CtrlState::Hot]		 = Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp].Back [CtrlState::Hot]		 = Color_RGB(  42,  42,  44 );

		theme->Visual[Visual::Menu_PopUp_Item].Text[CtrlState::Disabled] = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Menu_PopUp_Item].Text[CtrlState::Normal]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Menu_PopUp_Item].Text[CtrlState::Pushed]	 = Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::Menu_PopUp_Item].Text[CtrlState::Hot]		 = Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::Menu_PopUp_Item].Front[CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_PopUp_Item].Back [CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_PopUp_Item].Front[CtrlState::Normal]	  = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_PopUp_Item].Back [CtrlState::Normal]	  = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Menu_PopUp_Item].Front[CtrlState::Pushed]	  = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_PopUp_Item].Back [CtrlState::Pushed]	  = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_PopUp_Item].Front[CtrlState::Hot]	  = Color_RGB(  62,  64,  64 );
		theme->Visual[Visual::Menu_PopUp_Item].Back [CtrlState::Hot]	  = Color_RGB(  62,  64,  64 );

		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Front[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Back [CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Front[CtrlState::Normal]	= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Back [CtrlState::Normal]	= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Front[CtrlState::Hot]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Menu_PopUp_Item_Arrow].Back [CtrlState::Hot]		= Color_RGB(   0, 122, 204 );

		theme->Visual[Visual::Menu_PopUp_Icon].Front[CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Back [CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Front[CtrlState::Normal]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Back [CtrlState::Normal]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Front[CtrlState::Pushed]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Back [CtrlState::Pushed]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Front[CtrlState::Hot]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::Menu_PopUp_Icon].Back [CtrlState::Hot]		= Color_RGB(  42,  42,  44 );

		theme->Visual[Visual::Menu_PopUp_Separator].Front[CtrlState::Disabled]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Back [CtrlState::Disabled]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Front[CtrlState::Normal]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Back [CtrlState::Normal]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Front[CtrlState::Pushed]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Back [CtrlState::Pushed]	= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Front[CtrlState::Hot]		= Color_RGB( 51, 51, 55 );
		theme->Visual[Visual::Menu_PopUp_Separator].Back [CtrlState::Hot]		= Color_RGB( 51, 51, 55 );

		// drop down

		theme->Visual[Visual::DropDown].Front[CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Back [CtrlState::Disabled]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Front[CtrlState::Normal]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Back [CtrlState::Normal]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Front[CtrlState::Pushed]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Back [CtrlState::Pushed]	= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Front[CtrlState::Hot]		= Color_RGB(  42,  42,  44 );
		theme->Visual[Visual::DropDown].Back [CtrlState::Hot]		= Color_RGB(  42,  42,  44 );

		theme->Visual[Visual::DropDown_Item].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::DropDown_Item].Text[CtrlState::Normal]	= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::DropDown_Item].Text[CtrlState::Pushed]	= Color_RGB( 241, 241, 241 );
		theme->Visual[Visual::DropDown_Item].Text[CtrlState::Hot]		= Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::DropDown_Item].Front[CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::DropDown_Item].Back [CtrlState::Disabled] = Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::DropDown_Item].Front[CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::DropDown_Item].Back [CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::DropDown_Item].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::DropDown_Item].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::DropDown_Item].Front[CtrlState::Hot]		= Color_RGB(  62,  64,  64 );
		theme->Visual[Visual::DropDown_Item].Back [CtrlState::Hot]		= Color_RGB(  62,  64,  64 );

		theme->Visual[Visual::DropDown_Box].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::DropDown_Box].Text[CtrlState::Normal]		= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::DropDown_Box].Text[CtrlState::Pushed]		= Color_RGB( 241, 241, 241 );	// 255, 255, 255 (+AA)
		theme->Visual[Visual::DropDown_Box].Text[CtrlState::Hot]		= Color_RGB( 241, 241, 241 );

		theme->Visual[Visual::DropDown_Box].Back [CtrlState::Disabled]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::DropDown_Box].Front[CtrlState::Disabled]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::DropDown_Box].Back [CtrlState::Normal]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::DropDown_Box].Front[CtrlState::Normal]	= Color_RGB(  63,  63,  67 );
		theme->Visual[Visual::DropDown_Box].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::DropDown_Box].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::DropDown_Box].Back [CtrlState::Hot]		= Color_RGB(  31,  31,  32 );
		theme->Visual[Visual::DropDown_Box].Front[CtrlState::Hot]		= Color_RGB(   0, 122, 204 );

		// caption 

		theme->Visual[Visual::Caption].Text[CtrlState::Disabled] = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Caption].Text[CtrlState::Normal]	 = Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Caption].Text[CtrlState::Pushed]	 = Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Caption].Text[CtrlState::Hot]		 = Color_RGB( 153, 153, 153 );

		theme->Visual[Visual::Caption].Front[CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Caption].Back [CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Caption].Front[CtrlState::Normal]		= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Caption].Back [CtrlState::Normal]		= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Caption].Front[CtrlState::Pushed]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Caption].Back [CtrlState::Pushed]		= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Caption].Front[CtrlState::Hot]		= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Caption].Back [CtrlState::Hot]		= Color_RGB(  45,  45,  48 );

		// expander

		theme->Visual[Visual::Expander].Text[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Expander].Text[CtrlState::Normal]		= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander].Text[CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Expander].Text[CtrlState::Hot]		= Color_RGB( 153, 153, 153 );

		theme->Visual[Visual::Expander].Front[CtrlState::Disabled]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Expander].Back [CtrlState::Disabled]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Expander].Front[CtrlState::Normal]	= Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::Expander].Back [CtrlState::Normal]	= Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::Expander].Front[CtrlState::Pushed]	= Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::Expander].Back [CtrlState::Pushed]	= Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::Expander].Front[CtrlState::Hot]		= Color_RGB(  38,  87, 124 );
		theme->Visual[Visual::Expander].Back [CtrlState::Hot]		= Color_RGB(  38,  87, 124 );

		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Disabled] = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Disabled] = Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Normal]	 = Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Normal]	 = Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Pushed]	 = Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Pushed]	 = Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Hot]		 = Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Hot]		 = Color_RGB(  28, 151, 234 );

		// splitter

		theme->Visual[Visual::Splitter].Front[CtrlState::Disabled]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Splitter].Back [CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Splitter].Front[CtrlState::Normal]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Splitter].Back [CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Splitter].Front[CtrlState::Pushed]	= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Splitter].Back [CtrlState::Pushed]	= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Splitter].Front[CtrlState::Hot]		= Color_RGB(  63,  63,  70 );
		theme->Visual[Visual::Splitter].Back [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );

		// window 

		theme->Visual[Visual::Window].Front[CtrlState::Disabled] = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Back [CtrlState::Disabled] = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Front[CtrlState::Normal]	 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Back [CtrlState::Normal]	 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Front[CtrlState::Pushed]	 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Back [CtrlState::Pushed]	 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Front[CtrlState::Hot]		 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );
		theme->Visual[Visual::Window].Back [CtrlState::Hot]		 = Color_RGB(  42 + 10,  42 + 10,  44 + 10 );

		// frame border

		theme->Visual[Visual::Frame_Border].Front[ CtrlState::Disabled	] = Color_RGB(   63, 63,  70 );
		theme->Visual[Visual::Frame_Border].Back [ CtrlState::Disabled	] = Color_RGB(   63, 63,  70 );
		theme->Visual[Visual::Frame_Border].Front[ CtrlState::Normal	] = Color_RGB(   63, 63,  70 );
		theme->Visual[Visual::Frame_Border].Back [ CtrlState::Normal	] = Color_RGB(   63, 63,  70 );
		theme->Visual[Visual::Frame_Border].Front[ CtrlState::Pushed	] = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Frame_Border].Back [ CtrlState::Pushed	] = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Frame_Border].Front[ CtrlState::Hot		] = Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Frame_Border].Back [ CtrlState::Hot		] = Color_RGB(  28, 151, 234 );

		// dock indicator

		theme->Visual[Visual::DockIndicator].Front[ CtrlState::Disabled	] = Color_RGB( 51, 51,  55 );
		theme->Visual[Visual::DockIndicator].Back [ CtrlState::Disabled	] = Color_RGB( 33, 33,  35 );
		theme->Visual[Visual::DockIndicator].Front[ CtrlState::Normal	] = Color_RGB( 51, 51,  55 );
		theme->Visual[Visual::DockIndicator].Back [ CtrlState::Normal	] = Color_RGB( 33, 33,  35 );
		theme->Visual[Visual::DockIndicator].Front[ CtrlState::Pushed	] = Color_RGB( 51, 51,  55 );
		theme->Visual[Visual::DockIndicator].Back [ CtrlState::Pushed	] = Color_RGB( 33, 33,  35 );
		theme->Visual[Visual::DockIndicator].Front[ CtrlState::Hot		] = Color_RGB( 51, 51,  55 );
		theme->Visual[Visual::DockIndicator].Back [ CtrlState::Hot		] = Color_RGB( 10, 95, 152 );

		theme->Visual[Visual::DockIndicator_Target].Front[ CtrlState::Disabled	] = Color_ARGB( 127, 51, 51,  55 );
		theme->Visual[Visual::DockIndicator_Target].Back [ CtrlState::Disabled	] = Color_ARGB( 127, 10, 95, 152 );
		theme->Visual[Visual::DockIndicator_Target].Front[ CtrlState::Normal	] = Color_ARGB( 127, 51, 51,  55 );
		theme->Visual[Visual::DockIndicator_Target].Back [ CtrlState::Normal	] = Color_ARGB( 127, 10, 95, 152 );
		theme->Visual[Visual::DockIndicator_Target].Front[ CtrlState::Pushed	] = Color_ARGB( 127, 51, 51,  55 );
		theme->Visual[Visual::DockIndicator_Target].Back [ CtrlState::Pushed	] = Color_ARGB( 127, 10, 95, 152 );
		theme->Visual[Visual::DockIndicator_Target].Front[ CtrlState::Hot		] = Color_ARGB( 127, 51, 51,  55 );
		theme->Visual[Visual::DockIndicator_Target].Back [ CtrlState::Hot		] = Color_ARGB( 127, 10, 95, 152 );

	#if USE_HOT_SEL
		theme->Visual[Visual::ListColumn_Selection].Front[CtrlState::Pushed] = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ListColumn_Selection].Back [CtrlState::Pushed] = Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::ListColumn_Selection].Back [CtrlState::Hot]	 = Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::ListColumn_Selection].Front[CtrlState::Hot]	 = Color_RGB(  28, 151, 234 );

		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Normal]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Front[CtrlState::Hot]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Tree_Item_Arrow].Back [CtrlState::Hot]		= Color_RGB( 255, 255, 255 );

		theme->Visual[Visual::Tree_Selection].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tree_Selection].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Tree_Selection].Back [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Tree_Selection].Front[CtrlState::Hot]		= Color_RGB(  28, 151, 234 );

		theme->Visual[Visual::Expander].Front[CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Expander].Back [CtrlState::Disabled]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Expander].Front[CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Expander].Back [CtrlState::Normal]	= Color_RGB(  45,  45,  48 );
		theme->Visual[Visual::Expander].Front[CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Expander].Back [CtrlState::Pushed]	= Color_RGB(   0, 122, 204 );
		theme->Visual[Visual::Expander].Front[CtrlState::Hot]		= Color_RGB(  28, 151, 234 );
		theme->Visual[Visual::Expander].Back [CtrlState::Hot]		= Color_RGB(  28, 151, 234 );

		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Disabled]	= Color_RGB( 119, 119, 121 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Normal]		= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Normal]		= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Pushed]		= Color_RGB( 255, 255, 255 );
		theme->Visual[Visual::Expander_Arrow].Front[CtrlState::Hot]			= Color_RGB( 153, 153, 153 );
		theme->Visual[Visual::Expander_Arrow].Back [CtrlState::Hot]			= Color_RGB( 153, 153, 153 );
	#endif
	}

} }
