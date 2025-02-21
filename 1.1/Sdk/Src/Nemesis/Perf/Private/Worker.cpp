//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "Worker.h"

//======================================================================================
#include <Nemesis/Core/Process.h>

//======================================================================================
namespace nemesis { namespace profiling
{ 
	void Worker_Start( Worker_s* worker, const system::ThreadSetup_s& setup )
	{
		if (worker->Thread)
			return;
		worker->Continue = 1;
		worker->Thread = system::Thread_Create( setup );
	}

	void Worker_Stop( Worker_s* worker )
	{
		worker->Continue = 0;
	}

	void Worker_Wait( Worker_s* worker )
	{
		system::Thread_Wait( worker->Thread );
		worker->Thread = nullptr;
	}

} }