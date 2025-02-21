//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "gui_d3d11.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;
using namespace nemesis::gfx;

//======================================================================================
// Gui - d3d11 - Detail
//======================================================================================
static Lock::Mode ToGfx( TextureLock::Mode v )
{
	return (v == TextureLock::Read) 
		? Lock::Read
		: Lock::Write
		;
}

static Format::Enum ToGfx( TextureFormat::Enum v )
{
	switch (v)
	{
	case TextureFormat::A8:			return Format::R8_UNorm;
	case TextureFormat::A8R8G8B8:	return Format::R8G8B8A8_UNorm;
	default:
		break;
	}
	return Format::Unknown;
}

static TextureDesc_s ToGfx( const TextureInfo_s& v )
{
	return TextureDesc_s
	{ ToGfx(v.Format)
	, (uint32_t)v.Width
	, (uint32_t)v.Height
	};
}

static TextureFormat::Enum ToGui( Format::Enum v )
{
	switch (v)
	{
	case Format::R8_UNorm:		 return TextureFormat::A8;
	case Format::R8G8B8A8_UNorm: return TextureFormat::A8R8G8B8;
	default:
		break;
	}
	return TextureFormat::A8R8G8B8;
}

static TextureInfo_s ToGui( const TextureDesc_s& v )
{
	return TextureInfo_s 
	{ (int)v.Width
	, (int)v.Height
	, ToGui(v.Format)
	};
}

static TextureLock_s ToGui( const LockDesc_s& v )
{
	return TextureLock_s 
	{	   v.Data
	, (int)v.Pitch
	};
}

//======================================================================================
// Gui - d3d11 - Private
//======================================================================================
struct Gui_d3d11_s
{
	gui::Renderer_s			Renderer;
	Allocator_t				Alloc;
	Gfx_d3d11_t				Gfx;
	Array<Texture_d3d11_t>	Garbage;
};

static void Gui_d3d11_Purge( Gui_d3d11_t gui )
{
	for ( int i = 0; i < gui->Garbage.Count; ++i )
		Gfx_d3d11_Texture_Release( gui->Gfx, gui->Garbage[i] );
	gui->Garbage.Reset();
}

static bool NE_CALLBK Gui_CreateTexture( Renderer_t renderer, const TextureInfo_s& info, Texture_t* texture )
{
	return Gfx_d3d11_Texture_Create( ((Gui_d3d11_s*)renderer)->Gfx, ToGfx(info), (Texture_d3d11_t*)(texture) );
}

static bool NE_CALLBK Gui_GetTextureInfo( Renderer_t renderer, Texture_t texture, TextureInfo_s& info )
{
	TextureDesc_s desc = {};
	bool ok = Gfx_d3d11_Texture_GetDesc( ((Gui_d3d11_s*)renderer)->Gfx, (Texture_d3d11_t)texture, desc );
	info = ToGui(desc);
	return ok;
}

static bool NE_CALLBK Gui_LockTexture( Renderer_t renderer, Texture_t texture, TextureLock::Mode mode, TextureLock_s& info )
{
	LockDesc_s lock = {};
	bool ok = Gfx_d3d11_Texture_Lock( ((Gui_d3d11_s*)renderer)->Gfx, (Texture_d3d11_t)texture, ToGfx(mode), lock );
	info = ToGui(lock);
	return ok;
}

static void NE_CALLBK Gui_UnlockTexture( Renderer_t renderer, Texture_t texture )
{
	return Gfx_d3d11_Texture_Unlock( ((Gui_d3d11_s*)renderer)->Gfx, (Texture_d3d11_t)texture );
}

static void NE_CALLBK Gui_ReleaseTexture( Renderer_t renderer, Texture_t texture )
{
	((Gui_d3d11_s*)renderer)->Garbage.Append( (Texture_d3d11_t)texture );
}

static const Renderer_v Gui_Renderer_Api = 
{ Gui_CreateTexture
, Gui_GetTextureInfo
, Gui_LockTexture
, Gui_UnlockTexture
, Gui_ReleaseTexture
};

//======================================================================================
// Gui - d3d11 - Public
//======================================================================================
Gui_d3d11_t Gui_d3d11_Create( Allocator_t alloc, Gfx_d3d11_t gfx )
{
	Gui_d3d11_t out = Mem_Calloc<Gui_d3d11_s>( alloc );
	out->Renderer.Api = &Gui_Renderer_Api;
	out->Alloc = alloc;
	out->Gfx	= gfx;
	out->Garbage.Alloc = alloc;
	gui::System_Initialize( alloc, &out->Renderer );
	return out;
}

void Gui_d3d11_Release( Gui_d3d11_t gui )
{
	if (!gui)
		return;
	Gui_d3d11_Purge( gui );
	gui->Garbage.Clear();
	gui::System_Shutdown();
	Mem_Free( gui->Alloc, gui );
}

void Gui_d3d11_NextFrame( Gui_d3d11_t gui )
{
	Gui_d3d11_Purge( gui );
}
