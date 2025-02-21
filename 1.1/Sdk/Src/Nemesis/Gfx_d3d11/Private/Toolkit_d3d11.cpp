//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include <stdafx.h>
#include "Toolkit_d3d11.h"

//======================================================================================
#include <Nemesis/Core/Memory.h>

//======================================================================================
//																			   RESOURCES
//======================================================================================
namespace nemesis { namespace graphics
{
	static HRESULT D3D11Device_CreateDepthStencilState( ID3D11Device* device, D3D11_DEPTH_WRITE_MASK write, D3D11_COMPARISON_FUNC cmp, ID3D11DepthStencilState** out )
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = write;
		desc.DepthFunc = cmp;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		return device->CreateDepthStencilState( &desc, out );
	}

} }

//======================================================================================
namespace nemesis { namespace graphics
{
	HRESULT D3D11Device_CreateBlendState( ID3D11Device* device, ID3D11BlendState** out )
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		return device->CreateBlendState( &desc, out );
	}

	HRESULT D3D11Device_CreateBlendState2( ID3D11Device* device, ID3D11BlendState** out )
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		return device->CreateBlendState( &desc, out );
	}

	HRESULT D3D11Device_CreateSamplerState( ID3D11Device* device, ID3D11SamplerState** out )
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0.f;
		desc.MaxLOD = 0.f;
		return device->CreateSamplerState( &desc, out );
	}

	HRESULT D3D11Device_CreateConstantBuffer( ID3D11Device* device, uint32_t size, ID3D11Buffer** out )
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		return device->CreateBuffer( &desc, nullptr, out );
	}

	HRESULT D3D11Device_CreateRasterizerState( ID3D11Device* device, D3D11_CULL_MODE cull, ID3D11RasterizerState** out )
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = cull;
		desc.FrontCounterClockwise = false;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		return device->CreateRasterizerState( &desc, out );
	}

	HRESULT D3D11Device_CreateDepthStencilState_On( ID3D11Device* device, ID3D11DepthStencilState** out )
	{
		return D3D11Device_CreateDepthStencilState( device, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL, out );
	}

	HRESULT D3D11Device_CreateDepthStencilState_Off( ID3D11Device* device, ID3D11DepthStencilState** out )
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		return device->CreateDepthStencilState( &desc, out );
	}

	HRESULT D3D11Device_CreateTexture2D_White( ID3D11Device* device, ID3D11Texture2D** out )
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = 4;
		desc.Height = 4;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		uint8_t white_texture_data[ 4 * 4 * sizeof(uint32_t) ];
		Mem_Set( white_texture_data, 0xff, NeCountOf(white_texture_data) );

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = white_texture_data;
		data.SysMemPitch = NeCountOf(white_texture_data) / desc.Height;
		data.SysMemSlicePitch = 0;

		return device->CreateTexture2D( &desc, &data, out );
	}

	HRESULT D3D11Device_CreateDepthStencilTexture( ID3D11Device* device, UINT w, UINT h, ID3D11Texture2D** out )
	{
		D3D11_TEXTURE2D_DESC dsd = {};
		dsd.Width = w;
		dsd.Height = h;
		dsd.MipLevels = 1;
		dsd.ArraySize = 1;
		dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsd.SampleDesc.Count = 1;
		dsd.Usage = D3D11_USAGE_DEFAULT;
		dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsd.CPUAccessFlags = 0;
		dsd.MiscFlags = 0;

		return device->CreateTexture2D( &dsd, nullptr, out );
	}
} }

//======================================================================================
//																			   PRIMITIVE
//======================================================================================
namespace nemesis { namespace graphics 
{
	HRESULT D3D11Device_ResizePrimitive_Idx( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t idx_size, int num_idx, Primitive_d3d11_s* mesh )
	{
		const uint32_t idx_buf_size = num_idx * idx_size;
		if (mesh->IndexBuffer && (mesh->IndexBufferSize >= idx_buf_size))
			return S_OK;
		if (!mesh->IndexBuffer && (!idx_buf_size))
			return S_OK;

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = num_idx * idx_size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr;
		CComPtr<ID3D11Buffer> index_buffer;
		D3D11_VR(device->CreateBuffer( &desc, nullptr, &index_buffer.p ));

		mesh->IndexBuffer = index_buffer;
		mesh->IndexBufferSize = idx_buf_size;

		return S_OK;
	}

