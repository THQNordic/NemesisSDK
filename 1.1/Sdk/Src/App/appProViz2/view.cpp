//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#include "stdafx.h"
#include "view.h"
#include "doc.h"
#include "app.h"

//======================================================================================
using namespace nemesis;
using namespace nemesis::gui;
using namespace nemesis::system;

//======================================================================================
struct Bar_s
{
    Context_t Dc;
    Id_t      Id;
    Rect_s    Rect;
    Vec2_s    Pos;
    float     Spacing;
    int       NextId;
};

static float Bar_CalcH(Context_t dc)
{
    return Button_CalcHeight(dc) + 2.0f;
}

static void Bar_Begin(Bar_s& bar, Context_t dc, Id_t id, const Rect_s& r, float spacing)
{
    bar.Dc      = dc;
    bar.Id      = id;
    bar.Rect    = r;
    bar.Pos     = { r.x + 2.0f, r.y + 1.0f };
    bar.Spacing = spacing;
    bar.NextId  = 0;
}

static Bar_s Bar_Begin(Context_t dc, Id_t id, const Rect_s& r, float spacing)
{
    Bar_s bar = {};
    Bar_Begin(bar, dc, id, r, spacing);
    return bar;
}

static bool Bar_DoButtonH(Bar_s& bar, Text_s text, bool enabled)
{
    const Id_t id = Id_Cat(bar.Id, bar.NextId++);
    const Vec2_s size = Button_CalcSize(bar.Dc, text);
    const ButtonStyle::Mask_t  style = enabled ? ButtonStyle::None : ButtonStyle::Disabled;
    const ButtonAction::Enum action = Button_Do(bar.Dc, id, Rect_Ctor(bar.Pos, size), text, style);
    bar.Pos.x += size.x + bar.Spacing;
    return action == ButtonAction::Clicked;
}

//======================================================================================
void NE_CALLBK ZoneTab_Do(DockCtrl_t ctrl, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Id_t id = DockCtrl_GetId(ctrl);
    const Rect_s r = Context_GetChild(dc);
    Ctrl_DrawBox(dc, r, Visual::Window, Ctrl_GetState(dc, id));
    doc->ZoneRect = r;

    ScrollView_Begin(dc, id, r, doc->ZoneSize, doc->ZoneScroll);
    {
        ZonePanel_s& state = doc->ZoneState;
        doc->ZoneSize = ZonePanel_Do(dc, id, r, doc->Db, state);
    }
    ScrollView_End(dc, id, r, doc->ZoneSize, { 1,1 }, doc->ZoneScroll);
}

void NE_CALLBK FrameTab_Do(DockCtrl_t ctrl, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Id_t id = DockCtrl_GetId(ctrl);
    const Rect_s r = Context_GetChild(dc);
    Ctrl_DrawBox(dc, r, Visual::Window, Ctrl_GetState(dc, id));

    ZonePanel_s& state = doc->ZoneState;
    FramePanel_Do(dc, id, r, doc->Db, doc->FrameTimeline, doc->ZoneState.Timeline, doc->ZoneRect, doc->ZoneState.AutoScroll);
}

static void ServerTab_InitList(Doc_t doc, Id_t id)
{
    List_s& list = doc->ServerList.Ctrl;
    if (list.Id)
    {
        return;
    }

    struct Local
    {
        static Text_s NE_CALLBK DoFavorite(void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state)
        {
            Doc_t doc = (Doc_t)context;
            return "";
        }

        static Text_s NE_CALLBK DoAddress(void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state)
        {
            Doc_t doc = (Doc_t)context;
            const IpAddress_t addr = doc->ServerList.Hosts[row].Ip;
            const IpAddress_t peer = Socket_GetPeer(Receiver_GetSocket(doc->Receiver));
            if (BitwiseComparer::Equals(addr, peer))
            {
                Graphics_DrawRect(Context_GetGraphics(dc), r, Color::PaleGoldenrod);
            }
            return Context_FormatString(dc, "%d.%d.%d.%d", (int)addr.Ip[0], (int)addr.Ip[1], (int)addr.Ip[2], (int)addr.Ip[3]);
        }

        static Text_s NE_CALLBK DoName(void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state)
        {
            Doc_t doc = (Doc_t)context;
            char name[256];
            const bool ok = IpAddress_GetHostName(doc->ServerList.Hosts[row].Ip, name, sizeof(name) - 1);
            return ok ? Context_FormatString(dc, "%s", name) : "";
        }

        static Text_s NE_CALLBK DoStatus(void* context, Context_t dc, const Rect_s& r, int row, int col, CtrlState::Enum state)
        {
            Doc_t doc = (Doc_t)context;
            const bool alive = doc->ServerList.Hosts[row].Alive;
            return alive ? "Available" : "Offline";
        }
    };

    static ListHeaderItem_s headers[] =
    { { "Default"	,  80.0f, Alignment::Center , SortOrder::None }
    , { "Address"	,  80.0f, Alignment::Left   , SortOrder::None }
    , { "Name"	    , 130.0f, Alignment::Left   , SortOrder::None }
    , { "Status"	, 100.0f, Alignment::Left   , SortOrder::None }
    };

    static ListColumn_s columns[] =
    { { nullptr, nullptr, { Local::DoFavorite, doc }, headers[0].Alignment }
    , { nullptr, nullptr, { Local::DoAddress , doc }, headers[1].Alignment }
    , { nullptr, nullptr, { Local::DoName    , doc }, headers[2].Alignment }
    , { nullptr, nullptr, { Local::DoStatus  , doc }, headers[2].Alignment }
    };

    NeStaticAssert(NeCountOf(headers) == NeCountOf(columns));

    list.Id = id;
    list.Data.Header = headers;
    list.Data.Column = columns;
    list.Data.NumRows = 0;
    list.Data.NumColumns = NeCountOf(columns);
    list.Style = ListStyle::HorzLines | ListStyle::VertLines;
}

