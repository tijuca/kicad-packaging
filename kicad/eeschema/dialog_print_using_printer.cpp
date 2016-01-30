/****************************************/
/* File: dialog_print_using_printer.cpp */
/****************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"

#include "program.h"
#include "general.h"
#include "eeschema_config.h"

#include "dialog_print_using_printer_base.h"


/**
 * Print schematic dialog.
 *
 * Class derived from DIALOG_PRINT_USING_PRINTER_base created by wxFormBuilder
 */
class DIALOG_PRINT_USING_PRINTER : public DIALOG_PRINT_USING_PRINTER_BASE
{
public:
    DIALOG_PRINT_USING_PRINTER( WinEDA_SchematicFrame* aParent );
    ~DIALOG_PRINT_USING_PRINTER() {};

    WinEDA_SchematicFrame* GetParent() const;

private:
    void OnCloseWindow( wxCloseEvent& event );
    void OnInitDialog( wxInitDialogEvent& event );
    void OnPageSetup( wxCommandEvent& event );
    void OnPrintPreview( wxCommandEvent& event );
    void OnPrintButtonClick( wxCommandEvent& event );
    void OnButtonCancelClick( wxCommandEvent& event ){ Close(); }
};


/**
 * Custom print out for printing schematics.
 */
class SCH_PRINTOUT : public wxPrintout
{
private:
    DIALOG_PRINT_USING_PRINTER* m_Parent;

public:
    SCH_PRINTOUT( DIALOG_PRINT_USING_PRINTER* aParent, const wxString& aTitle ) :
        wxPrintout( aTitle )
    {
        wxASSERT( aParent != NULL );

        m_Parent = aParent;
    }

    bool OnPrintPage( int page );
    bool HasPage( int page );
    bool OnBeginDocument( int startPage, int endPage );
    void GetPageInfo( int* minPage, int* maxPage, int* selPageFrom, int* selPageTo );
    void DrawPage();
};


/**
 * Custom schematic print preview frame.
 */
class SCH_PREVIEW_FRAME : public wxPreviewFrame
{
public:
    SCH_PREVIEW_FRAME( wxPrintPreview* aPreview, DIALOG_PRINT_USING_PRINTER* aParent,
                       const wxString& aTitle, const wxPoint& aPos = wxDefaultPosition,
                       const wxSize& aSize = wxDefaultSize ) :
        wxPreviewFrame( aPreview, aParent, aTitle, aPos, aSize )
    {
    }

    DIALOG_PRINT_USING_PRINTER* GetParent()
    {
        return ( DIALOG_PRINT_USING_PRINTER* )wxWindow::GetParent();
    }

    void OnCloseWindow( wxCloseEvent& event )
    {
        if( !IsIconized() )
        {
            GetParent()->GetParent()->SetPreviewPosition( GetPosition() );
            GetParent()->GetParent()->SetPreviewSize( GetSize() );
        }

        wxPreviewFrame::OnCloseWindow( event );
    }

private:
    DECLARE_CLASS( SCH_PREVIEW_FRAME )
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS( SCH_PREVIEW_FRAME )
};


IMPLEMENT_CLASS( SCH_PREVIEW_FRAME, wxPreviewFrame )

BEGIN_EVENT_TABLE( SCH_PREVIEW_FRAME, wxPreviewFrame )
    EVT_CLOSE( SCH_PREVIEW_FRAME::OnCloseWindow )
END_EVENT_TABLE()


void WinEDA_SchematicFrame::OnPrint( wxCommandEvent& event )
{
    wxFileName fn;
    DIALOG_PRINT_USING_PRINTER dlg( this );

    dlg.ShowModal();

    fn = g_RootSheet->m_AssociatedScreen->m_FileName;

    wxString default_name = NAMELESS_PROJECT;
    default_name += wxT( ".sch" );
    if( fn.GetFullName() != default_name )
    {
        fn.SetExt( ProjectFileExtension );
        wxGetApp().WriteProjectConfig( fn.GetFullPath(), GROUP, GetProjectFileParameters() );
    }
}


DIALOG_PRINT_USING_PRINTER::DIALOG_PRINT_USING_PRINTER( WinEDA_SchematicFrame* aParent ) :
    DIALOG_PRINT_USING_PRINTER_BASE( aParent )
{
    wxASSERT( aParent != NULL );

    m_checkReference->SetValue( aParent->GetShowSheetReference() );
    m_checkMonochrome->SetValue( aParent->GetPrintMonochrome() );
}


WinEDA_SchematicFrame* DIALOG_PRINT_USING_PRINTER::GetParent() const
{
    return ( WinEDA_SchematicFrame* ) wxWindow::GetParent();
}


void DIALOG_PRINT_USING_PRINTER::OnInitDialog( wxInitDialogEvent& event )
{
    WinEDA_SchematicFrame* parent = GetParent();

    if ( GetSizer() )
        GetSizer()->SetSizeHints( this );

    if( parent->GetPrintDialogPosition() == wxDefaultPosition &&
        parent->GetPrintDialogSize() == wxDefaultSize )
    {
        Center();
    }
    else
    {
        SetPosition( parent->GetPrintDialogPosition() );
        SetSize( parent->GetPrintDialogSize() );
    }

    SetFocus();
}


