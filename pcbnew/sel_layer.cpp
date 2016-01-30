/* Set up the basic primitives for Layer control */

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "pcbnew.h"

#include "protos.h"


/* Variables locales */

/* Fonctions locales: */

enum layer_sel_id {
    ID_LAYER_SELECT_TOP = 1800,
    ID_LAYER_SELECT_BOTTOM,
    ID_LAYER_SELECT
};


/***********************************************/
/* classe pour la frame de selection de layers */
/***********************************************/

class WinEDA_SelLayerFrame : public wxDialog
{
private:
    WinEDA_BasePcbFrame* m_Parent;
    wxRadioBox*          m_LayerList;
    int m_LayerId[NB_LAYERS + 1]; // One extra element for "(Deselect)" radiobutton

public:

    // Constructor and destructor
    WinEDA_SelLayerFrame( WinEDA_BasePcbFrame* parent, int default_layer,
                          int min_layer, int max_layer, bool null_layer );
    ~WinEDA_SelLayerFrame() { };

private:
    void    Sel_Layer( wxCommandEvent& event );
	void    OnCancelClick( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};

/* Table des evenements pour WinEDA_SelLayerFrame */
BEGIN_EVENT_TABLE( WinEDA_SelLayerFrame, wxDialog )
	EVT_BUTTON( wxID_OK, WinEDA_SelLayerFrame::Sel_Layer )
	EVT_BUTTON( wxID_CANCEL, WinEDA_SelLayerFrame::OnCancelClick )
    EVT_RADIOBOX( ID_LAYER_SELECT, WinEDA_SelLayerFrame::Sel_Layer )
END_EVENT_TABLE()


/****************************************************************************************/
int WinEDA_BasePcbFrame::SelectLayer( int default_layer, int min_layer, int max_layer,
                                      bool null_layer )
/****************************************************************************************/

/** Install the dialog box for layer selection
 * @param default_layer = Preselection (NB_LAYERS for "(Deselect)" layer)
 * @param min_layer = min layer value (-1 if no min value)
 * @param max_layer = max layer value (-1 if no max value)
 * @param null_layer = display a "(Deselect)" radiobutton (when set to true)
 * @return new layer value (NB_LAYERS when "(Deselect)" radiobutton selected),
 *                         or -1 if cancelled
 *
 * Providing the option to also display a "(Deselect)" radiobutton makes the
 * "Swap Layers" command (and GerbView's "Export to Pcbnew" command) more "user
 * friendly", by permitting any layer to be "deselected" immediately after its
 * corresponding radiobutton has been clicked on. (It would otherwise be
 * necessary to first cancel the "Select Layer:" dialog box (invoked after a
 * different radiobutton is clicked on) prior to then clicking on the "Deselect"
 * button provided within the "Swap Layers:" or "Layer selection:" dialog box).
 */
{
    int layer;
    WinEDA_SelLayerFrame* frame =
        new WinEDA_SelLayerFrame( this, default_layer, min_layer, max_layer, null_layer );

    layer = frame->ShowModal();
    frame->Destroy();
    return layer;
}


/***********************************************************************/
WinEDA_SelLayerFrame::WinEDA_SelLayerFrame( WinEDA_BasePcbFrame* parent,
                                            int default_layer, int min_layer,
                                            int max_layer, bool null_layer ) :
    wxDialog( parent, -1, _("Select Layer:"), wxPoint( -1, -1 ),
              wxSize( 470, 250 ),
              DIALOG_STYLE )
/***********************************************************************/

/*
 * The "OK" and "Cancel" buttons are positioned (in a horizontal line)
 * beneath the "Layer" radiobox, unless that contains only one column of
 * radiobuttons, in which case they are positioned (in a vertical line)
 * to the right of that radiobox.
 */
{
    wxButton* Button;
    int       ii;
    wxString  LayerList[NB_LAYERS + 1]; // One extra element for "(Deselect)" radiobutton
    int       LayerCount, LayerSelect = -1;
    m_Parent = parent;
    SetFont( *g_DialogFont );

    /* Build the layer list */
    LayerCount = 0;
    int Masque_Layer = g_TabAllCopperLayerMask[g_DesignSettings.m_CopperLayerCount - 1];
    Masque_Layer += ALL_NO_CU_LAYERS;
    for( ii = 0; ii < NB_LAYERS; ii++ )
    {
        m_LayerId[ii] = 0;
        if( (g_TabOneLayerMask[ii] & Masque_Layer) )
        {
            if( min_layer > ii )
                continue;

            if( (max_layer >= 0) && (max_layer < ii) )
                break;

            LayerList[LayerCount] = ReturnPcbLayerName( ii );
            if( ii == default_layer )
                LayerSelect = LayerCount;

            m_LayerId[LayerCount] = ii;
            LayerCount++;
        }
    }
    // When appropriate, also provide a "(Deselect)" radiobutton
    if( null_layer )
    {
        LayerList[LayerCount] = _( "(Deselect)" );
        if( NB_LAYERS == default_layer )
            LayerSelect = LayerCount;

        m_LayerId[LayerCount] = NB_LAYERS;
        LayerCount++;
    }

    m_LayerList = new wxRadioBox( this, ID_LAYER_SELECT, _("Layer"),
                                  wxPoint( -1, -1 ), wxSize( -1, -1 ), LayerCount, LayerList,
                                  (LayerCount < 8) ? LayerCount : 8, wxRA_SPECIFY_ROWS );

    if( LayerSelect >= 0 )
        m_LayerList->SetSelection( LayerSelect );

    wxBoxSizer* FrameBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(FrameBoxSizer);
    FrameBoxSizer->Add(m_LayerList, 0, wxALIGN_TOP|wxALL, 5);
    wxBoxSizer* ButtonBoxSizer = new wxBoxSizer(wxVERTICAL);
    FrameBoxSizer->Add(ButtonBoxSizer, 0, wxALIGN_BOTTOM|wxALL, 0);

    Button = new wxButton( this, wxID_OK, _("OK") );
    Button->SetForegroundColour( *wxRED );
    ButtonBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

    Button = new wxButton( this, wxID_CANCEL, _("Cancel") );
    Button->SetForegroundColour( *wxBLUE );
    ButtonBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

    if( GetSizer() )
    {
        GetSizer()->SetSizeHints(this);
    }
}


/***************************************************************/
void WinEDA_SelLayerFrame::Sel_Layer( wxCommandEvent& event )
/***************************************************************/
{
    int ii = m_LayerId[m_LayerList->GetSelection()];

    EndModal( ii );
}


/***************************************************************/
void WinEDA_SelLayerFrame::OnCancelClick( wxCommandEvent& event )
/***************************************************************/
{
    EndModal( -1 );
}


/*********************************************************/
/* classe pour la frame de selection de paires de layers */
/*********************************************************/

class WinEDA_SelLayerPairFrame : public wxDialog
{
private:
    WinEDA_BasePcbFrame* m_Parent;
    wxRadioBox*          m_LayerListTOP;
    wxRadioBox*          m_LayerListBOTTOM;
    int m_LayerId[NB_COPPER_LAYERS];

public:

