#pragma once
#include <Nemesis/Font/System.h>

//======================================================================================
namespace nemesis { namespace font
{
	struct FontApi_t
	{
		bool (*GetTextMetric)	( Font_t* font, TextMetrics_t& tm );
		bool (*GetGlyphMetrics)	( Font_t* font, GlyphMetrics_t& gm );
		void (*GetGlyphInfos)	( Font_t* font, const wchar_t* t, int num_chars, GlyphInfo_t* infos );
		bool (*RenderChar)		( Font_t* font, wchar_t ch, const GlyphBuffer_t& buffer, GlyphInfo_t& info );
		void (*Release)			( Font_t* font );
	};

	struct Font_t
	{
		const FontApi_t* Api;
	};
} } 
