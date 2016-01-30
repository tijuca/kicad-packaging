/////////////////////////////////////////////////////////////////////////////
// Name:        dialog_edit_module_text.cpp
// Author:      jean-pierre Charras
// Licence:		GPL
/////////////////////////////////////////////////////////////////////////////


#include "fctsys.h"
#include "macros.h"
#include "common.h"
#include "class_drawpanel.h"
#include "pcbnew.h"

#include "dialog_edit_module_text_base.h"

extern wxPoint MoveVector;  // Move vector for move edge, imported from edtxtmod.cpp

/*************** **************/
/* class DialogEditModuleText */
/*************** **************/
class DialogEditModuleText : public DialogEditModuleText_base
{
private:
    WinEDA_BasePcbFrame* m_Parent;
    wxDC* m_DC;
    MODULE* m_Module;
    TEXTE_MODULE* m_CurrentTextMod;

public:
    DialogEditModuleText( WinEDA_BasePcbFrame* parent, TEXTE_MODULE* TextMod, wxDC* DC );
    ~DialogEditModuleText() {};

private:
    void OnInitDialog( wxInitDialogEvent& event );
    void OnOkClick( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
};

/***************************************************************************/
void WinEDA_BasePcbFrame::InstallTextModOptionsFrame( TEXTE_MODULE* TextMod,
                                                      wxDC* DC, const wxPoint& pos )
/***************************************************************************/
{
    DrawPanel->m_IgnoreMouseEvents = TRUE;
    DialogEditModuleText* frame = new DialogEditModuleText( this,
        TextMod, DC );
    frame->ShowModal(); frame->Destroy();
    DrawPanel->MouseToCursorSchema();
    DrawPanel->m_IgnoreMouseEvents = FALSE;
}


DialogEditModuleText::DialogEditModuleText( WinEDA_BasePcbFrame* parent,
                                                              TEXTE_MODULE* TextMod,
                                                              wxDC* DC ) :
    DialogEditModuleText_base(parent)

{
    m_Parent = parent;
    m_DC     = DC;
    m_Module = NULL;
    m_CurrentTextMod = TextMod;
    if( m_CurrentTextMod )
    {
        m_Module = (MODULE*) m_CurrentTextMod->GetParent();
    }
}


void DialogEditModuleText::OnCancelClick( wxCommandEvent& event )
{
    event.Skip();
}


/********************************************************/
void DialogEditModuleText::OnInitDialog( wxInitDialogEvent& event )
/********************************************************/
{
    SetFont( *g_DialogFont );
    SetFocus();

    wxString msg;

    if( m_Module )
    {
        wxString format = m_ModuleInfoText->GetLabel();
        msg.Printf( format,
            m_Module->m_Reference->m_Text.GetData(),
            m_Module->m_Value->m_Text.GetData(),
            (float) (m_Module->m_Orient / 10) );
        m_ModuleInfoText->SetLabel( msg );
    }

    if( m_CurrentTextMod->m_Type == TEXT_is_VALUE )
        m_TextDataTitle->SetLabel( _( "Value:" ) );
    else if( m_CurrentTextMod->m_Type == TEXT_is_DIVERS )
        m_TextDataTitle->SetLabel( _( "Text:" ) );
    else if( m_CurrentTextMod->m_Type != TEXT_is_REFERENCE )
        m_TextDataTitle->SetLabel( wxT( "???" ) );

    m_Name->SetValue( m_CurrentTextMod->m_Text );

    m_Style->SetSelection( m_CurrentTextMod->m_Italic ? 1 : 0 );

    AddUnitSymbol( *m_SizeXTitle );
    PutValueInLocalUnits( *m_TxtSizeCtrlX, m_CurrentTextMod->m_Size.x,
        m_Parent->m_InternalUnits );

    AddUnitSymbol( *m_SizeYTitle );
    PutValueInLocalUnits( *m_TxtSizeCtrlY, m_CurrentTextMod->m_Size.y,
        m_Parent->m_InternalUnits );

    AddUnitSymbol( *m_PosXTitle );
    PutValueInLocalUnits( *m_TxtPosCtrlX, m_CurrentTextMod->m_Pos0.x,
        m_Parent->m_InternalUnits );

    AddUnitSymbol( *m_PosYTitle );
    PutValueInLocalUnits( *m_TxtPosCtrlY, m_CurrentTextMod->m_Pos0.y,
        m_Parent->m_InternalUnits );

    AddUnitSymbol( *m_WidthTitle );
    PutValueInLocalUnits( *m_TxtWidthCtlr, m_CurrentTextMod->m_Width,
        m_Parent->m_InternalUnits );

    int text_orient = m_CurrentTextMod->m_Orient;
    NORMALIZE_ANGLE_90(text_orient)
    if( (text_orient != 0) )
        m_Orient->SetSelection( 1 );

    if( m_CurrentTextMod->m_NoShow )
        m_Show->SetSelection( 1 );;

    GetSizer()->Fit( this );
    GetSizer()->SetSizeHints( this );
}


/*********************************************************************************/
void DialogEditModuleText::OnOkClick( wxCommandEvent& event )
/*********************************************************************************/
{
    wxString msg;

    m_Parent->SaveCopyInUndoList( m_Parent->GetBoard()->m_Modules );
    if( m_DC )     //Erase old text on screen
    {
        m_CurrentTextMod->Draw( m_Parent->DrawPanel, m_DC, GR_XOR,
            (m_CurrentTextMod->m_Flags & IS_MOVED) ? MoveVector : wxPoint( 0, 0 ) );
    }
    m_CurrentTextMod->m_Text = m_Name->GetValue();

    m_CurrentTextMod->m_Italic = m_Style->GetSelection() == 1 ? true : false;


    msg = m_TxtPosCtrlX->GetValue();
    m_CurrentTextMod->m_Pos0.x = ReturnValueFromString( g_UnitMetric, msg,
        m_Parent->m_InternalUnits );
    msg = m_TxtPosCtrlY->GetValue();
    m_CurrentTextMod->m_Pos0.y = ReturnValueFromString( g_UnitMetric, msg,
        m_Parent->m_InternalUnits );

    msg = m_TxtSizeCtrlX->GetValue();
    m_CurrentTextMod->m_Size.x = ReturnValueFromString( g_UnitMetric, msg,
        m_Parent->m_InternalUnits );
    msg = m_TxtSizeCtrlY->GetValue();
    m_CurrentTextMod->m_Size.y = ReturnValueFromString( g_UnitMetric, msg,
        m_Parent->m_InternalUnits );

    // Test for a reasonnable size:
    if( m_CurrentTextMod->m_Size.x< TEXTS_MIN_SIZE )
        m_CurrentTextMod->m_Size.x = TEXTS_MIN_SIZE;
    if( m_CurrentTextMod->m_Size.y< TEXTS_MIN_SIZE )
        m_CurrentTextMod->m_Size.y = TEXTS_MIN_SIZE;

    msg = m_TxtWidthCtlr->GetValue();
    int width = ReturnValueFromString( g_UnitMetric, msg, m_Parent->m_InternalUnits );

    // Test for a reasonnable width:
    if( width <= 1 )
        width = 1;
    int minthickness = min(m_CurrentTextMod->m_Size.x, m_CurrentTextMod->m_Size.y) / 4;
    if( width > minthickness )
        width = minthickness;
    m_CurrentTextMod->SetWidth( width );

    m_CurrentTextMod->m_NoShow = (m_Show->GetSelection() == 0) ? 0 : 1;
    int text_orient = (m_Orient->GetSelection() == 0) ? 0 : 900;
    m_CurrentTextMod->m_Orient = text_orient;

    m_CurrentTextMod->SetDrawCoord();
    if( m_DC )     // Display new text
    {
        m_CurrentTextMod->Draw( m_Parent->DrawPanel, m_DC, GR_XOR,
            (m_CurrentTextMod->m_Flags & IS_MOVED) ? MoveVector : wxPoint( 0, 0 ) );
    }
    m_Parent->GetScreen()->SetModify();
    ( (MODULE*) m_CurrentTextMod->GetParent() )->m_LastEdit_Time = time( NULL );

    Close( TRUE );
}
