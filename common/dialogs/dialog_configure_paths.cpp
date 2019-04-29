/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2015 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2015-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <dialog_configure_paths.h>

#include <confirm.h>
#include <menus_helpers.h>
#include <validators.h>
#include <html_messagebox.h>
#include <filename_resolver.h>
#include <env_vars.h>
#include <grid_tricks.h>
#include <widgets/wx_grid.h>
#include <widgets/grid_text_button_helpers.h>

enum ENV_VAR_GRID_COLUMNS
{
    EV_NAME_COL = 0,
    EV_PATH_COL,
    EV_FLAG_COL
};

enum SEARCH_PATH_GRID_COLUMNS
{
    SP_ALIAS_COL = 0,
    SP_PATH_COL,
    SP_DESC_COL
};


DIALOG_CONFIGURE_PATHS::DIALOG_CONFIGURE_PATHS( wxWindow* aParent, FILENAME_RESOLVER* aResolver ) :
    DIALOG_CONFIGURE_PATHS_BASE( aParent ),
    m_errorGrid( nullptr ), m_errorRow( -1 ), m_errorCol( -1 ),
    m_resolver( aResolver ),
    m_gridWidthsDirty( true )
{
    m_btnAddEnvVar->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_btnDeleteEnvVar->SetBitmap( KiBitmap( trash_xpm ) );
    m_btnAddSearchPath->SetBitmap( KiBitmap( small_plus_xpm ) );
    m_btnDeleteSearchPath->SetBitmap( KiBitmap( trash_xpm ) );
    m_btnMoveUp->SetBitmap( KiBitmap( small_up_xpm ) );
    m_btnMoveDown->SetBitmap( KiBitmap( small_down_xpm ) );

    m_EnvVars->DeleteRows( 0, m_EnvVars->GetNumberRows() );
    m_EnvVars->AppendCols( 1 );     // for the isExternal flags
    m_EnvVars->HideCol( EV_FLAG_COL );
    m_EnvVars->UseNativeColHeader( true );

    wxGridCellAttr* attr = new wxGridCellAttr;
    attr->SetEditor( new GRID_CELL_PATH_EDITOR( this, &m_curdir ) );
    m_EnvVars->SetColAttr( EV_PATH_COL, attr );

    attr = new wxGridCellAttr;
    attr->SetEditor( new GRID_CELL_PATH_EDITOR( this, &m_curdir ) );
    m_SearchPaths->SetColAttr( EV_PATH_COL, attr );

    // Give a bit more room for combobox editors
    m_EnvVars->SetDefaultRowSize( m_EnvVars->GetDefaultRowSize() + 4 );
    m_SearchPaths->SetDefaultRowSize( m_SearchPaths->GetDefaultRowSize() + 4 );

    m_EnvVars->PushEventHandler( new GRID_TRICKS( m_EnvVars ) );
    m_SearchPaths->PushEventHandler( new GRID_TRICKS( m_SearchPaths ) );

    m_EnvVars->SetSelectionMode( wxGrid::wxGridSelectionModes::wxGridSelectRows );
    m_SearchPaths->SetSelectionMode( wxGrid::wxGridSelectionModes::wxGridSelectRows );

    if( m_resolver )
    {
        m_SearchPaths->DeleteRows( 0, m_SearchPaths->GetNumberRows() );
        m_SearchPaths->UseNativeColHeader( true );

        // prohibit these characters in the alias names: []{}()%~<>"='`;:.,&?/\|$
        m_aliasValidator.SetStyle( wxFILTER_EXCLUDE_CHAR_LIST );
        m_aliasValidator.SetCharExcludes( wxT( "{}[]()%~<>\"='`;:.,&?/\\|$" ) );
    }
    else
        m_sb3DSearchPaths->Show( false );

    SetInitialFocus( m_EnvVars );
    m_sdbSizerOK->SetDefault();

    // wxFormBuilder doesn't include this event...
    m_EnvVars->Connect( wxEVT_GRID_CELL_CHANGING, wxGridEventHandler( DIALOG_CONFIGURE_PATHS::OnGridCellChanging ), NULL, this );
    m_SearchPaths->Connect( wxEVT_GRID_CELL_CHANGING, wxGridEventHandler( DIALOG_CONFIGURE_PATHS::OnGridCellChanging ), NULL, this );

    GetSizer()->SetSizeHints( this );
    Centre();
}


