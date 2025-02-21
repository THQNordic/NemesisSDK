//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gfx/Gfx_d3d9.h>
#include <Nemesis/Core/Alloc.h>
#include <Nemesis/Core/Memory.h>

//======================================================================================
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <atlbase.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxerr.lib")

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;

//======================================================================================
#if (NE_CONFIG < NE_CONFIG_MASTER)
#	define D3D9_V(expr) do { hr = (expr); NeAssert( SUCCEEDED(hr) ); } while(false)
#else
#	define D3D9_V(expr) do { hr = (expr); } while(false)
#endif

//======================================================================================
//	Gfx - d3d9 - Detail
//======================================================================================
namespace nemesis { namespace graphics
{
	struct DrawVtx_d3d9_s
	{
		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };
		float    Pos[3];
		D3DCOLOR Color;
		float    UV[2];
	};

	static D3DPRESENT_PARAMETERS D3D9_MakePresentParams( HWND hWnd, bool windowed, bool vsync )
	{
		RECT rc;
		GetClientRect( hWnd, &rc );

		const BOOL bWindowed = windowed ? TRUE : FALSE;
		const BOOL bMultiSample = FALSE;

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.hDeviceWindow			 = hWnd;
		d3dpp.BackBufferCount		 = 1;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.Flags					 = 0;
		d3dpp.PresentationInterval	 = vsync 
										? D3DPRESENT_INTERVAL_DEFAULT 
										: D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.MultiSampleQuality	 = 0;
		d3dpp.Windowed				 = bWindowed;

		if (bWindowed)
		{
			d3dpp.BackBufferWidth			 = rc.right-rc.left;
			d3dpp.BackBufferHeight			 = rc.bottom-rc.top;
			d3dpp.BackBufferHeight			 = 0;
			d3dpp.SwapEffect				 = D3DSWAPEFFECT_COPY;
			d3dpp.MultiSampleType			 = D3DMULTISAMPLE_NONE;
			d3dpp.BackBufferFormat			 = D3DFMT_UNKNOWN;
			d3dpp.FullScreen_RefreshRateInHz = 0;
		}
		else
		{
			d3dpp.BackBufferWidth			 = rc.right-rc.left;
			d3dpp.BackBufferHeight			 = rc.bottom-rc.top;
			d3dpp.SwapEffect				 = bMultiSample ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_FLIP;
			d3dpp.MultiSampleType			 = D3DMULTISAMPLE_NONE;
			d3dpp.BackBufferFormat			 = D3DFMT_A8B8G8R8;
			d3dpp.FullScreen_RefreshRateInHz = 0;
		}
		
		return d3dpp;
	}

	static VOID WINAPI D3D9_FillTexture_Color(D3DXVECTOR4 *pOut, CONST D3DXVECTOR2 *pTexCoord, CONST D3DXVECTOR2 *pTexelSize, LPVOID pData)
	{
		*pOut = *((D3DXVECTOR4*)pData);
	}

	static Format::Enum Format_Of( D3DFORMAT v )
	{
		switch (v)
		{
		case D3DFMT_A8		: return Format::R8_UNorm;
		case D3DFMT_A8R8G8B8: return Format::R8G8B8A8_UNorm;
		default:
			break;
		}
		return Format::Unknown;
	}

	static D3DFORMAT Format_Of( Format::Enum v )
	{
		switch (v)
		{
		case Format::R8_UNorm		: return D3DFMT_A8;
		case Format::R8G8B8A8_UNorm	: return D3DFMT_A8R8G8B8;
		default:
			break;
		}
		return D3DFMT_UNKNOWN;
	}
} }

//======================================================================================
//	Gfx - d3d9 - Private
//======================================================================================
namespace nemesis { namespace graphics
{
	struct Gfx_d3d9_s
	{
		Allocator_t Alloc;
		CComPtr<IDirect3D9>	D3D;
		CComPtr<IDirect3DDevice9> Device;
		D3DPRESENT_PARAMETERS PresentParams;
		CComPtr<IDirect3DIndexBuffer9> IndexBuffer;
		CComPtr<IDirect3DVertexBuffer9> VertexBuffer;
		CComPtr<IDirect3DTexture9> WhiteTexture;
		UINT NumIdx;
		UINT NumVtx;
	};

