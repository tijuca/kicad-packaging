/////////////////////////////////////////////////////////////////////////////
// Name:        3d_frame.cpp
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif


#include "fctsys.h"

#include <wx/colordlg.h>

#if !wxUSE_GLCANVAS
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

#include "bitmaps.h"
#include "id.h"

#define VIEWER_MAIN
#include "3d_viewer.h"
#include "trackball.h"


BEGIN_EVENT_TABLE(WinEDA3D_DrawFrame, wxFrame)
	EVT_TOOL_RANGE(ID_ZOOM_PLUS_BUTT, ID_ZOOM_PAGE_BUTT,
			WinEDA3D_DrawFrame::Process_Zoom)
	EVT_TOOL_RANGE(ID_START_COMMAND_3D, ID_END_COMMAND_3D,
			WinEDA3D_DrawFrame::Process_Special_Functions)
    EVT_MENU(wxID_EXIT, WinEDA3D_DrawFrame::Exit3DFrame)
	EVT_MENU(ID_MENU_SCREENCOPY_PNG, WinEDA3D_DrawFrame::Process_Special_Functions)
	EVT_MENU(ID_MENU_SCREENCOPY_JPEG, WinEDA3D_DrawFrame::Process_Special_Functions)
    EVT_CLOSE(WinEDA3D_DrawFrame::OnCloseWindow)
END_EVENT_TABLE()

/*******************************************************************/
WinEDA3D_DrawFrame::WinEDA3D_DrawFrame(WinEDA_BasePcbFrame * parent,
		WinEDA_App *app_parent, const wxString& title ):
	wxFrame(parent, DISPLAY3D_FRAME, title,
		wxPoint(-1,-1), wxSize(-1,-1) )
/*******************************************************************/
{
	m_FrameName = wxT("Frame3D");
    m_Canvas = NULL;
	m_Parent = parent;
	m_ParentAppl = app_parent;
	m_HToolBar = NULL;
	m_VToolBar = NULL;
	m_InternalUnits = 10000;	// Unites internes = 1/10000 inch

	// Give it an icon
	SetIcon(wxICON(icon_w3d));

	GetSettings();
	SetSize(m_FramePos.x, m_FramePos.y, m_FrameSize.x, m_FrameSize.y);
	// Create the status line
int dims[5] = { -1, 100, 100, 100, 140};
	CreateStatusBar(5);
	SetStatusWidths(5,dims);
	ReCreateMenuBar();
	ReCreateHToolbar();
//	ReCreateAuxiliaryToolbar();
	ReCreateVToolbar();

	// Make a Pcb3D_GLCanvas

	m_Canvas = new Pcb3D_GLCanvas(this, -1, gl_attrib );
    /* init OpenGL once */
    m_Canvas->InitGL();
}



/***********************************************************/
void WinEDA3D_DrawFrame::Exit3DFrame(wxCommandEvent& event)
/***********************************************************/
{
	Close(TRUE);
}

/***********************************************************/
void WinEDA3D_DrawFrame::OnCloseWindow(wxCloseEvent & Event)
/***********************************************************/
{
	SaveSettings();
	if ( m_Parent )
	{
		m_Parent->m_Draw3DFrame = NULL;
	}
    Destroy();
}


/******************************************/
void WinEDA3D_DrawFrame::GetSettings(void)
/******************************************/
{
wxString text;
wxConfig * Config = m_ParentAppl->m_EDA_Config;		//  Current config used by application

	if( m_ParentAppl->m_EDA_Config )
	{
		text = m_FrameName + wxT("Pos_x");
		Config->Read(text, &m_FramePos.x);
		text = m_FrameName + wxT("Pos_y");
		Config->Read(text, &m_FramePos.y);
		text = m_FrameName + wxT("Size_x");
		Config->Read(text, &m_FrameSize.x, 600);
		text = m_FrameName + wxT("Size_y");
		Config->Read(text, &m_FrameSize.y, 400);
		Config->Read(wxT("BgColor_Red"), &g_Parm_3D_Visu.m_BgColor.m_Red, 0.0);
		Config->Read(wxT("BgColor_Green"), &g_Parm_3D_Visu.m_BgColor.m_Green, 0.0);
		Config->Read(wxT("BgColor_Blue"), &g_Parm_3D_Visu.m_BgColor.m_Blue, 0.0);
	}
#ifdef __WXMAC__
	// for macOSX, the window must be below system (macOSX) toolbar
	if ( m_FramePos.y < GetMBarHeight() ) m_FramePos.y = GetMBarHeight();
#endif
}

/*******************************************/
void WinEDA3D_DrawFrame::SaveSettings(void)
/*******************************************/
{
wxString text;
wxConfig * Config = m_ParentAppl->m_EDA_Config;		//  Current config used by application

	if( ! Config ) return;

	Config->Write(wxT("BgColor_Red"), g_Parm_3D_Visu.m_BgColor.m_Red);
	Config->Write(wxT("BgColor_Green"), g_Parm_3D_Visu.m_BgColor.m_Green);
	Config->Write(wxT("BgColor_Blue"), g_Parm_3D_Visu.m_BgColor.m_Blue);

	if( IsIconized() ) return;

	m_FrameSize = GetSize();
	m_FramePos = GetPosition();

	text = m_FrameName + wxT("Pos_x");
	Config->Write(text, (long)m_FramePos.x);
	text = m_FrameName + wxT("Pos_y");
	Config->Write(text, (long)m_FramePos.y);
	text = m_FrameName + wxT("Size_x");
	Config->Write(text, (long)m_FrameSize.x);
	text = m_FrameName + wxT("Size_y");
	Config->Write(text, (long)m_FrameSize.y);
}


