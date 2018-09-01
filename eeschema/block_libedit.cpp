/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2008-2011 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2004-2017 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file block_libedit.cpp
 */

#include <fctsys.h>
#include <gr_basic.h>
#include <class_drawpanel.h>
#include <confirm.h>

#include <general.h>
#include <class_library.h>
#include <lib_edit_frame.h>


static void DrawMovingBlockOutlines( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aPosition,
                                     bool aErase );


int LIB_EDIT_FRAME::BlockCommand( EDA_KEY key )
{
    int cmd = BLOCK_IDLE;

    switch( key )
    {
    default:
        cmd = key & 0xFF;
        break;

    case EDA_KEY_C( 0xffffffff ):   // -1
        // Historically, -1 has been used as a key, which can cause bit flag
        // clashes with unaware code. On debug builds, catch any old code that
        // might still be doing this. TODO: remove if sure all this old code is gone.
        wxFAIL_MSG( "negative EDA_KEY value should be converted to GR_KEY_INVALID" );
        // fall through on release builds

    case GR_KEY_INVALID:
        cmd = BLOCK_PRESELECT_MOVE;
        break;

    case GR_KEY_NONE:
        cmd = BLOCK_MOVE;
        break;

    case GR_KB_SHIFT:
        cmd = BLOCK_DUPLICATE;
        break;

    case GR_KB_ALT:
        cmd = BLOCK_ROTATE;
        break;

    case GR_KB_SHIFTCTRL:
        cmd = BLOCK_DELETE;
        break;

    case GR_KB_CTRL:
        cmd = BLOCK_MIRROR_Y;
        break;

    case MOUSE_MIDDLE:
        cmd = BLOCK_ZOOM;
        break;
    }

    return cmd;
}


bool LIB_EDIT_FRAME::HandleBlockEnd( wxDC* aDC )
{
    int ItemCount = 0;
    bool nextCmd = false;
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;
    wxPoint pt;

    if( block->GetCount() )
    {
        BLOCK_STATE_T state     = block->GetState();
        BLOCK_COMMAND_T command = block->GetCommand();
        m_canvas->CallEndMouseCapture( aDC );
        block->SetState( state );
        block->SetCommand( command );
        m_canvas->SetMouseCapture( DrawAndSizingBlockOutlines, AbortBlockCurrentCommand );
        SetCrossHairPosition( wxPoint( block->GetRight(),
                                       block->GetBottom() ) );
        m_canvas->MoveCursorToCrossHair();
    }

    switch( block->GetCommand() )
    {
    case  BLOCK_IDLE:
        DisplayError( this, wxT( "Error in HandleBlockPLace" ) );
        break;

    case BLOCK_DRAG:        // Drag
    case BLOCK_DRAG_ITEM:
    case BLOCK_MOVE:        // Move
    case BLOCK_DUPLICATE:   // Duplicate
        if( GetCurPart() )
            ItemCount = GetCurPart()->SelectItems( *block,
                                                  m_unit, m_convert,
                                                  m_syncPinEdit );
        if( ItemCount )
        {
            nextCmd = true;

            if( m_canvas->IsMouseCaptured() )
            {
                m_canvas->CallMouseCapture( aDC, wxDefaultPosition, false );
                m_canvas->SetMouseCaptureCallback( DrawMovingBlockOutlines );
                m_canvas->CallMouseCapture( aDC, wxDefaultPosition, false );
            }

            block->SetState( STATE_BLOCK_MOVE );
            m_canvas->Refresh( true );
        }
        break;

    case BLOCK_PRESELECT_MOVE:     // Move with preselection list
        nextCmd = true;
        m_canvas->SetMouseCaptureCallback( DrawMovingBlockOutlines );
        block->SetState( STATE_BLOCK_MOVE );
        break;

    case BLOCK_COPY:    // Save a copy of items in the clipboard buffer
    case BLOCK_CUT:
        if( GetCurPart() )
            ItemCount = GetCurPart()->SelectItems( *block, m_unit, m_convert,
                                                  m_syncPinEdit );

        if( ItemCount )
        {
            copySelectedItems();
            auto cmd = block->GetCommand();

            if( cmd == BLOCK_COPY )
            {
                GetCurPart()->ClearSelectedItems();
                block->ClearItemsList();
            }
            else if( cmd == BLOCK_CUT )
            {
                SaveCopyInUndoList( GetCurPart() );
                GetCurPart()->DeleteSelectedItems();
                OnModify();
            }
        }
        break;

    case BLOCK_DELETE:     // Delete
        if( GetCurPart() )
            ItemCount = GetCurPart()->SelectItems( *block,
                                                  m_unit, m_convert,
                                                  m_syncPinEdit );
        if( ItemCount )
            SaveCopyInUndoList( GetCurPart() );

        if( GetCurPart() )
        {
            GetCurPart()->DeleteSelectedItems();
            OnModify();
        }
        break;

    case BLOCK_PASTE:
        wxFAIL; // should not happen
        break;

    case BLOCK_FLIP:
        break;

    case BLOCK_ROTATE:
    case BLOCK_MIRROR_X:
    case BLOCK_MIRROR_Y:
        if( GetCurPart() )
            ItemCount = GetCurPart()->SelectItems( *block,
                                                  m_unit, m_convert,
                                                  m_syncPinEdit );
        if( ItemCount )
            SaveCopyInUndoList( GetCurPart() );

        pt = block->Centre();
        pt = GetNearestGridPosition( pt );
        pt.y = -pt.y;

        if( GetCurPart() )
        {
            OnModify();
            int block_cmd = block->GetCommand();

            if( block_cmd == BLOCK_MIRROR_Y)
                GetCurPart()->MirrorSelectedItemsH( pt );
            else if( block_cmd == BLOCK_MIRROR_X)
                GetCurPart()->MirrorSelectedItemsV( pt );
            else if( block_cmd == BLOCK_ROTATE )
                GetCurPart()->RotateSelectedItems( pt );
        }

        break;

    case BLOCK_ZOOM:     // Window Zoom
        Window_Zoom( *block );
        break;

    case BLOCK_ABORT:
        break;

    case BLOCK_SELECT_ITEMS_ONLY:
        break;

    case BLOCK_DUPLICATE_AND_INCREMENT: // not used in Eeschema
    case BLOCK_MOVE_EXACT:              // not used in Eeschema
        break;
    }

    if( !nextCmd )
    {
        if( block->GetCommand() != BLOCK_SELECT_ITEMS_ONLY &&  GetCurPart() )
            GetCurPart()->ClearSelectedItems();

        block->SetState( STATE_NO_BLOCK );
        block->SetCommand( BLOCK_IDLE );
        GetScreen()->SetCurItem( NULL );
        m_canvas->EndMouseCapture( GetToolId(), m_canvas->GetCurrentCursor(), wxEmptyString,
                                   false );
        m_canvas->Refresh( true );
    }

    return nextCmd;
}


