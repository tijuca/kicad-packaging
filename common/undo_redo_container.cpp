/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 jp.charras at wanadoo.fr
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <fctsys.h>
#include <base_struct.h>
#include <undo_redo_container.h>


ITEM_PICKER::ITEM_PICKER( EDA_ITEM* aItem, UNDO_REDO_T aUndoRedoStatus )
{
    m_undoRedoStatus = aUndoRedoStatus;
    SetItem( aItem );
    m_pickerFlags = 0;
    m_link = NULL;
}


PICKED_ITEMS_LIST::PICKED_ITEMS_LIST()
{
    m_Status = UR_UNSPECIFIED;
}

PICKED_ITEMS_LIST::~PICKED_ITEMS_LIST()
{
}


void PICKED_ITEMS_LIST::PushItem( const ITEM_PICKER& aItem )
{
    m_ItemsList.push_back( aItem );
}


ITEM_PICKER PICKED_ITEMS_LIST::PopItem()
{
    ITEM_PICKER item;

    if( m_ItemsList.size() != 0 )
    {
        item = m_ItemsList.back();
        m_ItemsList.pop_back();
    }

    return item;
}


bool PICKED_ITEMS_LIST::ContainsItem( const EDA_ITEM* aItem ) const
{
    for( size_t i = 0;  i < m_ItemsList.size();  i++ )
    {
        if( m_ItemsList[ i ].GetItem() == aItem )
            return true;
    }

    return false;
}


int PICKED_ITEMS_LIST::FindItem( const EDA_ITEM* aItem ) const
{
    for( size_t i = 0; i < m_ItemsList.size(); i++ )
    {
        if( m_ItemsList[i].GetItem() == aItem )
            return i;
    }

    return -1;
}


void PICKED_ITEMS_LIST::ClearItemsList()
{
    m_ItemsList.clear();
}


void PICKED_ITEMS_LIST::ClearListAndDeleteItems()
{
    // Delete items is they are not flagged UR_NEW, or if this is a block operation
    while( GetCount() > 0 )
    {
        ITEM_PICKER wrapper = PopItem();
        if( wrapper.GetItem() == NULL ) // No more item in list.
            break;

        // The Link is an undo construct; it is always owned by the undo/redo container
        if( wrapper.GetLink() )
            delete wrapper.GetLink();

        if( wrapper.GetFlags() & UR_TRANSIENT )
        {
            delete wrapper.GetItem();
        }
        else if( wrapper.GetStatus() == UR_WIRE_IMAGE )
        {
            // Specific to eeschema: a linked list of wires is stored.  The wrapper picks only
            // the first item (head of list), and is owner of all picked items.
            EDA_ITEM* item = wrapper.GetItem();

            while( item )
            {
                // Delete old copy of wires
                EDA_ITEM* nextitem = item->Next();
                delete item;
                item = nextitem;
            }
        }
        else if( wrapper.GetStatus() == UR_DELETED )
        {
            // This should really be replaced with UR_TRANSIENT, but currently many clients
            // (eeschema in particular) abuse this to achieve non-undo-related deletions.
            delete wrapper.GetItem();
        }
    }
}


ITEM_PICKER PICKED_ITEMS_LIST::GetItemWrapper( unsigned int aIdx ) const
{
    ITEM_PICKER picker;

    if( aIdx < m_ItemsList.size() )
        picker = m_ItemsList[aIdx];

    return picker;
}


EDA_ITEM* PICKED_ITEMS_LIST::GetPickedItem( unsigned int aIdx ) const
{
    if( aIdx < m_ItemsList.size() )
        return m_ItemsList[aIdx].GetItem();

    return NULL;
}


EDA_ITEM* PICKED_ITEMS_LIST::GetPickedItemLink( unsigned int aIdx ) const
{
    if( aIdx < m_ItemsList.size() )
        return m_ItemsList[aIdx].GetLink();

    return NULL;
}


