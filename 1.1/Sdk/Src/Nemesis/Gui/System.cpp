//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include <Nemesis/Gui/System.h>
#include <Nemesis/Core/Assert.h>
#include <Nemesis/Core/Math.h>
#include <Nemesis/Core/New.h>
#include <Nemesis/Core/Process.h>
#include "Font.h"
#include "Theme.h"
#include <math.h>

//======================================================================================
using namespace nemesis::system;

//======================================================================================
namespace nemesis { namespace gui
{
	/// Id

	namespace
	{
		/// djb2 http://www.cse.yorku.ca/~oz/hash.html

		static uint32_t Hash32( uint32_t seed, cstr_t key, size_t len )
		{
			uint32_t hash = seed ? seed : 5381;
			for ( size_t i = 0; i < len ; ++i )
				hash = ((hash << 5) + hash) + *key++;
			return hash;
		}

		static uint32_t Hash32( uint32_t seed, cstr_t key )
		{
			uint32_t hash = seed ? seed : 5381;
			uint8_t c;
			for ( ;; )
			{
				c = *key++;
				if (!c)
					break;
				hash = ((hash << 5) + hash) + c;
			}
			return hash;
		}
	}

	/// Identifier

	Id_t Id_Gen( cstr_t name )
	{ return name ? Hash32( 0, name ) : 0; }

	Id_t Id_Cat( Id_t id, int child )
	{
		// @note: this ensures that the first child (index 0)
		//		  is distinguishable from the parent
		++child;
		return Hash32( id, (cstr_t)&child, sizeof(child) ); 
	}

	Id_t Id_Cat( Id_t id, cstr_t child )
	{ return Hash32( id, child ); }

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// ScratchBuffer

	Text_s ScratchBuffer_FormatV( ScratchBuffer_t buffer, cstr_t text, va_list args )
	{
		if (!buffer || !text || !text[0])
			return Text_s( text );
		return Text_s( (cstr_t)buffer->Data, Str_FmtV( (char*)buffer->Data, sizeof(buffer->Data), text, args ) );
	}

	Text_s ScratchBuffer_FormatV( ScratchBuffer_t buffer, wcstr_t text, va_list args )
	{
		if (!buffer || !text || !text[0])
			return Text_s( text );
		return Text_s( buffer->Data, Wcs_Fmt( buffer->Data, text, args ) );
	}

	Text_s ScratchBuffer_Format( ScratchBuffer_t buffer, cstr_t text, ... )
	{
		if (!buffer || !text || !text[0])
			return Text_s( text );
		Text_s out;
		va_list args;
		va_start( args, text );
		out = ScratchBuffer_FormatV( buffer, text, args );
		va_end( args );
		return out;
	}

