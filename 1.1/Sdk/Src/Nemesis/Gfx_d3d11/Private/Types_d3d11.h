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
#include <Nemesis/Gfx/Gfx.h>
#include <Nemesis/Core/Assert.h>

//======================================================================================
#include <dxgi1_3.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <atlbase.h>

//======================================================================================
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")

//======================================================================================
#if (NE_CONFIG < NE_CONFIG_MASTER)
#	define D3D11_V(expr) do { hr = (expr); NeAssert( SUCCEEDED(hr) ); } while(false)
#else
#	define D3D11_V(expr) do { hr = (expr); } while(false)
#endif

//======================================================================================
#define D3D11_VR(expr) do { D3D11_V(expr); if(FAILED(hr)) return hr; } while(false)

//======================================================================================
namespace nemesis { namespace graphics 
{
	struct DepthMode
	{
		enum Enum
		{ On
		, Off
		, COUNT
		};
	};

	struct CullMode
	{
		enum Enum
		{ Front
		, Back
		, None
		, COUNT
		};
	};

} }

//======================================================================================
namespace nemesis { namespace graphics 
{
	struct Primitive_d3d11_s
	{
		CComPtr<ID3D11Buffer>	IndexBuffer;
		CComPtr<ID3D11Buffer>	VertexBuffer;
		UINT					IndexBufferSize;
		UINT					VertexBufferSize;
		DXGI_FORMAT				IndexFormat;
		UINT					VertexStride;
	};

	struct BlendShape_d3d11_s
	{
		CComPtr<ID3D11Buffer> Offsets;
		CComPtr<ID3D11Buffer> Weights;
		CComPtr<ID3D11ShaderResourceView> OffsetsView;
		CComPtr<ID3D11ShaderResourceView> WeightsView;
	};

} }