static void ServerTab_DoBar(DockCtrl_t ctrl, Id_t id, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Rect_s r = Context_GetChild(dc);

    const int server_index = doc->ServerList.Ctrl.State.SelRow;
    const ServerInfo_s* selected_server = (server_index >= 0) ? (doc->ServerList.Hosts.Data + server_index) : nullptr;

    const IpAddress_t target_ip = Tcp_GetPeer(Receiver_GetSocket(doc->Receiver));

    const bool can_connect = selected_server && (!BitwiseComparer::Equals(selected_server->Ip, target_ip));
    const bool can_disconnect = (socket != nullptr);

    Bar_s bar = Bar_Begin(dc, id, r, 2.0f);
    if (Bar_DoButtonH(bar, "Connect", can_connect))
    {
        Receiver_Disconnect(doc->Receiver);
        Receiver_Connect(doc->Receiver, selected_server->Ip, doc->ReceiverCallback );
    }

    if (Bar_DoButtonH(bar, "Disconnect", can_disconnect))
    {
        Receiver_Disconnect(doc->Receiver);
    }
}

static void ServerTab_DoList(DockCtrl_t ctrl, Id_t id, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Rect_s r = Context_GetChild(dc);

    ServerTab_InitList(doc, id);
    ServerList_AutoRefresh(doc->ServerList);

    List_s& list = doc->ServerList.Ctrl;
    list.Rect = r;
    list.Data.NumRows = doc->ServerList.Hosts.Count;
    for (int i = 0; i < list.Data.NumColumns; ++i)
        list.Data.Header[i].SortOrder = SortOrder::None;

    List_Do(dc, doc->ServerList.Ctrl);
}

void NE_CALLBK ServerTab_Do(DockCtrl_t ctrl, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Id_t id = DockCtrl_GetId(ctrl);
    const Rect_s r = Context_GetChild(dc);
    Ctrl_DrawBox(dc, r, Visual::Window, Ctrl_GetState(dc, id));

    const float bar_h = Bar_CalcH(dc);
    const Rect2_s split_bar_list = Rect_SplitT(r, bar_h);
    {
        NeGuiScopedChild(dc, split_bar_list.Rect[0]);
        ServerTab_DoBar(ctrl, Id_Cat(id, "Bar"), user);
    }
    {
        NeGuiScopedChild(dc, split_bar_list.Rect[1]);
        ServerTab_DoList(ctrl, Id_Cat(id, "List"), user);
    }
}

