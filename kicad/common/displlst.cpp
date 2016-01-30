/****************/
/* displlst.cpp */
/****************/

#include "fctsys.h"
#include "wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "macros.h"
#include "kicad_string.h"


enum listbox {
    ID_LISTBOX_LIST = 8000
};


BEGIN_EVENT_TABLE( WinEDAListBox, wxDialog )
    EVT_BUTTON( wxID_OK, WinEDAListBox::OnOkClick )
    EVT_BUTTON( wxID_CANCEL, WinEDAListBox::OnCancelClick )
    EVT_LISTBOX( ID_LISTBOX_LIST, WinEDAListBox::ClickOnList )
    EVT_LISTBOX_DCLICK( ID_LISTBOX_LIST, WinEDAListBox::D_ClickOnList )
    EVT_CHAR( WinEDAListBox::OnKeyEvent )
    EVT_CHAR_HOOK( WinEDAListBox::OnKeyEvent )
    EVT_CLOSE( WinEDAListBox::OnClose )
END_EVENT_TABLE()


/* Used to display a list of elements for selection.
 * ITEMLIST* = pointer to the list of names
 * = Reftext preselection
 * = Movefct callback function to display comments
 */
WinEDAListBox::WinEDAListBox( WinEDA_DrawFrame* parent, const wxString& title,
                              const wxChar** itemlist, const wxString& reftext,
                              void(* movefct)(wxString& Text) ,
                              const wxColour& colour, wxPoint dialog_position ) :
    wxDialog( parent, -1, title, dialog_position, wxDefaultSize,
              wxDEFAULT_DIALOG_STYLE | MAYBE_RESIZE_BORDER )
{
    const wxChar** names;

    m_ItemList = itemlist;
    m_Parent   = parent;
    m_MoveFct  = movefct;
    m_WinMsg   = NULL;
    SetReturnCode( -1 );

    wxBoxSizer* GeneralBoxSizer = new wxBoxSizer( wxVERTICAL );

    SetSizer( GeneralBoxSizer );

    m_List = new wxListBox( this, ID_LISTBOX_LIST, wxDefaultPosition,
                            wxSize( 300, 200 ), 0, NULL,
                            wxLB_NEEDED_SB | wxLB_SINGLE | wxLB_HSCROLL );

    if( colour != wxNullColour )
    {
        m_List->SetBackgroundColour( colour );
        m_List->SetForegroundColour( *wxBLACK );
    }

    GeneralBoxSizer->Add( m_List, 0, wxGROW | wxALL, 5 );

    if( itemlist )
    {
        for( names = m_ItemList; *names != NULL; names++ )
            m_List->Append( *names );
    }

    if( m_MoveFct )
    {
        m_WinMsg = new wxTextCtrl( this, -1, wxEmptyString,
                                   wxDefaultPosition, wxSize( -1, 60 ),
                                   wxTE_READONLY | wxTE_MULTILINE );

        GeneralBoxSizer->Add( m_WinMsg, 0, wxGROW | wxALL, 5 );
    }

    wxSizer* buttonSizer = CreateButtonSizer( wxOK | wxCANCEL );

    if( buttonSizer )
        GeneralBoxSizer->Add( buttonSizer, 0, wxGROW | wxALL, 5 );

    GetSizer()->Fit( this );
    GetSizer()->SetSizeHints( this );

    if( dialog_position == wxDefaultPosition )
    {
        Centre();
    }
    else    // Ensure the window dialog is inside the main window :
    {
        wxPoint pos = dialog_position;
        wxPoint maxpos;
        maxpos.x = parent->GetPosition().x + parent->GetSize().x;
        maxpos.y = parent->GetPosition().y + parent->GetSize().y;
        wxPoint endpoint;
        endpoint.x = pos.x + GetSize().x;
        endpoint.y = pos.y + GetSize().y;

        if( endpoint.x > maxpos.x )
            pos.x -= endpoint.x - maxpos.x;
        if( endpoint.y > maxpos.y )
            pos.y -= endpoint.y - maxpos.y;

        if( pos.x < parent->GetPosition().x )
            pos.x = parent->GetPosition().x;
        if( pos.y < parent->GetPosition().y )
            pos.y = parent->GetPosition().y;
        Move( pos );
    }
}


WinEDAListBox::~WinEDAListBox()
{
}


void WinEDAListBox::MoveMouseToOrigin()
{
    int    x, y, w, h;
    wxSize list_size = m_List->GetSize();
    int    orgx = m_List->GetRect().GetLeft();
    int    orgy = m_List->GetRect().GetTop();

    wxClientDisplayRect( &x, &y, &w, &h );

    WarpPointer( x + orgx + 20, y + orgy + (list_size.y / 2) );
}


wxString WinEDAListBox::GetTextSelection()
{
    wxString text = m_List->GetStringSelection();

    return text;
}


void WinEDAListBox::Append( const wxString& item )
{
    m_List->Append( item );
}


void WinEDAListBox::InsertItems( const wxArrayString& itemlist, int position )
{
    m_List->InsertItems( itemlist, position );
}


void WinEDAListBox::OnCancelClick( wxCommandEvent& event )
{
    EndModal( -1 );
}


void WinEDAListBox::ClickOnList( wxCommandEvent& event )
{
    wxString text;

    if( m_MoveFct )
    {
        m_WinMsg->Clear();
        text = m_List->GetStringSelection();
        m_MoveFct( text );
        m_WinMsg->WriteText( text );
    }
}


void WinEDAListBox::D_ClickOnList( wxCommandEvent& event )
{
    int ii = m_List->GetSelection();

    EndModal( ii );
}


void WinEDAListBox::OnOkClick( wxCommandEvent& event )
{
    int ii = m_List->GetSelection();

    EndModal( ii );
}


void WinEDAListBox::OnClose( wxCloseEvent& event )
{
    EndModal( -1 );
}


/* Sort alphabetically, case insensitive.
 */
static int SortItems( const wxString** ptr1, const wxString** ptr2 )
{
    return StrNumICmp( (*ptr1)->GetData(), (*ptr2)->GetData() );
}


void WinEDAListBox:: SortList()
{
    int ii, NbItems = m_List->GetCount();
    const wxString** BufList;

    if( NbItems <= 0 )
        return;

    BufList = (const wxString**) MyZMalloc( 100 * NbItems * sizeof(wxString*) );
    for( ii = 0; ii < NbItems; ii++ )
    {
        BufList[ii] = new wxString( m_List->GetString (ii) );
    }

    qsort( BufList, NbItems, sizeof(wxString*),
           ( int( * ) ( const void*, const void* ) )SortItems );

    m_List->Clear();
    for( ii = 0; ii < NbItems; ii++ )
    {
        m_List->Append( *BufList[ii] );
        delete BufList[ii];
    }

    free( BufList );
}


void WinEDAListBox::OnKeyEvent( wxKeyEvent& event )
{
    event.Skip();
}