DIALOG_CONFIGURE_PATHS::~DIALOG_CONFIGURE_PATHS()
{
    // Delete the GRID_TRICKS.
    m_SearchPaths->PopEventHandler( true );
    m_EnvVars->PopEventHandler( true );

    m_EnvVars->Disconnect( wxEVT_GRID_CELL_CHANGING, wxGridEventHandler( DIALOG_CONFIGURE_PATHS::OnGridCellChanging ), NULL, this );
    m_SearchPaths->Disconnect( wxEVT_GRID_CELL_CHANGING, wxGridEventHandler( DIALOG_CONFIGURE_PATHS::OnGridCellChanging ), NULL, this );
}


bool DIALOG_CONFIGURE_PATHS::TransferDataToWindow()
{
   if( !wxDialog::TransferDataToWindow() )
       return false;

   // Do 3D search paths first so they get first crack at setting m_curdir

   if( m_resolver )
   {
       const std::list<SEARCH_PATH>* paths = m_resolver->GetPaths();

       for( auto it = paths->begin(); it != paths->end(); ++it )
       {
           if ( !( *it ).m_alias.StartsWith( "${" ) && !( *it ).m_alias.StartsWith( "$(" ) )
           {
               AppendSearchPath( it->m_alias, it->m_pathvar, it->m_description );

               if( m_curdir.IsEmpty() )
                   m_curdir = it->m_pathexp;
           }
       }
   }

    // Environment variables

    const ENV_VAR_MAP& envVars = Pgm().GetLocalEnvVariables();

    for( auto it = envVars.begin(); it != envVars.end(); ++it )
    {
        const wxString& path = it->second.GetValue();
        AppendEnvVar( it->first, path, it->second.GetDefinedExternally() );

        if( m_curdir.IsEmpty() && !path.StartsWith( "${" ) && !path.StartsWith( "$(" ) )
            m_curdir = path;
    }

    return true;
}


void DIALOG_CONFIGURE_PATHS::AppendEnvVar( const wxString& aName, const wxString& aPath,
                                           bool isExternal )
{
    int i = m_EnvVars->GetNumberRows();

    m_EnvVars->AppendRows( 1 );

    m_EnvVars->SetCellValue( i, EV_NAME_COL, aName );

    wxGridCellAttr* nameCellAttr = m_EnvVars->GetOrCreateCellAttr( i, EV_NAME_COL );
    wxGridCellTextEditor* nameTextEditor = new GRID_CELL_TEXT_EDITOR();
    nameTextEditor->SetValidator( ENV_VAR_NAME_VALIDATOR() );
    nameCellAttr->SetEditor( nameTextEditor );
    nameCellAttr->SetReadOnly( IsEnvVarImmutable( aName ) );
    nameCellAttr->DecRef();

    m_EnvVars->SetCellValue( i, EV_PATH_COL, aPath );

    wxGridCellAttr* pathCellAttr = m_EnvVars->GetOrCreateCellAttr( i, EV_PATH_COL );
    wxSystemColour c = isExternal ? wxSYS_COLOUR_MENU : wxSYS_COLOUR_LISTBOX;
    pathCellAttr->SetBackgroundColour( wxSystemSettings::GetColour( c ) );
    pathCellAttr->DecRef();

    m_EnvVars->SetCellValue( i, EV_FLAG_COL, isExternal ? wxT( "external" ) : wxEmptyString );
}


void DIALOG_CONFIGURE_PATHS::AppendSearchPath( const wxString& aName, const wxString& aPath,
                                          const wxString& aDescription )
{
    int i = m_SearchPaths->GetNumberRows();

    m_SearchPaths->AppendRows( 1 );

    m_SearchPaths->SetCellValue( i, SP_ALIAS_COL, aName );

    wxGridCellAttr* nameCellAttr = m_SearchPaths->GetOrCreateCellAttr( i, SP_ALIAS_COL );
    wxGridCellTextEditor* nameTextEditor = new GRID_CELL_TEXT_EDITOR();
    nameTextEditor->SetValidator( m_aliasValidator );
    nameCellAttr->SetEditor( nameTextEditor );
    nameCellAttr->DecRef();

    m_SearchPaths->SetCellValue( i, SP_PATH_COL, aPath );
    m_SearchPaths->SetCellValue( i, SP_DESC_COL, aDescription );
}