void NE_CALLBK SettingsTab_DoGrid(DockCtrl_t ctrl, Id_t id, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Rect_s r = Context_GetChild(dc);

    Font_t font = Context_GetFont( dc );
    Theme_t theme = Context_GetTheme( dc );
    Graphics_t g = Context_GetGraphics( dc );

    static cstr_t label[] =
    { "Auto Scroll"
    , "Show Idle"
    , "Show Empty"
    , "Max Depth"
    , "Recording"
    , "Database Size"
    };

    const float text_w = TextList_CalcMaxWidth(dc, label, NeCountOf(label));
    const float text_h = Context_GetLineHeight( dc );
    const float ctrl_h = Button_CalcSize( dc, "" ).y;
    const float item_h = 2.0f + NeMax( text_h + 2.0f, ctrl_h );

    const uint32_t text_color = theme->Visual[Visual::Label].Text[CtrlState::Normal];

    Rect_s pos = Rect_s { r.x, r.y, r.w, item_h };
    for ( int i = 0; i < NeCountOf(label); ++i )
    {
        Graphics_DrawString( g, pos, label[i], font, TextFormat::Middle, text_color );
        pos.y += pos.h + 2.0f;
    }

    int child = 0;
    pos = Rect_s { r.x + text_w + 10.0f, r.y, 100.0f, item_h };
    {
        CheckBox_DoButton( dc, Id_Cat(id, child++), pos, doc->ZoneState.AutoScroll ? "On" : "Off", doc->ZoneState.AutoScroll);
        pos.y += pos.h + 2.0f;
    }
    {
        CheckBox_DoButton( dc, Id_Cat(id, child++), pos, doc->ZoneState.Cull.ShowIdle ? "On" : "Off", doc->ZoneState.Cull.ShowIdle );
        pos.y += pos.h + 2.0f;
    }
    {
        CheckBox_DoButton( dc, Id_Cat(id, child++), pos, doc->ZoneState.Cull.ShowEmpty ? "On" : "Off", doc->ZoneState.Cull.ShowEmpty );
        pos.y += pos.h + 2.0f;
    }
    {
        float max_depth = (float)doc->ZoneState.Cull.MaxZoneLevel;
        Slider_DoH(dc, Id_Cat(id, child++), pos, 0.0f, 32.0f, max_depth);
        doc->ZoneState.Cull.MaxZoneLevel = (uint8_t)max_depth;
        pos.y += pos.h + 2.0f;
    }
    {
        if (doc->Parser)
        {
            bool paused = Parser_IsPaused( doc->Parser );
            if (Button_Do( dc, Id_Cat(id, child++), pos, paused ? "Start" : "Stop" ))
                Parser_Pause( doc->Parser, !paused );
        }
        pos.y += pos.h + 2.0f;
    }
    {
        if (doc->Db)
        {
            const float bytes_to_mb = (1024 * 1024);
            float old_capacity_mb = ((float)Database_GetCapacity(doc->Db)) / bytes_to_mb;
            float new_capacity_mb = old_capacity_mb;

            TextEdit_DoFloat(dc, Id_Cat(id, child++), pos, TextEditStyle::None, new_capacity_mb);

            if (new_capacity_mb != old_capacity_mb)
                Database_SetCapacity(doc->Db, (size_t)(new_capacity_mb * bytes_to_mb));
        }
        pos.y += pos.h + 2.0f;
    }
}

void NE_CALLBK SettingsTab_Do(DockCtrl_t ctrl, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Id_t id = DockCtrl_GetId(ctrl);
    const Rect_s r = Context_GetChild(dc);
    Ctrl_DrawBox(dc, r, Visual::Window, Ctrl_GetState(dc, id));
    {
        NeGuiScopedChild(dc, Rect_Margin(r, { 4.0f, 2.0f }));
        SettingsTab_DoGrid(ctrl, Id_Cat(id, "Grid"), user);
    }
}

void NE_CALLBK StatisticsTab_Do(DockCtrl_t ctrl, ptr_t user)
{
    Doc_t doc = App_GetDoc();
    Context_t dc = DockCtrl_GetDc(ctrl);
    const Id_t id = DockCtrl_GetId(ctrl);
    const Rect_s r = Context_GetChild(dc);
    Ctrl_DrawBox(dc, r, Visual::Window, Ctrl_GetState(dc, id));

    Database_t db = doc->Db;

	const cstr_t labels[] = 
	{ "Size"
	, "Cpus"
	, "Threads"
	, "Frames"
	, "Scopes"
	, "Locks"
	, "Locations"
	};

	const PerfStat_s items[] = 
	{ { (uint32_t)	Database_GetSize		( db ), (uint32_t)Database_GetCapacity( db ), PerfStat::Bytes   }
	, {	(uint32_t)	Database_GetNumCpus		( db ), 0									, PerfStat::Counter	}
	, {	(uint32_t)	Database_GetNumThreads	( db ), 0									, PerfStat::Counter }
	, {	(uint32_t)	Database_GetNumFrames	( db ), 0									, PerfStat::Counter }
	, {	(uint32_t)	Database_GetNumScopes	( db ), 0									, PerfStat::Counter }
	, {	(uint32_t)	Database_GetNumLocks	( db ), 0									, PerfStat::Counter }
	, {	(uint32_t)	Database_GetNumLocations( db ), 0									, PerfStat::Counter }
	}; 

	NeStaticAssert( NeCountOf(labels) == NeCountOf(items) );
	PerfStatList_DoView( dc, r, labels, items, NeCountOf(items) );
}