    // Constructor and destructor
    WinEDA_SelLayerPairFrame( WinEDA_BasePcbFrame* parent );
    ~WinEDA_SelLayerPairFrame() { };

private:
	void    OnOkClick( wxCommandEvent& event );
	void    OnCancelClick( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};


/* Table des evenements pour WinEDA_SelLayerPairFrame */
BEGIN_EVENT_TABLE( WinEDA_SelLayerPairFrame, wxDialog )
	EVT_BUTTON( wxID_OK, WinEDA_SelLayerPairFrame::OnOkClick )
	EVT_BUTTON( wxID_CANCEL, WinEDA_SelLayerPairFrame::OnCancelClick )
END_EVENT_TABLE()


/***********************************************/
void WinEDA_BasePcbFrame::SelectLayerPair()
/***********************************************/

/* Affiche une double liste de layers cuivre pour selection d'une paire de layers
 *  pour autorutage, vias...
 */
{
    // Check whether more than one copper layer has been enabled for the
    // current PCB file, as Layer Pairs can only meaningfully be defined
    // within PCB files which contain at least two copper layers.
    if( m_Pcb->m_BoardSettings->m_CopperLayerCount < 2 )
    {
        wxString InfoMsg;
        InfoMsg = _( "Less than two copper layers are being used." );
        InfoMsg << wxT( "\n" ) << _( "Hence Layer Pairs cannot be specified." );
        DisplayInfo( this, InfoMsg );
        return;
    }

    WinEDA_SelLayerPairFrame* frame =
        new WinEDA_SelLayerPairFrame( this );

    int result = frame->ShowModal();
    frame->Destroy();
    DrawPanel->MouseToCursorSchema();
    SetToolbars();

    // if user changed colors and we are in high contrast mode, then redraw 
    // because the SMD pads may change color.     
    if( result >= 0  &&  DisplayOpt.ContrastModeDisplay )
    {
        ReDrawPanel();
    }
}


/*******************************************************************************/
WinEDA_SelLayerPairFrame::WinEDA_SelLayerPairFrame( WinEDA_BasePcbFrame* parent ) :
    wxDialog( parent, -1, _("Select Layer Pair:"), wxPoint( -1, -1 ),
              wxSize( 470, 250 ), DIALOG_STYLE )
/*******************************************************************************/
{
    wxButton* Button;
    int       ii, LayerCount;
    wxString  LayerList[NB_COPPER_LAYERS];
    int       LayerTopSelect = 0, LayerBottomSelect = 0;

    m_Parent = parent;
    SetFont( *g_DialogFont );

    PCB_SCREEN* screen = (PCB_SCREEN*) m_Parent->m_CurrentScreen;
    /* Construction de la liste des couches autoris�s */
    int         Masque_Layer = g_TabAllCopperLayerMask[g_DesignSettings.m_CopperLayerCount - 1];
    Masque_Layer += ALL_NO_CU_LAYERS;
    for( ii = 0, LayerCount = 0; ii < NB_COPPER_LAYERS; ii++ )
    {
        m_LayerId[ii] = 0;
        if( (g_TabOneLayerMask[ii] & Masque_Layer) )
        {
            LayerList[LayerCount] = ReturnPcbLayerName( ii );
            if( ii == screen->m_Route_Layer_TOP )
                LayerTopSelect = LayerCount;
            if( ii == screen->m_Route_Layer_BOTTOM )
                LayerBottomSelect = LayerCount;
            m_LayerId[LayerCount] = ii;
            LayerCount++;
        }
    }

    m_LayerListTOP = new wxRadioBox( this, ID_LAYER_SELECT_TOP, _("Top Layer"),
                                     wxPoint( -1, -1 ), wxSize( -1, -1 ), LayerCount, LayerList,
                                     (LayerCount < 8) ? LayerCount : 8, wxRA_SPECIFY_ROWS );
    m_LayerListTOP->SetSelection( LayerTopSelect );

    m_LayerListBOTTOM = new wxRadioBox( this, ID_LAYER_SELECT_BOTTOM, _("Bottom Layer"),
                                        wxPoint( -1, -1 ), wxSize( -1, -1 ), LayerCount, LayerList,
                                        (LayerCount < 8) ? LayerCount : 8, wxRA_SPECIFY_ROWS );
    m_LayerListBOTTOM->SetSelection( LayerBottomSelect );

    wxBoxSizer* FrameBoxSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(FrameBoxSizer);

    wxBoxSizer* RadioBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    FrameBoxSizer->Add(RadioBoxSizer, 0, wxALIGN_LEFT|wxALL, 0);

    wxBoxSizer* ButtonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    FrameBoxSizer->Add(ButtonBoxSizer, 0, wxALIGN_RIGHT|wxALL, 0);

    RadioBoxSizer->Add(m_LayerListTOP, 0, wxALIGN_TOP|wxALL, 5);
    RadioBoxSizer->Add(m_LayerListBOTTOM, 0, wxALIGN_TOP|wxALL, 5);

    Button = new wxButton( this, wxID_OK, _("OK") );
    Button->SetForegroundColour( *wxRED );
    ButtonBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

    Button = new wxButton( this, wxID_CANCEL, _("Cancel") );
    Button->SetForegroundColour( *wxBLUE );
    ButtonBoxSizer->Add(Button, 0, wxGROW|wxALL, 5);

    if( GetSizer() )
    {
        GetSizer()->SetSizeHints(this);
    }
}


/***************************************************************/
void WinEDA_SelLayerPairFrame::OnOkClick( wxCommandEvent& event )
/***************************************************************/
{
    // Check whether whichever layer has been specified as the "Top"
    // layer is really "above" whichever layer has been specified as
    // the "Bottom" layer; those values will only be updated (and the
    // dialog box subsequently closed) when that condition is met.
//  if( m_LayerId[m_LayerListTOP->GetSelection()]
//          <= m_LayerId[m_LayerListBOTTOM->GetSelection()] )
//  {
//      DisplayError( this, _( "The Top Layer must be above the Bottom Layer" ) );
//      return;
//  }

    // Code for previous test commented out because other code now
    // transposes the assignment of the Top and Bottom layers if
    // the former layer is actually "below" the latter. However,
    // it is still desirable to check that those layers differ.

    // Check whether whichever layer has been specified as the "Top"
    // layer differs from whichever layer has been specified as the
    // "Bottom" layer; those values will only be updated (and the
    // dialog box subsequently closed) when that condition is met.
    if( m_LayerId[m_LayerListTOP->GetSelection()]
            == m_LayerId[m_LayerListBOTTOM->GetSelection()] )
    {
        DisplayError( this, _( "The Top Layer and Bottom Layer must differ" ) );
        return;
    }

    PCB_SCREEN* screen = (PCB_SCREEN*) m_Parent->m_CurrentScreen;

    screen->m_Route_Layer_TOP    = m_LayerId[m_LayerListTOP->GetSelection()];
    screen->m_Route_Layer_BOTTOM = m_LayerId[m_LayerListBOTTOM->GetSelection()];

    EndModal( 0 );
}


/***************************************************************/
void WinEDA_SelLayerPairFrame::OnCancelClick( wxCommandEvent& event )
/***************************************************************/
{
    EndModal( -1 );
}
