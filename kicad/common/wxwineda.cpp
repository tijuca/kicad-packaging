 /***************/
/* wxwineda.cpp */
/****************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "fctsys.h"
#include "common.h"
#include "wxstruct.h"


/*
 * Text entry dialog to enter one or more lines of text.
 */
WinEDA_EnterText::WinEDA_EnterText( wxWindow* parent,
                                    const wxString& Title,
                                    const wxString& TextToEdit,
                                    wxBoxSizer* BoxSizer,
                                    const wxSize& Size, bool Multiline )
{
    m_Modify = FALSE;
    if( ! TextToEdit.IsEmpty() )
        m_NewText = TextToEdit;

    m_Title = new wxStaticText( parent, -1, Title );

    BoxSizer->Add( m_Title, 0,
                   wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );

    long style = 0;

    if (Multiline)
      style = wxTE_MULTILINE;

    m_FrameText = new wxTextCtrl( parent, -1, TextToEdit, wxDefaultPosition,
                                  Size,style );

    m_FrameText->SetInsertionPoint( 1 );
    BoxSizer->Add( m_FrameText,
                   0,
                   wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM,
                   5 );
}


wxString WinEDA_EnterText::GetValue()
{
    m_Modify  = m_FrameText->IsModified();
    m_NewText = m_FrameText->GetValue();
    return m_NewText;
}


void WinEDA_EnterText::GetValue( char* buffer, int lenmax )
{
    m_Modify = m_FrameText->IsModified();
    if( buffer )
    {
        m_NewText = m_FrameText->GetValue();
        int ii, ll = m_NewText.Len();
        for( ii = 0; ii < ll && ii < (lenmax - 1); ii++ )
            ;

        buffer[ii] = m_NewText.GetChar( ii );
        buffer[lenmax - 1] = 0;
    }
}


void WinEDA_EnterText::SetValue( const wxString& new_text )
{
    m_FrameText->SetValue( new_text );
}


void WinEDA_EnterText::Enable( bool enbl )
{
    m_Title->Enable( enbl );
    m_FrameText->Enable( enbl );
}


