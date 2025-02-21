//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gfx/Gfx_d3d11.h>
#include <Nemesis/Core/Array.h>
#include <Nemesis/Core/Math.h>

//======================================================================================
#define NO_COMPOSITION	0

//======================================================================================
#include "Private/Toolkit_d3d11.h"

//======================================================================================
#include <dcomp.h>
#pragma comment(lib, "dcomp.lib")

//======================================================================================
using namespace nemesis::gui;

//======================================================================================
// Gfx - d3d11 - Shared Types
//======================================================================================
namespace nemesis { namespace graphics 
{
	#include "hlsl/hlsl_phong.h"

} }

//======================================================================================
// Gfx - d3d11 - Shaders
//======================================================================================
namespace
{
	#if (NE_CONFIG == NE_CONFIG_DEBUG)
	#	include "hlsl\inc\Debug\ps_2d.h"
	#	include "hlsl\inc\Debug\ps_3d.h"
	#	include "hlsl\inc\Debug\ps_3d_phong.h"
	#	include "hlsl\inc\Debug\vs_2d.h"
	#	include "hlsl\inc\Debug\vs_3d.h"
	#	include "hlsl\inc\Debug\vs_3d_phong.h"
	#	include "hlsl\inc\Debug\vs_3d_lines.h"
	#	include "hlsl\inc\Debug\vs_3d_skinned.h"
	#elif (NE_CONFIG == NE_CONFIG_RELEASE)
	#	include "hlsl\inc\Release\ps_2d.h"
	#	include "hlsl\inc\Release\ps_3d.h"
	#	include "hlsl\inc\Release\ps_3d_phong.h"
	#	include "hlsl\inc\Release\vs_2d.h"
	#	include "hlsl\inc\Release\vs_3d.h"
	#	include "hlsl\inc\Release\vs_3d_phong.h"
	#	include "hlsl\inc\Release\vs_3d_lines.h"
	#	include "hlsl\inc\Release\vs_3d_skinned.h"
	#elif (NE_CONFIG == NE_CONFIG_MASTER)
	#	include "hlsl\inc\Master\ps_2d.h"
	#	include "hlsl\inc\Master\ps_3d.h"
	#	include "hlsl\inc\Master\ps_3d_phong.h"
	#	include "hlsl\inc\Master\vs_2d.h"
	#	include "hlsl\inc\Master\vs_3d.h"
	#	include "hlsl\inc\Master\vs_3d_phong.h"
	#	include "hlsl\inc\Master\vs_3d_lines.h"
	#	include "hlsl\inc\Master\vs_3d_skinned.h"
	#endif
}