/***********************************************************/
void WinEDA3D_DrawFrame::Process_Zoom(wxCommandEvent& event)
/***********************************************************/
{
int ii;
    switch(event.GetId())
	{
		case ID_ZOOM_PAGE_BUTT:
		for ( ii = 0; ii < 4; ii++ ) g_Parm_3D_Visu.m_Rot[ii] = 0.0;
		g_Parm_3D_Visu.m_Zoom = 1.0;
		g_Draw3d_dx = g_Draw3d_dy = 0;
		trackball(g_Parm_3D_Visu.m_Quat, 0.0, 0.0, 0.0, 0.0 );
		break;

		case ID_ZOOM_PLUS_BUTT:
			g_Parm_3D_Visu.m_Zoom /= 1.2;
			if ( g_Parm_3D_Visu.m_Zoom <= 0.01)
				g_Parm_3D_Visu.m_Zoom = 0.01;
			break;

		case ID_ZOOM_MOINS_BUTT:
			g_Parm_3D_Visu.m_Zoom *= 1.2;
			break;

		case ID_ZOOM_REDRAW_BUTT:
			break;

		default:
			return;
    }
	m_Canvas->DisplayStatus();
    m_Canvas->Refresh(FALSE);
}



/************************************************************************/
void WinEDA3D_DrawFrame::OnLeftClick(wxDC * DC, const wxPoint& MousePos)
/************************************************************************/
{
}


/*******************************************************************************/
void WinEDA3D_DrawFrame::OnRightClick(const wxPoint& MousePos, wxMenu * PopMenu)
/*******************************************************************************/
{
}

/************************************/
int WinEDA3D_DrawFrame::BestZoom(void)
/************************************/
// Retourne le meilleur zoom
{
	return 1;
}


/*******************************************************************/
void WinEDA3D_DrawFrame::RedrawActiveWindow(wxDC * DC, bool EraseBg)
/*******************************************************************/
{
}


/************************************************************************/
void WinEDA3D_DrawFrame::Process_Special_Functions(wxCommandEvent& event)
/************************************************************************/
{
#define ROT_ANGLE 10.0
double delta_move = 0.7 * g_Parm_3D_Visu.m_Zoom;

    switch(event.GetId())
	{
		case ID_RELOAD3D_BOARD:
			NewDisplay();
			break;

		case ID_ROTATE3D_X_POS:
			g_Parm_3D_Visu.m_ROTX += ROT_ANGLE;
			break;

		case ID_ROTATE3D_X_NEG:
			g_Parm_3D_Visu.m_ROTX -= ROT_ANGLE;
			break;

		case ID_ROTATE3D_Y_POS:
			g_Parm_3D_Visu.m_ROTY += ROT_ANGLE;
			break;

		case ID_ROTATE3D_Y_NEG:
			g_Parm_3D_Visu.m_ROTY -= ROT_ANGLE;
			break;

		case ID_ROTATE3D_Z_POS:
			g_Parm_3D_Visu.m_ROTZ += ROT_ANGLE;
			break;

		case ID_ROTATE3D_Z_NEG:
			g_Parm_3D_Visu.m_ROTZ -= ROT_ANGLE;
			break;

		case ID_MOVE3D_LEFT:
			m_Canvas->SetView3D(WXK_LEFT);
			return;
		
		case ID_MOVE3D_RIGHT:
			m_Canvas->SetView3D(WXK_RIGHT);
			return;
		
		case ID_MOVE3D_UP:
			m_Canvas->SetView3D(WXK_UP);
			return;
		
		case ID_MOVE3D_DOWN:
			m_Canvas->SetView3D(WXK_DOWN);
			return;

		case ID_TOOL_SCREENCOPY_TOCLIBBOARD:
		case ID_MENU_SCREENCOPY_PNG:
		case ID_MENU_SCREENCOPY_JPEG:
			m_Canvas->TakeScreenshot(event);
			break;

		case ID_MENU3D_BGCOLOR_SELECTION:
			Set3DBgColor();
			return;

		default:
			wxMessageBox(
				wxT("WinEDA3D_DrawFrame::Process_Special_Functions() error: unknown command"));
			return;
    }
	m_Canvas->DisplayStatus();
    m_Canvas->Refresh(FALSE);
}


/*****************************************/
void WinEDA3D_DrawFrame::NewDisplay(void)
/*****************************************/
{
	m_Canvas->ClearLists();
    m_Canvas->InitGL();
	m_Canvas->DisplayStatus();
    m_Canvas->Refresh(FALSE);
}


/******************************************/
void WinEDA3D_DrawFrame::Set3DBgColor(void)
/******************************************/
/* called to set the background color of the 3D scene
*/
{
S3D_Color color;
wxColour newcolor, oldcolor;

	oldcolor.Set((int) round(g_Parm_3D_Visu.m_BgColor.m_Red*255),
		(int) round(g_Parm_3D_Visu.m_BgColor.m_Green*255),
	(int) round(g_Parm_3D_Visu.m_BgColor.m_Blue*255));
	
	newcolor = wxGetColourFromUser(this, oldcolor);
	if ( newcolor != oldcolor )
	{
		g_Parm_3D_Visu.m_BgColor.m_Red = (double) newcolor.Red() / 255.0;
		g_Parm_3D_Visu.m_BgColor.m_Green = (double) newcolor.Green() / 255.0;
		g_Parm_3D_Visu.m_BgColor.m_Blue = (double) newcolor.Blue() / 255.0;
		NewDisplay();
	}
}
