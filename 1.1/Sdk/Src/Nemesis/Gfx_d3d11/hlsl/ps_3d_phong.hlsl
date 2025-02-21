//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "hlsl_phong.h"
#include "hlsl_post.h"

//======================================================================================
struct PS_INPUT
{
	float4 ClipPos  : SV_POSITION;
	float3 Normal   : NORMAL;
	float4 Color    : COLOR0;
	float2 Texture  : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 Color: SV_TARGET;
};

//======================================================================================
PS_OUTPUT main( PS_INPUT input )
{
    float3 normal = normalize(input.Normal);
    float3 view_dir = normalize(EyePos.xyz - input.WorldPos);

    float3 result = 0.0f;

	int i;
    for ( i = 0; i < NumDirLights; ++i )
        result += Phong_DirLight(Material, DirLight[i], normal, view_dir);

    for ( i = 0; i < NumPointLights; ++i )
        result += Phong_PointLight(Material, PointLight[i], normal, input.WorldPos, view_dir);

    for ( i = 0; i < NumSpotLights; ++i )
        result += Phong_SpotLight(Material, SpotLight[i], normal, input.WorldPos, view_dir);

    PS_OUTPUT output;
    output.Color = float4(Post_Default(result), Opacity);
	return output;
}