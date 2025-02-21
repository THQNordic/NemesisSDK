//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
float4x4 WorldViewProj;

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR0;
	float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos: SV_POSITION;
	float3 Nor: NORMAL;
	float4 Col: COLOR0;
	float2 Tex: TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;
	output.Pos = mul( WorldViewProj, float4(input.Pos, 1.0f) );
	output.Nor = input.Nor;
	output.Col = input.Col;
	output.Tex = input.Tex;
	return output;
}