/*******************************************************/
/* Class to edit a graphic + text size in INCHES or MM */
/*******************************************************/
WinEDA_GraphicTextCtrl::WinEDA_GraphicTextCtrl( wxWindow*       parent,
                                                const wxString& Title,
                                                const wxString& TextToEdit,
                                                int             textsize,
                                                int             units,
                                                wxBoxSizer*     BoxSizer,
                                                int             framelen,
                                                int             internal_unit )
{
    m_Units = units;
    m_Internal_Unit = internal_unit;
    m_Title = NULL;

    m_Title = new wxStaticText( parent, -1, Title );

    BoxSizer->Add( m_Title, 0,
                   wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );

    m_FrameText = new wxTextCtrl( parent, -1, TextToEdit );

    BoxSizer->Add( m_FrameText, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    if( !Title.IsEmpty() )
    {
        wxString      msg = _( "Size" ) + ReturnUnitSymbol( m_Units );
        wxStaticText* text = new wxStaticText( parent, -1, msg );

        BoxSizer->Add( text, 0,
                       wxGROW | wxLEFT | wxRIGHT | wxADJUST_MINSIZE, 5 );
    }

    wxString value = FormatSize( m_Internal_Unit, m_Units, textsize );

    m_FrameSize = new wxTextCtrl( parent, -1, value, wxDefaultPosition,
                                  wxSize( 70, -1 ) );

    BoxSizer->Add( m_FrameSize, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
}


WinEDA_GraphicTextCtrl::~WinEDA_GraphicTextCtrl()
{
    /* no, these are deleted by the BoxSizer
    delete m_FrameText;
    delete m_Title;
    */
}


wxString WinEDA_GraphicTextCtrl::FormatSize( int internalUnit, int units,
                                             int textSize )
{
    wxString value;

    // Limiting the size of the text of reasonable values.
    if( textSize < 10 )
        textSize = 10;

    if( textSize > 3000 )
        textSize = 3000;

    value.Printf( ( internalUnit > 1000 ) ? wxT( "%.4f" ) : wxT( "%.3f" ),
        To_User_Unit( units, textSize, internalUnit ) );

    return value;
}


void WinEDA_GraphicTextCtrl::SetTitle( const wxString& title )
{
    m_Title->SetLabel( title );
}


void WinEDA_GraphicTextCtrl::SetValue( const wxString& value )
{
    m_FrameText->SetValue( value );
}


void WinEDA_GraphicTextCtrl::SetValue( int textSize )
{
    wxString value = FormatSize( m_Internal_Unit, m_Units, textSize );
    m_FrameSize->SetValue( value );
}


wxString WinEDA_GraphicTextCtrl::GetText()
{
    wxString text = m_FrameText->GetValue();
    return text;
}


int WinEDA_GraphicTextCtrl::ParseSize( const wxString& sizeText,
                                       int internalUnit, int units )
{
    int    textsize;

    double dtmp;

    sizeText.ToDouble( &dtmp );

    textsize = (int) From_User_Unit( units, dtmp, internalUnit );

    // Limit to reasonable size
    if( textsize < 10 )
        textsize = 10;

    if( textsize > 3000 )
        textsize = 3000;

    return textsize;
}


int WinEDA_GraphicTextCtrl::GetTextSize()
{
    return ParseSize( m_FrameSize->GetValue(), m_Internal_Unit, m_Units );
}


void WinEDA_GraphicTextCtrl::Enable( bool state )
{
    m_FrameText->Enable( state );
}


/********************************************************/
/* Class to display and edit a coordinated INCHES or MM */
/********************************************************/
WinEDA_PositionCtrl::WinEDA_PositionCtrl( wxWindow*       parent,
                                          const wxString& title,
                                          const wxPoint&  pos_to_edit,
                                          int             units,
                                          wxBoxSizer*     BoxSizer,
                                          int             internal_unit )
{
    wxString text;

    m_Units = units;
    m_Internal_Unit = internal_unit;
    if( title.IsEmpty() )
        text = _( "Pos " );
    else
        text = title;
    text   += _( "X" ) + ReturnUnitSymbol( m_Units );
    m_TextX = new wxStaticText( parent, -1, text );

    BoxSizer->Add( m_TextX, 0,
                   wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );
    m_FramePosX = new wxTextCtrl( parent, -1, wxEmptyString,
                                  wxDefaultPosition );

    BoxSizer->Add( m_FramePosX, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );


    if( title.IsEmpty() )
        text = _( "Pos " );
    else
        text = title;
    text   += _( "Y" ) + ReturnUnitSymbol( m_Units );
    m_TextY = new wxStaticText( parent, -1, text );

    BoxSizer->Add( m_TextY, 0,
                   wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );

    m_FramePosY = new wxTextCtrl( parent, -1, wxEmptyString );

    BoxSizer->Add( m_FramePosY, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

    SetValue( pos_to_edit.x, pos_to_edit.y );
}


WinEDA_PositionCtrl::~WinEDA_PositionCtrl()
{
    delete m_TextX;
    delete m_TextY;
    delete m_FramePosX;
    delete m_FramePosY;
}


/* Returns (in internal units) to coordinate between (in user units)
 */
wxPoint WinEDA_PositionCtrl::GetValue()
{
    wxPoint coord;
    double  value = 0;

    m_FramePosX->GetValue().ToDouble( &value );
    coord.x = From_User_Unit( m_Units, value, m_Internal_Unit );
    m_FramePosY->GetValue().ToDouble( &value );
    coord.y = From_User_Unit( m_Units, value, m_Internal_Unit );

    return coord;
}


void WinEDA_PositionCtrl::Enable( bool x_win_on, bool y_win_on )
{
    m_FramePosX->Enable( x_win_on );
    m_FramePosY->Enable( y_win_on );
}


void WinEDA_PositionCtrl::SetValue( int x_value, int y_value )
{
    wxString msg;

    m_Pos_To_Edit.x = x_value;
    m_Pos_To_Edit.y = y_value;

    msg = ReturnStringFromValue( m_Units, m_Pos_To_Edit.x, m_Internal_Unit );
    m_FramePosX->Clear();
    m_FramePosX->SetValue( msg );

    msg = ReturnStringFromValue( m_Units, m_Pos_To_Edit.y, m_Internal_Unit );
    m_FramePosY->Clear();
    m_FramePosY->SetValue( msg );
}


/*******************/
/* WinEDA_SizeCtrl */
/*******************/
WinEDA_SizeCtrl::WinEDA_SizeCtrl( wxWindow* parent, const wxString& title,
                                  const wxSize& size_to_edit,
                                  int units, wxBoxSizer* BoxSizer,
                                  int internal_unit ) :
    WinEDA_PositionCtrl( parent, title,
                         wxPoint( size_to_edit.x, size_to_edit.y ),
                         units, BoxSizer, internal_unit )
{
}


wxSize WinEDA_SizeCtrl::GetValue()
{
    wxPoint pos = WinEDA_PositionCtrl::GetValue();
    wxSize  size;

    size.x = pos.x;
    size.y = pos.y;
    return size;
}


/**************************************************************/
/* Class to display and edit a dimension INCHES, MM, or other */
/**************************************************************/
WinEDA_ValueCtrl::WinEDA_ValueCtrl( wxWindow* parent, const wxString& title,
                                    int value, int units, wxBoxSizer* BoxSizer,
                                    int internal_unit )
{
    wxString label = title;

    m_Units = units;
    m_Internal_Unit = internal_unit;
    m_Value = value;
    label  += ReturnUnitSymbol( m_Units );

    m_Text = new wxStaticText( parent, -1, label );

    BoxSizer->Add( m_Text, 0,
                   wxGROW | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );

    wxString stringvalue = ReturnStringFromValue( m_Units, m_Value,
                                                  m_Internal_Unit );
    m_ValueCtrl = new   wxTextCtrl( parent, -1, stringvalue );

    BoxSizer->Add( m_ValueCtrl,
                   0,
                   wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM,
                   5 );
}


WinEDA_ValueCtrl::~WinEDA_ValueCtrl()
{
    delete m_ValueCtrl;
    delete m_Text;
}


int WinEDA_ValueCtrl::GetValue()
{
    int      coord;
    wxString txtvalue = m_ValueCtrl->GetValue();

    coord = ReturnValueFromString( m_Units, txtvalue, m_Internal_Unit );
    return coord;
}


void WinEDA_ValueCtrl::SetValue( int new_value )
{
    wxString buffer;

    m_Value = new_value;

    buffer = ReturnStringFromValue( m_Units, m_Value, m_Internal_Unit );
    m_ValueCtrl->SetValue( buffer );
}


void WinEDA_ValueCtrl::Enable( bool enbl )
{
    m_ValueCtrl->Enable( enbl );
    m_Text->Enable( enbl );
}


/**********************************************************************/
/* Class to display and edit a double precision floating point value. */
/**********************************************************************/
WinEDA_DFloatValueCtrl::WinEDA_DFloatValueCtrl( wxWindow* parent,
                                                const wxString& title,
                                                double value,
                                                wxBoxSizer* BoxSizer )
{
    wxString buffer;
    wxString label = title;

    m_Value = value;

    m_Text = new wxStaticText( parent, -1, label );

    BoxSizer->Add( m_Text, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5 );

    buffer.Printf( wxT( "%f" ), m_Value );
    m_ValueCtrl = new   wxTextCtrl( parent, -1, buffer );

    BoxSizer->Add( m_ValueCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
}


WinEDA_DFloatValueCtrl::~WinEDA_DFloatValueCtrl()
{
    delete m_ValueCtrl;
    delete m_Text;
}


double WinEDA_DFloatValueCtrl::GetValue()
{
    double coord = 0;

    m_ValueCtrl->GetValue().ToDouble( &coord );
    return coord;
}


void WinEDA_DFloatValueCtrl::SetValue( double new_value )
{
    wxString buffer;

    m_Value = new_value;

    buffer.Printf( wxT( "%f" ), m_Value );
    m_ValueCtrl->SetValue( buffer );
}


void WinEDA_DFloatValueCtrl::Enable( bool enbl )
{
    m_ValueCtrl->Enable( enbl );
    m_Text->Enable( enbl );
}