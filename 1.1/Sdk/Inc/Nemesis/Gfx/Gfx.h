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
#include <Nemesis/Core/Types.h>
#include <Nemesis/Gui/Types.h>

//======================================================================================
namespace nemesis { namespace graphics {} };
namespace nemesis {	namespace gfx = graphics; };

//======================================================================================
namespace nemesis { namespace graphics 
{
	struct Format
	{
		enum Enum
		{ Unknown
		, R8_UNorm
		, R8_UInt
		, R8G8B8A8_UNorm
		, R8G8B8A8_UInt
		, B8G8R8A8_Unorm
		, R32G32_Float
		, R32G32_UInt
		, R32G32B32_Float
		, R32G32B32_UInt
		, R32G32B32A32_Float
		, R32G32B32A32_UInt
		};
	};

	struct Lock
	{
		enum Mode
		{ Read
		, Write
		};
	};

	struct LockDesc_s
	{
		void*	 Data;
		uint32_t Pitch;
	};

	struct TextureDesc_s
	{
		Format::Enum Format;
		uint32_t	 Width;
		uint32_t	 Height;
	};

	struct SwapChainDesc_s
	{
		ptr_t	Wnd;
		int32_t Width;
		int32_t Height;
	};

} }

//======================================================================================
namespace nemesis { namespace graphics 
{
	using gui::DrawIdx_t;
	using gui::DrawVtx_s;
	using gui::DrawCmd_s;
	using gui::DrawBatch_s;

	struct LineVtx_s
	{
		Vec3_s	 Position;
		uint32_t Color;
	};

	struct LineBatch_s
	{
		Rect_s Clip;
		Rect_s Rect;
		Mat4_s Proj;
		Mat4_s View;
		Mat4_s World;
		const LineVtx_s* Vtx;
		int NumVtx;
	};

	struct MeshVtx_s
	{
		Vec3_s	 Position;
		Vec3_s	 Normal;
		uint32_t Color;
		Vec2_s	 TexCoord;
	};

	struct Mesh_s
	{
		const MeshVtx_s* Vtx;
		const uint32_t*  Idx;
		int NumVtx;
		int NumIdx;
	};

	struct MeshCmd_s
	{
		const Mesh_s* Mesh;
		const Mat4_s* Wsm;
		int	NumWsm;
	};

	struct MeshBatch_s
	{
		Rect_s Clip;
		Rect_s Rect;
		Mat4_s Proj;
		Mat4_s View;
		const MeshCmd_s* Cmd;
		int NumCmd;
	};

	struct SkinVtx_s
	{
		Vec3_s	 Pos;
		float	 Weight[4];
		uint16_t BoneId[4];
		Vec3_s	 Normal;
		uint32_t Color;
		Vec2_s	 Texture;
		uint32_t BlendIdx;
		uint32_t BlendCnt;
	};

	struct BlendVtx_s
	{
		Vec3_s	 PosOffset;
		uint32_t ShapeIdx;
	};

	struct SkinnedMesh_s
	{
		int NumIdx;
		int NumVtx;
		int NumBones;
		const uint32_t*	  Idx;
		const SkinVtx_s*  Vtx;
		const int32_t*	  SkinToRigBone;
		const Mat4_s*	  BindPose;

		int NumShapes;
		int NumOffsets;
		const BlendVtx_s* Offsets;
		const int32_t*	  SkinToRigShape;
	};

	struct SkinCmd_s
	{
		const SkinnedMesh_s* Mesh;
		const Mat4_s*		 Wsm;
		const float*		 Flt;
		int					 NumWsm;
		int					 NumFlt;
	};

	struct SkinBatch_s
	{
		Rect_s Clip;
		Rect_s Rect;
		Mat4_s Proj;
		Mat4_s View;
		const SkinCmd_s* Cmd;
		int NumCmd;
	};
} }