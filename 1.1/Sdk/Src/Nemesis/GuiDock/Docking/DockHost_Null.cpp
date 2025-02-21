//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "DockHost_Null.h"
#include "DockMgr.h"

//======================================================================================
using namespace nemesis::gui;

//======================================================================================
namespace nemesis
{
	typedef struct DockOs_Null_s  *DockOs_Null_t;
	typedef struct DockHost_Null_s *DockHost_Null_t;

	struct DockOs_Null_s
	{
	};

	struct DockHost_Null_s
	{
		DockOs_Null_t	Os;
		DockCtrl_t		Ctrl;
		Vec2_s			Pos;
		Vec2_s			Size;
		bool			Hidden;
	};

}

//======================================================================================
namespace nemesis
{
	static DockHost_Null_t DockHost_Null_CreateMain( DockOs_Null_t os, DockCtrl_t ctrl )
	{
		NeAssert(DockCtrl_IsMain(ctrl));
		NeAssert(ctrl->Host == nullptr);

		DockHost_Null_t host = Mem_Calloc<DockHost_Null_s>( ctrl->Mgr->Alloc );
		host->Ctrl = ctrl;

		ctrl->Host = (DockHost_t)host;

		DockHost_Null_Move( (DockHost_t)host );
		DockHost_Null_Show( (DockHost_t)host );

		return host;
	}

	static DockHost_Null_t DockHost_Null_CreateFloating( DockOs_Null_t os, DockCtrl_t ctrl )
	{
		DockHost_Null_t host = Mem_Calloc<DockHost_Null_s>( ctrl->Mgr->Alloc );
		host->Ctrl = ctrl;

		ctrl->Host = (DockHost_t)host;

		DockHost_Null_Move( (DockHost_t)host );
		DockHost_Null_Show( (DockHost_t)host );

		return host;
	}

	static void DockHost_Null_RenderMain( DockHost_Null_t host )
	{
		DockCtrl_DoRoot( host->Ctrl );
	}

	static void DockHost_Null_RenderFloating( DockHost_Null_t host )
	{
		Context_t dc = host->Ctrl->Dc;
		const Rect_s r = Rect_Ctor( host->Pos, host->Size );
		Context_BeginChild( dc, r );
		DockCtrl_DoChild( host->Ctrl );
		Context_EndChild( dc );
	}

}

//======================================================================================
namespace nemesis
{
	static void DockOs_Null_Release( DockOs_Null_t os )
	{
	}

	static int DockOs_Null_OrderZ( DockOs_Null_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts )
	{
		// @todo:
		//	replace this hack with actual z-order
		//	tracking
		int pos = 0;

		// overlay
		for ( int i = 0; i < num_ctrl; ++i )
		{
			DockCtrl_t ctrl = ctrls[i];
			if (!ctrl->Host)
				continue;
			switch (ctrl->Kind)
			{
			case DockCtrl::Overlay:
				hosts[pos++] = (DockHost_t)ctrl->Host;
				break;
			default:
				break;
			}
		}

		// floating
		for ( int i = 0; i < num_ctrl; ++i )
		{
			DockCtrl_t ctrl = ctrls[i];
			if (!ctrl->Host)
				continue;
			switch (ctrl->Kind)
			{
			case DockCtrl::Main:
			case DockCtrl::Overlay:
				break;
			default:
				hosts[pos++] = (DockHost_t)ctrl->Host;
				break;
			}
		}

		// main 
		for ( int i = 0; i < num_ctrl; ++i )
		{
			DockCtrl_t ctrl = ctrls[i];
			if (!ctrl->Host)
				continue;
			switch (ctrl->Kind)
			{
			case DockCtrl::Main:
				hosts[pos++] = (DockHost_t)ctrl->Host;
				break;
			default:
				break;
			}
		}

		return pos;
	}

	static DockHost_Null_t DockHost_Null_Create( DockOs_Null_t os, DockCtrl_t ctrl )
	{
		return DockCtrl_IsMain( ctrl )
			? DockHost_Null_CreateMain( os, ctrl )
			: DockHost_Null_CreateFloating( os, ctrl );
	}

	static bool	DockHost_Null_IsActive( DockHost_Null_t host )
	{
		return false;
	}

	static void	DockHost_Null_Move( DockHost_Null_t host )
	{
		if (!host)
			return;
		host->Pos  = host->Ctrl->FloatPos;
		host->Size = Rect_Size( host->Ctrl->LocalRect );
	}

	static void	DockHost_Null_Move_Rect( DockHost_Null_t host, const Rect_s& r )
	{
		if (!host)
			return;
		host->Pos  = Rect_Pos ( r );
		host->Size = Rect_Size( r );
	}

	static void	DockHost_Null_Show( DockHost_Null_t host )
	{
		if (!host)
			return;
		host->Hidden = host->Ctrl->Hidden;
	}

	static void	DockHost_Null_Render( DockHost_Null_t host )
	{
		const bool is_main = DockCtrl_IsMain(host->Ctrl);
		return is_main 
			? DockHost_Null_RenderMain	 ( host )
			: DockHost_Null_RenderFloating( host );
	}

	static void	DockHost_Null_Release( DockHost_Null_t host )
	{
		if (!host)
			return;
		Mem_Free( host->Ctrl->Mgr->Alloc, host );
	}

}

//======================================================================================
namespace nemesis
{
	DockOs_t DockOs_Null_Create( ptr_t user )
	{
		return nullptr;
	}

	void DockOs_Null_Release( DockOs_t os )
	{
		return DockOs_Null_Release( (DockOs_Null_t)(os) );
	}

	int DockOs_Null_OrderZ( DockOs_t os, const DockCtrl_t* ctrls, int num_ctrl, DockHost_t* hosts )
	{
		return DockOs_Null_OrderZ( (DockOs_Null_t)(os), ctrls, num_ctrl, hosts );
	}

	DockHost_t DockHost_Null_Create( DockOs_t os, DockCtrl_t ctrl )
	{
		return (DockHost_t)DockHost_Null_Create( (DockOs_Null_t)(os), ctrl );
	}

	bool DockHost_Null_IsActive( DockHost_t host )
	{
		return DockHost_Null_IsActive( (DockHost_Null_t)(host) );
	}

	void DockHost_Null_Move( DockHost_t host )
	{
		return DockHost_Null_Move( (DockHost_Null_t)(host) );
	}

	void DockHost_Null_Move_Rect( DockHost_t host, const Rect_s& r )
	{
		return DockHost_Null_Move_Rect( (DockHost_Null_t)(host), r );
	}

	void DockHost_Null_Show( DockHost_t host )
	{
		return DockHost_Null_Show( (DockHost_Null_t)(host) );
	}

	void DockHost_Null_Render( DockHost_t host )
	{
		return DockHost_Null_Render( (DockHost_Null_t)(host) );
	}

	void DockHost_Null_Release( DockHost_t host )
	{
		return DockHost_Null_Release( (DockHost_Null_t)(host) );
	}
}

//======================================================================================
namespace nemesis
{
	DockMgrApi_Os_s	DockOs_Null_GetApi()
	{
		return DockMgrApi_Os_s
			{ DockOs_Null_Create
			, DockOs_Null_Release
			, DockOs_Null_OrderZ
			, DockHost_Null_Create
			, DockHost_Null_IsActive
			, DockHost_Null_Move
			, DockHost_Null_Move_Rect
			, DockHost_Null_Show
			, DockHost_Null_Render
			, DockHost_Null_Release
			, nullptr
			};
		
	}
}