void DIALOG_PRINT_USING_PRINTER::OnCloseWindow( wxCloseEvent& event )
{
    WinEDA_SchematicFrame* parent = GetParent();

    if( !IsIconized() )
    {
        parent->SetPrintDialogPosition( GetPosition() );
        parent->SetPrintDialogSize( GetSize() );
    }

    parent->SetPrintMonochrome( m_checkMonochrome->IsChecked() );
    parent->SetShowSheetReference( m_checkReference->IsChecked() );

    EndDialog( wxID_CANCEL );
}


/* Open a dialog box for printer setup (printer options, page size ...)
 */
void DIALOG_PRINT_USING_PRINTER::OnPageSetup( wxCommandEvent& event )
{
    WinEDA_SchematicFrame* parent = GetParent();

    wxPageSetupDialog pageSetupDialog( this, &parent->GetPageSetupData() );

    pageSetupDialog.ShowModal();

    parent->GetPageSetupData() = pageSetupDialog.GetPageSetupDialogData();
}


/* Open and display a previewer frame for printing
 */
void DIALOG_PRINT_USING_PRINTER::OnPrintPreview( wxCommandEvent& event )
{
    WinEDA_SchematicFrame* parent = GetParent();

    parent->SetPrintMonochrome( m_checkMonochrome->IsChecked() );
    parent->SetShowSheetReference( m_checkReference->IsChecked() );

    // Pass two printout objects: for preview, and possible printing.
    wxString        title   = _( "Preview" );
    wxPrintPreview* preview = new wxPrintPreview( new SCH_PRINTOUT( this, title ),
                                                  new SCH_PRINTOUT( this, title ),
                                                  &parent->GetPageSetupData().GetPrintData() );

    if( preview == NULL )
    {
        DisplayError( this, wxT( "Print preview error!" ) );
        return;
    }

    preview->SetZoom( 100 );

    SCH_PREVIEW_FRAME* frame = new SCH_PREVIEW_FRAME( preview, this, title,
                                                      parent->GetPreviewPosition(),
                                                      parent->GetPreviewSize() );
    frame->Initialize();
    frame->Show( true );
}


void DIALOG_PRINT_USING_PRINTER::OnPrintButtonClick( wxCommandEvent& event )
{
    WinEDA_SchematicFrame* parent = GetParent();

    parent->SetPrintMonochrome( m_checkMonochrome->IsChecked() );
    parent->SetShowSheetReference( m_checkReference->IsChecked() );

    wxPrintDialogData printDialogData( parent->GetPageSetupData().GetPrintData() );
    printDialogData.SetMaxPage( g_RootSheet->CountSheets() );

    if( g_RootSheet->CountSheets() > 1 )
        printDialogData.EnablePageNumbers( true );

    wxPrinter printer( &printDialogData );
    SCH_PRINTOUT printout( this, _( "Print Schematic" ) );

    if( !printer.Print( this, &printout, true ) )
    {
        if( wxPrinter::GetLastError() == wxPRINTER_ERROR )
            wxMessageBox( _( "An error occurred attempting to print the schematic." ),
                          _( "Printing" ), wxOK );
    }
    else
    {
        parent->GetPageSetupData() = printer.GetPrintDialogData().GetPrintData();
    }
}


bool SCH_PRINTOUT::OnPrintPage( int page )
{
    wxString msg;
    WinEDA_SchematicFrame* parent = m_Parent->GetParent();
    msg.Printf( _( "Print page %d" ), page );
    parent->ClearMsgPanel();
    parent->AppendMsgPanel( msg, wxEmptyString, CYAN );

    SCH_SCREEN*     screen       = parent->GetScreen();
    SCH_SCREEN*     oldscreen    = screen;
    SCH_SHEET_PATH* oldsheetpath = parent->GetSheet();
    SCH_SHEET_PATH  list;
    SCH_SHEET_LIST  SheetList( NULL );
    SCH_SHEET_PATH* sheetpath = SheetList.GetSheet( page - 1 );

    if( list.BuildSheetPathInfoFromSheetPathValue( sheetpath->Path() ) )
    {
        parent->m_CurrentSheet = &list;
        parent->m_CurrentSheet->UpdateAllScreenReferences();
        parent->SetSheetNumberAndCount();
        screen = parent->m_CurrentSheet->LastScreen();
    }
    else
    {
        screen = NULL;
    }

    if( screen == NULL )
        return false;

    ActiveScreen = screen;
    DrawPage();
    ActiveScreen = oldscreen;
    parent->m_CurrentSheet = oldsheetpath;
    parent->m_CurrentSheet->UpdateAllScreenReferences();
    parent->SetSheetNumberAndCount();

    return true;
}


