//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#if defined __cplusplus
    #define cbuffer struct
#else
    typedef float2 Vec2_s;
    typedef float3 Vec3_s;
    typedef float4 Vec4_s;
#endif

//======================================================================================
struct DirLight_s
{
    Vec3_s Direction;
    float _pad_0_;
    Vec3_s Ambient;
    float _pad_1_;
    Vec3_s Diffuse;
    float _pad_2_;
    Vec3_s Specular;
    float _pad_3_;
};

struct PointLight_s
{
    Vec3_s Position;
    float Constant;
    Vec3_s Ambient;
    float Linear;
    Vec3_s Diffuse;
    float Quadratic;
    Vec3_s Specular;
    float _pad_0_;
};

struct SpotLight_s
{
    Vec3_s Position;
    float CutOff;
    Vec3_s Direction;
    float OuterCutOff;
    Vec3_s Ambient;
    float Constant;
    Vec3_s Diffuse;
    float Linear;
    Vec3_s Specular;
    float Quadratic;
};

struct PhongMaterial_s
{
    Vec3_s Diffuse;
    float _pad_0_;
    Vec3_s Specular;
    float Shininess;
};

cbuffer Ps_Phong_s
{
    Vec3_s          EyePos;
    float           Opacity;
    PhongMaterial_s Material;
    DirLight_s      DirLight[2];
    SpotLight_s     SpotLight[2];
    PointLight_s    PointLight[8];
    int             NumDirLights;
    int             NumSpotLights;
    int             NumPointLights;
    int             _pad_0_;
};

//======================================================================================
//                                                                                 Phong
//======================================================================================
#ifndef __cplusplus
float3 Phong_DirLight( PhongMaterial_s material, DirLight_s light, float3 normal, float3 view_dir )
{
    float3 light_dir = normalize(-light.Direction);

    // diffuse
    float diff = max(dot(normal, light_dir), 0.0f);

    // specular
    float3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.Shininess);

    // combine
    float3 ambient = light.Ambient * material.Diffuse;
    float3 diffuse = light.Diffuse * diff * material.Diffuse;
    float3 specular = light.Specular * spec * material.Specular;
    return ambient + diffuse + specular;
}

float3 Phong_PointLight( PhongMaterial_s material, PointLight_s light, float3 normal, float3 world_pos, float3 view_dir )
{
    float3 light_dir = normalize(light.Position - world_pos);

    // diffuse
    float diff = max(dot(normal, light_dir), 0.0f);

    // specular
    float3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.Shininess);

    // attenuation
    float distance = length(light.Position - world_pos);
    float attenuation = 1.0f / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    // combine
    float3 ambient = light.Ambient * material.Diffuse;
    float3 diffuse = light.Diffuse * diff * material.Diffuse;
    float3 specular = light.Specular * spec * material.Specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

float3 Phong_SpotLight( PhongMaterial_s material, SpotLight_s light, float3 normal, float3 world_pos, float3 view_dir )
{
    float3 light_dir = normalize(light.Position - world_pos);

    // diffuse
    float diff = max(dot(normal, light_dir), 0.0f);

    // specular
    float3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.Shininess);

    // attenuation
    float distance = length(light.Position - world_pos);
    float attenuation = 1.0f / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(light_dir, normalize(-light.Direction));
    float epsilon = light.CutOff - light.OuterCutOff;
    float intensity = saturate((theta - light.OuterCutOff) / epsilon);

    // combine
    float3 ambient = light.Ambient * material.Diffuse;
    float3 diffuse = light.Diffuse * diff * material.Diffuse;
    float3 specular = light.Specular * spec * material.Specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ambient + diffuse + specular;
}
#endif