	static void Gfx_d3d9_ApplyState_2D( Gfx_d3d9_t g )
	{
		D3DVIEWPORT9 vp;
		vp.X = vp.Y = 0;
		vp.Width  = (DWORD)g->PresentParams.BackBufferWidth;
		vp.Height = (DWORD)g->PresentParams.BackBufferHeight;
		vp.MinZ	  = 0.0f;
		vp.MaxZ	  = 1.0f;
		g->Device->SetViewport( &vp );
		g->Device->SetPixelShader( nullptr );
		g->Device->SetVertexShader( nullptr );
		g->Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		g->Device->SetRenderState( D3DRS_LIGHTING, false );
		g->Device->SetRenderState( D3DRS_ZENABLE, false );
		g->Device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		g->Device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
		g->Device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		g->Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g->Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g->Device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
		g->Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		g->Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g->Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g->Device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		g->Device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		g->Device->SetFVF( DrawVtx_d3d9_s::FVF );
	}

	static void Gfx_d3d9_ApplyTransform_2D( Gfx_d3d9_t g )
	{
		const float L = 0.5f;
		const float R = 0.5f + g->PresentParams.BackBufferWidth;
		const float T = 0.5f;
		const float B = 0.5f + g->PresentParams.BackBufferHeight;

		const D3DMATRIX mat_identity = 
		{ 1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 1.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		};

		const D3DMATRIX mat_projection =
		{ 2.0f/(R-L),   0.0f,         0.0f,  0.0f
		, 0.0f,         2.0f/(T-B),   0.0f,  0.0f
		, 0.0f,         0.0f,         0.5f,  0.0f
		, (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
		};

		g->Device->SetTransform( D3DTS_WORLD	 , &mat_identity   );
		g->Device->SetTransform( D3DTS_VIEW		 , &mat_identity   );
		g->Device->SetTransform( D3DTS_PROJECTION, &mat_projection );
	}

	static void Gfx_d3d9_ApplyState_3D( Gfx_d3d9_t g )
	{
		g->Device->SetPixelShader( nullptr );
		g->Device->SetVertexShader( nullptr );
		g->Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		g->Device->SetRenderState( D3DRS_LIGHTING, false );
		g->Device->SetRenderState( D3DRS_ZENABLE, true );
		g->Device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		g->Device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
		g->Device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		g->Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g->Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g->Device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
		g->Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		g->Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g->Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g->Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g->Device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		g->Device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		g->Device->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	}

	static bool Gfx_d3d9_CopyData_2D( Gfx_d3d9_t g, const DrawBatch_s* batch, int count )
	{
		// @todo: remove this and use DrawIndexPrimitiveUP instead

		using namespace gui;

		// sanitize
		if (!batch || !count)
			return false;

		// calculate totals
		uint32_t total_num_idx = 0;
		uint32_t total_num_vtx = 0;
		for ( int i = 0; i < count; ++i )
		{
			total_num_idx += batch[i].NumIdx;
			total_num_vtx += batch[i].NumVtx;
		}
		if (!total_num_idx || !total_num_vtx)
			return false;

		// grow buffers
		{
			// grow index buffer
			if (g->NumIdx < total_num_idx)
			{
				g->IndexBuffer = nullptr;
				const uint32_t num_alloc_idx = total_num_idx;
				g->Device->CreateIndexBuffer( num_alloc_idx * sizeof(DrawIdx_t), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(DrawIdx_t) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g->IndexBuffer, nullptr );
				if (!g->IndexBuffer)
					return false;
				g->NumIdx = num_alloc_idx;
			}

			// grow vertex buffer
			if (g->NumVtx < total_num_vtx)
			{
				g->VertexBuffer = nullptr;
				const uint32_t num_alloc_vtx = total_num_vtx;
				g->Device->CreateVertexBuffer( num_alloc_vtx * sizeof(DrawVtx_d3d9_s), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, DrawVtx_d3d9_s::FVF, D3DPOOL_DEFAULT, &g->VertexBuffer, nullptr );
				if (!g->VertexBuffer)
					return false;
				g->NumVtx = num_alloc_vtx;
			}
		}

		// fill buffers
		{
			// fill index buffer
			{
				DrawIdx_t* dst;
				g->IndexBuffer->Lock( 0, total_num_idx * sizeof(DrawIdx_t), (void**)&dst, D3DLOCK_DISCARD );
				for ( int i = 0; i < count; ++i )
				{
					const DrawBatch_s& bat = batch[i];
					memcpy( dst, bat.Idx, bat.NumIdx * sizeof(DrawIdx_t) );
					dst += bat.NumIdx;
				}
				g->IndexBuffer->Unlock();
			}

			// fill vertex buffer
			{
				DrawVtx_d3d9_s* dst;
				g->VertexBuffer->Lock( 0, total_num_vtx * sizeof(DrawVtx_d3d9_s), (void**)&dst, D3DLOCK_DISCARD );
				for ( int i = 0; i < count; ++i )
				{
					const DrawBatch_s& bat = batch[i];
					const DrawVtx_s* src = bat.Vtx;
					for ( uint32_t j = 0; j < bat.NumVtx; ++j, ++src, ++dst )
					{
						dst->Pos[0]	= src->Pos.x;
						dst->Pos[1]	= src->Pos.y;
						dst->Pos[2]	= 0.0f;
						dst->Color  = src->Color;
						dst->UV[0]	= src->UV.x;
						dst->UV[1]	= src->UV.y;
					}
				}
				g->VertexBuffer->Unlock();
			}
		}

		return true;
	}

	static void Gfx_d3d9_DrawBatch_2D( Gfx_d3d9_t g, const DrawBatch_s* batch, int count )
	{
		using namespace gui;

		g->Device->SetIndices	  (	   g->IndexBuffer );
		g->Device->SetStreamSource( 0, g->VertexBuffer, 0, sizeof(DrawVtx_d3d9_s) );

		RECT clip_rect;
		UINT idx_offset = 0;
		UINT vtx_offset = 0;
		for ( int j = 0; j < count; ++j )
		{
			const DrawBatch_s& bat = batch[ j ];
			for ( uint32_t i = 0; i < bat.NumCmd; ++i )
			{
				const DrawCmd_s& cmd = bat.Cmd[ i ];

				// scissor
				if ( NeHasFlag( cmd.Flags, DrawCmd::Clip ) )
				{
					clip_rect.left	 = (LONG)(cmd.Clip.x);
					clip_rect.top	 = (LONG)(cmd.Clip.y);
					clip_rect.right  = (LONG)(cmd.Clip.x + cmd.Clip.w + 0.5f);
					clip_rect.bottom = (LONG)(cmd.Clip.y + cmd.Clip.h + 0.5f);
				}
				else
				{
					clip_rect.left	 = 0;
					clip_rect.top	 = 0;
					clip_rect.right  = g->PresentParams.BackBufferWidth;
					clip_rect.bottom = g->PresentParams.BackBufferHeight;
				}
				//if (!no_scissor)
					g->Device->SetScissorRect( &clip_rect );

				// texture
				IDirect3DTexture9* texture = cmd.Texture ? (IDirect3DTexture9*)cmd.Texture : g->WhiteTexture;
				g->Device->SetTexture( 0, texture );

				// draw
				g->Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vtx_offset, 0, bat.NumVtx /*- vtx_offset*/, idx_offset, cmd.NumIdx/3 );
				idx_offset += cmd.NumIdx;
			}
			vtx_offset += batch[ j ].NumVtx;
		}
	}

