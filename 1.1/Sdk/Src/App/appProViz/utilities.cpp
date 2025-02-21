//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "utilities.h"

// import EnvDTE
#pragma warning(disable : 4278)
#pragma warning(disable : 4146)
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning(default : 4146)
#pragma warning(default : 4278)

//======================================================================================
namespace nemesis { namespace utilities
{
	bool OpenFileInVisualStudio( const char* file, int line )
	{
		// http://stackoverflow.com/questions/350323/open-a-file-in-visual-studio-at-a-specific-line-number
		HRESULT result;
		CLSID clsid;
		result = ::CLSIDFromProgID(L"VisualStudio.DTE", &clsid);
		if (FAILED(result))
			return false;

		CComPtr<IUnknown> punk;
		result = ::GetActiveObject(clsid, NULL, &punk);
		if (FAILED(result))
			return false;

		CComPtr<EnvDTE::_DTE> DTE;
		DTE = punk;

		CComPtr<EnvDTE::ItemOperations> item_ops;
		result = DTE->get_ItemOperations(&item_ops);
		if (FAILED(result))
			return false;

		CComBSTR bstrFileName(file);
		CComBSTR bstrKind(EnvDTE::vsViewKindTextView);
		CComPtr<EnvDTE::Window> window;
		result = item_ops->OpenFile(bstrFileName, bstrKind, &window);
		if (FAILED(result))
			return false;

		CComPtr<EnvDTE::Document> doc;
		result = DTE->get_ActiveDocument(&doc);
		if (FAILED(result))
			return false;

		CComPtr<IDispatch> selection_dispatch;
		result = doc->get_Selection(&selection_dispatch);
		if (FAILED(result))
			return false;

		CComPtr<EnvDTE::TextSelection> selection;
		result = selection_dispatch->QueryInterface(&selection);
		if (FAILED(result))
			return false;

		result = selection->GotoLine(line, TRUE);
		if (FAILED(result))
			return false;

		return true;
	}

} }