void LIB_EDIT_FRAME::HandleBlockPlace( wxDC* DC )
{
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;
    wxPoint pt;

    if( !m_canvas->IsMouseCaptured() )
    {
        DisplayError( this, wxT( "HandleBlockPLace : m_mouseCaptureCallback = NULL" ) );
    }

    block->SetState( STATE_BLOCK_STOP );

    switch( block->GetCommand() )
    {
    case  BLOCK_IDLE:
        break;

    case BLOCK_DRAG:                // Drag
    case BLOCK_DRAG_ITEM:
    case BLOCK_MOVE:                // Move
    case BLOCK_PRESELECT_MOVE:      // Move with preselection list
        block->ClearItemsList();

        if( GetCurPart() )
            SaveCopyInUndoList( GetCurPart() );

        pt = block->GetMoveVector();
        pt.y *= -1;

        if( GetCurPart() )
            GetCurPart()->MoveSelectedItems( pt );

        m_canvas->Refresh( true );
        break;

    case BLOCK_DUPLICATE:           // Duplicate
        block->ClearItemsList();

        if( GetCurPart() )
            SaveCopyInUndoList( GetCurPart() );

        pt = block->GetMoveVector();
        pt.y = -pt.y;

        if( GetCurPart() )
            GetCurPart()->CopySelectedItems( pt );

        break;

    case BLOCK_PASTE:       // Paste (recopy the last block saved)
        block->ClearItemsList();

        if( GetCurPart() )
            SaveCopyInUndoList( GetCurPart() );

        pt = block->GetMoveVector();
        pt.y = -pt.y;

        pasteClipboard( pt );
        break;

    case BLOCK_ROTATE:      // Invert by popup menu, from block move
    case BLOCK_MIRROR_X:    // Invert by popup menu, from block move
    case BLOCK_MIRROR_Y:    // Invert by popup menu, from block move
        if( GetCurPart() )
            SaveCopyInUndoList( GetCurPart() );

        pt = block->Centre();
        pt = GetNearestGridPosition( pt );
        pt.y = -pt.y;

        if( GetCurPart() )
        {
            int block_cmd = block->GetCommand();

            if( block_cmd == BLOCK_MIRROR_Y)
                GetCurPart()->MirrorSelectedItemsH( pt );
            else if( block_cmd == BLOCK_MIRROR_X)
                GetCurPart()->MirrorSelectedItemsV( pt );
            else if( block_cmd == BLOCK_ROTATE )
                GetCurPart()->RotateSelectedItems( pt );
        }

        break;

    case BLOCK_ZOOM:        // Handled by HandleBlockEnd
    case BLOCK_DELETE:
    case BLOCK_COPY:
    case BLOCK_ABORT:
    default:
        break;
    }

    OnModify();

    block->SetState( STATE_NO_BLOCK );
    block->SetCommand( BLOCK_IDLE );
    GetScreen()->SetCurItem( NULL );
    m_canvas->EndMouseCapture( GetToolId(), m_canvas->GetCurrentCursor(), wxEmptyString, false );
    m_canvas->Refresh( true );
}