	static void Gfx_d3d9_DrawBatch_3D( Gfx_d3d9_t g, const MeshBatch_s* batch, int count )
	{
		for ( int i = 0; i < count; ++i )
		{
			const MeshBatch_s& bat = batch[i];

			D3DVIEWPORT9 vp;
			vp.X	  = (DWORD)batch->Rect.x;
			vp.Y	  = (DWORD)batch->Rect.y;
			vp.Width  = (DWORD)batch->Rect.w;
			vp.Height = (DWORD)batch->Rect.h;
			vp.MinZ	  = 0.0f;
			vp.MaxZ	  = 1.0f;

			RECT clip_rect;
			clip_rect.left	 = (LONG)(batch->Clip.x);
			clip_rect.top	 = (LONG)(batch->Clip.y);
			clip_rect.right  = (LONG)(batch->Clip.x + batch->Clip.w);
			clip_rect.bottom = (LONG)(batch->Clip.y + batch->Clip.h);

			g->Device->SetViewport( &vp );
			g->Device->SetScissorRect( &clip_rect );
			g->Device->SetTransform( D3DTS_PROJECTION, (const D3DXMATRIX*)(&bat.Proj) );
			g->Device->SetTransform( D3DTS_VIEW		 , (const D3DXMATRIX*)(&bat.View) );
			for ( int j = 0; j < bat.NumCmd; ++j )
			{
				const MeshCmd_s& cmd = bat.Cmd[j];
				for ( int k = 0; k < cmd.NumWsm; ++k )
				{
					g->Device->SetTransform( D3DTS_WORLD, (const D3DXMATRIX*)(&cmd.Wsm[ k ]) );
					g->Device->DrawIndexedPrimitiveUP
							( D3DPT_TRIANGLELIST
							, 0
							, cmd.Mesh->NumVtx
							, cmd.Mesh->NumIdx/3
							, cmd.Mesh->Idx
							, D3DFMT_INDEX16
							, cmd.Mesh->Vtx
							, sizeof(MeshVtx_s) 
							);
				}
			}
		}
	}