//======================================================================================
// Gfx - d3d11 - Input Layouts
//======================================================================================
namespace nemesis { namespace graphics
{
	static const D3D11_INPUT_ELEMENT_DESC vs_2d_il[]
	{ { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT	 , 0, offsetof(DrawVtx_s, Pos)	, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	 , 0, offsetof(DrawVtx_s, UV)	, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "COLOR"	  , 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, offsetof(DrawVtx_s, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC vs_3d_il[]
	{ { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVtx_s, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVtx_s, Normal)  , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	  , 0, offsetof(MeshVtx_s, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "COLOR"	  , 0, DXGI_FORMAT_B8G8R8A8_UNORM , 0, offsetof(MeshVtx_s, Color)   , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC vs_3d_phong_il[]
	{ { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVtx_s, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVtx_s, Normal)  , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	  , 0, offsetof(MeshVtx_s, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "COLOR"	  , 0, DXGI_FORMAT_B8G8R8A8_UNORM , 0, offsetof(MeshVtx_s, Color)   , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC vs_3d_lines_il[]
	{ { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(LineVtx_s, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "COLOR"	  , 0, DXGI_FORMAT_B8G8R8A8_UNORM , 0, offsetof(LineVtx_s, Color)   , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC vs_3d_skinned_il[]
	{ { "POSITION"	  , 0, DXGI_FORMAT_R32G32B32_FLOAT	 , 0, offsetof(SkinVtx_s, Pos)	   , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(SkinVtx_s, Weight)  , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "BLENDINDICES", 0, DXGI_FORMAT_R16G16B16A16_UINT , 0, offsetof(SkinVtx_s, BoneId)  , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "NORMAL"	  , 0, DXGI_FORMAT_R32G32B32_FLOAT	 , 0, offsetof(SkinVtx_s, Normal)  , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "TEXCOORD"	  , 0, DXGI_FORMAT_R32G32_FLOAT		 , 0, offsetof(SkinVtx_s, Texture) , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "COLOR"		  , 0, DXGI_FORMAT_B8G8R8A8_UNORM	 , 0, offsetof(SkinVtx_s, Color)   , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	, { "BLENDINDICES", 1, DXGI_FORMAT_R32G32_UINT		 , 0, offsetof(SkinVtx_s, BlendIdx), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

} }

//======================================================================================
// Gfx - d3d11 - Swap Chain
//======================================================================================
namespace nemesis { namespace graphics
{
	static void D3D11_BindSwapChain( ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swap, ID3D11Texture2D* depth_stencil )
	{
		HRESULT hr;
		CComPtr<ID3D11Texture2D> back_buffer;
		D3D11_V(swap->GetBuffer( 0, IID_PPV_ARGS(&back_buffer) ));

		CComPtr<ID3D11RenderTargetView> rtv;
		CComPtr<ID3D11DepthStencilView> dsv;
		D3D11_V(device->CreateRenderTargetView( back_buffer, nullptr, &rtv ));
		if (depth_stencil)
			D3D11_V(device->CreateDepthStencilView( depth_stencil, nullptr, &dsv ));

		D3D11_TEXTURE2D_DESC rtd = {};
		D3D11_TEXTURE2D_DESC dsd = {};
		back_buffer->GetDesc(&rtd);
		if (depth_stencil)
			depth_stencil->GetDesc(&dsd);

		context->OMSetRenderTargets( 1, &rtv.p, dsv );
	}

} }

//======================================================================================
// Gfx - d3d11 - Constant Buffers
//======================================================================================
namespace nemesis { namespace graphics
{
	struct Vs_2d_s
	{
		Mat4_s Wvp;
	};

	struct Vs_3d_s
	{
		Mat4_s Wvp;
	};

	struct Vs_3d_Lines_s
	{
		Mat4_s Wvp;
	};

	struct Vs_3d_Phong_s
	{
		Mat4_s Vp;
		Mat4_s Wsm;
	};

	struct Vs_3d_Skinned_s
	{
		Mat4_s Vp;
		Mat4_s Wsm[255];
	};

	static Mat4_s Mat4_View( const DXGI_SWAP_CHAIN_DESC& scd )
	{
		const float sub_pixel_offset = 0.0f;

		const float L = sub_pixel_offset;
		const float R = sub_pixel_offset + scd.BufferDesc.Width;
		const float T = sub_pixel_offset;
		const float B = sub_pixel_offset + scd.BufferDesc.Height;

		return Mat4_s
			{ 2.0f/(R-L),   0.0f,         0.0f,  0.0f
			, 0.0f,         2.0f/(T-B),   0.0f,  0.0f
			, 0.0f,         0.0f,         0.5f,  0.0f
			, (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
			};
	}

	static HRESULT D3D11DeviceContext_FillConstantBuffer_Vs_2d( ID3D11DeviceContext* dc, ID3D11Buffer* gpu_data, const Mat4_s& wvp )
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_VR(dc->Map( gpu_data, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
		{
			Vs_2d_s* cpu_data = (Vs_2d_s*)map.pData;
			cpu_data->Wvp = wvp;
		}
		dc->Unmap( gpu_data, 0 );
		return S_OK;
	}

	static HRESULT D3D11DeviceContext_FillConstantBuffer_Vs_3d( ID3D11DeviceContext* dc, ID3D11Buffer* gpu_data, const Mat4_s& wvp )
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_VR(dc->Map( gpu_data, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
		{
			Vs_3d_s* cpu_data = (Vs_3d_s*)map.pData;
			cpu_data->Wvp = wvp;
		}
		dc->Unmap( gpu_data, 0 );
		return S_OK;
	}

	static HRESULT D3D11DeviceContext_FillConstantBuffer_Vs_3d_Phong( ID3D11DeviceContext* dc, ID3D11Buffer* gpu_data, const Mat4_s& world, const Mat4_s& view_proj )
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_VR(dc->Map( gpu_data, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
		{
			Vs_3d_Phong_s* cpu_data = (Vs_3d_Phong_s*)map.pData;
			cpu_data->Vp = view_proj;
			cpu_data->Wsm = world;
		}
		dc->Unmap( gpu_data, 0 );
		return S_OK;
	}

	static HRESULT D3D11DeviceContext_FillConstantBuffer_Vs_3d_Skinned( ID3D11DeviceContext* dc, ID3D11Buffer* gpu_data, const SkinBatch_s& bat, const SkinCmd_s& cmd )
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_VR(dc->Map( gpu_data, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
		{
			Vs_3d_Skinned_s* cpu_data = (Vs_3d_Skinned_s*)map.pData;
			cpu_data->Vp = bat.View * bat.Proj;
			for ( int k = 0; k < cmd.Mesh->NumBones; ++k )
			{
				const int rig_bone = cmd.Mesh->SkinToRigBone[k];
				if (rig_bone > 0)
				{
					NeAssert(rig_bone < cmd.NumWsm);
					const Mat4_s& wsm_bone = cmd.Wsm[ rig_bone ];
					const Mat4_s& bind_pose = cmd.Mesh->BindPose[ k ];
					const Mat4_s wsm = bind_pose * wsm_bone;
					cpu_data->Wsm[k] = wsm;
				}
				else
				{
					cpu_data->Wsm[k] = Mat4_Id(); 
				}
			}
		}
		dc->Unmap( gpu_data, 0 );
		return S_OK;
	}

} }

//======================================================================================
// Gfx - d3d11 - Pixel State
//======================================================================================
namespace nemesis { namespace graphics
{
	struct PixelMode
	{
		enum Enum
		{ Ps_2d
		, Ps_3d
		, Ps_3d_Phong
		, COUNT 
		};
	};

	struct State_Ps_d3d11_s
	{
		CComPtr<ID3D11PixelShader>	PsShader;
		CComPtr<ID3D11Buffer>		PsConstants;
		FLOAT						BlendFactor[4];
		UINT						SampleMask;
		UINT						StencilRef;
	};

	static HRESULT D3D11Device_CreateState_Ps_2d( ID3D11Device* device, State_Ps_d3d11_s& s )
	{
		s.SampleMask = 0xffffffff;

		HRESULT hr;
		D3D11_VR(device->CreatePixelShader( ps_2d, sizeof(ps_2d), nullptr, &s.PsShader ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Ps_3d( ID3D11Device* device, State_Ps_d3d11_s& s )
	{
		s.SampleMask = 0xffffffff;

		HRESULT hr;
		D3D11_VR(device->CreatePixelShader( ps_3d, sizeof(ps_3d), nullptr, &s.PsShader ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Ps_3d_Phong( ID3D11Device* device, State_Ps_d3d11_s& s )
	{
		s.SampleMask = 0xffffffff;

		HRESULT hr;
		D3D11_VR(device->CreatePixelShader( ps_3d_phong, sizeof(ps_3d_phong), nullptr, &s.PsShader ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Ps_Phong_s), &s.PsConstants ));
		return S_OK;
	}

	static HRESULT D3D11DeviceContext_FillConstantBuffer_Ps_3d_Phong( ID3D11DeviceContext* dc, ID3D11Buffer* gpu_data, const Ps_Phong_s& v )
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_VR(dc->Map( gpu_data, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
		{
			Mem_Cpy( map.pData, &v, sizeof(v) );
		}
		dc->Unmap( gpu_data, 0 );
		return S_OK;
	}

	static void D3D11DeviceContext_BindState_Ps( ID3D11DeviceContext* dc, const State_Ps_d3d11_s& s )
	{
		dc->PSSetShader( s.PsShader, nullptr, 0 );
		dc->PSSetConstantBuffers( 0, 1, &s.PsConstants.p );
	}

} }

//======================================================================================
// Gfx - d3d11 - Vertex State
//======================================================================================
namespace nemesis { namespace graphics
{
	struct VertexMode
	{
		enum Enum
		{ Vs_2d
		, Vs_3d
		, Vs_3d_Lines
		, Vs_3d_Phong
		, Vs_3d_Skinned
		, COUNT 
		};
	};

	struct State_Vs_d3d11_s
	{
		CComPtr<ID3D11InputLayout>	InputLayout;
		CComPtr<ID3D11VertexShader>	VsShader;
		CComPtr<ID3D11Buffer>		VsConstants;
	};

	static HRESULT D3D11Device_CreateState_Vs_2d( ID3D11Device* device, State_Vs_d3d11_s& s )
	{
		HRESULT hr;
		D3D11_VR(device->CreateVertexShader( vs_2d, sizeof(vs_2d), nullptr, &s.VsShader ));
		D3D11_VR(device->CreateInputLayout( vs_2d_il, NeCountOf(vs_2d_il), vs_2d, sizeof(vs_2d), &s.InputLayout ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Vs_2d_s), &s.VsConstants ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Vs_3d( ID3D11Device* device, State_Vs_d3d11_s& s )
	{
		HRESULT hr;
		D3D11_VR(device->CreateVertexShader( vs_3d, sizeof(vs_3d), nullptr, &s.VsShader ));
		D3D11_VR(device->CreateInputLayout( vs_3d_il, NeCountOf(vs_3d_il), vs_3d, sizeof(vs_3d), &s.InputLayout ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Vs_3d_s), &s.VsConstants ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Vs_3d_Lines( ID3D11Device* device, State_Vs_d3d11_s& s )
	{
		HRESULT hr;
		D3D11_VR(device->CreateVertexShader( vs_3d_lines, sizeof(vs_3d_lines), nullptr, &s.VsShader ));
		D3D11_VR(device->CreateInputLayout( vs_3d_lines_il, NeCountOf(vs_3d_lines_il), vs_3d_lines, sizeof(vs_3d_lines), &s.InputLayout ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Vs_3d_Lines_s), &s.VsConstants ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Vs_3d_Phong( ID3D11Device* device, State_Vs_d3d11_s& s )
	{
		HRESULT hr;
		D3D11_VR(device->CreateVertexShader( vs_3d_phong, sizeof(vs_3d_phong), nullptr, &s.VsShader ));
		D3D11_VR(device->CreateInputLayout( vs_3d_phong_il, NeCountOf(vs_3d_phong_il), vs_3d_phong, sizeof(vs_3d_phong), &s.InputLayout ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Vs_3d_Phong_s), &s.VsConstants ));
		return S_OK;
	}

	static HRESULT D3D11Device_CreateState_Vs_3d_Skinned( ID3D11Device* device, State_Vs_d3d11_s& s )
	{
		HRESULT hr;
		D3D11_VR(device->CreateVertexShader( vs_3d_skinned, sizeof(vs_3d_skinned), nullptr, &s.VsShader ));
		D3D11_VR(device->CreateInputLayout( vs_3d_skinned_il, NeCountOf(vs_3d_skinned_il), vs_3d_skinned, sizeof(vs_3d_skinned), &s.InputLayout ));
		D3D11_VR(D3D11Device_CreateConstantBuffer( device, sizeof(Vs_3d_Skinned_s), &s.VsConstants ));
		return S_OK;
	}

	static void D3D11DeviceContext_BindState_Vs( ID3D11DeviceContext* dc, const State_Vs_d3d11_s& s )
	{
		dc->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		dc->IASetInputLayout( s.InputLayout );
		dc->VSSetShader( s.VsShader, nullptr, 0 );
		dc->VSSetConstantBuffers( 0, 1, &s.VsConstants.p );
	}

	static void D3D11DeviceContext_BindState_Vs_Lines( ID3D11DeviceContext* dc, const State_Vs_d3d11_s& s )
	{
		dc->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
		dc->IASetInputLayout( s.InputLayout );
		dc->VSSetShader( s.VsShader, nullptr, 0 );
		dc->VSSetConstantBuffers( 0, 1, &s.VsConstants.p );
	}

} }

//======================================================================================
// Gfx - d3d11 - Detail
//======================================================================================
namespace nemesis { namespace graphics
{
	static Format::Enum Format_Of( DXGI_FORMAT v )
	{
		switch (v)
		{
		case DXGI_FORMAT_R8_UNORM			: return Format::R8_UNorm			;
		case DXGI_FORMAT_R8_UINT			: return Format::R8_UInt			;
		case DXGI_FORMAT_R8G8B8A8_UNORM		: return Format::R8G8B8A8_UNorm		;
		case DXGI_FORMAT_R8G8B8A8_UINT		: return Format::R8G8B8A8_UInt		;
		case DXGI_FORMAT_B8G8R8A8_UNORM		: return Format::B8G8R8A8_Unorm		;
		case DXGI_FORMAT_R32G32_FLOAT		: return Format::R32G32_Float		;
		case DXGI_FORMAT_R32G32_UINT		: return Format::R32G32_UInt		;
		case DXGI_FORMAT_R32G32B32_FLOAT	: return Format::R32G32B32_Float	;
		case DXGI_FORMAT_R32G32B32_UINT		: return Format::R32G32B32_UInt		;
		case DXGI_FORMAT_R32G32B32A32_FLOAT	: return Format::R32G32B32A32_Float	;
		case DXGI_FORMAT_R32G32B32A32_UINT	: return Format::R32G32B32A32_UInt	;
		default:
			break;
		}
		return Format::Unknown;
	}

	static DXGI_FORMAT Format_Of( Format::Enum v )
	{
		switch (v)
		{
		case Format::R8_UNorm			: return DXGI_FORMAT_R8_UNORM			;
		case Format::R8_UInt			: return DXGI_FORMAT_R8_UINT			;
		case Format::R8G8B8A8_UNorm		: return DXGI_FORMAT_R8G8B8A8_UNORM		;
		case Format::R8G8B8A8_UInt		: return DXGI_FORMAT_R8G8B8A8_UINT		;
		case Format::B8G8R8A8_Unorm		: return DXGI_FORMAT_B8G8R8A8_UNORM		;
		case Format::R32G32_Float		: return DXGI_FORMAT_R32G32_FLOAT		;
		case Format::R32G32_UInt		: return DXGI_FORMAT_R32G32_UINT		;
		case Format::R32G32B32_Float	: return DXGI_FORMAT_R32G32B32_FLOAT	;
		case Format::R32G32B32_UInt		: return DXGI_FORMAT_R32G32B32_UINT		;
		case Format::R32G32B32A32_Float	: return DXGI_FORMAT_R32G32B32A32_FLOAT	;
		case Format::R32G32B32A32_UInt	: return DXGI_FORMAT_R32G32B32A32_UINT	;
		default:
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

} }

//======================================================================================
// Gfx - d3d11 - Private
//======================================================================================
namespace nemesis { namespace graphics 
{
	#define NeMarker ::nemesis::graphics::ScopedMarker_c NeUnique(marker)

	class ScopedMarker_c
	{
	public:
		ScopedMarker_c( ID3DUserDefinedAnnotation* p, LPCWSTR name )
			: Annotation( p )
		{
			Annotation->BeginEvent( name );
		}

		~ScopedMarker_c()
		{
			Annotation->EndEvent();
		}

	public:
		ID3DUserDefinedAnnotation* Annotation;
	};

	struct SwapChain_d3d11_s
	{
		Gfx_d3d11_t Gfx;
		CComPtr<IDXGISwapChain> SwapChain;
		CComPtr<ID3D11Texture2D> DepthStencil;
		CComPtr<IDCompositionVisual> WdmVisual;
		CComPtr<IDCompositionTarget> WdmTarget;
	};

	struct Staging_d3d11_s
	{
		ID3D11Texture2D* Src;
		ID3D11Texture2D* Dst;
	};

	struct Gfx_d3d11_s
	{
		Allocator_t							Alloc;
		CComPtr<IDXGIFactory2>				Factory;
		CComPtr<IDXGIAdapter>				Adapter;
		CComPtr<IDXGIOutput>				Monitor;
		CComPtr<ID3D11Device>				Device;
		CComPtr<ID3D11DeviceContext>		Context;
		CComPtr<ID3DUserDefinedAnnotation>	Annotation;
		CComPtr<IDCompositionDevice>		Composition;
		CComPtr<ID3D11Texture2D>			WhiteTexture;
		CComPtr<ID3D11BlendState>			BlendState;
		CComPtr<ID3D11BlendState>			BlendState2;
		CComPtr<ID3D11SamplerState>			SamplerState;
		CComPtr<ID3D11RasterizerState>		RasterizerState	 [ CullMode ::COUNT ];
		CComPtr<ID3D11DepthStencilState>	DepthStencilState[ DepthMode::COUNT ];
		State_Ps_d3d11_s					PixelState		 [ PixelMode::COUNT ];
		State_Vs_d3d11_s					VertexState		 [ VertexMode::COUNT ];
		SwapChain_d3d11_s					DeviceSwapChain;
		SwapChain_d3d11_s*					SwapChain;
		Ps_Phong_s							Phong;
		Primitive_d3d11_s					Primitive;
		BlendShape_d3d11_s					BlendShape;
		Array<Staging_d3d11_s>				Staging;
		Array<const SkinnedMesh_s*>			SkinnedMeshKeys;
		Array<Primitive_d3d11_s*>			SkinnedMeshVals;
		Array<const SkinnedMesh_s*>			BlendShapeKeys;
		Array<BlendShape_d3d11_s*>			BlendShapeVals;
	};

	static void Gfx_d3d11_BindSwapChain( Gfx_d3d11_t g, SwapChain_d3d11_s* swap )
	{
		NeAssert(swap->Gfx == g);
		D3D11_BindSwapChain( g->Device, g->Context, swap->SwapChain, swap->DepthStencil );
		g->SwapChain = swap;
	}

	static void Gfx_d3d11_ClearSwapChain( Gfx_d3d11_t g, SwapChain_d3d11_s* swap, uint32_t color, float depth = 1.0f )
	{
		NeAssert(swap->Gfx == g);

		HRESULT hr;
		CComPtr<ID3D11Texture2D> buffer;
		D3D11_V(swap->SwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**) &(buffer.p) ));

		CComPtr<ID3D11RenderTargetView> rtv;
		D3D11_V(g->Device->CreateRenderTargetView( buffer, nullptr, &rtv ));
		const Color_c c = Color_FromArgb( color );
		g->Context->ClearRenderTargetView( rtv, &c.r );

		if (swap->DepthStencil)
		{
			CComPtr<ID3D11DepthStencilView> dsv;
			D3D11_V(g->Device->CreateDepthStencilView( swap->DepthStencil, nullptr, &dsv ));
			g->Context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, depth, 0 );
		}
	}

	static void Gfx_d3d11_ResizeSwapChain( Gfx_d3d11_t g, SwapChain_d3d11_s* swap, UINT w, UINT h )
	{
		NeAssert(g == swap->Gfx);

		// resize buffers
		HRESULT hr;
		D3D11_V(swap->SwapChain->ResizeBuffers( 0, w, h, DXGI_FORMAT_UNKNOWN, 0 ));

		// create depth stencil
		if (swap->DepthStencil)
		{
			DXGI_SWAP_CHAIN_DESC scd = {};
			D3D11_V(swap->SwapChain->GetDesc( &scd ));

			CComPtr<ID3D11Texture2D> depth_stencil;
			D3D11_V(D3D11Device_CreateDepthStencilTexture( g->Device, scd.BufferDesc.Width, scd.BufferDesc.Height, &depth_stencil ));

			swap->DepthStencil = depth_stencil;
		}

		// re-bind
		if (swap == g->SwapChain)
			Gfx_d3d11_BindSwapChain( g, swap );
	}

	static void Gfx_d3d11_ResizeSwapChain( Gfx_d3d11_t g, SwapChain_d3d11_s* swap )
	{
		Gfx_d3d11_ResizeSwapChain( g, swap, 0, 0 );
	}

	static void Gfx_d3d11_ApplyState_2D( Gfx_d3d11_t g )
	{
		HRESULT hr;

		DXGI_SWAP_CHAIN_DESC scd = {};
		g->SwapChain->SwapChain->GetDesc( &scd );

		const bool is_composite = (g->SwapChain->WdmVisual != nullptr);
		ID3D11BlendState* bs = is_composite ? g->BlendState2 : g->BlendState;

		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_2d ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_2d ];
		D3D11_V(D3D11DeviceContext_FillConstantBuffer_Vs_2d( g->Context, vs.VsConstants, Mat4_View( scd ) ));
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::None ] );
		g->Context->OMSetBlendState( bs, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::Off ], ps.StencilRef );
	}

	static void Gfx_d3d11_ApplyState_3D( Gfx_d3d11_t g )
	{
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d ];
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::Back ] );
		g->Context->OMSetBlendState( g->BlendState, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::On ], ps.StencilRef );
	}

	static void Gfx_d3d11_ApplyState_3D_Lit( Gfx_d3d11_t g )
	{
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d ];
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::Back ] );
		g->Context->OMSetBlendState( g->BlendState, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::On ], ps.StencilRef );
	}

	static void Gfx_d3d11_ApplyState_3D_Lines( Gfx_d3d11_t g )
	{
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Lines ];
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs_Lines( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::None ] );
		g->Context->OMSetBlendState( g->BlendState, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::Off ], ps.StencilRef );
	}

	static void Gfx_d3d11_ApplyState_3D_Phong( Gfx_d3d11_t g )
	{
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Phong ];
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::Back ] );
		g->Context->OMSetBlendState( g->BlendState, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::On ], ps.StencilRef );
	}

	static void Gfx_d3d11_ApplyState_3D_Skinned( Gfx_d3d11_t g )
	{
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Skinned ];
		D3D11DeviceContext_BindState_Ps( g->Context, ps );
		D3D11DeviceContext_BindState_Vs( g->Context, vs );
		g->Context->PSSetSamplers( 0, 1, &g->SamplerState.p );
		g->Context->RSSetState( g->RasterizerState[ CullMode::Back ] );
		g->Context->OMSetBlendState( g->BlendState, ps.BlendFactor, ps.SampleMask );
		g->Context->OMSetDepthStencilState( g->DepthStencilState[ DepthMode::On ], ps.StencilRef );
	}

	static void Gfx_d3d11_UpdatePhong( Gfx_d3d11_t g, const Vec3_s& tint )
	{
		g->Phong.DirLight[0].Ambient	= tint * Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.DirLight[0].Diffuse	= tint * Vec3_s { 0.4f, 0.4f, 0.4f };
		g->Phong.DirLight[0].Specular	= tint * Vec3_s { 0.5f, 0.5f, 0.5f };
		g->Phong.PointLight[0].Ambient	= tint * Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.PointLight[0].Diffuse	= tint * Vec3_s { 0.8f, 0.8f, 0.8f };
		g->Phong.PointLight[0].Specular = tint * Vec3_s { 1.0f, 1.0f, 1.0f };
		g->Phong.PointLight[1].Ambient	= tint * Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.PointLight[1].Diffuse	= tint * Vec3_s { 0.8f, 0.8f, 0.8f };
		g->Phong.PointLight[1].Specular = tint * Vec3_s { 1.0f, 1.0f, 1.0f };
	}

	static bool Gfx_d3d11_UpdateMesh_2D( Gfx_d3d11_t g, const DrawBatch_s* batch, int count )
	{
		HRESULT hr;
		D3D11_V(D3D11Device_CreatePrimitive_2D( g->Device, g->Context, batch, count, &g->Primitive ));
		return SUCCEEDED(hr);
	}

	static bool Gfx_d3d11_UpdateMesh_3D( Gfx_d3d11_t g, const LineBatch_s* batch, int count )
	{
		HRESULT hr;
		D3D11_V(D3D11Device_CreatePrimitive_3D( g->Device, g->Context, batch, count, &g->Primitive ));
		return SUCCEEDED(hr);
	}

	static bool Gfx_d3d11_UpdateMesh_3D( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		HRESULT hr;
		D3D11_V(D3D11Device_CreatePrimitive_3D( g->Device, g->Context, batch, count, &g->Primitive ));
		return SUCCEEDED(hr);
	}

	static bool Gfx_d3d11_UpdateMesh_3D_Phong( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		HRESULT hr;
		D3D11_V(D3D11Device_CreatePrimitive_3D( g->Device, g->Context, batch, count, &g->Primitive ));
		return SUCCEEDED(hr);
	}

	static void Gfx_d3d11_DrawMeshes_2D( Gfx_d3d11_t g, const DrawBatch_s* batch, int count )
	{
		HRESULT hr;

		D3D11DeviceContext_BindPrimitive( g->Context, &g->Primitive );

		DXGI_SWAP_CHAIN_DESC scd = {};
		g->SwapChain->SwapChain->GetDesc( &scd );

		uint32_t index = 0;
		uint32_t vertex = 0;
		for ( int batch_id = 0; batch_id < count; ++batch_id )
		{
			const DrawBatch_s& bat = batch[batch_id];

			for ( uint32_t cmd_id = 0; cmd_id < bat.NumCmd; ++cmd_id )
			{
				const DrawCmd_s& cmd = bat.Cmd[cmd_id];
				D3D11DeviceContext_SetRects( g->Context, cmd, scd );

				ID3D11Texture2D* texture = cmd.Texture ? (ID3D11Texture2D*)cmd.Texture : g->WhiteTexture;
				D3D11_V(D3D11Device_SetTexture( g->Device, g->Context, texture ));

				g->Context->DrawIndexed( cmd.NumIdx, index, vertex );
				index += cmd.NumIdx;
			}
			vertex += bat.NumVtx;
		}
	}

	static void Gfx_d3d11_DrawMeshes_3D_Lines( Gfx_d3d11_t g, const LineBatch_s* batch, int count )
	{
		HRESULT hr;

		const State_Vs_d3d11_s& s = g->VertexState[ VertexMode::Vs_3d ];

		D3D11DeviceContext_BindPrimitive( g->Context, &g->Primitive );
		D3D11_V(D3D11Device_SetTexture( g->Device, g->Context, g->WhiteTexture ));

		DXGI_SWAP_CHAIN_DESC scd = {};
		g->SwapChain->SwapChain->GetDesc( &scd );

		uint32_t index = 0;
		uint32_t vertex = 0;
		uint32_t batch_vtx = 0;
		for ( int batch_id = 0; batch_id < count; ++batch_id )
		{
			const LineBatch_s& bat = batch[batch_id];
			D3D11DeviceContext_SetRects( g->Context, bat.Rect, bat.Clip );

			const Mat4_s wvp = bat.World * bat.View * bat.Proj;
			D3D11_V(D3D11DeviceContext_FillConstantBuffer_Vs_3d( g->Context, s.VsConstants, wvp ));
			g->Context->VSSetConstantBuffers( 0, 1, &s.VsConstants.p );
			g->Context->DrawInstanced( batch->NumVtx, 1, 0, 0 );

			vertex += batch->NumVtx;
			batch_vtx = 0;
		}
	}

	static void Gfx_d3d11_DrawMeshes_3D( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		HRESULT hr;

		const State_Vs_d3d11_s& s = g->VertexState[ VertexMode::Vs_3d ];

		D3D11DeviceContext_BindPrimitive( g->Context, &g->Primitive );
		D3D11_V(D3D11Device_SetTexture( g->Device, g->Context, g->WhiteTexture ));

		DXGI_SWAP_CHAIN_DESC scd = {};
		g->SwapChain->SwapChain->GetDesc( &scd );

		uint32_t index = 0;
		uint32_t vertex = 0;
		uint32_t batch_vtx = 0;
		for ( int batch_id = 0; batch_id < count; ++batch_id )
		{
			const MeshBatch_s& bat = batch[batch_id];
			D3D11DeviceContext_SetRects( g->Context, bat.Rect, bat.Clip );

			const Mat4_s mat_vp = bat.View * bat.Proj;

			for ( int cmd_id = 0; cmd_id < bat.NumCmd; ++cmd_id )
			{
				const MeshCmd_s& cmd = bat.Cmd[cmd_id];

				for ( int wsm_id = 0; wsm_id < cmd.NumWsm; ++wsm_id )
				{
					const Mat4_s wvp = cmd.Wsm[wsm_id] * mat_vp;
					D3D11_V(D3D11DeviceContext_FillConstantBuffer_Vs_3d( g->Context, s.VsConstants, wvp ));
					g->Context->VSSetConstantBuffers( 0, 1, &s.VsConstants.p );
					g->Context->DrawIndexed( cmd.Mesh->NumIdx, index, vertex );
				}

				index += cmd.Mesh->NumIdx;
				batch_vtx += cmd.Mesh->NumVtx;
			}
			vertex += batch_vtx;
			batch_vtx = 0;
		}
	}

	static void Gfx_d3d11_DrawMeshes_3D_Phong( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		HRESULT hr;

		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Phong ];

		D3D11DeviceContext_BindPrimitive( g->Context, &g->Primitive );
		D3D11_V(D3D11Device_SetTexture( g->Device, g->Context, g->WhiteTexture ));

		DXGI_SWAP_CHAIN_DESC scd = {};
		g->SwapChain->SwapChain->GetDesc( &scd );

		uint32_t index = 0;
		uint32_t vertex = 0;
		uint32_t batch_vtx = 0;
		for ( int batch_id = 0; batch_id < count; ++batch_id )
		{
			const MeshBatch_s& bat = batch[batch_id];
			g->Phong.EyePos = (const Vec3_s&)(Mat4_Inv(bat.View).r[3]);
			D3D11DeviceContext_FillConstantBuffer_Ps_3d_Phong( g->Context, ps.PsConstants, g->Phong );
			D3D11DeviceContext_SetRects( g->Context, bat.Rect, bat.Clip );

			const Mat4_s mat_vp = bat.View * bat.Proj;

			for ( int cmd_id = 0; cmd_id < bat.NumCmd; ++cmd_id )
			{
				const MeshCmd_s& cmd = bat.Cmd[cmd_id];

				for ( int wsm_id = 0; wsm_id < cmd.NumWsm; ++wsm_id )
				{
					D3D11_V(D3D11DeviceContext_FillConstantBuffer_Vs_3d_Phong( g->Context, vs.VsConstants, cmd.Wsm[wsm_id], mat_vp ));
					g->Context->VSSetConstantBuffers( 0, 1, &vs.VsConstants.p );
					g->Context->DrawIndexed( cmd.Mesh->NumIdx, index, vertex );
				}

				index += cmd.Mesh->NumIdx;
				batch_vtx += cmd.Mesh->NumVtx;
			}
			vertex += batch_vtx;
			batch_vtx = 0;
		}
	}

	static Primitive_d3d11_s* Gfx_d3d11_EnsurePrimitive( Gfx_d3d11_t g, const SkinnedMesh_s* mesh )
	{
		const int found = Array_BinaryFind( g->SkinnedMeshKeys, mesh );
		if (found >= 0)
			return g->SkinnedMeshVals[found];

		HRESULT hr;
		const int idx = ~found;
		Primitive_d3d11_s* primitive = Mem_Calloc<Primitive_d3d11_s>( g->Alloc );
		D3D11_V(D3D11Device_CreatePrimitive_3D_Skinned( g->Device, g->Context, mesh, primitive ));
		g->SkinnedMeshKeys.InsertAt(idx, mesh);
		g->SkinnedMeshVals.InsertAt(idx, primitive);
		return primitive;
	}

	static BlendShape_d3d11_s* Gfx_d3d11_EnsureBlendShape( Gfx_d3d11_t g, const SkinnedMesh_s* mesh )
	{
		const int found = Array_BinaryFind( g->BlendShapeKeys, mesh );
		if (found >= 0)
			return g->BlendShapeVals[found];

		HRESULT hr;
		const int idx = ~found;
		BlendShape_d3d11_s* shape = Mem_Calloc<BlendShape_d3d11_s>( g->Alloc );
		D3D11_V(D3D11Device_CreateBlendShape_3D_Skinned( g->Device, g->Context, mesh, shape ));
		g->BlendShapeKeys.InsertAt(idx, mesh);
		g->BlendShapeVals.InsertAt(idx, shape);
		return shape;
	}

	static void Gfx_d3d11_DrawCmd_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s& batch, const SkinCmd_s& cmd )
	{
		HRESULT hr;
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Skinned ];

		// Update mesh
		Primitive_d3d11_s* primitive = Gfx_d3d11_EnsurePrimitive( g, cmd.Mesh );
		D3D11DeviceContext_BindPrimitive( g->Context, primitive );

		// Update blend shapes
		const bool has_blend_shapes = cmd.NumFlt && cmd.Mesh->NumOffsets && cmd.Mesh->NumShapes;
		if (has_blend_shapes)
		{
			BlendShape_d3d11_s* blend_shape = Gfx_d3d11_EnsureBlendShape( g, cmd.Mesh );
			D3D11DeviceContext_UpdateBlendShape( g->Context, blend_shape, cmd.Mesh, cmd.Flt, cmd.NumFlt );
			D3D11DeviceContext_BindBlendShape( g->Context, blend_shape, 0, 1 );
		}
		else
		{
			D3D11DeviceContext_BindBlendShape( g->Context, nullptr, 0, 1 );
		}

		// Update constants
		D3D11_V(D3D11DeviceContext_FillConstantBuffer_Vs_3d_Skinned( g->Context, vs.VsConstants, batch, cmd ));
		g->Context->VSSetConstantBuffers( 0, 1, &vs.VsConstants.p );

		// Draw
		g->Context->DrawIndexed( cmd.Mesh->NumIdx, 0, 0 );
	}

	static void Gfx_d3d11_DrawBatch_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s& batch )
	{
		HRESULT hr;
		const State_Ps_d3d11_s& ps = g->PixelState[ PixelMode::Ps_3d_Phong ];
		const State_Vs_d3d11_s& vs = g->VertexState[ VertexMode::Vs_3d_Skinned ];
		g->Phong.EyePos = (const Vec3_s&)(Mat4_Inv(batch.View).r[3]);
		D3D11DeviceContext_FillConstantBuffer_Ps_3d_Phong( g->Context, ps.PsConstants, g->Phong );
		D3D11DeviceContext_SetRects( g->Context, batch.Rect, batch.Clip );
		D3D11_V(D3D11Device_SetTexture( g->Device, g->Context, g->WhiteTexture ));
		const int num_cmd = batch.NumCmd;
		for ( int i = 0; i < num_cmd; ++i )
			Gfx_d3d11_DrawCmd_3D_Skinned( g, batch, batch.Cmd[i] );
	}

} }

//======================================================================================
// Gfx - d3d11 - Public
//======================================================================================
namespace nemesis { namespace graphics 
{
	Gfx_d3d11_t	Gfx_d3d11_Create( Allocator_t alloc, void* wnd )
	{
		HRESULT hr;

		// Adapter and Monitor
		CComPtr<IDXGIFactory2> dxgi;
		DWORD factory_flags = 0;
	#if NE_CONFIG == NE_CONFIG_DEBUG
		factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
	#endif
		D3D11_V(CreateDXGIFactory2(factory_flags, __uuidof(IDXGIFactory2), (void**)&dxgi ));
		if (!dxgi)
			return nullptr;

		CComPtr<IDXGIAdapter> adapter;
		D3D11_V(dxgi->EnumAdapters( 0, &adapter ));
		if (!adapter)
			return nullptr;

		CComPtr<IDXGIOutput> monitor;
		D3D11_V(adapter->EnumOutputs( 0, &monitor ));
		if (!monitor)
			return nullptr;

		// Create device and swap chain
		RECT rc;
		GetClientRect( (HWND)wnd, &rc );

		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferDesc.Width = rc.right-rc.left;
		scd.BufferDesc.Height = rc.bottom-rc.top;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.SampleDesc.Count = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.OutputWindow = (HWND)wnd;
		scd.Windowed = true;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scd.Flags = 0;

		D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
		uint32_t flags = 0;
	#if NE_CONFIG < NE_CONFIG_MASTER
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		CComPtr<ID3D11Device> device;
		CComPtr<ID3D11DeviceContext> context;
		CComPtr<IDXGISwapChain> swap_chain;
		D3D11_V(D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &feature_level, 1, D3D11_SDK_VERSION, &scd, &swap_chain, &device, nullptr, &context )); 
		if (!device || !context || !swap_chain)
			return nullptr;

		// Create depth/stencil buffer
		CComPtr<ID3D11Texture2D> depth_stencil;
		D3D11_V(D3D11Device_CreateDepthStencilTexture( device, scd.BufferDesc.Width, scd.BufferDesc.Height, &depth_stencil ));

		// Create white fallback texture
		CComPtr<ID3D11Texture2D> white_texture;
		D3D11_V(D3D11Device_CreateTexture2D_White( device, &white_texture ));

		CComPtr<ID3DUserDefinedAnnotation> annotation;
		D3D11_V(context->QueryInterface( &annotation ));

		// composition device
		CComPtr<IDXGIDevice> dxgi_device;
		CComPtr<IDCompositionDevice> composition;
	#if	(!NO_COMPOSITION)
		device->QueryInterface(&dxgi_device);
		if (dxgi_device)
			hr = DCompositionCreateDevice( dxgi_device, __uuidof(composition), (void**)&composition.p );
	#endif

		// instance
		Gfx_d3d11_t g = Mem_Calloc<Gfx_d3d11_s>( alloc );
		if (!g)
			return nullptr;

		g->Alloc = alloc;
		g->Factory = dxgi;
		g->Adapter = adapter;
		g->Monitor = monitor;
		g->Device = device;
		g->Context = context;
		g->Annotation = annotation;
		g->Composition = composition;
		g->DeviceSwapChain.Gfx = g;
		g->DeviceSwapChain.SwapChain = swap_chain;
		g->DeviceSwapChain.DepthStencil = depth_stencil;
		g->WhiteTexture = white_texture;
		g->Staging.Alloc = alloc;
		g->SkinnedMeshKeys.Alloc = alloc;
		g->SkinnedMeshVals.Alloc = alloc;
		g->SkinnedMeshKeys.Reserve(1024);
		g->SkinnedMeshVals.Reserve(1024);
		g->BlendShapeKeys.Alloc = alloc;
		g->BlendShapeVals.Alloc = alloc;
		g->BlendShapeKeys.Reserve(1024);
		g->BlendShapeVals.Reserve(1024);

		// Bind swap chain
		Gfx_d3d11_BindSwapChain( g, &g->DeviceSwapChain );

		// states
		D3D11_V(D3D11Device_CreateBlendState( device, &g->BlendState ));
		D3D11_V(D3D11Device_CreateBlendState2( device, &g->BlendState2 ));
		D3D11_V(D3D11Device_CreateSamplerState( device, &g->SamplerState ));
		D3D11_V(D3D11Device_CreateRasterizerState( device, D3D11_CULL_BACK , &(g->RasterizerState[ CullMode::Back  ]) ));
		D3D11_V(D3D11Device_CreateRasterizerState( device, D3D11_CULL_FRONT, &(g->RasterizerState[ CullMode::Front ]) ));
		D3D11_V(D3D11Device_CreateRasterizerState( device, D3D11_CULL_NONE , &(g->RasterizerState[ CullMode::None  ]) ));
		D3D11_V(D3D11Device_CreateDepthStencilState_On ( device, &(g->DepthStencilState[ DepthMode::On  ]) ));
		D3D11_V(D3D11Device_CreateDepthStencilState_Off( device, &(g->DepthStencilState[ DepthMode::Off ]) ));
		D3D11_V(D3D11Device_CreateState_Ps_2d( device, g->PixelState[PixelMode::Ps_2d] ));
		D3D11_V(D3D11Device_CreateState_Ps_3d( device, g->PixelState[PixelMode::Ps_3d] ));
		D3D11_V(D3D11Device_CreateState_Ps_3d_Phong( device, g->PixelState[PixelMode::Ps_3d_Phong] ));
		D3D11_V(D3D11Device_CreateState_Vs_2d( device, g->VertexState[VertexMode::Vs_2d] ));
		D3D11_V(D3D11Device_CreateState_Vs_3d( device, g->VertexState[VertexMode::Vs_3d] ));
		D3D11_V(D3D11Device_CreateState_Vs_3d_Lines( device, g->VertexState[VertexMode::Vs_3d_Lines] ));
		D3D11_V(D3D11Device_CreateState_Vs_3d_Phong( device, g->VertexState[VertexMode::Vs_3d_Phong] ));
		D3D11_V(D3D11Device_CreateState_Vs_3d_Skinned( device, g->VertexState[VertexMode::Vs_3d_Skinned] ));

		// lights
		g->Phong.Opacity = 1.0f;
		g->Phong.Material.Diffuse = Vec3_s { 1.0f, 1.0f, 1.0f };
		g->Phong.Material.Specular = Vec3_s { 1.0f, 1.0f, 1.0f };
		g->Phong.Material.Shininess = 32.0f;
		g->Phong.DirLight[0].Direction = Vec3_s { -0.2f, -1.0f, -0.3f };
		g->Phong.DirLight[0].Ambient = Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.DirLight[0].Diffuse = Vec3_s { 0.4f, 0.4f, 0.4f };
		g->Phong.DirLight[0].Specular = Vec3_s { 0.5f, 0.5f, 0.5f };
		g->Phong.PointLight[0].Position = Vec3_s { 1.5f, 1.5f, 5.0f };
		g->Phong.PointLight[0].Ambient = Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.PointLight[0].Diffuse = Vec3_s { 0.8f, 0.8f, 0.8f };
		g->Phong.PointLight[0].Specular = Vec3_s { 1.0f, 1.0f, 1.0f };
		g->Phong.PointLight[0].Constant = 1.0f;
		g->Phong.PointLight[0].Linear = 0.09f;
		g->Phong.PointLight[0].Quadratic = 0.032f;
		g->Phong.PointLight[1].Position = Vec3_s { -1.5f, -1.5f, -5.0f };
		g->Phong.PointLight[1].Ambient = Vec3_s { 0.05f, 0.05f, 0.05f };
		g->Phong.PointLight[1].Diffuse = Vec3_s { 0.8f, 0.8f, 0.8f };
		g->Phong.PointLight[1].Specular = Vec3_s { 1.0f, 1.0f, 1.0f };
		g->Phong.PointLight[1].Constant = 1.0f;
		g->Phong.PointLight[1].Linear = 0.09f;
		g->Phong.PointLight[1].Quadratic = 0.032f;
		g->Phong.NumDirLights = 1;
		g->Phong.NumPointLights = 2;

		return g;
	}

	void Gfx_d3d11_Resize( Gfx_d3d11_t g )
	{
		Gfx_d3d11_ResizeSwapChain( g, &g->DeviceSwapChain );
	}

	void Gfx_d3d11_CacheOut( Gfx_d3d11_t g )
	{
		for ( int i = 0; i < g->SkinnedMeshVals.Count; ++i )
		{
			Primitive_d3d11_s* primitive = g->SkinnedMeshVals[i];
			primitive->~Primitive_d3d11_s();
			Mem_Free( g->Alloc, primitive );
		}

		for ( int i = 0; i < g->BlendShapeVals.Count; ++i )
		{
			BlendShape_d3d11_s* shape = g->BlendShapeVals[i];
			shape->~BlendShape_d3d11_s();
			Mem_Free( g->Alloc, shape );
		}

		g->SkinnedMeshKeys.Count = 0;
		g->SkinnedMeshVals.Count = 0;
		g->BlendShapeKeys.Count = 0;
		g->BlendShapeVals.Count = 0;
	}

	void Gfx_d3d11_Release( Gfx_d3d11_t g )
	{
		if (!g)
			return;
		Gfx_d3d11_CacheOut( g );
		g->~Gfx_d3d11_s();
		Mem_Free( g->Alloc, g );
	}

	void Gfx_d3d11_Begin( Gfx_d3d11_t g )
	{
	}

	void Gfx_d3d11_Clear( Gfx_d3d11_t g, uint32_t color )
	{
		Gfx_d3d11_ClearSwapChain( g, g->SwapChain, color );
	}

	void Gfx_d3d11_End( Gfx_d3d11_t g )
	{
	}

	void Gfx_d3d11_Present( Gfx_d3d11_t g )
	{
		HRESULT hr;
		D3D11_V(g->SwapChain->SwapChain->Present( 0, 0 ));
		Gfx_d3d11_BindSwapChain( g, &g->DeviceSwapChain );
	}

	void Gfx_d3d11_Present( Gfx_d3d11_t g, bool v_sync )
	{
		HRESULT hr;
		D3D11_V(g->SwapChain->SwapChain->Present( v_sync ? 1 : 0, 0 ));
		Gfx_d3d11_BindSwapChain( g, &g->DeviceSwapChain );
	}

	void Gfx_d3d11_Scope_Begin( Gfx_d3d11_t g, wcstr_t name )
	{
		if (g->Annotation)
			g->Annotation->BeginEvent( name );
	}

	void Gfx_d3d11_Scope_End( Gfx_d3d11_t g )
	{
		if (g->Annotation)
			g->Annotation->EndEvent();
	}

	bool Gfx_d3d11_Texture_Create( Gfx_d3d11_t g, const TextureDesc_s& desc, Texture_d3d11_t* texture )
	{
		if (!texture)
			return false;

		*texture = nullptr;

		D3D11_TEXTURE2D_DESC td = {};
		td.Width = desc.Width;
		td.Height = desc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = Format_Of( desc.Format );
		td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_DYNAMIC;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ |*/ D3D11_CPU_ACCESS_WRITE;
		td.MiscFlags = 0;

		HRESULT hr; 
		ID3D11Texture2D* instance = nullptr;
		D3D11_V(g->Device->CreateTexture2D( &td, nullptr, &instance ));
		*texture = (Texture_d3d11_t)instance;
		return SUCCEEDED(hr);
	}

	bool Gfx_d3d11_Texture_GetDesc( Gfx_d3d11_t g, Texture_d3d11_t texture, TextureDesc_s& desc )
	{
		desc = {};
		if (!texture)
			return false;

		D3D11_TEXTURE2D_DESC td = {};
		ID3D11Texture2D* instance = (ID3D11Texture2D*)texture;
		instance->GetDesc( &td );
		desc.Width = td.Width;
		desc.Height = td.Height;
		desc.Format = Format_Of( td.Format );
		return true;
	}

	bool Gfx_d3d11_Texture_Lock( Gfx_d3d11_t g, Texture_d3d11_t texture, Lock::Mode mode, LockDesc_s& lock )
	{
		lock = {};
		if (!texture)
			return false;

		HRESULT hr;
		ID3D11Texture2D* instance = (ID3D11Texture2D*)texture;
		if (mode == Lock::Write)
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(g->Context->Map( instance, 0, D3D11_MAP_WRITE_DISCARD, 0 /*D3D11_MAP_FLAG_DO_NOT_WAIT*/, &map ));
			lock.Data = map.pData;
			lock.Pitch = map.RowPitch;
			return SUCCEEDED(hr);
		}

		D3D11_TEXTURE2D_DESC td = {};
		instance->GetDesc( &td );

		td.Usage = D3D11_USAGE_STAGING;
		td.BindFlags = 0;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ID3D11Texture2D* copy;
		D3D11_V(g->Device->CreateTexture2D( &td, nullptr, &copy ));
		g->Context->CopyResource( copy, instance );

		D3D11_MAPPED_SUBRESOURCE map = {};
		D3D11_V(g->Context->Map( copy, 0, D3D11_MAP_READ, 0, &map ));
		lock.Data = map.pData;
		lock.Pitch = map.RowPitch;

		if (FAILED(hr))
		{
			copy->Release();
			return false;
		}

		g->Staging.Append( { instance, copy } );
		return true;
	}

	void Gfx_d3d11_Texture_Unlock( Gfx_d3d11_t g, Texture_d3d11_t texture )
	{
		if (!texture)
			return;

		ID3D11Texture2D* instance = (ID3D11Texture2D*)texture;
		for ( int i = 0; i < g->Staging.Count; ++i )
		{
			if (g->Staging[i].Src == instance)
			{
				g->Context->Unmap( g->Staging[i].Dst, 0 );
				g->Staging[i].Dst->Release();
				g->Staging.RemoveAt( i );
				return;
			}
		}

		g->Context->Unmap( instance, 0 );
	}

	void Gfx_d3d11_Texture_Release( Gfx_d3d11_t g, Texture_d3d11_t texture )
	{
		if (!texture)
			return;
		ID3D11Texture2D* instance = (ID3D11Texture2D*)texture;
		instance->Release();
	}

	bool Gfx_d3d11_SwapChain_Create( Gfx_d3d11_t g, const SwapChainDesc_d3d11_s& setup, SwapChain_d3d11_t* out )
	{
		if (!out)
			return false;

		HRESULT hr;
		CComPtr<IDXGISwapChain> swap_chain;
		CComPtr<IDXGISwapChain1> swap_chain_1;
		CComPtr<IDCompositionTarget> target;
		CComPtr<IDCompositionVisual> visual;
		if (setup.Composition)
		{
			if (g->Composition)
			{
				RECT rc;
				GetClientRect( (HWND)setup.Hdr.Wnd, &rc );

				DXGI_SWAP_CHAIN_DESC1 scd = {};
				scd.Width			 = setup.Hdr.Width ? setup.Hdr.Width : rc.right;
				scd.Height			 = setup.Hdr.Height ? setup.Hdr.Height : rc.bottom;
				scd.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;     
				scd.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				scd.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
				scd.BufferCount      = 2;                              
				scd.SampleDesc.Count = 1;                              
				scd.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;

				D3D11_V(g->Factory->CreateSwapChainForComposition( g->Device, &scd, nullptr, &swap_chain_1 ));
				D3D11_V(g->Composition->CreateTargetForHwnd( (HWND)setup.Hdr.Wnd, true, &target.p ));
				D3D11_V(g->Composition->CreateVisual( &visual.p ));
				D3D11_V(visual->SetContent( swap_chain_1 ));
				D3D11_V(target->SetRoot( visual ));
				D3D11_V(g->Composition->Commit());

				swap_chain = swap_chain_1;
			}
			else
			{
				hr = E_NOINTERFACE;
			}
		}
		else
		{
			// create the swap chain
			DXGI_SWAP_CHAIN_DESC scd = {};
			scd.BufferDesc.Width	= setup.Hdr.Width;
			scd.BufferDesc.Height	= setup.Hdr.Height;
			scd.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
			scd.SampleDesc.Count	= 1;
			scd.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scd.BufferCount			= 2;
			scd.OutputWindow		= (HWND)setup.Hdr.Wnd;
			scd.Windowed			= TRUE;
			scd.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			scd.Flags				= 0;

			D3D11_V(g->Factory->CreateSwapChain( g->Device, &scd, &swap_chain ));
		}
		if (FAILED(hr))
		{
			*out = nullptr;
			return false;
		}

		// create depth stencil
		CComPtr<ID3D11Texture2D> depth_stencil;
		if (!setup.Composition)
		{
			DXGI_SWAP_CHAIN_DESC scd = {};
			D3D11_V(swap_chain->GetDesc(&scd));
			D3D11_V(D3D11Device_CreateDepthStencilTexture( g->Device, scd.BufferDesc.Width, scd.BufferDesc.Height, &depth_stencil ));
			if (FAILED(hr))
			{
				*out = nullptr;
				return false;
			}
		}

		SwapChain_d3d11_s* wrapper = Mem_Calloc<SwapChain_d3d11_s>( g->Alloc );
		wrapper->Gfx		  = g;
		wrapper->SwapChain	  = swap_chain;
		wrapper->DepthStencil = depth_stencil;
		wrapper->WdmTarget	  = target;
		wrapper->WdmVisual	  = visual;
		*out = wrapper;
		return true;
	}

	bool Gfx_d3d11_SwapChain_Create( Gfx_d3d11_t g, const SwapChainDesc_s& setup, SwapChain_d3d11_t* out )
	{
		if (!out)
			return false;

		// create the swap chain
		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferDesc.Width = setup.Width;
		scd.BufferDesc.Height = setup.Height;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.SampleDesc.Count = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.OutputWindow = (HWND)setup.Wnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scd.Flags = 0;

		HRESULT hr;
		CComPtr<IDXGISwapChain> swap_chain;
		D3D11_V(g->Factory->CreateSwapChain( g->Device, &scd, &swap_chain ));
		if (FAILED(hr))
		{
			*out = nullptr;
			return false;
		}

		// query actual desc
		D3D11_V(swap_chain->GetDesc(&scd));

		// create depth stencil
		CComPtr<ID3D11Texture2D> depth_stencil;
		D3D11_V(D3D11Device_CreateDepthStencilTexture( g->Device, scd.BufferDesc.Width, scd.BufferDesc.Height, &depth_stencil ));
		if (FAILED(hr))
		{
			*out = nullptr;
			return false;
		}

		SwapChain_d3d11_s* wrapper = Mem_Calloc<SwapChain_d3d11_s>( g->Alloc );
		wrapper->Gfx = g;
		wrapper->SwapChain = swap_chain;
		wrapper->DepthStencil = depth_stencil;
		*out = wrapper;
		return true;
	}

	void Gfx_d3d11_SwapChain_Resize( SwapChain_d3d11_t swap, int w, int h )
	{
		Gfx_d3d11_ResizeSwapChain( swap->Gfx, swap, w, h );
	}

	void Gfx_d3d11_SwapChain_Begin( SwapChain_d3d11_t swap )
	{
		Gfx_d3d11_BindSwapChain( swap->Gfx, swap );
	}

	void Gfx_d3d11_SwapChain_End( SwapChain_d3d11_t swap )
	{
		Gfx_d3d11_BindSwapChain( swap->Gfx, &swap->Gfx->DeviceSwapChain );
	}

	void Gfx_d3d11_SwapChain_Present( SwapChain_d3d11_t swap )
	{
		HRESULT hr;
		D3D11_V(swap->SwapChain->Present( 0, 0 ));
	}

	void Gfx_d3d11_SwapChain_Present( SwapChain_d3d11_t swap, bool v_sync )
	{
		HRESULT hr;
		D3D11_V(swap->SwapChain->Present( v_sync ? 1 : 0, 0 ));
	}

	void Gfx_d3d11_SwapChain_Release( SwapChain_d3d11_t swap )
	{
		if (!swap)
			return;
		swap->~SwapChain_d3d11_s();
		Mem_Free( swap->Gfx->Alloc, swap );
	}

	void Gfx_d3d11_Draw_2D( Gfx_d3d11_t g, const DrawBatch_s* batch, int count )
	{
		NeMarker(g->Annotation, L"2D");
		Gfx_d3d11_UpdateMesh_2D( g, batch, count );
		Gfx_d3d11_ApplyState_2D( g );
		Gfx_d3d11_DrawMeshes_2D( g, batch, count );
	}

	void Gfx_d3d11_Draw_3D( Gfx_d3d11_t g, const LineBatch_s* batch, int count )
	{
		NeMarker(g->Annotation, L"3D Lines");
		Gfx_d3d11_UpdateMesh_3D( g, batch, count );
		Gfx_d3d11_ApplyState_3D_Lines( g );
		Gfx_d3d11_DrawMeshes_3D_Lines( g, batch, count );
	}

	void Gfx_d3d11_Draw_3D( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		NeMarker(g->Annotation, L"3D Meshes");
		Gfx_d3d11_UpdateMesh_3D( g, batch, count );
		Gfx_d3d11_ApplyState_3D( g );
		Gfx_d3d11_DrawMeshes_3D( g, batch, count );
	}

	void Gfx_d3d11_Draw_3D_Lit( Gfx_d3d11_t g, const MeshBatch_s* batch, int count )
	{
		Gfx_d3d11_Draw_3D_Lit( g, batch, count, Vec3_s { 1.0f, 1.0f, 1.0f } );
	}

	void Gfx_d3d11_Draw_3D_Lit( Gfx_d3d11_t g, const MeshBatch_s* batch, int count, const Vec3_s& tint )
	{
		NeMarker(g->Annotation, L"3D Meshes (Lit)");
		Gfx_d3d11_UpdatePhong( g, tint );
		Gfx_d3d11_UpdateMesh_3D_Phong( g, batch, count );
		Gfx_d3d11_ApplyState_3D_Phong( g );
		Gfx_d3d11_DrawMeshes_3D_Phong( g, batch, count );
	}

	void Gfx_d3d11_Draw_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s* batch, int count )
	{
		Gfx_d3d11_Draw_3D_Skinned( g, batch, count, Vec3_s { 1.0f, 1.0f, 1.0f } );
	}

	void Gfx_d3d11_Draw_3D_Skinned( Gfx_d3d11_t g, const SkinBatch_s* batch, int count, const Vec3_s& tint )
	{
		NeMarker(g->Annotation, L"3D Meshes (Lit, Skinned)");
		Gfx_d3d11_UpdatePhong( g, tint );
		Gfx_d3d11_ApplyState_3D_Skinned( g );
		for ( int i = 0; i < count; ++i )
			Gfx_d3d11_DrawBatch_3D_Skinned( g, batch[i] );
	}

} }