void LIB_EDIT_FRAME::InitBlockPasteInfos()
{
    BLOCK_SELECTOR& block = GetScreen()->m_BlockLocate;

    // Copy the clipboard contents to the screen block selector
    // (only the copy, the new instances will be appended to the part once the items are placed)
    block.GetItems().CopyList( m_clipboard.GetItems() );

    // Set the pate reference point
    block.SetLastCursorPosition( m_clipboard.GetLastCursorPosition() );
    m_canvas->SetMouseCaptureCallback( DrawMovingBlockOutlines );
}


void LIB_EDIT_FRAME::copySelectedItems()
{
    LIB_PART* part = GetCurPart();

    if( !part )
        return;

    m_clipboard.ClearListAndDeleteItems();   // delete previous saved list, if exists
    m_clipboard.SetLastCursorPosition( GetScreen()->m_BlockLocate.GetEnd() );    // store the reference point

    for( LIB_ITEM& item : part->GetDrawItems() )
    {
        // We *do not* copy fields because they are unique for the whole component
        // so skip them (do not duplicate) if they are flagged selected.
        if( item.Type() == LIB_FIELD_T )
            item.ClearFlags( SELECTED );

        if( !item.IsSelected() )
            continue;

        // Do not clear the 'selected' flag. It is required to have items drawn when they are pasted.
        LIB_ITEM* copy = (LIB_ITEM*) item.Clone();
        copy->SetFlags( copy->GetFlags() | UR_TRANSIENT );
        ITEM_PICKER picker( copy, UR_NEW );
        m_clipboard.PushItem( picker );
    }
}


void LIB_EDIT_FRAME::pasteClipboard( const wxPoint& aOffset )
{
    LIB_PART* part = GetCurPart();

    if( !part || m_clipboard.GetCount() == 0 )
        return;

    for( unsigned int i = 0; i < m_clipboard.GetCount(); i++ )
    {
        // Append a copy to the current part, so the clipboard buffer might be pasted multiple times
        LIB_ITEM* item = (LIB_ITEM*) m_clipboard.GetItem( i )->Clone();
        item->SetParent( part );
        item->SetSelected();
        item->SetUnit( GetUnit() );
        part->AddDrawItem( item );
    }

    GetCurPart()->MoveSelectedItems( aOffset );
    OnModify();
}


/*
 * Traces the outline of the search block structures
 * The entire block follows the cursor
 */
void DrawMovingBlockOutlines( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aPosition,
                              bool aErase )
{
    BASE_SCREEN* screen = aPanel->GetScreen();
    BLOCK_SELECTOR* block = &screen->m_BlockLocate;

    LIB_EDIT_FRAME* parent = (LIB_EDIT_FRAME*) aPanel->GetParent();
    wxASSERT( parent != NULL );

    LIB_PART* component = parent->GetCurPart();

    if( component == NULL )
        return;

    int unit = parent->GetUnit();
    int convert = parent->GetConvert();

    auto opts = PART_DRAW_OPTIONS::Default();
    opts.draw_mode = g_XorMode;
    opts.only_selected = true;

    if( aErase )
    {
        block->Draw( aPanel, aDC, block->GetMoveVector(), g_XorMode, block->GetColor() );
        component->Draw( aPanel, aDC, block->GetMoveVector(), unit, convert, opts );

        for( unsigned ii = 0; ii < block->GetCount(); ii++ )
        {
            LIB_ITEM* libItem = (LIB_ITEM*) block->GetItem( ii );
            libItem->Draw( aPanel, aDC, block->GetMoveVector(), g_GhostColor, g_XorMode, nullptr, opts.transform );
        }
    }

    // Repaint new view
    block->SetMoveVector( parent->GetCrossHairPosition() - block->GetLastCursorPosition() );

    GRSetDrawMode( aDC, g_XorMode );
    block->Draw( aPanel, aDC, block->GetMoveVector(), g_XorMode, block->GetColor() );

    for( unsigned ii = 0; ii < block->GetCount(); ii++ )
    {
        LIB_ITEM* libItem = (LIB_ITEM*) block->GetItem( ii );
        libItem->Draw( aPanel, aDC, block->GetMoveVector(), g_GhostColor, g_XorMode, nullptr, opts.transform );
    }

    component->Draw( aPanel, aDC, block->GetMoveVector(), unit, convert, opts );
}
