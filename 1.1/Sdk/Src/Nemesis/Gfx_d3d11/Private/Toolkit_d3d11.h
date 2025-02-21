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
#include "Types_d3d11.h"

//======================================================================================
namespace nemesis { namespace graphics
{
	HRESULT D3D11Device_CreateBlendState( ID3D11Device* device, ID3D11BlendState** out );
	HRESULT D3D11Device_CreateBlendState2( ID3D11Device* device, ID3D11BlendState** out );
	HRESULT D3D11Device_CreateSamplerState( ID3D11Device* device, ID3D11SamplerState** out );
	HRESULT D3D11Device_CreateConstantBuffer( ID3D11Device* device, uint32_t size, ID3D11Buffer** out );
	HRESULT D3D11Device_CreateRasterizerState( ID3D11Device* device, D3D11_CULL_MODE cull, ID3D11RasterizerState** out );
	HRESULT D3D11Device_CreateDepthStencilState_On( ID3D11Device* device, ID3D11DepthStencilState** out );
	HRESULT D3D11Device_CreateDepthStencilState_Off( ID3D11Device* device, ID3D11DepthStencilState** out );
	HRESULT D3D11Device_CreateTexture2D_White( ID3D11Device* device, ID3D11Texture2D** out );
	HRESULT D3D11Device_CreateDepthStencilTexture( ID3D11Device* device, UINT w, UINT h, ID3D11Texture2D** out );

} }

//======================================================================================
namespace nemesis { namespace graphics
{
	HRESULT D3D11Device_ResizePrimitive_Idx( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t idx_size, int num_idx, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_ResizePrimitive_Vtx( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_ResizePrimitive( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_ResizePrimitive( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t idx_size, int num_idx, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_CreatePrimitive_2D( ID3D11Device* device, ID3D11DeviceContext* dc, const DrawBatch_s* batch, int count, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_CreatePrimitive_3D( ID3D11Device* device, ID3D11DeviceContext* dc, const LineBatch_s* batch, int count, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_CreatePrimitive_3D( ID3D11Device* device, ID3D11DeviceContext* dc, const MeshBatch_s* batch, int count, Primitive_d3d11_s* mesh );
	HRESULT D3D11Device_CreatePrimitive_3D_Skinned( ID3D11Device* device, ID3D11DeviceContext* dc, const SkinnedMesh_s* skin, Primitive_d3d11_s* mesh );
	void D3D11DeviceContext_BindPrimitive( ID3D11DeviceContext* dc, Primitive_d3d11_s* mesh );

} }

//======================================================================================
namespace nemesis { namespace graphics
{
	HRESULT D3D11Device_ResizeBlendShape_Offsets( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_offsets, BlendShape_d3d11_s* shape );
	HRESULT D3D11Device_ResizeBlendShape_Weights( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_weights, BlendShape_d3d11_s* shape );
	HRESULT D3D11Device_ResizeBlendShape( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_offsets, uint32_t num_weights, BlendShape_d3d11_s* shape );
	HRESULT D3D11Device_CreateBlendShape_3D_Skinned( ID3D11Device* device, ID3D11DeviceContext* dc, const SkinnedMesh_s* skin, BlendShape_d3d11_s* shape );
	void D3D11DeviceContext_UpdateBlendShape( ID3D11DeviceContext* dc, BlendShape_d3d11_s* shape, const SkinnedMesh_s* skin, const float* flt, int num_flt );
	void D3D11DeviceContext_BindBlendShape( ID3D11DeviceContext* dc, BlendShape_d3d11_s* shape, int offset_slot, int weight_slot );

} }

//======================================================================================
namespace nemesis { namespace graphics
{
	void D3D11DeviceContext_SetRects( ID3D11DeviceContext* dc, const Rect_s& view, const Rect_s& scissor );
	void D3D11DeviceContext_SetRects( ID3D11DeviceContext* dc, const DrawCmd_s& cmd, const DXGI_SWAP_CHAIN_DESC& scd );
	HRESULT D3D11Device_SetTexture( ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11Texture2D* texture );

} }