bool DIALOG_CONFIGURE_PATHS::TransferDataFromWindow()
{
    if( !m_EnvVars->CommitPendingChanges() || !m_SearchPaths->CommitPendingChanges() )
        return false;

    if( !wxDialog::TransferDataFromWindow() )
        return false;

    // Environment variables

    ENV_VAR_MAP envVarMap;

    for( int row = 0; row < m_EnvVars->GetNumberRows(); ++row )
    {
        wxString name = m_EnvVars->GetCellValue( row, EV_NAME_COL );
        wxString path = m_EnvVars->GetCellValue( row, EV_PATH_COL );
        wxString external = m_EnvVars->GetCellValue( row, EV_FLAG_COL );

        if( external.Length() )
            continue;

        if( name.IsEmpty() )
        {
            m_errorGrid = m_EnvVars;
            m_errorRow = row;
            m_errorCol = EV_NAME_COL;
            m_errorMsg = _( "Environment variable name cannot be empty." );
            return false;
        }
        else if( path.IsEmpty() )
        {
            m_errorGrid = m_EnvVars;
            m_errorRow = row;
            m_errorCol = EV_PATH_COL;
            m_errorMsg = _( "Environment variable path cannot be empty." );
            return false;
        }

        envVarMap[ name ] = ENV_VAR_ITEM( path );
    }

    Pgm().SetLocalEnvVariables( envVarMap );

    // 3D search paths

    if( m_resolver )
    {
        std::vector<SEARCH_PATH> alist;
        SEARCH_PATH alias;

        for( int row = 0; row < m_SearchPaths->GetNumberRows(); ++row )
        {
            alias.m_alias = m_SearchPaths->GetCellValue( row, SP_ALIAS_COL );
            alias.m_pathvar = m_SearchPaths->GetCellValue( row, SP_PATH_COL );
            alias.m_description = m_SearchPaths->GetCellValue( row, SP_DESC_COL );

            if( alias.m_alias.IsEmpty() )
            {
                m_errorGrid = m_SearchPaths;
                m_errorRow = row;
                m_errorCol = SP_ALIAS_COL;
                m_errorMsg = _( "3D search path alias cannot be empty." );
                return false;
            }
            else if( alias.m_pathvar.IsEmpty() )
            {
                m_errorGrid = m_SearchPaths;
                m_errorRow = row;
                m_errorCol = SP_PATH_COL;
                m_errorMsg = _( "3D search path cannot be empty." );
                return false;
            }

            alist.push_back( alias );
        }

        if( !m_resolver->UpdatePathList( alist ) )
            return false;
    }

    return true;
}


void DIALOG_CONFIGURE_PATHS::OnGridCellChanging( wxGridEvent& event )
{
    wxGrid*  grid = dynamic_cast<wxGrid*>( event.GetEventObject() );
    int      row = event.GetRow();
    int      col = event.GetCol();
    wxString text = event.GetString();

    if( text.IsEmpty() )
    {
        if( grid == m_EnvVars )
        {
            if( col == EV_NAME_COL )
                m_errorMsg = _( "Environment variable name cannot be empty." );
            else
                m_errorMsg = _( "Environment variable path cannot be empty." );
        }
        else
        {
            if( col == SP_ALIAS_COL )
                m_errorMsg = _( "3D search path alias cannot be empty." );
            else
                m_errorMsg = _( "3D search path cannot be empty." );
        }
        m_errorGrid = dynamic_cast<wxGrid*>( event.GetEventObject() );
        m_errorRow = row;
        m_errorCol = col;

        event.Veto();
    }

    if( grid == m_EnvVars )
    {
        if( col == EV_PATH_COL && m_EnvVars->GetCellValue( row, EV_FLAG_COL ).Length() )
        {
            wxString msg1 = _( "This path was defined  externally to the running process and\n"
                               "will only be temporarily overwritten." );
            wxString msg2 = _( "The next time KiCad is launched, any paths that have already\n"
                               "been defined are honored and any settings defined in the path\n"
                               "configuration dialog are ignored.  If you did not intend for\n"
                               "this behavior, either rename any conflicting entries or remove\n"
                               "the external environment variable(s) from your system." );
            KIDIALOG dlg( this, msg1, KIDIALOG::KD_WARNING );
            dlg.ShowDetailedText( msg2 );
            dlg.DoNotShowCheckbox( __FILE__, __LINE__ );
            dlg.ShowModal();
        }
        else if( col == EV_NAME_COL && m_EnvVars->GetCellValue( row, EV_NAME_COL ) != text )
        {
            if( text == PROJECT_VAR_NAME )    // This env var name is reserved and cannot be added here:
            {
                wxMessageBox( wxString::Format(
                              _( "The name %s is reserved, and cannot be used here" ),
                              PROJECT_VAR_NAME ) );
                event.Veto();
            }
            else    // Changing name; clear external flag
                m_EnvVars->SetCellValue( row, EV_FLAG_COL, wxEmptyString );
        }
    }
}