UNDO_REDO_T PICKED_ITEMS_LIST::GetPickedItemStatus( unsigned int aIdx ) const
{
    if( aIdx < m_ItemsList.size() )
        return m_ItemsList[aIdx].GetStatus();

    return UR_UNSPECIFIED;
}


STATUS_FLAGS PICKED_ITEMS_LIST::GetPickerFlags( unsigned aIdx ) const
{
    if( aIdx < m_ItemsList.size() )
        return m_ItemsList[aIdx].GetFlags();

    return 0;
}


bool PICKED_ITEMS_LIST::SetPickedItem( EDA_ITEM* aItem, unsigned aIdx )
{
    if( aIdx < m_ItemsList.size() )
    {
        m_ItemsList[aIdx].SetItem( aItem );
        return true;
    }

    return false;
}


bool PICKED_ITEMS_LIST::SetPickedItemLink( EDA_ITEM* aLink, unsigned aIdx )
{
    if( aIdx < m_ItemsList.size() )
    {
        m_ItemsList[aIdx].SetLink( aLink );
        return true;
    }

    return false;
}


bool PICKED_ITEMS_LIST::SetPickedItem( EDA_ITEM* aItem, UNDO_REDO_T aStatus, unsigned aIdx )
{
    if( aIdx < m_ItemsList.size() )
    {
        m_ItemsList[aIdx].SetItem( aItem );
        m_ItemsList[aIdx].SetStatus( aStatus );
        return true;
    }

    return false;
}


bool PICKED_ITEMS_LIST::SetPickedItemStatus( UNDO_REDO_T aStatus, unsigned aIdx )
{
    if( aIdx < m_ItemsList.size() )
    {
        m_ItemsList[aIdx].SetStatus( aStatus );
        return true;
    }

    return false;
}


bool PICKED_ITEMS_LIST::SetPickerFlags( STATUS_FLAGS aFlags, unsigned aIdx )
{
    if( aIdx < m_ItemsList.size() )
    {
        m_ItemsList[aIdx].SetFlags( aFlags );
        return true;
    }

    return false;
}


bool PICKED_ITEMS_LIST::RemovePicker( unsigned aIdx )
{
    if( aIdx >= m_ItemsList.size() )
        return false;

    m_ItemsList.erase( m_ItemsList.begin() + aIdx );
    return true;
}


void PICKED_ITEMS_LIST::CopyList( const PICKED_ITEMS_LIST& aSource )
{
    m_ItemsList = aSource.m_ItemsList;  // Vector's copy
}


void PICKED_ITEMS_LIST::ReversePickersListOrder()
{
    std::vector <ITEM_PICKER> tmp;
    while( !m_ItemsList.empty() )
    {
        tmp.push_back( m_ItemsList.back() );
        m_ItemsList.pop_back();
    }

    m_ItemsList.swap( tmp );
}


/**********************************************/
/********** UNDO_REDO_CONTAINER ***************/
/**********************************************/

UNDO_REDO_CONTAINER::UNDO_REDO_CONTAINER()
{
}


UNDO_REDO_CONTAINER::~UNDO_REDO_CONTAINER()
{
    ClearCommandList();
}


void UNDO_REDO_CONTAINER::ClearCommandList()
{
    for( unsigned ii = 0; ii < m_CommandsList.size(); ii++ )
        delete m_CommandsList[ii];

    m_CommandsList.clear();
}


void UNDO_REDO_CONTAINER::PushCommand( PICKED_ITEMS_LIST* aItem )
{
    m_CommandsList.push_back( aItem );
}


PICKED_ITEMS_LIST* UNDO_REDO_CONTAINER::PopCommand()
{
    if( m_CommandsList.size() != 0 )
    {
        PICKED_ITEMS_LIST* item = m_CommandsList.back();
        m_CommandsList.pop_back();
        return item;
    }

    return NULL;
}