	HRESULT D3D11Device_ResizePrimitive_Vtx( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh )
	{
		const uint32_t vtx_buf_size = num_vtx * vtx_size;
		if (mesh->VertexBuffer && (mesh->VertexBufferSize >= vtx_buf_size))
			return S_OK;
		if (!mesh->VertexBuffer && (!vtx_buf_size))
			return S_OK;

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = num_vtx * vtx_size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr;
		CComPtr<ID3D11Buffer> vertex_buffer;
		D3D11_VR(device->CreateBuffer( &desc, nullptr, &vertex_buffer.p ));

		mesh->VertexBuffer = vertex_buffer;
		mesh->VertexBufferSize = vtx_buf_size;

		return S_OK;
	}

	HRESULT D3D11Device_ResizePrimitive( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;
		D3D11_V (D3D11Device_ResizePrimitive_Vtx( device, dc, vtx_size, num_vtx, mesh ));
		mesh->VertexStride = vtx_size;
		return hr;
	}

	HRESULT D3D11Device_ResizePrimitive( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t idx_size, int num_idx, uint32_t vtx_size, int num_vtx, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;
		D3D11_VR(D3D11Device_ResizePrimitive_Idx( device, dc, idx_size, num_idx, mesh ));
		D3D11_V (D3D11Device_ResizePrimitive_Vtx( device, dc, vtx_size, num_vtx, mesh ));
		mesh->IndexFormat = (idx_size == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		mesh->VertexStride = vtx_size;
		return hr;
	}

	HRESULT D3D11Device_CreatePrimitive_2D( ID3D11Device* device, ID3D11DeviceContext* dc, const DrawBatch_s* batch, int count, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;

		// Calculate total number of indices/vertices
		uint32_t num_idx = 0;
		uint32_t num_vtx = 0;
		for ( int i = 0; i < count; ++i )
		{
			num_idx += batch[i].NumIdx;
			num_vtx += batch[i].NumVtx;
		}

		// Calculate single index/vertex size
		const uint32_t idx_size = sizeof(batch[0].Idx[0]);
		const uint32_t vtx_size = sizeof(batch[0].Vtx[0]);

		// Resize the primtive's buffers
		D3D11_VR(D3D11Device_ResizePrimitive( device, dc, idx_size, num_idx, vtx_size, num_vtx, mesh ));

		// Copy indices to index buffer
		if (mesh->IndexBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			for ( int i = 0; i < count; ++i )
			{
				const uint32_t copy_size = batch[i].NumIdx * idx_size;
				Mem_Cpy( dst, batch[i].Idx, copy_size );
				dst += copy_size;
			}
			dc->Unmap( mesh->IndexBuffer, 0 );
		}

		// Copy vertices to vertex buffer
		if (mesh->VertexBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			for ( int i = 0; i < count; ++i )
			{
				const uint32_t copy_size = batch[i].NumVtx * vtx_size;
				Mem_Cpy( dst, batch[i].Vtx, copy_size );
				dst += copy_size;
			}
			dc->Unmap( mesh->VertexBuffer, 0 );
		}
		return S_OK;
	}
	
	HRESULT D3D11Device_CreatePrimitive_3D( ID3D11Device* device, ID3D11DeviceContext* dc, const LineBatch_s* batch, int count, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;

		// Calculate total number of vertices
		uint32_t num_vtx = 0;
		for ( int i = 0; i < count; ++i )
			num_vtx += batch[i].NumVtx;

		// Calculate single vertex size
		const uint32_t vtx_size = sizeof(batch[0].Vtx[0]);

		// Resize the primtive's vertex buffer
		D3D11_VR(D3D11Device_ResizePrimitive( device, dc, vtx_size, num_vtx, mesh ));

		// Copy vertices to vertex buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			for ( int i = 0; i < count; ++i )
			{
				const uint32_t copy_size = batch[i].NumVtx * vtx_size;
				Mem_Cpy( dst, batch[i].Vtx, copy_size );
				dst += copy_size;
			}
			dc->Unmap( mesh->VertexBuffer, 0 );
		}
		return S_OK;
	}