void DIALOG_CONFIGURE_PATHS::OnAddEnvVar( wxCommandEvent& event )
{
    if( !m_EnvVars->CommitPendingChanges() )
        return;

    AppendEnvVar( wxEmptyString, wxEmptyString, false );

    m_EnvVars->MakeCellVisible( m_EnvVars->GetNumberRows() - 1, EV_NAME_COL );
    m_EnvVars->SetGridCursor( m_EnvVars->GetNumberRows() - 1, EV_NAME_COL );

    m_EnvVars->EnableCellEditControl( true );
    m_EnvVars->ShowCellEditControl();
}


void DIALOG_CONFIGURE_PATHS::OnAddSearchPath( wxCommandEvent& event )
{
    if( !m_SearchPaths->CommitPendingChanges() )
        return;

    AppendSearchPath( wxEmptyString, wxEmptyString, wxEmptyString);

    m_SearchPaths->MakeCellVisible( m_SearchPaths->GetNumberRows() - 1, SP_ALIAS_COL );
    m_SearchPaths->SetGridCursor( m_SearchPaths->GetNumberRows() - 1, SP_ALIAS_COL );

    m_SearchPaths->EnableCellEditControl( true );
    m_SearchPaths->ShowCellEditControl();
}


void DIALOG_CONFIGURE_PATHS::OnRemoveEnvVar( wxCommandEvent& event )
{
    int curRow = m_EnvVars->GetGridCursorRow();

    if( curRow < 0 || m_EnvVars->GetNumberRows() <= curRow )
        return;
    else if( IsEnvVarImmutable( m_EnvVars->GetCellValue( curRow, EV_NAME_COL ) ) )
    {
        wxBell();
        return;
    }

    m_EnvVars->CommitPendingChanges( true /* silent mode; we don't care if it's valid */ );
    m_EnvVars->DeleteRows( curRow, 1 );

    m_EnvVars->MakeCellVisible( std::max( 0, curRow-1 ), m_EnvVars->GetGridCursorCol() );
    m_EnvVars->SetGridCursor( std::max( 0, curRow-1 ), m_EnvVars->GetGridCursorCol() );
}


void DIALOG_CONFIGURE_PATHS::OnDeleteSearchPath( wxCommandEvent& event )
{
    int curRow = m_SearchPaths->GetGridCursorRow();

    if( curRow < 0 || m_SearchPaths->GetNumberRows() <= curRow )
        return;

    m_SearchPaths->CommitPendingChanges( true /* silent mode; we don't care if it's valid */ );
    m_SearchPaths->DeleteRows( curRow, 1 );

    m_SearchPaths->MakeCellVisible( std::max( 0, curRow-1 ), m_SearchPaths->GetGridCursorCol() );
    m_SearchPaths->SetGridCursor( std::max( 0, curRow-1 ), m_SearchPaths->GetGridCursorCol() );
}


void DIALOG_CONFIGURE_PATHS::OnSearchPathMoveUp( wxCommandEvent& event )
{
    if( !m_SearchPaths->CommitPendingChanges() )
        return;

    int curRow   = m_SearchPaths->GetGridCursorRow();
    int prevRow  = curRow - 1;

    if( curRow > 0 )
    {
        for( int i = 0; i < m_SearchPaths->GetNumberCols(); ++i )
        {
            wxString tmp = m_SearchPaths->GetCellValue( curRow, i );
            m_SearchPaths->SetCellValue( curRow, i, m_SearchPaths->GetCellValue( prevRow, i ) );
            m_SearchPaths->SetCellValue( prevRow, i, tmp );
        }

        m_SearchPaths->SetGridCursor( prevRow, m_SearchPaths->GetGridCursorCol() );
    }
    else
        wxBell();
}


void DIALOG_CONFIGURE_PATHS::OnSearchPathMoveDown( wxCommandEvent& event )
{
    if( !m_SearchPaths->CommitPendingChanges() )
        return;

    int curRow   = m_SearchPaths->GetGridCursorRow();
    int nextRow  = curRow + 1;

    if( curRow < m_SearchPaths->GetNumberRows() - 1 )
    {
        for( int i = 0; i < m_SearchPaths->GetNumberCols(); ++i )
        {
            wxString tmp = m_SearchPaths->GetCellValue( curRow, i );
            m_SearchPaths->SetCellValue( curRow, i, m_SearchPaths->GetCellValue( nextRow, i ) );
            m_SearchPaths->SetCellValue( nextRow, i, tmp );
        }

        m_SearchPaths->SetGridCursor( nextRow, m_SearchPaths->GetGridCursorCol() );
    }
    else
        wxBell();
}