	struct SwapChain_d3d9_s
	{
		Gfx_d3d9_t Gfx;
		D3DPRESENT_PARAMETERS PresentParams;
		IDirect3DSwapChain9* SwapChain;
	};

} }

//======================================================================================
// Gfx - d3d9 - Public
//======================================================================================
namespace nemesis { namespace graphics 
{
	Gfx_d3d9_t Gfx_d3d9_Create( Allocator_t alloc, void* wnd )
	{
		// d3d9
		CComPtr<IDirect3D9> d3d = Direct3DCreate9( D3D_SDK_VERSION );
		if (!d3d)
			return nullptr;

		// device
		const bool windowed = true;
		const bool vsync	= true;
		D3DPRESENT_PARAMETERS pp = D3D9_MakePresentParams( (HWND)wnd, windowed, vsync );
		CComPtr<IDirect3DDevice9> device;
		d3d->CreateDevice
			( 0
			, D3DDEVTYPE_HAL
			, pp.hDeviceWindow
			, D3DCREATE_HARDWARE_VERTEXPROCESSING
			, &pp
			, &device
			);
		if (!device)
			return nullptr;

		// texture
		CComPtr<IDirect3DTexture9> texture;
		D3DXCreateTexture( device, 2, 2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture );
		if (!texture)
			return nullptr;
		D3DXCOLOR color = 0xffffffff;
		D3DXFillTexture( texture, D3D9_FillTexture_Color, &color );

		// instance
		Gfx_d3d9_t g = Mem_Calloc<Gfx_d3d9_s>( alloc );
		if (!g)
			return nullptr;
		g->Alloc		 = alloc;
		g->D3D			 = d3d;
		g->Device		 = device;
		g->PresentParams = pp;
		g->WhiteTexture  = texture;
		return g;
	}

	void Gfx_d3d9_Resize( Gfx_d3d9_t g )
	{
		if (!g)
			return;

		// release buffers
		g->IndexBuffer  = nullptr;
		g->VertexBuffer = nullptr;
		g->NumIdx = 0;
		g->NumVtx = 0;

		// update size
		RECT rc;
		GetClientRect( g->PresentParams.hDeviceWindow, &rc );
		g->PresentParams.BackBufferWidth = rc.right-rc.left;
		g->PresentParams.BackBufferHeight = rc.bottom-rc.top;

		// reset
		HRESULT hr;
		D3D9_V(g->Device->Reset( &g->PresentParams ));
	}