	HRESULT D3D11Device_CreatePrimitive_3D( ID3D11Device* device, ID3D11DeviceContext* dc, const MeshBatch_s* batch, int count, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;

		// Calculate total number of indices/vertices
		uint32_t num_idx = 0;
		uint32_t num_vtx = 0;
		for ( int i = 0; i < count; ++i )
		{
			for ( int j = 0; j < batch[i].NumCmd; ++j )
			{
				num_idx += batch[i].Cmd[j].Mesh->NumIdx;
				num_vtx += batch[i].Cmd[j].Mesh->NumVtx;
			}
		}

		// Calculate single index/vertex size
		const uint32_t idx_size = sizeof(batch[0].Cmd[0].Mesh->Idx[0]);
		const uint32_t vtx_size = sizeof(batch[0].Cmd[0].Mesh->Vtx[0]);

		// Resize the primtive's buffers
		D3D11_VR(D3D11Device_ResizePrimitive( device, dc, idx_size, num_idx, vtx_size, num_vtx, mesh ));

		// Copy indices to index buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			for ( int i = 0; i < count; ++i )
			{
				const MeshBatch_s& this_batch = batch[i];
				const int num_cmd = this_batch.NumCmd;
				for ( int j = 0; j < num_cmd; ++j )
				{
					const MeshCmd_s& cmd = this_batch.Cmd[j];
					const uint32_t copy_size = cmd.Mesh->NumIdx * idx_size;
					Mem_Cpy( dst, cmd.Mesh->Idx, copy_size );
					dst += copy_size;
				}
			}
			dc->Unmap( mesh->IndexBuffer, 0 );
		}

		// Copy vertices to vertex buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			for ( int i = 0; i < count; ++i )
			{
				for ( int j = 0; j < batch[i].NumCmd; ++j )
				{
					const uint32_t copy_size = batch[i].Cmd[j].Mesh->NumVtx * vtx_size;
					Mem_Cpy( dst, batch[i].Cmd[j].Mesh->Vtx, copy_size );
					dst += copy_size;
				}
			}
			dc->Unmap( mesh->VertexBuffer, 0 );
		}
		return S_OK;
	}

	HRESULT D3D11Device_CreatePrimitive_3D_Skinned( ID3D11Device* device, ID3D11DeviceContext* dc, const SkinnedMesh_s* skin, Primitive_d3d11_s* mesh )
	{
		HRESULT hr;

		// Calculate single index/vertex count and size
		const uint32_t num_idx = skin->NumIdx;
		const uint32_t num_vtx = skin->NumVtx;
		const uint32_t idx_size = sizeof(skin->Idx[0]);
		const uint32_t vtx_size = sizeof(skin->Vtx[0]);

		// Resize the primtive's buffers
		D3D11_VR(D3D11Device_ResizePrimitive( device, dc, idx_size, num_idx, vtx_size, num_vtx, mesh ));

		// Copy indices to index buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			{
				const uint32_t copy_size = skin->NumIdx * idx_size;
				Mem_Cpy( dst, skin->Idx, copy_size );
				dst += copy_size;
			}
			dc->Unmap( mesh->IndexBuffer, 0 );
		}

		// Copy vertices to vertex buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( mesh->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			{
				const uint32_t copy_size = skin->NumVtx * vtx_size;
				Mem_Cpy( dst, skin->Vtx, copy_size );
				dst += copy_size;
			}
			dc->Unmap( mesh->VertexBuffer, 0 );
		}
		return S_OK;
	}

	void D3D11DeviceContext_BindPrimitive( ID3D11DeviceContext* dc, Primitive_d3d11_s* mesh )
	{
		UINT vertex_offset = 0;
		dc->IASetIndexBuffer( mesh->IndexBuffer, mesh->IndexFormat, 0 );
		dc->IASetVertexBuffers( 0, 1, &mesh->VertexBuffer.p, &mesh->VertexStride, &vertex_offset );
	}

} }