void DIALOG_CONFIGURE_PATHS::OnGridCellRightClick( wxGridEvent& aEvent )
{
    wxASSERT( (int) EV_PATH_COL == (int) SP_PATH_COL );

    if( aEvent.GetCol() == EV_PATH_COL )
    {
        wxMenu menu;

        AddMenuItem( &menu, 1, _( "File Browser..." ), KiBitmap( folder_xpm ) );
        if( GetPopupMenuSelectionFromUser( menu ) == 1 )
        {
            wxDirDialog dlg( nullptr, _( "Select Path" ), m_curdir,
                             wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );

            if( dlg.ShowModal() == wxID_OK )
            {
                wxGrid* grid = dynamic_cast<wxGrid*>( aEvent.GetEventObject() );
                grid->SetCellValue( aEvent.GetRow(), EV_PATH_COL, dlg.GetPath() );
                m_curdir = dlg.GetPath();
            }
        }
    }
}


void DIALOG_CONFIGURE_PATHS::OnGridCellChange( wxGridEvent& aEvent )
{
    m_gridWidthsDirty = true;

    aEvent.Skip();
}


void DIALOG_CONFIGURE_PATHS::OnUpdateUI( wxUpdateUIEvent& event )
{
    if( m_gridWidthsDirty && ( !m_EnvVars->IsCellEditControlShown()
                               && !m_SearchPaths->IsCellEditControlShown() ) )
    {
        int width = m_EnvVars->GetClientRect().GetWidth();

        m_EnvVars->AutoSizeColumn( EV_NAME_COL );
        m_EnvVars->SetColSize( EV_NAME_COL, std::max( m_EnvVars->GetColSize( EV_NAME_COL ), 120 ) );

        m_EnvVars->SetColSize( EV_PATH_COL, width - m_EnvVars->GetColSize( EV_NAME_COL ) );

        width = m_SearchPaths->GetClientRect().GetWidth();

        m_SearchPaths->AutoSizeColumn( SP_ALIAS_COL );
        m_SearchPaths->SetColSize( SP_ALIAS_COL, std::max( m_SearchPaths->GetColSize( SP_ALIAS_COL ), 120 ) );

        m_SearchPaths->AutoSizeColumn( SP_PATH_COL );
        m_SearchPaths->SetColSize( SP_PATH_COL, std::max( m_SearchPaths->GetColSize( SP_PATH_COL ), 300 ) );

        m_SearchPaths->SetColSize( SP_DESC_COL, width - ( m_SearchPaths->GetColSize( SP_ALIAS_COL )
                                                          + m_SearchPaths->GetColSize( SP_PATH_COL ) ) );
        m_gridWidthsDirty = false;
    }

    // Handle a grid error.  This is delayed to OnUpdateUI so that we can change focus
    // even when the original validation was triggered from a killFocus event (and for
    // dialog with notebooks, so that the corresponding notebook page can be shown in
    // the background when triggered from an OK).
    if( m_errorGrid )
    {
        // We will re-enter this routine when the error dialog is displayed, so make
        // sure we don't keep putting up more dialogs.
        wxGrid* grid = m_errorGrid;
        m_errorGrid = nullptr;

        DisplayErrorMessage( this, m_errorMsg );

        grid->SetFocus();
        grid->MakeCellVisible( m_errorRow, m_errorCol );
        grid->SetGridCursor( m_errorRow, m_errorCol );

        grid->EnableCellEditControl( true );
        grid->ShowCellEditControl();
    }
}


void DIALOG_CONFIGURE_PATHS::OnGridSize( wxSizeEvent& event )
{
    m_gridWidthsDirty = true;

    event.Skip();
}


void DIALOG_CONFIGURE_PATHS::OnHelp( wxCommandEvent& event )
{
    wxString msg = _( "Enter the name and value for each environment variable.  Grey entries "
                      "are names that have been defined externally at the system or user "
                      "level.  Environment variables defined at the system or user level "
                      "take precedence over the ones defined in this table.  This means the "
                      "values in this table are ignored." );
    msg << "<br><br><b>";
    msg << _( "To ensure environment variable names are valid on all platforms, the name field "
              "will only accept upper case letters, digits, and the underscore characters." );
    msg << "</b>";

    for( const auto& var: GetPredefinedEnvVars() )
    {
        msg << "<br><br><b>" << var << "</b>";

        const auto desc = LookUpEnvVarHelp( var );

        if( desc.size() > 0 )
            msg << ": " << desc;

    }

    HTML_MESSAGE_BOX dlg( GetParent(), _( "Environment Variable Help" ) );
    dlg.SetDialogSizeInDU( 400, 250 );

    dlg.AddHTML_Text( msg );
    dlg.ShowModal();
}