	void Gfx_d3d9_Release( Gfx_d3d9_t g )
	{
		if (!g)
			return;
		g->~Gfx_d3d9_s();
		Mem_Free( g->Alloc, g );
	}

	void Gfx_d3d9_Begin( Gfx_d3d9_t g )
	{
		HRESULT hr;
		D3D9_V(g->Device->BeginScene());
	}

	void Gfx_d3d9_Clear( Gfx_d3d9_t g, uint32_t color )
	{
		HRESULT hr;
		D3D9_V(g->Device->Clear( 0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 1 ));
	}

	void Gfx_d3d9_End( Gfx_d3d9_t g )
	{
		HRESULT hr;
		D3D9_V(g->Device->EndScene());
	}

	void Gfx_d3d9_Present( Gfx_d3d9_t g )
	{
		HRESULT hr;
		D3D9_V(g->Device->Present( nullptr, nullptr, nullptr, nullptr ));
	}

	bool Gfx_d3d9_Texture_Create( Gfx_d3d9_t g, const TextureDesc_s& info, Texture_d3d9_t* texture )
	{
		HRESULT hr;
		D3DFORMAT fmt = Format_Of(info.Format);
		IDirect3DTexture9* t = nullptr;
		D3D9_V(g->Device->CreateTexture( info.Width, info.Height, 1, 0, fmt, D3DPOOL_MANAGED, &t, nullptr ));
		*texture = (Texture_d3d9_t)t;
		return SUCCEEDED(hr);
	}

	bool Gfx_d3d9_Texture_GetDesc( Gfx_d3d9_t g, Texture_d3d9_t texture, TextureDesc_s& desc )
	{
		HRESULT hr;
		IDirect3DTexture9* t = (IDirect3DTexture9*)texture;
		D3DSURFACE_DESC d3dsd = {};
		D3D9_V(t->GetLevelDesc( 0, &d3dsd ));
		desc.Width = d3dsd.Width;
		desc.Height = d3dsd.Height;
		desc.Format = Format_Of(d3dsd.Format);
		return SUCCEEDED(hr);
	}

	bool Gfx_d3d9_Texture_Lock( Gfx_d3d9_t g, Texture_d3d9_t texture, Lock::Mode mode, LockDesc_s& lock )
	{
		HRESULT hr;
		IDirect3DTexture9* t = (IDirect3DTexture9*)texture;
		DWORD flags = (mode == gui::TextureLock::Read ? D3DLOCK_READONLY : 0);
		D3DLOCKED_RECT lr = {};
		D3D9_V(t->LockRect( 0, &lr, NULL, flags ));
		lock.Data = lr.pBits;
		lock.Pitch = lr.Pitch;
		return SUCCEEDED(hr);
	}

	void Gfx_d3d9_Texture_Unlock( Gfx_d3d9_t g, Texture_d3d9_t texture )
	{
		HRESULT hr;
		IDirect3DTexture9* t = (IDirect3DTexture9*)texture;
		D3D9_V(t->UnlockRect( 0 ));
	}

	void Gfx_d3d9_Texture_Release( Gfx_d3d9_t g, Texture_d3d9_t texture )
	{
		if (!texture)
			return;
		IDirect3DTexture9* t = (IDirect3DTexture9*)texture;
		t->Release();
	}

	bool Gfx_d3d9_SwapChain_Create( Gfx_d3d9_t g, const SwapChainDesc_s& setup, SwapChain_d3d9_t* out )
	{
		HWND hWnd = (HWND)setup.Wnd;
		uint32_t buffer_w = setup.Width;
		uint32_t buffer_h = setup.Height;
		if ((setup.Width < 0) || (setup.Width < 0))
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			buffer_w = rc.right-rc.left;
			buffer_h = rc.bottom-rc.top;
		}

		D3DPRESENT_PARAMETERS desc;
		NeZero(desc);
		desc.Windowed = true;
		desc.SwapEffect = D3DSWAPEFFECT_DISCARD;
		desc.BackBufferWidth = buffer_w;
		desc.BackBufferHeight = buffer_h;
		desc.BackBufferFormat = g->PresentParams.BackBufferFormat;
		desc.EnableAutoDepthStencil = g->PresentParams.EnableAutoDepthStencil;
		desc.AutoDepthStencilFormat = g->PresentParams.AutoDepthStencilFormat;
		desc.hDeviceWindow = hWnd;