//======================================================================================
//																			 BLEND SHAPE
//======================================================================================
namespace nemesis { namespace graphics 
{
	static uint32_t D3D11Buffer_GetByteSize( ID3D11Buffer* buffer )
	{
		if (!buffer)
			return 0;
		D3D11_BUFFER_DESC desc = {};
		buffer->GetDesc( &desc );
		return desc.ByteWidth;
	}

	HRESULT D3D11Device_ResizeBlendShape_Offsets( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_offsets, BlendShape_d3d11_s* shape )
	{
		const uint32_t req_size = num_offsets * sizeof(BlendVtx_s);
		const uint32_t cur_size = D3D11Buffer_GetByteSize( shape->Offsets );
		if (cur_size >= req_size)
			return S_OK;

		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = req_size;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffer_desc.StructureByteStride = sizeof(BlendVtx_s);

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = DXGI_FORMAT_UNKNOWN;
		view_desc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		view_desc.Buffer.FirstElement = 0;
		view_desc.Buffer.ElementOffset = 0;
		view_desc.Buffer.NumElements = num_offsets;

		HRESULT hr;
		CComPtr<ID3D11Buffer> buffer = nullptr;
		CComPtr<ID3D11ShaderResourceView> view = nullptr;
		D3D11_VR(device->CreateBuffer( &buffer_desc, nullptr, &buffer ));
		D3D11_VR(device->CreateShaderResourceView( buffer, &view_desc, &view ));

		shape->Offsets = buffer;
		shape->OffsetsView = view;
		buffer.Detach();
		view.Detach();

		return S_OK;
	}

	HRESULT D3D11Device_ResizeBlendShape_Weights( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_weights, BlendShape_d3d11_s* shape )
	{
		const uint32_t req_size = num_weights * sizeof(float);
		const uint32_t cur_size = D3D11Buffer_GetByteSize( shape->Weights );
		if (cur_size >= req_size)
			return S_OK;

		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = req_size;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffer_desc.StructureByteStride = sizeof(float);

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = DXGI_FORMAT_UNKNOWN;
		view_desc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		view_desc.Buffer.FirstElement = 0;
		view_desc.Buffer.ElementOffset = 0;
		view_desc.Buffer.NumElements = num_weights;

		HRESULT hr;
		CComPtr<ID3D11Buffer> buffer = nullptr;
		CComPtr<ID3D11ShaderResourceView> view = nullptr;
		D3D11_VR(device->CreateBuffer( &buffer_desc, nullptr, &buffer ));
		D3D11_VR(device->CreateShaderResourceView( buffer, &view_desc, &view ));

		shape->Weights = buffer;
		shape->WeightsView = view;
		buffer.Detach();
		view.Detach();

		return S_OK;
	}

	HRESULT D3D11Device_ResizeBlendShape( ID3D11Device* device, ID3D11DeviceContext* dc, uint32_t num_offsets, uint32_t num_weights, BlendShape_d3d11_s* shape )
	{
		HRESULT hr;
		D3D11_VR(D3D11Device_ResizeBlendShape_Offsets( device, dc, num_offsets, shape ));
		D3D11_VR(D3D11Device_ResizeBlendShape_Weights( device, dc, num_weights, shape ));
		return S_OK;
	}

	HRESULT D3D11Device_CreateBlendShape_3D_Skinned( ID3D11Device* device, ID3D11DeviceContext* dc, const SkinnedMesh_s* skin, BlendShape_d3d11_s* shape )
	{
		// Calculate total number of offsets/weights
		const uint32_t num_offsets = skin->NumOffsets;
		const uint32_t num_weights = skin->NumShapes;

		// Resize blend shape
		HRESULT hr;
		D3D11_VR(D3D11Device_ResizeBlendShape( device, dc, num_offsets, num_weights, shape ));

		// Copy offsets to offset buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( shape->Offsets, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			{
				const uint32_t copy_size = skin->NumOffsets * sizeof(BlendVtx_s);
				Mem_Cpy( dst, skin->Offsets, copy_size );
				dst += copy_size;
			}
			dc->Unmap( shape->Offsets, 0 );
		}

