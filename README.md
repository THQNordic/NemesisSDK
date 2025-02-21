# Nemesis SDK

This package contains the Nemesis SDK, which is a collection of utility libraries used in multiple Piranha Bytes games.

## Directory Layout

* ``1.1``

	Contains batch files for cleaning and building libraries and applications.

	* ``Ext``

	Contains external dependencies (only needed if you reactivate the DX9 code path)

	* ``Sdk``
		* ``Bin``

		Generated upon building the solution.
		
		Contains applications and their respective program databases.

		* ``Cfg``

		Contains propery sheets referenced by both libraries and applications.

		* ``Inc``

		All include directives are relative to this directory.

		* ``Lib``

		Generated upon building the solution.

		Contains static libraries and their respective program databases.

		* ``Sln``

		Contains the solution file used to build libraries and applications.

		* ``Src``

		Contains implementation files and projects refereced by the solution.

		* ``Tmp``

		Generated upon building the solution.

		Contains intermediate files.

		* ``Xtr``

		Contains a .natvis file for core types.

## Building

Install [Visual Studio](http://www.visualstudio.com/downloads) 2017 or higher.

The solutiuon can either be built from Visual Studio or from the command line.

### From Visual Studio

Open ``1.1\Sdk\Sln\Ne1.sln`` and build the solution.

### From the command line

Open a command line at the ``1.1`` directory.

Run either of the batch files.

* ``clean.bat``

	Deletes all output files, including temporary build files and Visual Studio cache files.

* ``build.bat``

	Redirects to ``build_vs2022.bat``.

* ``build_vs2017.bat``

	Builds the solution using the Visual Studio 2017 toolchain.

	This batch file requires an environment variable `VS150COMNTOOLS` that must point to the `Common7\Tools` directory of Visual Studio 2017.

	By default it is located in `C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\`.

* ``build_vs2019.bat``

	Builds the solution using the Visual Studio 2019 toolchain.

	This batch file requires an environment variable `VS160COMNTOOLS` that must point to the `Common7\Tools` directory of Visual Studio 2019.

	By default it is located in `C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\`.

* ``build_vs2022.bat``

	Builds the solution using the Visual Studio 2022 toolchain.

	This batch file requires an environment variable `VS170COMNTOOLS` that must point to the `Common7\Tools` directory of Visual Studio 2022.

	By default it is located in `C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\`.

## Applications

### ProViz

ProViz is a visualizer for displaying profiling data sent by applications using the Nemesis Profiler library. 

Written in 2012, this application served as a proof of concept for both the use of an ImGUI and the profiler. The user interface predates the implementation of the ImGui library and was heavily inspired by the excellent [RAD Game Tools' Telemetry](https://www.radgametools.com/telemetry.htm) while the implementation draws from concepts mentioned in the [BitSquid Development Blog](https://bitsquid.blogspot.com/2011/05/monitoring-your-game.html). The project was later retro-fitted to use the Nemesis Gui library.

### ProViz 2

This is an updated version of Pro Viz that supports docking and uses a different set of controls for display.

### Gui Demo

This application demonstrates how to use the Gui library.

It also contains a newer version of the profiling visualizer which was used to render profiling data directly into the main window of running games.

### Gui Dock

This application demonstrates how to use the docking library in conjunction with the gui library to create panels that can be dragged and docked outside of the application's main window.

## Libraries

* ``libNeCore``

This library serves as the base layer for all nemesis projects.
It includes containers, algorithms and the platform abstraction layer.

* ``libNeFont``

This library serves as an abstraction layer for generating gylph bitmaps. 

Supported backends currently include [GDI](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getglyphoutlinew) and [Sean Barret's stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h). 

* ``libNeGui``

This library is an implementation of the [Immediate Mode GUI](https://caseymuratori.com/blog_0001) paradigm.

* ``libNeGuiDock``

This library adds docking support to the Gui.

* ``libNePerf``

This library contains the profiling server, parser and database.

* ``libNeGfx_d3d9``

This library implements the graphics backend using DirectX 9.

* ``libNeGfx_d3d11``

This library implements the graphics backend using DirectX 11.

## Using the profiler

Build the solution as explained in the [Building](#Building) section.

Add the following include path to your project:

``nemesis\1.1\Sdk\Inc``

Add the following library path to your project:

``nemesis\1.1\Sdk\Lib\$(PlatformName)\$(PlatformToolset)\$(Configuration)\$(ProjectName)``

Link against the following libraries:

* ``libNeCore.lib``
* ``libNePerf.lib``

The followings code snippet demonstrates how to use the profiling server:

```C++
#include <Nemesis/Perf/Server.h>

void run();

void main()
{
	NePerfInit(nullptr);		// using the default memory allocator
	NePerfStartSender(16001);	// listen for visualizer on TCP port 16001

	run();

	NePerfShutdown();
}

void run()
{
	NePerfFunc;

	while (!quit_requested())
	{
		NePerfScope("Next Frame");

		// @todo: render the next frame

		NePerfNextFrame();
	}
}
```

## License
Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see https://www.gnu.org/licenses/.

Feel free to fork and improve this software.

## Acknowledgements

This SDK was developed by Roman Keskenti while working at Piranha Bytes