//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
float4x4 ViewProj	: register(c0);
float4x4 World[255];

struct BlendVtx_s
{
	float3 PosOffset;
	uint   ShapeIdx;
};

StructuredBuffer<BlendVtx_s> BlendVtx: register(t0);
StructuredBuffer<float>      BlendFlt: register(t1);

struct VS_INPUT
{
	float3 Pos		: POSITION;
	float4 Weight	: BLENDWEIGHTS;
	uint4  Index	: BLENDINDICES;
	float3 Nor		: NORMAL;
	float4 Col		: COLOR0;
	float2 Tex		: TEXCOORD0;
	uint2  Blend	: BLENDINDICES1;
};

struct VS_OUTPUT
{
	float4 Pos      : SV_POSITION;
	float3 Nor      : NORMAL;
	float4 Col      : COLOR0;
	float2 Tex      : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input )
{
	uint idx0 = input.Index[0];
	uint idx1 = input.Index[1];
	uint idx2 = input.Index[2];
	uint idx3 = input.Index[3];

	float4x4 wsm0 = World[idx0];
	float4x4 wsm1 = World[idx1];
	float4x4 wsm2 = World[idx2];
	float4x4 wsm3 = World[idx3];

    float3 n = normalize(input.Nor);

    float3 vNor3 = 
        mul( (float3x3)wsm0, n ) * input.Weight.x +
        mul( (float3x3)wsm1, n ) * input.Weight.y +
        mul( (float3x3)wsm2, n ) * input.Weight.z +
        mul( (float3x3)wsm3, n ) * input.Weight.w;

	float3 vPos3 = 
        mul( wsm0, float4(input.Pos, 1.0f) ) * input.Weight.x +
        mul( wsm1, float4(input.Pos, 1.0f) ) * input.Weight.y +
        mul( wsm2, float4(input.Pos, 1.0f) ) * input.Weight.z +
        mul( wsm3, float4(input.Pos, 1.0f) ) * input.Weight.w;

	uint idx = input.Blend[0];
	uint cnt = input.Blend[1];
	for ( uint i = 0; i < cnt; ++i )
	{
		uint vtx_idx   = idx + i;
		uint shape_idx = BlendVtx[vtx_idx].ShapeIdx;
		vPos3 += BlendVtx[vtx_idx].PosOffset * BlendFlt[shape_idx];
	}

	float4 vPosition = float4(vPos3,1.0f);

	VS_OUTPUT output;
	output.Pos      = mul( ViewProj, vPosition );
    output.Nor      = normalize( vNor3 );
	output.Col      = input.Col;
	output.Tex      = input.Tex;
    output.WorldPos = vPosition.xyz;
	return output;
}