		return S_OK;
	}

	void D3D11DeviceContext_UpdateBlendShape( ID3D11DeviceContext* dc, BlendShape_d3d11_s* shape, const SkinnedMesh_s* skin, const float* flt, int num_flt )
	{
		HRESULT hr;

		// Copy weights to weights buffer
		{
			D3D11_MAPPED_SUBRESOURCE map = {};
			D3D11_V(dc->Map( shape->Weights, 0, D3D11_MAP_WRITE_DISCARD, 0, &map ));
			uint8_t* dst = (uint8_t*)map.pData;
			{
				float* p = (float*)dst;
				for ( int32_t i = 0; i < skin->NumShapes; ++i )
				{
					const int rig_float = skin->SkinToRigShape[i];
					if ((rig_float >= 0) && (rig_float < num_flt))
					{
						p[i] = flt[rig_float];
					}
					else
					{
						p[i] = 0.0f;
					}
				}
			}
			dc->Unmap( shape->Weights, 0 );
		}
	}

	void D3D11DeviceContext_BindBlendShape( ID3D11DeviceContext* dc, BlendShape_d3d11_s* shape, int offset_slot, int weight_slot )
	{
		if (shape)
		{
			dc->VSSetShaderResources( offset_slot, 1, &shape->OffsetsView.p );
			dc->VSSetShaderResources( weight_slot, 1, &shape->WeightsView.p );
		}
		else
		{
			ID3D11ShaderResourceView* view = nullptr;
			dc->VSSetShaderResources( offset_slot, 1, &view );
			dc->VSSetShaderResources( weight_slot, 1, &view );
		}
	}

} } 

//======================================================================================
//																			RENDER STATE
//======================================================================================
namespace nemesis { namespace graphics 
{
	void D3D11DeviceContext_SetRects( ID3D11DeviceContext* dc, const Rect_s& view, const Rect_s& scissor )
	{
		D3D11_VIEWPORT vp = {};
		vp.Width = view.w;
		vp.Height = view.h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = view.x;
		vp.TopLeftY = view.y;
		dc->RSSetViewports( 1, &vp );

		const RECT scissor_rect = 
		{ (LONG) scissor.x
		, (LONG) scissor.y
		, (LONG)(scissor.x + scissor.w + 0.5f)
		, (LONG)(scissor.y + scissor.h + 0.5f) 
		};
		dc->RSSetScissorRects( 1, &scissor_rect );
	}

	void D3D11DeviceContext_SetRects( ID3D11DeviceContext* dc, const DrawCmd_s& cmd, const DXGI_SWAP_CHAIN_DESC& scd )
	{
		D3D11_VIEWPORT vp = {};
		vp.Width = (FLOAT)scd.BufferDesc.Width;
		vp.Height = (FLOAT)scd.BufferDesc.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		dc->RSSetViewports( 1, &vp );

		const RECT scissor_rect = NeHasFlag( cmd.Flags, gui::DrawCmd::Clip ) 
			? RECT { (LONG) cmd.Clip.x, (LONG) cmd.Clip.y, (LONG) (cmd.Clip.x + cmd.Clip.w /*+ 0.5f*/), (LONG) (cmd.Clip.y + cmd.Clip.h /*+ 0.5f*/) }
			: RECT {		0		  ,		   0		 , (LONG) scd.BufferDesc.Width			  , (LONG) scd.BufferDesc.Height			}
			;
		dc->RSSetScissorRects( 1, &scissor_rect );
	}

	HRESULT D3D11Device_SetTexture( ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11Texture2D* texture )
	{
		HRESULT hr;
		CComPtr<ID3D11ShaderResourceView> srv;
		D3D11_VR(device->CreateShaderResourceView( texture, nullptr, &srv ));
		dc->PSSetShaderResources( 0, 1, &srv.p );
		return S_OK;
	}

} }