	Text_s ScratchBuffer_Format( ScratchBuffer_t buffer, wcstr_t text, ... )
	{
		if (!buffer || !text || !text[0])
			return Text_s( text );
		Text_s out;
		va_list args;
		va_start( args, text );
		out = ScratchBuffer_FormatV( buffer, text, args );
		va_end( args );
		return out;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// DrawPath

	struct DrawPath_s
	{
		Array<Vec2_s> Pos;
	};

	static void DrawPath_Initialize( DrawPath_s& path, Allocator_t alloc )
	{
		path.Pos.Init(alloc);
	}

	static void DrawPath_Shutdown( DrawPath_s& path )
	{
		path.Pos.Clear();
	}

	void DrawPath_Reset( DrawPath_s& path )
	{
		path.Pos.Reset();
	}

	void DrawPath_ArcTo( DrawPath_s& path, const Vec2_s& center, float radius, float min_angle, float max_angle, int num_seg )
	{
		const float range = max_angle - min_angle;
		float angle = min_angle;
		for ( int i = 0; i <= num_seg; ++i )
		{
			angle = min_angle + range * (float)i / float(num_seg);

			Vec2_s& pos = path.Pos.Append();
			pos.x = center.x + cosf( angle ) * radius;
			pos.y = center.y + sinf( angle ) * radius;
		}
	}

	bool DrawPath_DrawCircle( DrawPath_s& path, const Vec2_s& center, float radius, int num_seg )
	{
		if (radius == 0.0f)
			return false;
		if (num_seg < 3)
			return false;

		const float circle	  = 2.0f * NE_PI;
		const float step	  = circle / (float)num_seg;
		const float min_angle = 0.0f   + 0.5f * step;
		const float max_angle = circle + 0.5f * step;
		DrawPath_ArcTo( path, center, radius, min_angle, max_angle, num_seg );
		return true;
	}

	void DrawPath_LineTo( DrawPath_s& path, const Vec2_s& to )
	{
		path.Pos.Append( to );
	}

	void DrawPath_DrawTriangle( DrawPath_s& path, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c )
	{
		DrawPath_LineTo( path, a );
		DrawPath_LineTo( path, b );
		DrawPath_LineTo( path, c );
		DrawPath_LineTo( path, a );
	}

	void DrawPath_BezierCurveTo
		( DrawPath_s& path
		, float x1, float y1
		, float x2, float y2
		, float x3, float y3
		, float x4, float y4
		, float tolerance
		, int level 
		)
	{
		float dx = x4 - x1;
		float dy = y4 - y1;
		float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
		float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
		d2 = (d2 >= 0) ? d2 : -d2;
		d3 = (d3 >= 0) ? d3 : -d3;
		if ((d2+d3) * (d2+d3) < tolerance * (dx*dx + dy*dy))
		{
			DrawPath_LineTo( path, Vec2_s { x4, y4 } );
		}
		else if (level < 10)
		{
			float x12 = (x1+x2)*0.5f,       y12 = (y1+y2)*0.5f;
			float x23 = (x2+x3)*0.5f,       y23 = (y2+y3)*0.5f;
			float x34 = (x3+x4)*0.5f,       y34 = (y3+y4)*0.5f;
			float x123 = (x12+x23)*0.5f,    y123 = (y12+y23)*0.5f;
			float x234 = (x23+x34)*0.5f,    y234 = (y23+y34)*0.5f;
			float x1234 = (x123+x234)*0.5f, y1234 = (y123+y234)*0.5f;

			DrawPath_BezierCurveTo(path, x1,y1,        x12,y12,    x123,y123,  x1234,y1234, tolerance, level+1 );
			DrawPath_BezierCurveTo(path, x1234,y1234,  x234,y234,  x34,y34,    x4,y4,       tolerance, level+1 );
		}
	}

	void DrawPath_BezierCurveTo( DrawPath_s& path, const Vec2_s& p1, const Vec2_s& p2, const Vec2_s& p3, int num_segments )
	{
		const Vec2_s p0 = path.Pos[0];
		if (!num_segments)
			return DrawPath_BezierCurveTo( path, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 1.25f, 0 );

		const float step = 1.0f / num_segments;
		for ( int i = 0; i <= num_segments; ++i )
		{
			const float t = step * i;
			const float u = 1.0f - t;
			const float w0 = u*u*u;
			const float w1 = 3*u*u*t;
			const float w2 = 3*u*t*t;
			const float w3 = t*t*t;
			const Vec2_s pos = 
			{ w0*p0.x + w1*p1.x + w2*p2.x + w3*p3.x
			, w0*p0.y + w1*p1.y + w2*p2.y + w3*p3.y
			};
			DrawPath_LineTo( path, pos );
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// DrawMesh

	struct DrawPrim_s
	{
		DrawIdx_t* Idx;
		DrawVtx_s* Vtx;
		int NumIdx;
		int NumVtx;
		int FirstIdx;
		int FirstVtx;
	};

	struct DrawMesh_s
	{
		Array<DrawIdx_t> Idx;
		Array<DrawVtx_s> Vtx;
	};

	static void DrawMesh_Initialize( DrawMesh_s& mesh, Allocator_t alloc )
	{
		mesh.Idx.Init( alloc );
		mesh.Vtx.Init( alloc );
	}

	static void DrawMesh_Shutdown( DrawMesh_s& mesh )
	{
		mesh.Idx.Clear();
		mesh.Vtx.Clear();
	}

	void DrawMesh_Alloc( DrawMesh_s& mesh, int num_idx, int num_vtx, DrawPrim_s& prim )
	{
		prim.NumIdx = num_idx;
		prim.NumVtx = num_vtx;
		prim.FirstIdx = mesh.Idx.Count;
		prim.FirstVtx = mesh.Vtx.Count;
		mesh.Idx.GrowBy( num_idx );
		mesh.Vtx.GrowBy( num_vtx );
		prim.Idx = mesh.Idx.Data + prim.FirstIdx;
		prim.Vtx = mesh.Vtx.Data + prim.FirstVtx;
	}

	int DrawMesh_FillRect( DrawMesh_s& mesh, const Vec2_s& p0, const Vec2_s& p2, const Vec2_s& uv0, const Vec2_s& uv2, uint32_t argb0, uint32_t argb1, uint32_t argb2, uint32_t argb3 )
	{
		const float w = p2.x - p0.x;
		const float h = p2.y - p0.y;
		if ((w <= 0.0f) || (h <= 0.0f))
			return 0;
		if (((argb0 | argb1 | argb2 | argb3) & 0xff000000) == 0)
			return 0;

		DrawPrim_s prim;
		DrawMesh_Alloc( mesh, 6, 4, prim );

		const DrawIdx_t idx = (DrawIdx_t)prim.FirstVtx;

		const float pos_l = p0.x;
		const float pos_r =	p2.x;
		const float pos_t = p0.y;
		const float pos_b = p2.y;

		const Vec2_s lt =  { pos_l, pos_t };
		const Vec2_s rb =  { pos_r, pos_b };
		const Vec2_s rt =  { pos_r, pos_t };
		const Vec2_s lb =  { pos_l, pos_b };

		const Vec2_s uv1 = { uv2.x	, uv0.y };
		const Vec2_s uv3 = { uv0.x, uv2.y };

		prim.Idx[0] = idx;
		prim.Idx[1] = idx + 1;
		prim.Idx[2] = idx + 2;
		prim.Idx[3] = idx;
		prim.Idx[4] = idx + 2;
		prim.Idx[5] = idx + 3;

		prim.Vtx[0].Pos = lt;	prim.Vtx[0].UV = uv0;	prim.Vtx[0].Color = argb0;
		prim.Vtx[1].Pos = rt;	prim.Vtx[1].UV = uv1;	prim.Vtx[1].Color = argb1;
		prim.Vtx[2].Pos = rb;	prim.Vtx[2].UV = uv2;	prim.Vtx[2].Color = argb2;
		prim.Vtx[3].Pos = lb;	prim.Vtx[3].UV = uv3;	prim.Vtx[3].Color = argb3;

		return prim.NumIdx;
	}

	int DrawMesh_FillCircle( DrawMesh_s& mesh, const Vec2_s& center, float radius, int num_seg, const Vec2_s& uv, uint32_t argb )
	{
		if (radius == 0.0f)
			return 0;

		DrawPrim_s prim;
		DrawMesh_Alloc( mesh, 3 * num_seg, num_seg + 1, prim );

		// idx
		DrawIdx_t* idx = prim.Idx;
		for ( int i = 0; i < num_seg-1; ++i )
		{
			idx[0] = (DrawIdx_t)(prim.FirstVtx +     0);
			idx[1] = (DrawIdx_t)(prim.FirstVtx + i + 1);
			idx[2] = (DrawIdx_t)(prim.FirstVtx + i + 2);
			idx += 3;
		}
		idx[0] = (DrawIdx_t)(prim.FirstVtx + 0);
		idx[1] = (DrawIdx_t)(prim.FirstVtx + num_seg);
		idx[2] = (DrawIdx_t)(prim.FirstVtx + 1);
		idx += 3;

		// vtx
		DrawVtx_s* vtx = prim.Vtx;
		vtx[0].Pos	 = center;
		vtx[0].UV	 = uv;
		vtx[0].Color = argb;
		vtx++;

		const float range = 2.0f * NE_PI;
		const float step  = range / (float)num_seg;
		float min_angle = 0.5f * step;
		float angle;
		for ( int i = 0; i < num_seg; ++i )
		{
			angle = min_angle + range * (float)i / float(num_seg);
			vtx[0].Pos.x = center.x + cosf( angle ) * radius;
			vtx[0].Pos.y = center.y + sinf( angle ) * radius;
			vtx[0].UV	 = uv;
			vtx[0].Color = argb;
			++vtx;
		}
		return prim.NumIdx;
	}

	int DrawMesh_DrawShape( DrawMesh_s& mesh, const Vec2_s* pos, int num_pos, bool closed, const float thickness, const Vec2_s& uv, uint32_t argb )
	{
		if (!pos || !num_pos)
			return 0;

		const int count = closed ? num_pos : num_pos-1;

		DrawPrim_s prim;
		DrawMesh_Alloc( mesh, 6 * count, 4 * count, prim );

		DrawVtx_s* vtx = prim.Vtx;
		DrawIdx_t* idx = prim.Idx;
		int cur_vtx = prim.FirstVtx;

		for ( int i = 0; i < count; ++i )
		{
			const int i1 = i;
			const int i2 = ((i+1) == num_pos) ? 0 : (i+1);

			const Vec2_s& p1 = pos[i1];
			const Vec2_s& p2 = pos[i2];
			const Vec2_s dir = Vec2_Dir( p1, p2 );
            const float dx = dir.x * (thickness * 0.5f);
            const float dy = dir.y * (thickness * 0.5f);

            vtx[0].Pos.x = p1.x + dy; vtx[0].Pos.y = p1.y - dx; vtx[0].UV = uv; vtx[0].Color = argb;
            vtx[1].Pos.x = p2.x + dy; vtx[1].Pos.y = p2.y - dx; vtx[1].UV = uv; vtx[1].Color = argb;
            vtx[2].Pos.x = p2.x - dy; vtx[2].Pos.y = p2.y + dx; vtx[2].UV = uv; vtx[2].Color = argb;
            vtx[3].Pos.x = p1.x - dy; vtx[3].Pos.y = p1.y + dx; vtx[3].UV = uv; vtx[3].Color = argb;

            idx[0] = (DrawIdx_t)(cur_vtx); 
			idx[1] = (DrawIdx_t)(cur_vtx + 1); 
			idx[2] = (DrawIdx_t)(cur_vtx + 2);
            idx[3] = (DrawIdx_t)(cur_vtx); 
			idx[4] = (DrawIdx_t)(cur_vtx + 2); 
			idx[5] = (DrawIdx_t)(cur_vtx + 3);

            cur_vtx += 4;
            vtx		+= 4;
			idx		+= 6;
		}

		return prim.NumIdx;
	}

	int DrawMesh_FillTriangle( DrawMesh_s& mesh, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, const Vec2_s& uv, uint32_t argb )
	{
		DrawPrim_s prim;
		DrawMesh_Alloc( mesh, 3, 3, prim );

		DrawVtx_s* vtx = prim.Vtx;
		DrawIdx_t* idx = prim.Idx;
		int cur_vtx = prim.FirstVtx;

		// @todo: detect and fix wrong winding	(shouldn't really matter tho since back face culling should be disabled)

        idx[0] = (DrawIdx_t)(cur_vtx); 
		idx[1] = (DrawIdx_t)(cur_vtx + 1); 
		idx[2] = (DrawIdx_t)(cur_vtx + 2);

		vtx[0].Pos = a;	vtx[0].UV = uv; vtx[0].Color = argb;
		vtx[1].Pos = b;	vtx[1].UV = uv; vtx[1].Color = argb;
		vtx[2].Pos = c;	vtx[2].UV = uv; vtx[2].Color = argb;

		return prim.NumIdx;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	static Rect_s ClipRect_Normalize( const Rect_s& clip_rect )
	{
		return Rect_s
			{ NeMax( clip_rect.x, 0.0f )
			, NeMax( clip_rect.y, 0.0f )
			, NeMax( clip_rect.w, 0.0f )
			, NeMax( clip_rect.h, 0.0f )
			};
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	//@todo: Move Path to Graphics
	//		 We don't need a seperate path for each layer

	/// DrawList

	struct DrawList_s;
	typedef DrawList_s* DrawList_t;

	struct DrawList_s
	{
		Array<DrawCmd_s> Cmd;
		DrawMesh_s		 Mesh;
		DrawPath_s		 Path;
		Vec2_s			 WhiteUV;
		DrawCmd_s		 Next;
	};

	static void DrawList_Initialze( DrawList_t list, Allocator_t alloc )
	{
		list->Cmd.Init( alloc );
		DrawMesh_Initialize( list->Mesh, alloc );
		DrawPath_Initialize( list->Path, alloc );
		list->WhiteUV = Vec2_s {};
	}

	static void DrawList_Shutdown( DrawList_t list )
	{
		list->Cmd.Clear();
		DrawPath_Shutdown(list->Path);
		DrawMesh_Shutdown(list->Mesh);
	}

	Result_t DrawList_Create( Allocator_t alloc, DrawList_t* list )
	{
		if (!list)
			return NE_ERR_INVALID_ARG;
		*list = Mem_Calloc<DrawList_s>( alloc );
		DrawList_Initialze(*list, alloc);
		return NE_OK;
	}
	
	void DrawList_Reset( DrawList_t list )
	{
		list->Cmd.Reset();
		list->Mesh.Idx.Reset();
		list->Mesh.Vtx.Reset();
		NeZero( list->Next );
	}

	void DrawList_Sync( DrawList_t list )
	{
		if (!list->Next.NumIdx)
			return;
		list->Cmd.Append( list->Next );
		list->Next.NumIdx = 0;
	}

	void DrawList_Append( DrawList_t list, DrawList_t other )
	{
		DrawList_Sync(list);
		DrawList_Sync(other);
		const int cmd = list->Cmd.Count;
		const int idx = list->Mesh.Idx.Count;
		const int vtx = list->Mesh.Vtx.Count;
		list->Cmd.Append( other->Cmd );
		list->Mesh.Idx.Append( other->Mesh.Idx );
		list->Mesh.Vtx.Append( other->Mesh.Vtx );
		for ( int i = idx; i < list->Mesh.Idx.Count; ++i )
			list->Mesh.Idx[i] += (DrawIdx_t)vtx;
	}

	void DrawList_GetBatch( DrawList_t list, DrawBatch_s* out )
	{
		DrawList_Sync(list);
		out->Cmd	= list->Cmd.Data;
		out->NumCmd = list->Cmd.Count;
		out->Idx	= list->Mesh.Idx.Data;
		out->NumIdx = list->Mesh.Idx.Count;
		out->Vtx	= list->Mesh.Vtx.Data;
		out->NumVtx = list->Mesh.Vtx.Count;
	}

	bool DrawList_IsClipRect( DrawList_t list, const Rect_s* clip )
	{
		if (clip && NeHasFlag(list->Next.Flags, DrawCmd::Clip))
		{
			return (list->Next.Clip.x == clip->x)
				&& (list->Next.Clip.y == clip->y)
				&& (list->Next.Clip.w == clip->w)
				&& (list->Next.Clip.h == clip->h);
		}
		return false;
	}

	void DrawList_SetClipRect( DrawList_t list, const Rect_s* clip )
	{
		if (DrawList_IsClipRect(list, clip))
			return;
		DrawList_Sync(list);
		if (clip)
		{
			list->Next.Clip = ClipRect_Normalize( *clip );
			list->Next.Flags |= DrawCmd::Clip;
		}
		else
		{
			list->Next.Clip = Rect_s {};
			list->Next.Flags &= ~DrawCmd::Clip;
		}
	}

	void DrawList_SetTexture( DrawList_t list, Texture_t texture, const Vec2_s& white_uv )
	{
		if (list->Next.Texture == texture)
			return;
		DrawList_Sync(list);
		list->Next.Texture = texture;
		list->WhiteUV = white_uv;
	}

	void DrawList_FillRect( DrawList_t list, const Vec2_s& a, const Vec2_s& b, uint32_t argb )
	{
		list->Next.NumIdx += DrawMesh_FillRect( list->Mesh, a, b, list->WhiteUV, list->WhiteUV, argb, argb, argb, argb );
	}

	void DrawList_FillRect( DrawList_t list, const Vec2_s& a, const Vec2_s& b, const Vec2_s& uv0, const Vec2_s& uv1, uint32_t argb )
	{
		list->Next.NumIdx += DrawMesh_FillRect( list->Mesh, a, b, uv0, uv1, argb, argb, argb, argb );
	}

	void DrawList_FillRect( DrawList_t list, const Vec2_s& a, const Vec2_s& b, uint32_t argb0, uint32_t argb1, uint32_t argb2, uint32_t argb3 )
	{
		list->Next.NumIdx += DrawMesh_FillRect( list->Mesh, a, b, list->WhiteUV, list->WhiteUV, argb0, argb1, argb2, argb3 );
	}

	void DrawList_DrawLine( DrawList_t list, const Vec2_s& a, const Vec2_s& b, uint32_t argb, float thickness )
	{
		if (a.x == b.x)
			return DrawList_FillRect( list, a, Vec2_s { a.x + thickness, b.y }, argb );

		if (a.y == b.y)
			return DrawList_FillRect( list, a, Vec2_s { b.x, a.y + thickness }, argb );

		const Vec2_s pos[] = { a,b };
		list->Next.NumIdx += DrawMesh_DrawShape( list->Mesh, pos, 2, false, thickness, list->WhiteUV, argb  );
	}

	void DrawList_DrawRect( DrawList_t list, const Vec2_s& a, const Vec2_s& b, uint32_t argb )
	{
		const float w = b.x - a.x;
		const float h = b.y - a.y;
		if (w <= 0.0f || h <= 0.0f)
			return;
		if (w <= 2.0f || h <= 2.0f)
			return DrawList_FillRect( list, a, b, argb );

		DrawList_DrawLine( list, Vec2_s { a.x - 0.0f, a.y - 0.0f }, Vec2_s { a.x - 0.0f, b.y - 0.0f }, argb, 1.0f );	// left
		DrawList_DrawLine( list, Vec2_s { a.x - 0.0f, a.y - 0.0f }, Vec2_s { b.x - 0.0f, a.y - 0.0f }, argb, 1.0f );	// top
		DrawList_DrawLine( list, Vec2_s { b.x - 1.0f, a.y - 0.0f }, Vec2_s { b.x - 1.0f, b.y - 0.0f }, argb, 1.0f );	// right
		DrawList_DrawLine( list, Vec2_s { a.x - 0.0f, b.y - 1.0f }, Vec2_s { b.x - 0.0f, b.y - 1.0f }, argb, 1.0f );	// bottom
	}

	void DrawList_DrawCircle( DrawList_t list, const Vec2_s& center, float radius, int num_seg, uint32_t argb )
	{
		if (radius == 0.0f)
			return;
		if (!DrawPath_DrawCircle( list->Path, center, radius - 0.5f, num_seg ))
			return;
		const Vec2_s* pos = list->Path.Pos.Data;
		const int num_pos = list->Path.Pos.Count;
		list->Next.NumIdx += DrawMesh_DrawShape( list->Mesh, pos, num_pos, true, 1.0f, list->WhiteUV, argb );
		DrawPath_Reset( list->Path );
	}

	void DrawList_FillCircle( DrawList_t list, const Vec2_s& center, float radius, int num_seg, uint32_t argb )
	{
		list->Next.NumIdx += DrawMesh_FillCircle( list->Mesh, center, radius, num_seg, list->WhiteUV, argb );
	}

	void DrawList_DrawGlyph( DrawList_t list, const GlyphInfo_s& glyph, const Vec2_s& caret, const Rect_s* clip, float scale, uint32_t argb )
	{
		// scale and snap
		Vec2_s p0 = Vec2_Floor(caret + scale * glyph.Pos[0]);
		Vec2_s p1 = Vec2_Floor(caret + scale * glyph.Pos[1]);

		// cull empty glyphs
		const float dx = p1.x - p0.x;
		const float dy = p1.y - p0.y;
		if ((dx == 0.0f) || (dy == 0.0f))
			return;

		// cull & clip
		Vec2_s uv0 = glyph.UV[0];
		Vec2_s uv1 = glyph.UV[1];
		if (clip)
		{
			const float clip_r = clip->x + clip->w;
			const float clip_b = clip->y + clip->h;
			if (   (p0.x > clip_r)
				|| (p0.y > clip_b)
				|| (p1.x < clip->x)
				|| (p1.y < clip->y) )
				return;

			const float dl = p0.x - clip->x;
			const float dt = p0.y - clip->y;
			const float dr = p1.x - clip_r;
			const float db = p1.y - clip_b;
			const float du = uv1.x - uv0.x;
			const float dv = uv1.y - uv0.y;
			const float sx = du / dx;
			const float sy = dv / dy;
			if (dl < 0.0f)
			{
				uv0.x -= dl * sx;
				p0.x = clip->x;
			}
			if (dt < 0.0f)
			{
				uv0.y -= dt * sy;
				p0.y = clip->y;
			}
			if (dr > 0.0f)
			{
				uv1.x -= dr * sx;
				p1.x = clip_r;
			}
			if (db > 0.0f)
			{
				uv1.y -= db * sy;
				p1.y = clip_b;
			}
		}

		// draw
		DrawList_FillRect( list, p0, p1, uv0, uv1, argb );
	}

	void DrawList_DrawTriangle( DrawList_t list, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb )
	{
		DrawPath_DrawTriangle( list->Path, a, b, c );
		const Vec2_s* pos = list->Path.Pos.Data;
		const int num_pos = list->Path.Pos.Count;
		list->Next.NumIdx += DrawMesh_DrawShape( list->Mesh, pos, num_pos, true, 1.0f, list->WhiteUV, argb );
		DrawPath_Reset( list->Path );
	}

	void DrawList_FillTriangle( DrawList_t list, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb )
	{
		list->Next.NumIdx += DrawMesh_FillTriangle( list->Mesh, a, b, c, list->WhiteUV, argb );
	}

	void DrawList_DrawBezierCurve( DrawList_t list, const Vec2_s& p0, const Vec2_s& cp0, const Vec2_s& cp1, const Vec2_s& p1, uint32_t argb, float thickness, int num_segments )
	{
		DrawPath_LineTo( list->Path, p0 );
		DrawPath_BezierCurveTo( list->Path, cp0, cp1, p1, num_segments );
		const Vec2_s* pos = list->Path.Pos.Data;
		const int num_pos = list->Path.Pos.Count;
		list->Next.NumIdx += DrawMesh_DrawShape( list->Mesh, pos, num_pos, false, thickness, list->WhiteUV, argb );
		DrawPath_Reset( list->Path );
	}

	void DrawList_Release( DrawList_t list )
	{
		DrawList_Shutdown(list);
		Mem_Free( list->Cmd.Alloc, list );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Font

	struct CharInfoComparer
	{
		static int Compare( const CharInfo_s& lhs, wchar_t rhs )
		{ return IntrinsicComparer::Compare( lhs.Char, rhs ); }
	};

	const GlyphInfo_s* Font_FindGlyph( const FontInfo_s& font, wchar_t ch )
	{
		const int pos = Array_BinaryFind< CharInfoComparer >( font.Char, font.NumChars, ch );
		return (pos >= 0) ? &font.Glyph[ font.Char[ pos ].Glyph ] : nullptr;
	}

	template < typename T >
	static Rect_s Font_MeasureString_Impl( const FontInfo_s& font, const T* t, int l, TextFormat::Mask_t fmt, const Rect_s& r, float scale )
	{
		if (!t || !l)
			return Rect_s { r.x, r.y, 0.0f, 0.0f };

		const bool no_wrap = NeHasFlag( fmt, TextFormat::NoWrap );
		const float line_height = scale * font.LineHeight;

		size_t n;
		uint32_t ch;
		Rect_s b = r;
		Vec2_s caret = Vec2_s { r.x, r.y };
		float max_line_width = 0.0f;
		const GlyphInfo_s* g = nullptr;
		const size_t len = (l < 0) ? CharTrait< T >::Length( t ) : l;
		for ( size_t i = 0; i < len; )
		{
			// decode
			n = CharTrait< T >::Decode( t + i, t + len, ch );
			if (!n)
				break;
			i += n;

			// new-line
			if (ch == L'\n')
			{
				if (no_wrap)
				{
					// single line: expand width and set height to a single line
					b.x = r.x;
					b.y = r.y;
					b.w = (caret.x - r.x);
					b.h = line_height;
					return b;
				}
				else
				{
					// multi line: track max line width
					max_line_width = NeMax( max_line_width, (caret.x - r.x) );
					caret.y += line_height;
					caret.x = r.x;
					continue;
				}
			}

			// find glyph
			g = Font_FindGlyph( font, (wchar_t)ch );
			if (!g)
				continue;

			// advance
			caret.x += scale * g->Advance;
		}

		// determine caret displacement from origin
		const float caret_w = caret.x - r.x;
		const float caret_h = caret.y - r.y;

		// update max line width (last line might be the widest)
		max_line_width = NeMax( max_line_width, caret_w );

		// fill in bounds
		b = Rect_s
			{ r.x
			, r.y
			, max_line_width ? max_line_width : caret_w
			, caret_h + (caret_w ? line_height : 0)
			};
		return b;
	}

	Rect_s Font_MeasureString( const FontInfo_s& font, cstr_t t, int l, TextFormat::Mask_t fmt, const Rect_s& r, float scale )
	{
		return Font_MeasureString_Impl( font, t, l, fmt, r, scale );
	}
	
	Rect_s Font_MeasureString( const FontInfo_s& font, wcstr_t t, int l, TextFormat::Mask_t fmt, const Rect_s& r, float scale )
	{
		return Font_MeasureString_Impl( font, t, l, fmt, r, scale );
	}
	
	void Font_DrawGlyph( DrawList_t list, const GlyphInfo_s& g, const Vec2_s& p, uint32_t argb, const Rect_s& clip, TextFormat::Mask_t fmt, float scale )
	{
		DrawList_DrawGlyph( list, g, p, &clip, scale, argb );
	}

	static void Font_SetTexture( DrawList_t list, const FontInfo_s& font )
	{
		DrawList_SetTexture( list, font.Texture, font.WhiteUV );
	}

	template < typename T >
	static void Font_DrawText_Impl( DrawList_t list, const FontInfo_s& font, const Vec2_s& pos, uint32_t argb, const Rect_s& clip, TextFormat::Mask_t fmt, float scale, const T* t )
	{
		if (!list || !t)
			return;

		Font_SetTexture( list, font );

		size_t n;
		uint32_t cp;
		Vec2_s caret = pos;
		const GlyphInfo_s* g;
		for ( const T* text = t; text && text[0]; )
		{
			n = CharTrait< T >::Decode( text, nullptr, cp );
			if (!n)
				break;
			text += n;
			g = Font_FindGlyph( font, (wchar_t)cp );
			if (!g)
				continue;
			Font_DrawGlyph( list, *g, caret, argb, clip, fmt, scale );
			caret.x += scale * g->Advance;
		}
	}

	void Font_DrawText( DrawList_t list, const FontInfo_s& font, const Vec2_s& pos, uint32_t argb, const Rect_s& clip, TextFormat::Mask_t fmt, float scale, cstr_t utf8 )
	{
		Font_DrawText_Impl( list, font, pos, argb, clip, fmt, scale, utf8 );
	}

	void Font_DrawText( DrawList_t list, const FontInfo_s& font, const Vec2_s& pos, uint32_t argb, const Rect_s& clip, TextFormat::Mask_t fmt, float scale, wcstr_t utf16 )
	{
		Font_DrawText_Impl( list, font, pos, argb, clip, fmt, scale, utf16 );
	}

	template < typename T >
	static Vec2_s Font_DrawString_Impl( DrawList_t list, const FontInfo_s& font, const T* t, int l, const Vec2_s& pos, const Rect_s* clip, uint32_t argb, float scale )
	{
		if (!list || !t || !t[0] || !l)
			return Vec2_s { 0.0f, font.LineHeight };

		DrawList_SetTexture( list, font.Texture, font.WhiteUV );

		size_t n;
		wchar_t cp = 0;
		Vec2_s caret = pos;
		const GlyphInfo_s* glyph;
		const T* end = t + Utf_Len( t, l );
		for ( const T* text = t; text < end; )
		{
			n = Utf_Decode( text, end, &cp );
			if (!n)
				break;
			text += n;
			glyph = Font_FindGlyph( font, cp );
			if (!glyph)
				continue;
			DrawList_DrawGlyph( list, *glyph, caret, clip, scale, argb );
			caret.x += scale * glyph->Advance;
		}
		return Vec2_s { caret.x - pos.x, font.LineHeight };
	}

	template < typename T >
	static void Font_DrawString_Impl( DrawList_t list, const FontInfo_s& font, const T* t, int l, TextFormat::Mask_t fmt, const Rect_s& r, uint32_t argb, float scale )
	{
		if (!list || !t || !l || !scale)
			return;

		// calculate alignment offset
		Vec2_s offset = Vec2_s {}; 
		int horz = fmt & TextFormat::Horz;
		int vert = fmt & TextFormat::Vert;
		if ( horz || vert )
		{
			Rect_s bounds = Font_MeasureString_Impl< T >( font, t, l, fmt, r, scale );

			switch ( horz )
			{
			case TextFormat::Center:
				offset.x = 0.5f * (r.w - bounds.w);
				break;
			case TextFormat::Right:
				offset.x = r.w - bounds.w;
				break;
			default:
				offset.x = 0;
				break;
			}

			switch ( vert )
			{
			case TextFormat::Middle:
				offset.y = 0.5f * (r.h - bounds.h);
				break;
			case TextFormat::Bottom:
				// @todo: this will align the ascent 
				offset.y = r.h - bounds.h;
				break;
			default:
				offset.y = 0;
				break;
			}
		}

		// draw
		Font_SetTexture( list, font );

		const bool no_wrap = NeHasFlag( fmt, TextFormat::NoWrap );
		const float line_height = scale * font.LineHeight;
		const Rect_s clip = r;
		const float clip_right  = r.x + r.w;
		const float clip_bottom = r.y + r.h;

		size_t n;
		uint32_t ch;
		Vec2_s caret = Rect_Pos( r ) + offset;
		const GlyphInfo_s* g = nullptr;
		const size_t len = (l < 0) ? CharTrait< T >::Length( t ) : l;
		for ( size_t i = 0; i < len; )
		{
			// decode
			n = CharTrait< T >::Decode( t + i, t + len, ch );
			if (!n)
				break;
			i += n;

			// escape codes
			switch (ch)
			{
			case L'\n':
				{
					if (no_wrap)
					{
						ch = L' ';
					}
					else
					{
						caret.x = r.x;
						caret.y += line_height;

						// cull vertically
						if (caret.y >= clip_bottom)
							break;

						continue;
					}
				}
				break;

			case L'\r':
				continue;

			case L'\t':
				{
					g = Font_FindGlyph( font, L' ' );						// measure tabs in multiples of the space char
					const float tab_stop = 4.0f;							// there's a tab stop at every 4th char
					const size_t dist = (size_t)(((caret.x - r.x)/scale));	// unscaled pixel distance from start of line to caret
					const size_t stop = (size_t)(tab_stop * g->Advance);	// a single tab stop's pixel width
					const size_t align = Size_Align(dist+1, stop);			// align pixel to next tab stop
					caret.x = r.x + scale * align;							// goto scaled position
					continue;												// don't draw the glyph
				}

			default:
				break;
			}
			
			// cull horizontally
			if (caret.x >= clip_right)
			{
				// if we never break lines,
				// we can stop here
				if (no_wrap)
					break;

				// continue looking for line breaks
				continue;
			}

			// find glyph
			g = Font_FindGlyph( font, (wchar_t)ch );
			if (!g)
				continue;

			// draw glyph
			Font_DrawGlyph( list, *g, caret, argb, clip, fmt, scale );
			caret.x += scale * g->Advance;
		}
	}

	void Font_DrawString( DrawList_t list, const FontInfo_s& font, wcstr_t t, int l, TextFormat::Mask_t fmt, const Rect_s& r, uint32_t argb, float scale )
	{
		Font_DrawString_Impl( list, font, t, l, fmt, r, argb, scale );
	}

	void Font_DrawString( DrawList_t list, const FontInfo_s& font, wcstr_t t, int l, TextFormat::Mask_t fmt, const Vec2_s& p, uint32_t argb, float scale )
	{
		const Rect_s bounds = Font_MeasureString( font, t, l, fmt, Rect_s { p.x, p.y, 0.0f, 0.0f }, scale );
		Font_DrawString( list, font, t, l, fmt, bounds, argb, scale );
	}

	void Font_DrawString( DrawList_t list, const FontInfo_s& font, cstr_t t, int l, TextFormat::Mask_t fmt, const Rect_s& r, uint32_t argb, float scale )
	{
		Font_DrawString_Impl( list, font, t, l, fmt, r, argb, scale );
	}

	void Font_DrawString( DrawList_t list, const FontInfo_s& font, cstr_t t, int l, TextFormat::Mask_t fmt, const Vec2_s& p, uint32_t argb, float scale )
	{
		const Rect_s bounds = Font_MeasureString( font, t, l, fmt, Rect_s { p.x, p.y, 0.0f, 0.0f }, scale );
		Font_DrawString( list, font, t, l, fmt, bounds, argb, scale );
	}

	Vec2_s Font_DrawString( DrawList_t list, const FontInfo_s& font, cstr_t t, int l, const Vec2_s& p, uint32_t argb, float scale )
	{
		return Font_DrawString_Impl( list, font, t, l, p, nullptr, argb, scale );
	}

	Vec2_s Font_DrawString( DrawList_t list, const FontInfo_s& font, wcstr_t t, int l, const Vec2_s& p, uint32_t argb, float scale )
	{
		return Font_DrawString_Impl( list, font, t, l, p, nullptr, argb, scale );
	}

	template < typename T >
	float Font_CaretToCoord( const FontInfo_s& font, const T* t, int l, int caret )
	{
		if (caret <= 0)
			return 0.0f;

		int n;
		int pos = 0;
		float x = 0.0f;
		wchar_t ch = 0;
		const GlyphInfo_s* glyph = nullptr;
		const int len = Utf_Len( t, l );
		for ( int i = 0; i < len; )
		{
			// end
			if ( pos == caret )
				break;

			// decode
			n = Utf_Decode( t + i, t + len, &ch );
			if (!n)
				break;
			i   += n;
			pos += 1;

			// find glyph
			glyph = Font_FindGlyph( font, (wchar_t)ch );
			if (!glyph)
				continue;
			x += glyph->Advance;
		}
		return x;
	}

	template < typename T >
	int Font_CoordToCaret( const FontInfo_s& font, const T* t, int l, float coord )
	{
		if (coord <= 0.0f)
			return 0;

		int n;
		int pos = 0;
		float x = 0.0f;
		float x1 = 0.0f;
		wchar_t ch = 0;
		const GlyphInfo_s* glyph = nullptr;
		const int len = Utf_Len( t, l );
		for ( int i = 0; i < len; )
		{
			// decode
			n = Utf_Decode( t + i, t + len, &ch );
			if (!n)
				break;
			i += n;

			// find glyph
			glyph = Font_FindGlyph( font, (wchar_t)ch );
			if (!glyph)
				continue;

			// test
			x1 = x + glyph->Advance;
			if ((coord >= x) && (coord < x1))
				break;

			// advance
			x = x1;
			++pos;
		}
		return pos;
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Transform

	Transform_s Transform_Id()
	{
		return Transform_s {};
	}

	Transform_s Transform_Build( const Vec2_s& v )
	{
		return Transform_s { v };
	}

	Vec2_s Transform_Apply( const Transform_s& t, const Vec2_s& v )
	{
		return v + t.Offset;
	}

	Vec2_s Transform_Unapply( const Transform_s& t, const Vec2_s& v )
	{
		return v - t.Offset;
	}

	Rect_s Transform_Apply( const Transform_s& t, const Rect_s& r )
	{
		return r + t.Offset;
	}

	Rect_s Transform_Unapply( const Transform_s& t, const Rect_s& r )
	{
		return r - t.Offset;
	}

	Transform_s Transform_Concat( const Transform_s& t, const Vec2_s& v )
	{
		return Transform_s { t.Offset + v };
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Graphics

	enum { MAX_NUM_LAYERS = 4 };

	struct Graphics_s
	{
		int32_t		Layer;
		int32_t		_pad_;
		Transform_s	Transform;
		DrawList_t	DrawList[ MAX_NUM_LAYERS ];
		FontCache_t FontCache;
		Allocator_t Alloc;
	};

	Result_t Graphics_Create( Allocator_t alloc, FontCache_t font_cache, Graphics_t* g )
	{
		if (!g)
			return NE_ERR_INVALID_ARG;

		Graphics_s* instance = Mem_Calloc<Graphics_s>( alloc );

		Result_t hr;
		for ( int i = 0; i < MAX_NUM_LAYERS; ++i )
			hr = DrawList_Create( alloc, instance->DrawList + i );

		instance->FontCache = font_cache;
		instance->Alloc		= alloc;

		*g = instance;
		return NE_OK;
	}

	void Graphics_Release( Graphics_t g )
	{
		if (!g)
			return;
		for ( int i = 0; i < MAX_NUM_LAYERS; ++i )
			DrawList_Release( g->DrawList[i] ); 
		Mem_Free( g->Alloc, g );
	}

	static void Graphics_BeginFlush( Graphics_t g, DrawBatch_s* batch )
	{
		for ( int i = 1; i < NeCountOf(g->DrawList); ++i )
		{
			DrawList_Append( g->DrawList[0], g->DrawList[i] );
			DrawList_Reset ( g->DrawList[i] );
		}
		DrawList_GetBatch( g->DrawList[0], batch );
	}

	static void Graphics_EndFlush( Graphics_t g, const Rect_s& clip )
	{
		for ( int i = 0; i < NeCountOf(g->DrawList); ++i )
			DrawList_Reset( g->DrawList[i] );
		DrawList_SetClipRect(g->DrawList[0], &clip);
	}

	static void Graphics_BeginFrame( Graphics_t g )
	{
		for ( int i = 0; i < NeCountOf(g->DrawList); ++i )
			DrawList_Reset( g->DrawList[i] );
		g->Layer = 0;
	}

	static void Graphics_EndFrame( Graphics_t g, DrawBatch_s* batch )
	{
		Graphics_BeginFlush( g, batch );
	}

	static Transform_s Graphics_GetTransform_( Graphics_t g )
	{
		return g->Transform;
	}

	Vec2_s Graphics_GetTransform( Graphics_t g )
	{
		return g->Transform.Offset;
	}

	void Graphics_SetTransform( Graphics_t g, const Transform_s& t )
	{
		g->Transform = t;
	}

	static int Graphics_GetLayer( Graphics_t g )
	{
		return g->Layer;
	}

	static void Graphics_BeginLayer( Graphics_t g )
	{
		NeAssert( g->Layer < MAX_NUM_LAYERS );
		++g->Layer;
	}

	static void Graphics_EndLayer( Graphics_t g )
	{
		NeAssert( g->Layer > 0 );
		--g->Layer;
	}

	void Graphics_SetClipRect( Graphics_t g, const Rect_s* r )
	{
		DrawList_t draw_list = g->DrawList[ g->Layer ];
		DrawList_SetClipRect( draw_list, r );
	}

	void Graphics_SetTexture( Graphics_t g, Texture_t texture, const Vec2_s& white_uv )
	{
		DrawList_SetTexture( g->DrawList[ g->Layer ], texture, white_uv );
	}

	void Graphics_DrawLine( Graphics_t g, const Vec2_s& a, const Vec2_s& b, uint32_t argb )
	{
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_DrawLine( g->DrawList[ g->Layer ], ta, tb, argb, 1.0f );
	}

	void Graphics_DrawLine( Graphics_t g, const Vec2_s& a, const Vec2_s& b, uint32_t argb, float thickness )
	{
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_DrawLine( g->DrawList[ g->Layer ], ta, tb, argb, thickness );
	}

	void Graphics_DrawRect( Graphics_t g, const Rect_s& r, uint32_t argb )
	{
		const Vec2_s a = { r.x, r.y };
		const Vec2_s b = { r.x + r.w, r.y + r.h };
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_DrawRect( g->DrawList[ g->Layer ], ta, tb, argb );
	}

	void Graphics_FillRect( Graphics_t g, const Rect_s& r, uint32_t argb )
	{
		const Vec2_s a = { r.x, r.y };
		const Vec2_s b = { r.x + r.w, r.y + r.h };
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_FillRect( g->DrawList[ g->Layer ], ta, tb, argb );
	}

	void Graphics_FillRect( Graphics_t g, const Rect_s& r, const Vec2_s& uv0, const Vec2_s& uv1, uint32_t argb )
	{
		const Vec2_s a = { r.x, r.y };
		const Vec2_s b = { r.x + r.w, r.y + r.h };
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_FillRect( g->DrawList[ g->Layer ], ta, tb, uv0, uv1, argb );
	}

	void Graphics_FillRect( Graphics_t g, const Rect_s& r, uint32_t argb0, uint32_t argb1, uint32_t argb2, uint32_t argb3 )
	{
		const Vec2_s a = { r.x, r.y };
		const Vec2_s b = { r.x + r.w, r.y + r.h };
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		DrawList_FillRect( g->DrawList[ g->Layer ], ta, tb, argb0, argb1, argb2, argb3 );
	}

	void Graphics_FillRect( Graphics_t g, const Rect_s& r, uint32_t argb1, uint32_t argb2, LinearGradient::Mode m )
	{ 
		uint32_t color[4];
		switch ( m )
		{
		case LinearGradient::Horizontal:
			color[0] = argb1;
			color[1] = argb2;
			color[2] = argb2;
			color[3] = argb1;
			break;
		case LinearGradient::Vertical:
			color[0] = argb1;
			color[1] = argb1;
			color[2] = argb2;
			color[3] = argb2;
			break;
		case LinearGradient::ForwardDiagonal:
			color[0] = argb2;
			color[1] = argb2;
			color[2] = argb1;
			color[3] = argb2;
			break;
		case LinearGradient::BackwardDiagonal:
			color[0] = argb1;
			color[1] = argb2;
			color[2] = argb2;
			color[3] = argb2;
			break;
		}
		Graphics_FillRect( g, r, color[0], color[1], color[2], color[3] );
	}

	void Graphics_DrawCircle( Graphics_t g, const Vec2_s& center, float radius, uint32_t argb )
	{
		const Vec2_s tc = Transform_Apply( g->Transform, center );
		DrawList_DrawCircle( g->DrawList[ g->Layer ], tc, radius, 12, argb );
	}

	void Graphics_DrawCircle( Graphics_t g, const Vec2_s& center, float radius, int num_seg, uint32_t argb )
	{
		const Vec2_s tc = Transform_Apply( g->Transform, center );
		DrawList_DrawCircle( g->DrawList[ g->Layer ], tc, radius, num_seg, argb );
	}

	void Graphics_FillCircle( Graphics_t g, const Vec2_s& center, float radius, uint32_t argb )
	{
		const Vec2_s tc = Transform_Apply( g->Transform, center );
		DrawList_FillCircle( g->DrawList[ g->Layer ], tc, radius, 12, argb );
	}

	void Graphics_FillCircle( Graphics_t g, const Vec2_s& center, float radius, int num_seg, uint32_t argb )
	{
		const Vec2_s tc = Transform_Apply( g->Transform, center );
		DrawList_FillCircle( g->DrawList[ g->Layer ], tc, radius, num_seg, argb );
	}

	void Graphics_DrawTriangle( Graphics_t g, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb )
	{
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		const Vec2_s tc = Transform_Apply( g->Transform, c );
		DrawList_DrawTriangle( g->DrawList[ g->Layer ], ta, tb, tc, argb );
	}

	void Graphics_FillTriangle( Graphics_t g, const Vec2_s& a, const Vec2_s& b, const Vec2_s& c, uint32_t argb )
	{
		const Vec2_s ta = Transform_Apply( g->Transform, a );
		const Vec2_s tb = Transform_Apply( g->Transform, b );
		const Vec2_s tc = Transform_Apply( g->Transform, c );
		DrawList_FillTriangle( g->DrawList[ g->Layer ], ta, tb, tc, argb );
	}

	void Graphics_DrawBezierCurve( Graphics_t g, const Vec2_s& p0, const Vec2_s& cp0, const Vec2_s& cp1, const Vec2_s& p1, uint32_t argb )
	{
		const Vec2_s tp0  = Transform_Apply( g->Transform, p0  );
		const Vec2_s tcp0 = Transform_Apply( g->Transform, cp0 );
		const Vec2_s tcp1 = Transform_Apply( g->Transform, cp1 );
		const Vec2_s tp1  = Transform_Apply( g->Transform, p1  );
		DrawList_DrawBezierCurve( g->DrawList[ g->Layer ], tp0, tcp0, tcp1, tp1, argb, 1.0f, 0 );
	}

	void Graphics_DrawBezierCurve( Graphics_t g, const Vec2_s& p0, const Vec2_s& cp0, const Vec2_s& cp1, const Vec2_s& p1, uint32_t argb, float thickness )
	{
		const Vec2_s tp0  = Transform_Apply( g->Transform, p0  );
		const Vec2_s tcp0 = Transform_Apply( g->Transform, cp0 );
		const Vec2_s tcp1 = Transform_Apply( g->Transform, cp1 );
		const Vec2_s tp1  = Transform_Apply( g->Transform, p1  );
		DrawList_DrawBezierCurve( g->DrawList[ g->Layer ], tp0, tcp0, tcp1, tp1, argb, thickness, 0 );
	}

	Vec2_s Graphics_DrawString( Graphics_t g, const Vec2_s& p, cstr_t t, int len, Font_t f, uint32_t argb )
	{
		if (!f)
			return Vec2_s { 0.0f, Graphics_GetLineHeight( g, f ) };
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Vec2_s tp = Transform_Apply( g->Transform, p );
		return Font_DrawString( g->DrawList[ g->Layer ], font, t, len, tp, argb, 1.0f );
	}

	Vec2_s Graphics_DrawString( Graphics_t g, const Vec2_s& p, wcstr_t t, int len, Font_t f, uint32_t argb )
	{
		if (!f)
			return Vec2_s { 0.0f, Graphics_GetLineHeight( g, f ) };
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Vec2_s tp = Transform_Apply( g->Transform, p );
		return Font_DrawString( g->DrawList[ g->Layer ], font, t, len, tp, argb, 1.0f );
	}

	Vec2_s Graphics_DrawString( Graphics_t g, const Vec2_s& p, const Text_s& t, Font_t f, uint32_t argb )
	{
		return t.Utf16 
			? Graphics_DrawString( g, p, t.Utf16, t.NumChars, f, argb )
			: Graphics_DrawString( g, p, t.Utf8 , t.NumChars, f, argb )
			;
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, cstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb )
	{
		if (!f)
			return;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Rect_s tr = Transform_Apply( g->Transform, r );
		Font_DrawString( g->DrawList[ g->Layer ], font, t, len, fmt, tr, argb, 1.0f );
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb )
	{
		if (!f)
			return;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Rect_s tr = Transform_Apply( g->Transform, r );
		Font_DrawString( g->DrawList[ g->Layer ], font, t, len, fmt, tr, argb, 1.0f );
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt, uint32_t argb )
	{
		return text.Utf16 
			? Graphics_DrawString( g, r, text.Utf16, text.NumChars, f, fmt, argb )
			: Graphics_DrawString( g, r, text.Utf8 , text.NumChars, f, fmt, argb )
			;
	}

	static void Graphics_DrawString_NoSubst( Graphics_t g, const Rect_s& r, cstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		if (!f)
			return;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Rect_s tr = Transform_Apply( g->Transform, r );
		Font_DrawString( g->DrawList[ g->Layer ], font, t, len, fmt, tr, argb, scale );
	}

	static void Graphics_DrawString_NoSubst( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		if (!f)
			return;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		const Rect_s tr = Transform_Apply( g->Transform, r );
		Font_DrawString( g->DrawList[ g->Layer ], font, t, len, fmt, tr, argb, scale );
	}

	static void Graphics_DrawString_NoSubst( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		return text.Utf16 
			? Graphics_DrawString_NoSubst( g, r, text.Utf16, text.NumChars, f, fmt, argb, scale )
			: Graphics_DrawString_NoSubst( g, r, text.Utf8 , text.NumChars, f, fmt, argb, scale )
			;
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, cstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		const ScaledFont_s sf = FontCache_ScaleFont( g->FontCache, f, scale, 6.0f, 64.0f );
		return Graphics_DrawString_NoSubst( g, r, t, sf.Font, fmt, argb, sf.DrawScale );
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		const ScaledFont_s sf = FontCache_ScaleFont( g->FontCache, f, scale, 6.0f, 64.0f );
		return Graphics_DrawString_NoSubst( g, r, t, sf.Font, fmt, argb, sf.DrawScale );
	}

	void Graphics_DrawString( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt, uint32_t argb, float scale )
	{
		return text.Utf16 
			? Graphics_DrawString( g, r, text.Utf16, text.NumChars, f, fmt, argb, scale )
			: Graphics_DrawString( g, r, text.Utf8 , text.NumChars, f, fmt, argb, scale )
			;
	}

	Rect_s Graphics_MeasureString( Graphics_t g, const Rect_s& r, cstr_t t, int len, Font_t f, TextFormat::Mask_t fmt )
	{
		if (!f)
			return Rect_s();
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_MeasureString( font, t, len, fmt, r, 1.0f );
	}

	Rect_s Graphics_MeasureString( Graphics_t g, const Rect_s& r, wcstr_t t, int len, Font_t f, TextFormat::Mask_t fmt )
	{
		if (!f)
			return Rect_s();
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_MeasureString( font, t, len, fmt, r, 1.0f );
	}

	Rect_s Graphics_MeasureString( Graphics_t g, const Rect_s& r, const Text_s& text, Font_t f, TextFormat::Mask_t fmt )
	{
		return text.Utf16 
			? Graphics_MeasureString( g, r, text.Utf16, text.NumChars, f, fmt )
			: Graphics_MeasureString( g, r, text.Utf8 , text.NumChars, f, fmt );
	}

	Rect_s Graphics_MeasureString( Graphics_t g, cstr_t t, int len, Font_t f )
	{
		return Graphics_MeasureString( g, Rect_s(), t, len, f, 0 );
	}

	Rect_s Graphics_MeasureString( Graphics_t g, wcstr_t t, int len, Font_t f )
	{
		return Graphics_MeasureString( g, Rect_s(), t, len, f, 0 );
	}

	Rect_s Graphics_MeasureString( Graphics_t g, const Text_s& text, Font_t f )
	{
		return text.Utf16 
			? Graphics_MeasureString( g, text.Utf16, text.NumChars, f )
			: Graphics_MeasureString( g, text.Utf8 , text.NumChars, f );
	}

	float Graphics_CaretToCoord( Graphics_t g, cstr_t t, int len, Font_t f, int caret )
	{
		if (!f)
			return 0.0f;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_CaretToCoord( font, t, len, caret );
	}

	float Graphics_CaretToCoord( Graphics_t g, wcstr_t t, int len, Font_t f, int caret )
	{
		if (!f)
			return 0.0f;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_CaretToCoord( font, t, len, caret );
	}

	float Graphics_CaretToCoord( Graphics_t g, const Text_s& text, Font_t f, int caret )
	{
		return text.Utf16 
			? Graphics_CaretToCoord( g, text.Utf16, text.NumChars, f, caret )
			: Graphics_CaretToCoord( g, text.Utf8 , text.NumChars, f, caret );
	}

	int	Graphics_CoordToCaret( Graphics_t g, cstr_t t, int len, Font_t f, float coord )
	{
		if (!f)
			return 0;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_CoordToCaret( font, t, len, coord );
	}

	int	Graphics_CoordToCaret( Graphics_t g, wcstr_t t, int len, Font_t f, float coord )
	{
		if (!f)
			return 0;
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, t, len, font );
		return Font_CoordToCaret( font, t, len, coord );
	}

	int	Graphics_CoordToCaret( Graphics_t g, const Text_s& text, Font_t f, float coord )
	{
		return text.Utf16 
			? Graphics_CoordToCaret( g, text.Utf16, text.NumChars, f, coord )
			: Graphics_CoordToCaret( g, text.Utf8 , text.NumChars, f, coord );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	/// Graphics - v1

	void Graphics_DrawBorder( Graphics_t g, const Rect_s& r, uint32_t argb, const Vec2_s& size )
	{
		const Rect_s rt = Rect_s { r.x				 , r.y				 , r.w	 , size.y			   };
		const Rect_s rb = Rect_s { r.x				 , r.y + r.h - size.y, r.w	 , size.y			   };
		const Rect_s rl = Rect_s { r.x				 , r.y + size.y		 , size.x, r.h - 2.0f * size.y };
		const Rect_s rr = Rect_s { r.x + r.w - size.x, r.y + size.y		 , size.x, r.h - 2.0f * size.y };
		Graphics_FillRect( g, rt, argb );
		Graphics_FillRect( g, rb, argb );
		Graphics_FillRect( g, rl, argb );
		Graphics_FillRect( g, rr, argb );
	}

	void Graphics_DrawBorder( Graphics_t g, const Rect_s& r, uint32_t argb, float thickness )
	{
		return Graphics_DrawBorder( g, r, argb, Vec2_s { thickness, thickness } );
	}

	void Graphics_DrawBox( Graphics_t g, const Rect_s& r, uint32_t back, uint32_t front )
	{  
		const Rect_s r_back = r;
		const Rect_s r_face = Rect_Margin( r, 1.0f );
		Graphics_DrawRect( g, r_back, back  );
		Graphics_FillRect( g, r_face, front );
	}
	
	void Graphics_DrawRadio( Graphics_t g, const Rect_s& r, uint32_t back, uint32_t front )
	{  
		const float  radius_x = r.w * 0.5f;
		const float  radius_y = r.h * 0.5f;
		const float  radius   = NeMin( radius_x, radius_y );
		const Vec2_s center   = { r.x + radius_x, r.y + radius_y };
		Graphics_DrawCircle( g, center, radius		 , back  );
		Graphics_FillCircle( g, center, radius - 1.0f, front );
	}

	float Graphics_GetLineHeight( Graphics_t g, Font_t f )
	{ 
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, (wcstr_t)nullptr, 0, font );
		return font.LineHeight; 
	}

	float Graphics_GetMaxCharWidth( Graphics_t g, Font_t f )
	{ 
		FontInfo_s font;
		FontCache_PreloadText( g->FontCache, f, (wcstr_t)nullptr, 0, font );
		return font.MaxCharWidth; 
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	void Caret_Show( CaretInfo_s& caret )
	{
		caret.Show = true;
		caret.Tick = Clock_GetTick();
	}

	void Caret_Tick( CaretInfo_s& caret )
	{
		const Tick_t clock_freq		  = Clock_GetFreq();
		const Tick_t clock_tick		  = Clock_GetTick();
		const Tick_t caret_elapsed    = clock_tick - caret.Tick;
		const Tick_t caret_elapsed_ms = (caret_elapsed * 1000) / clock_freq;
		if (caret_elapsed_ms < 700)
			return;
		caret.Show = !caret.Show;
		caret.Tick = clock_tick;
	}

} }

//======================================================================================
namespace nemesis { namespace gui
{
	/// State Cache

	struct StateCachePage_s
	{
		uint8_t* Head;
		uint8_t* Pos;
	};

	struct StateCache_s
	{
		enum { PAGE_SIZE = 16 * 1024 - sizeof( StateCachePage_s ) };
		
		explicit StateCache_s( Allocator_t alloc )
			: Alloc	( alloc )
			, Id	( alloc )
			, State	( alloc )
			, Size	( alloc )
			, Page	( alloc )
		{}

		Allocator_t					Alloc;
		Array< Id_t >				Id;
		Array< uint8_t* >			State;
		Array< size_t >				Size;
		Array< StateCachePage_s* >	Page;
	};

	Result_t StateCache_Create( Allocator_t alloc, StateCache_t* cache )
	{
		if (!cache)
			return NE_ERR_INVALID_ARG;
		*cache = NE_ALLOC_NEW( alloc, StateCache_s )( alloc );
		return NE_OK;
	}

	void* StateCache_Ensure( StateCache_t cache, Id_t id, size_t size )
	{
		// lookup
		const int pos = Array_BinaryFind( cache->Id, id );
		if (pos >= 0)
		{
			NeAssert( cache->Size[ pos ] == size );
			return cache->State[ pos ];
		}

		// alloc
		uint8_t* state = nullptr;
		{
			int target_page = -1;
			for ( int i = cache->Page.Count-1; i >=0 ; --i )
			{
				const StateCachePage_s* item = cache->Page[i];
				const uint8_t* tail = item->Head + StateCache_s::PAGE_SIZE;
				const uint32_t remain = (uint32_t)(tail - item->Pos);
				if ( remain >= size )
				{
					target_page = i;
					break;
				}
			}

			if (target_page < 0)
			{
				target_page = cache->Page.Count;

				const size_t page_size = sizeof( StateCachePage_s* ) + StateCache_s::PAGE_SIZE;
				uint8_t* page_mem = (uint8_t*)Mem_Calloc( cache->Alloc, page_size );

				StateCachePage_s* page = (StateCachePage_s*) page_mem;
				page->Head = page->Pos = (uint8_t*)(page+1);

				cache->Page.Append( page );
			}

			state = cache->Page[ target_page ]->Pos;
			cache->Page[ target_page ]->Pos += size;
		}

		// insert
		const int idx = ~pos;
		cache->Id	.InsertAt( idx, id    );
		cache->State.InsertAt( idx, state );
		cache->Size .InsertAt( idx, size  );
		return state;
	}

	void StateCache_Release( StateCache_t cache )
	{
		if (!cache)
			return;
		for ( int i = 0; i < cache->Page.Count; ++i )
			Mem_Free( cache->Alloc, cache->Page[i] );
		NE_ALLOC_DELETE( cache->Alloc, cache );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	// Theme

	void Theme_ApplyPreset( Theme_t theme, ThemePreset::Enum preset )
	{
		switch ( preset )
		{
		case ThemePreset::Default:
			Theme_ApplyPreset_Default( theme );
			break;
		case ThemePreset::Dark:
			Theme_ApplyPreset_Dark( theme );
			break;
		}
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	template < typename T, int N > 
	struct BoundedArray	
	{
	public:
		void Push( const T& v )
		{
			NeAssert( Count < N );
			Item[ Count++ ] = v;
		}

		T Pop()
		{
			NeAssert(Count > 0);
			return Item[--Count];
		}

	public:
		const T& operator [] ( int i ) const
		{
			NeAssert(Array_IsValidIndex( *this, i ));
			return Item[i];
		}

	public:
		T Item[N];
		int Count;
	};

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	enum { MAX_DEPTH = 64 };

	struct Host_s
	{
		Vec2_s		Mouse;
		Rect_s		Screen;
		Graphics_t	Graphics;
	};

	struct Stack_s
	{
		BoundedArray<Rect_s		, MAX_DEPTH> ClipRect;
		BoundedArray<Rect_s		, MAX_DEPTH> ChildRect;
		BoundedArray<Transform_s, MAX_DEPTH> Transform;
		BoundedArray<Vec2_s		, MAX_DEPTH> View;
		BoundedArray<Id_t		, MAX_DEPTH> NextHot;
		BoundedArray<Host_s		, MAX_DEPTH> Host;
		BoundedArray<Graphics_t	, MAX_DEPTH> FreeGfx;
		BoundedArray<Graphics_t	, MAX_DEPTH> UsedGfx;
	};

	struct Context_s
	{
		Id_t			Wnd;
		Id_t			Hot;
		Id_t			Pushed;
		Id_t			Focus;
		Id_t			NextHot;
		int				NextHotLayer;

		DragInfo_s		Drag;
		CaretInfo_s		Caret;
		Mouse_s			Mouse;
		Keyboard_s		Keyboard;

		Theme_t			Theme;
		Graphics_t		Graphics;

		Stack_s			Stack;
		Rect_s			Screen;
		Rect_s			Clip;
		Rect_s			Child;
		Vec2_s			View;

		ScratchBuffer_s	Scratch;
		StateCache_t	StateCache;
	};

	static Graphics_t Context_AllocGraphics( Context_t dc )
	{ 
		if (dc->Stack.FreeGfx.Count)
			return dc->Stack.FreeGfx.Pop();

		Graphics_t g = nullptr;
		Graphics_Create( dc->Graphics->Alloc, dc->Graphics->FontCache, &g );
		return g;
	}

	static void Context_CollectGraphics( Context_t dc )
	{ 
		for ( int i = 0; i < dc->Stack.UsedGfx.Count; ++i )
			dc->Stack.FreeGfx.Push(dc->Stack.UsedGfx.Item[i]);
		dc->Stack.UsedGfx.Count = 0;
	}

	static void Context_ReleaseGraphics( Context_t dc )
	{ 
		for ( int i = 0; i < dc->Stack.UsedGfx.Count; ++i )
			Graphics_Release( dc->Stack.UsedGfx.Item[i] );
		dc->Stack.UsedGfx.Count = 0;

		for ( int i = 0; i < dc->Stack.FreeGfx.Count; ++i )
			Graphics_Release( dc->Stack.FreeGfx.Item[i] );
		dc->Stack.FreeGfx.Count = 0;

		Graphics_Release(dc->Graphics);
	}

	static void Context_PushClipRect( Context_t dc, const Rect_s& r )
	{
		dc->Stack.ClipRect.Push( dc->Clip );
		dc->Clip = r;
		Graphics_SetClipRect( dc->Graphics, &dc->Clip );
	}

	static void Context_PopClipRect( Context_t dc )
	{
		dc->Clip = dc->Stack.ClipRect.Pop();
		Graphics_SetClipRect( dc->Graphics, &dc->Clip );
	}

	static void Context_PushTransform( Context_t dc, const Transform_s& v )
	{
		dc->Stack.Transform.Push( Graphics_GetTransform_( dc->Graphics ) );
		Graphics_SetTransform( dc->Graphics, v );
	}

	static void Context_PopTransform( Context_t dc )
	{
		Graphics_SetTransform( dc->Graphics, dc->Stack.Transform.Pop() );
	}

	static void Context_PushChildRect( Context_t dc, const Rect_s& r )
	{
		dc->Stack.ChildRect.Push( dc->Child );
		dc->Child = r;
	}

	static void Context_PopChildRect( Context_t dc )
	{
		dc->Child = dc->Stack.ChildRect.Pop();
	}

	static void Context_PushView( Context_t dc, const Vec2_s& v )
	{
		dc->Stack.View.Push( dc->View );
		dc->View = v;
	}

	static void Context_PopView( Context_t dc )
	{
		dc->View = dc->Stack.View.Pop();
	}

	static Result_t Context_Create( Allocator_t alloc, FontCache_t font_cache, Theme_t skin, Context_t* dc )
	{
		if (!dc)
			return NE_ERR_INVALID_ARG;

		*dc = nullptr;

		if (!font_cache || !skin)
			return NE_ERR_INVALID_ARG;

		Result_t hr;
		Graphics_t g = nullptr;
		hr = Graphics_Create( alloc, font_cache, &g );
		if (NeFailed(hr))
		{
			*dc = nullptr;
			return hr;
		}

		StateCache_t cache = nullptr;
		hr = StateCache_Create( alloc, &cache );
		if (NeFailed(hr))
		{
			Graphics_Release( g );
			*dc = nullptr;
			return hr;
		}

		Context_t instance = Mem_Calloc<Context_s>( alloc );
		
		instance->Graphics = g;
		instance->Theme		 = skin;
		instance->StateCache = cache;

		*dc = instance;
		return NE_OK;
	}

	static void Context_Release( Allocator_t alloc, Context_t dc )
	{
		if (!dc)
			return;
		Context_ReleaseGraphics( dc );
		StateCache_Release( dc->StateCache );
		Mem_Free( alloc, dc );
	}

	void Context_BeginFrame( Context_t dc, const Rect_s& desktop_rect, const Mouse_s& mouse, const Keyboard_s& kb )
	{
		NeAssert( !dc->Stack.ClipRect .Count );
		NeAssert( !dc->Stack.ChildRect.Count );
		NeAssert( !dc->Stack.Transform.Count );
		NeAssert( !dc->Stack.View	  .Count );

		// state
		{
			// pushed
			if ((dc->Pushed != 0) && (!dc->Mouse.Button[dc->Drag.Button].IsPressed))
			{
				dc->Pushed = 0;
				NeZero(dc->Drag);
			}

			// hot
			if ((dc->Pushed == 0) || (dc->Pushed == dc->NextHot))
				dc->Hot = dc->NextHot;
			dc->NextHot		= 0;
			dc->NextHotLayer = 0;
		}

		// caret
		Caret_Tick( dc->Caret );

		// input
		dc->Mouse = mouse;
		dc->Keyboard = kb; 

		// graphics
		dc->Screen = desktop_rect;
		dc->Clip   = desktop_rect;
		dc->Child  = desktop_rect;
		dc->View   = Vec2_s {};
		Graphics_BeginFrame( dc->Graphics );
	}

	void Context_EndFrame( Context_t dc, DrawBatch_s* batch )
	{ 
		NeAssert(dc->Stack.Host.Count == 0);
		Graphics_EndFrame( dc->Graphics, batch ); 
		Context_CollectGraphics( dc );
	}

	void Context_BeginHost( Context_t dc, const Rect_s& desktop_rect, const Vec2_s& mouse )
	{
		// push current state
		dc->Stack.ClipRect	.Push(dc->Clip);
		dc->Stack.ChildRect	.Push(dc->Child);
		dc->Stack.View		.Push(dc->View);

		// push host
		const Host_s host = 
		{ dc->Mouse.Pos
		, dc->Screen
		, dc->Graphics
		};
		dc->Stack.Host.Push(host);

		// create graphics for new host
		dc->Graphics = Context_AllocGraphics( dc );

		// reset state
		dc->Screen = desktop_rect;
		dc->Clip   = desktop_rect;
		dc->Child  = desktop_rect;
		dc->View   = Vec2_s {};

		// begin frame on new host
		Graphics_BeginFrame(dc->Graphics);

		// bind input
		dc->Mouse.Pos = mouse;
	}

	void Context_EndHost( Context_t dc, DrawBatch_s* batch )
	{
		NeAssert(dc->Stack.Host.Count);

		// end frame on current host
		Graphics_EndFrame( dc->Graphics, batch );

		// store completed hosts
		dc->Stack.UsedGfx.Push(dc->Graphics);

		// pop host
		const Host_s host = dc->Stack.Host.Pop();
		dc->Graphics  = host.Graphics;
		dc->Screen	  = host.Screen;
		dc->Mouse.Pos = host.Mouse;

		// pop state
		dc->Clip	= dc->Stack.ClipRect .Pop();
		dc->Child	= dc->Stack.ChildRect.Pop();
		dc->View	= dc->Stack.View	 .Pop();
	}

	void Context_BeginFlush( Context_t dc, DrawBatch_s* batch )
	{ Graphics_BeginFlush( dc->Graphics, batch ); }

	void Context_EndFlush( Context_t dc )
	{ Graphics_EndFlush( dc->Graphics, dc->Clip ); }
	
	Theme_t Context_GetTheme( Context_t dc )
	{ return dc->Theme; }

	StateCache_t Context_GetCache( Context_t dc )
	{ return dc->StateCache; }

	ScratchBuffer_t Context_GetScratch( Context_t dc )
	{ return &dc->Scratch; }

	Graphics_t Context_GetGraphics( Context_t dc )
	{ return dc->Graphics; }

	Rect_s Context_GetDektop( Context_t dc )
	{ return dc->Screen; }

	Rect_s Context_GetChild( Context_t dc )
	{ return dc->Child; }

	Vec2_s Context_GetView( Context_t dc )
	{ return dc->View; }

	const CaretInfo_s& Context_GetCaret( Context_t dc )
	{ return dc->Caret; }

	const DragInfo_s& Context_GetDrag( Context_t dc )
	{ return dc->Drag; }

	const Mouse_s& Context_GetMouse( Context_t dc )
	{ return dc->Mouse; }

	const Keyboard_s& Context_GetKeyboard( Context_t dc )
	{ return dc->Keyboard; }

	bool Context_IsWnd( Context_t dc, Id_t id )
	{ return dc->Wnd == id; }

	bool Context_IsHot( Context_t dc, Id_t id )
	{ return dc->Hot == id; }

	bool Context_IsPushed( Context_t dc, Id_t id )
	{ return dc->Pushed == id; }

	bool Context_IsDragging( Context_t dc, Id_t id )
	{ return dc->Drag.Id == id;	}

	bool Context_HasFocus( Context_t dc, Id_t id )
	{ return dc->Focus == id; }

	Id_t Context_GetWnd( Context_t dc )
	{ return dc->Wnd; }

	Id_t Context_GetHot( Context_t dc )
	{ return dc->Hot; }

	Id_t Context_GetPushed( Context_t dc )
	{ return dc->Pushed; }

	Id_t Context_GetFocus( Context_t dc )
	{ return dc->Focus; }

	void Context_SetWnd( Context_t dc, Id_t id )
	{ dc->Wnd = id; }

	void Context_SetHot( Context_t dc, Id_t id )
	{
		if (dc->Graphics->Layer >= dc->NextHotLayer)
		{
			dc->NextHot		 = id;
			dc->NextHotLayer = dc->Graphics->Layer;
		}
	}

	void Context_SetFocus( Context_t dc, Id_t id )
	{
		dc->Focus = id;
		Caret_Show( dc->Caret );
	}

	void Context_SetPushed( Context_t dc, Id_t id )
	{
		if (!Context_IsHot( dc, id ))
			return;
		dc->Pushed = id;
		Context_SetFocus( dc, id );
	}

	void Context_BeginDrag( Context_t dc, Id_t id, const Vec2_s& pos )
	{
		Context_BeginDrag( dc, id, pos, MouseButton::Left );
	}
	
	void Context_BeginDrag( Context_t dc, Id_t id, const Vec2_s& pos, MouseButton::Enum button )
	{
		if (!Context_IsPushed( dc, id ))
			return;
		dc->Drag.Id = id;
		dc->Drag.Pos = pos;
		dc->Drag.Button = button;
	}

	void Context_BeginModal( Context_t dc )
	{
		Context_PushClipRect( dc, dc->Screen );
		Context_PushChildRect( dc, Rect_s { 0.0f, 0.0f, dc->Screen.w, dc->Screen.h } );
		Graphics_BeginLayer( dc->Graphics );
		dc->Stack.NextHot.Push( dc->NextHot );
		dc->NextHot = 0;
		dc->NextHotLayer = Graphics_GetLayer( dc->Graphics );
	}

	void Context_EndModal( Context_t dc )
	{
		Graphics_EndLayer( dc->Graphics );
		Context_PopClipRect( dc );
		Context_PopChildRect( dc );
		const Id_t old_next_hot = dc->Stack.NextHot.Pop();
		if (dc->NextHot)
			return;
		dc->NextHot = old_next_hot;
		dc->NextHotLayer = Graphics_GetLayer( dc->Graphics );
	}

	void Context_BeginChild( Context_t dc, const Rect_s& r )
	{
		const Transform_s	old_transform	= Graphics_GetTransform_( dc->Graphics );
		const Rect_s		old_clip_rect	= dc->Clip;
		const Transform_s	new_transform	= Transform_Concat( old_transform, Rect_Pos( r ) );
		const Rect_s		screen_rect		= Transform_Apply ( old_transform, r );
		const Rect_s		new_clip_rect	= Rect_Intersect( old_clip_rect, screen_rect );

		Context_PushTransform( dc, new_transform );
		Context_PushClipRect( dc, new_clip_rect );
		Context_PushChildRect( dc, Rect_s { 0.0f, 0.0f, r.w, r.h } );
	}

	void Context_EndChild( Context_t dc )
	{
		Context_PopTransform( dc );
		Context_PopChildRect( dc );
		Context_PopClipRect( dc );
	}

	void Context_BeginView( Context_t dc, const Vec2_s& offset )
	{
		const Transform_s old_transform = Graphics_GetTransform_( dc->Graphics );
		const Transform_s new_transform = Transform_Concat( old_transform, -offset );
		Context_PushTransform( dc, new_transform );
		Context_PushView( dc, offset );
	}

	void Context_EndView( Context_t dc )
	{
		Context_PopTransform( dc );
		Context_PopView( dc );
	}

	bool Context_Cull( Context_t dc, const Rect_s& r )
	{
		return !Rect_Intersects( Context_GetClipRect( dc ), r );
	}

	Font_t Context_GetFont( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		return theme->Font;
	}

	float Context_GetLineHeight( Context_t dc )
	{
		Theme_t theme = Context_GetTheme( dc );
		Graphics_t g = Context_GetGraphics( dc );
		return Graphics_GetLineHeight( g, theme->Font );
	}

	Vec2_s Context_GetMousePos( Context_t dc )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const Transform_s global_to_local = Graphics_GetTransform_( g );
		const MouseState_s& mouse = Context_GetMouse( dc );
		return Transform_Unapply( global_to_local, mouse.Pos );
	}

	Rect_s Context_GetClipRect( Context_t dc )
	{
		Graphics_t g = Context_GetGraphics( dc );
		const Transform_s global_to_local = Graphics_GetTransform_( g );
		return Transform_Unapply( global_to_local, dc->Clip );
	}

	void Context_BeginDragOnce( Context_t dc, Id_t id, const Vec2_s& pos )
	{
		Context_BeginDragOnce( dc, id, pos, MouseButton::Left );
	}

	void Context_BeginDragOnce( Context_t dc, Id_t id, const Vec2_s& pos, MouseButton::Enum button )
	{
		if (Context_IsDragging( dc, id ))
			return;
		Context_BeginDrag( dc, id, pos, button );
	}

	Text_s Context_FormatStringV( Context_t dc, cstr_t text, va_list args )
	{
		return ScratchBuffer_FormatV( &dc->Scratch, text, args );
	}

	Text_s Context_FormatStringV( Context_t dc, wcstr_t text, va_list args )
	{
		return ScratchBuffer_FormatV( &dc->Scratch, text, args );
	}

	Text_s Context_FormatString( Context_t dc, cstr_t text, ... )
	{
		if (!dc || !text || !text[0])
			return Text_s( text );
		Text_s out;
		va_list args;
		va_start( args, text );
		out = Context_FormatStringV( dc, text, args );
		va_end( args );
		return out;
	}

	Text_s Context_FormatString( Context_t dc, wcstr_t text, ... )
	{
		if (!dc || !text || !text[0])
			return Text_s( text );
		Text_s out;
		va_list args;
		va_start( args, text );
		out = Context_FormatStringV( dc, text, args );
		va_end( args );
		return out;
	}

	void* Context_CacheIntern( Context_t dc, Id_t id, size_t size )
	{
		StateCache_t cache = Context_GetCache( dc );
		return StateCache_Ensure( cache, id, size );
	}

} }

//======================================================================================
namespace nemesis { namespace gui 
{
	struct System_s
	{ 
		Allocator_t	 Alloc;
		Context_t	 Context;
		FontCache_t FontCache;
		Renderer_t	 Renderer;
		Theme_s		 Theme;
	};

	static System_s UI = {};

	Result_t System_Initialize( Allocator_t alloc, Renderer_t renderer )
	{
		if (UI.Context)
			return NE_ERR_INVALID_CALL;
		if (!renderer)
			return NE_ERR_INVALID_ARG;

		Result_t hr;

		font::System_Initialize( alloc );

		FontCache_t font_cache = Mem_Calloc<FontCache_s>( alloc );
		FontCache_Initialize( font_cache, alloc, renderer );

		Context_t context = nullptr;
		hr = Context_Create( alloc, font_cache, &UI.Theme, &context );
		if (NeFailed(hr))
		{
			FontCache_Shutdown( font_cache );
			return hr;
		}

		UI.Alloc		= alloc;
		UI.Context		= context;
		UI.FontCache	= font_cache;
		UI.Renderer		= renderer;
		Theme_ApplyPreset( &UI.Theme, ThemePreset::Default );

		return NE_OK;
	}

	void System_Shutdown()
	{
		if (!UI.Context)
			return;
		Context_Release( UI.Alloc, UI.Context );
		FontCache_Shutdown( UI.FontCache );
		Mem_Free( UI.Alloc, UI.FontCache );
		NeZero( UI );
	}

	Renderer_t System_GetRenderer()
	{
		return UI.Renderer;
	}

	FontCache_t System_GetFontCache()
	{
		return UI.FontCache;
	}

	Context_t System_GetContext()
	{
		return UI.Context;
	}

} }