		HRESULT hr;
		IDirect3DSwapChain9* swap_chain = nullptr;
		D3D9_V(g->Device->CreateAdditionalSwapChain(&desc, &swap_chain));
		if (FAILED(hr))
		{
			*out = nullptr;
			return false;
		}

		SwapChain_d3d9_s* wrapper = Mem_Calloc<SwapChain_d3d9_s>(g->Alloc);
		wrapper->Gfx = g;
		wrapper->PresentParams = desc;
		wrapper->SwapChain = swap_chain;
		*out = wrapper;
		return SUCCEEDED(hr);
	}

	void Gfx_d3d9_SwapChain_Resize( SwapChain_d3d9_t swap, int w, int h )
	{
		uint32_t buffer_w = w;
		uint32_t buffer_h = h;
		if ((w < 0) || (h < 0))
		{
			RECT rc;
			GetClientRect(swap->PresentParams.hDeviceWindow, &rc);
			buffer_w = rc.right-rc.left;
			buffer_h = rc.bottom-rc.top;
		}

		if ((swap->PresentParams.BackBufferWidth == buffer_w) && (swap->PresentParams.BackBufferHeight == buffer_h))
			return;

		D3DPRESENT_PARAMETERS desc = swap->PresentParams;
		desc.BackBufferWidth = w;
		desc.BackBufferHeight = h;

		HRESULT hr;
		IDirect3DSwapChain9* swap_chain = nullptr;
		D3D9_V(swap->Gfx->Device->CreateAdditionalSwapChain(&desc, &swap_chain));
		if (FAILED(hr))
			return;

		swap->SwapChain->Release();
		swap->PresentParams = desc;
		swap->SwapChain = swap_chain;
	}

	void Gfx_d3d9_SwapChain_Begin( SwapChain_d3d9_t swap )
	{
		HRESULT hr;
		CComPtr<IDirect3DSurface9> render_target;
		D3D9_V(swap->SwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &render_target));
		D3D9_V(swap->Gfx->Device->SetRenderTarget(0, render_target));
	}

	void Gfx_d3d9_SwapChain_End( SwapChain_d3d9_t swap )
	{
		HRESULT hr;
		CComPtr<IDirect3DSurface9> render_target;
		D3D9_V(swap->Gfx->Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &render_target));
		D3D9_V(swap->Gfx->Device->SetRenderTarget(0, render_target));
	}

	void Gfx_d3d9_SwapChain_Present( SwapChain_d3d9_t swap )
	{
		HRESULT hr;
		D3D9_V(swap->SwapChain->Present(nullptr, nullptr, nullptr, nullptr, 0));
	}

	void Gfx_d3d9_SwapChain_Release( SwapChain_d3d9_t swap )
	{
		swap->SwapChain->Release();
		Mem_Free(swap->Gfx->Alloc, swap);
	}

	void Gfx_d3d9_Draw_2D( Gfx_d3d9_t g, const DrawBatch_s* batch, int count )
	{
		using namespace gui;

		if (!Gfx_d3d9_CopyData_2D( g, batch, count ))
			return;

		CComPtr<IDirect3DStateBlock9> state_block;
		g->Device->CreateStateBlock( D3DSBT_ALL, &state_block );
		Gfx_d3d9_ApplyState_2D( g );
		Gfx_d3d9_ApplyTransform_2D( g );
		Gfx_d3d9_DrawBatch_2D( g, batch, count );
		state_block->Apply();
	}

	void Gfx_d3d9_Draw_3D( Gfx_d3d9_t g, const MeshBatch_s* batch, int count )
	{
		CComPtr<IDirect3DStateBlock9> state_block;
		g->Device->CreateStateBlock( D3DSBT_ALL, &state_block );
		Gfx_d3d9_ApplyState_3D( g );
		Gfx_d3d9_DrawBatch_3D( g, batch, count );
		state_block->Apply();
	}
} }