void SCH_PRINTOUT::GetPageInfo( int* minPage, int* maxPage,
                                int* selPageFrom, int* selPageTo )
{
    *minPage = *selPageFrom = 1;
    *maxPage = *selPageTo   = g_RootSheet->CountSheets();
}


bool SCH_PRINTOUT::HasPage( int pageNum )
{
    int pageCount;

    pageCount = g_RootSheet->CountSheets();
    if( pageCount >= pageNum )
        return true;

    return false;
}


bool SCH_PRINTOUT::OnBeginDocument( int startPage, int endPage )
{
    if( !wxPrintout::OnBeginDocument( startPage, endPage ) )
        return false;

#ifdef __WXDEBUG__
    WinEDA_SchematicFrame* parent = m_Parent->GetParent();
#endif
    wxLogDebug( wxT( "Printer name: " ) +
                parent->GetPageSetupData().GetPrintData().GetPrinterName() );
    wxLogDebug( wxT( "Paper ID: %d" ),
                parent->GetPageSetupData().GetPrintData().GetPaperId() );
    wxLogDebug( wxT( "Color: %d" ),
                (int) parent->GetPageSetupData().GetPrintData().GetColour() );
    wxLogDebug( wxT( "Monochrome: %d" ), parent->GetPrintMonochrome() );
    wxLogDebug( wxT( "Orientation: %d:" ),
                parent->GetPageSetupData().GetPrintData().GetOrientation() );
    wxLogDebug( wxT( "Quality: %d"),
                parent->GetPageSetupData().GetPrintData().GetQuality() );

    return true;
}


/*
 * This is the real print function: print the active screen
 */
void SCH_PRINTOUT::DrawPage()
{
    int      oldZoom;
    wxPoint  tmp_startvisu;
    wxSize   SheetSize;      // Page size in internal units
    wxPoint  old_org;
    EDA_Rect oldClipBox;
    wxRect   fitRect;
    wxDC*    dc = GetDC();
    WinEDA_SchematicFrame* parent = m_Parent->GetParent();
    WinEDA_DrawPanel* panel = parent->DrawPanel;

    wxBusyCursor dummy;

    /* Save current scale factor, offsets, and clip box. */
    tmp_startvisu = ActiveScreen->m_StartVisu;
    oldZoom = ActiveScreen->GetZoom();
    old_org = ActiveScreen->m_DrawOrg;
    oldClipBox = panel->m_ClipBox;

    /* Change scale factor, offsets, and clip box to print the whole page. */
    ActiveScreen->SetScalingFactor( 1.0 );
    ActiveScreen->m_DrawOrg.x   = ActiveScreen->m_DrawOrg.y = 0;
    ActiveScreen->m_StartVisu.x = ActiveScreen->m_StartVisu.y = 0;
    panel->m_ClipBox.SetOrigin( wxPoint( 0, 0 ) );
    panel->m_ClipBox.SetSize( wxSize( 0x7FFFFF0, 0x7FFFFF0 ) );

    bool printReference = parent->GetShowSheetReference();

    if( printReference )
    {
        /* Draw the page to a memory and let the dc calculate the drawing
         * limits.
         */
        wxBitmap psuedoBitmap( 1, 1 );
        wxMemoryDC memDC;
        memDC.SelectObject( psuedoBitmap );
        parent->PrintPage( &memDC, true, 0xFFFFFFFF, false );
        wxLogDebug( wxT( "MinX = %d, MaxX = %d, MinY = %d, MaxY = %d" ),
                    memDC.MinX(), memDC.MaxX(), memDC.MinY(), memDC.MaxY() );

        SheetSize.x = memDC.MaxX() - memDC.MinX();
        SheetSize.y = memDC.MaxY() - memDC.MinY();

        FitThisSizeToPageMargins( SheetSize, parent->GetPageSetupData() );
        fitRect = GetLogicalPageMarginsRect( parent->GetPageSetupData() );
    }
    else
    {
        SheetSize = ActiveScreen->m_CurrentSheetDesc->m_Size;
        FitThisSizeToPaper( SheetSize );
        fitRect = GetLogicalPaperRect();
    }

    wxLogDebug( wxT( "Fit rectangle: %d, %d, %d, %d" ),
                fitRect.x, fitRect.y, fitRect.width, fitRect.height );

    GRResetPenAndBrush( dc );

    if( parent->GetPrintMonochrome() )
        GRForceBlackPen( true );

    ActiveScreen->m_IsPrinting = true;
    int bg_color = g_DrawBgColor;

    parent->PrintPage( dc, printReference, 0xFFFFFFFF, false );

    g_DrawBgColor = bg_color;
    ActiveScreen->m_IsPrinting = false;
    panel->m_ClipBox = oldClipBox;

    GRForceBlackPen( false );

    ActiveScreen->m_StartVisu = tmp_startvisu;
    ActiveScreen->m_DrawOrg   = old_org;
    ActiveScreen->SetZoom( oldZoom );
}
