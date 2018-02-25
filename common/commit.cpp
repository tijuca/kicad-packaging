/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright 2016-2017 CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#include <algorithm>

#include <commit.h>
#include <base_struct.h>

COMMIT::COMMIT()
{
}


COMMIT::~COMMIT()
{
    for( COMMIT_LINE& ent : m_changes )
    {
        if( ent.m_copy )
            delete ent.m_copy;
    }
}


COMMIT& COMMIT::Stage( EDA_ITEM* aItem, CHANGE_TYPE aChangeType )
{
    // CHT_MODIFY and CHT_DONE are not compatible
    assert( ( aChangeType & ( CHT_MODIFY | CHT_DONE ) ) != ( CHT_MODIFY | CHT_DONE ) );

    int flag = aChangeType & CHT_FLAGS;

    switch( aChangeType & CHT_TYPE )
    {
        case CHT_ADD:
            assert( m_changedItems.find( aItem ) == m_changedItems.end() );
            makeEntry( aItem, CHT_ADD | flag );
            return *this;

        case CHT_REMOVE:
            makeEntry( aItem, CHT_REMOVE | flag );
            return *this;

        case CHT_MODIFY:
        {
            EDA_ITEM* parent = parentObject( aItem );
            return createModified( parent, parent->Clone(), flag );
        }

        default:
            assert( false );
    }

    return *this;
}


COMMIT& COMMIT::Stage( std::vector<EDA_ITEM*>& container, CHANGE_TYPE aChangeType )
{
    for( EDA_ITEM* item : container )
    {
        Stage( item, aChangeType );
    }

    return *this;
}


COMMIT& COMMIT::Stage( const PICKED_ITEMS_LIST& aItems, UNDO_REDO_T aModFlag )
{
    for( unsigned int i = 0; i < aItems.GetCount(); i++ )
    {
        UNDO_REDO_T change_type = aItems.GetPickedItemStatus( i );
        EDA_ITEM* item = aItems.GetPickedItem( i );
        EDA_ITEM* copy = NULL;

        if( change_type == UR_UNSPECIFIED )
            change_type = aItems.m_Status;

        if( change_type == UR_UNSPECIFIED )
            change_type = aModFlag;

        if( ( copy = aItems.GetPickedItemLink( i ) ) )
        {
            assert( change_type == UR_CHANGED );

            // There was already a copy created, so use it
            Modified( item, copy );
        }
        else
        {
            Stage( item, convert( change_type ) );
        }
    }

    return *this;
}


int COMMIT::GetStatus( EDA_ITEM* aItem )
{
    COMMIT_LINE* entry = findEntry( parentObject( aItem ) );

    return entry ? entry->m_type : 0;
}


template <class Container, class F>
void eraseIf( Container& c, F&& f )
{
    c.erase( std::remove_if( c.begin(),
                    c.end(),
                    std::forward<F>( f ) ),
            c.end() );
}


COMMIT& COMMIT::createModified( EDA_ITEM* aItem, EDA_ITEM* aCopy, int aExtraFlags )
{
    EDA_ITEM* parent = parentObject( aItem );
    auto entryIt = m_changedItems.find( parent );

    if( entryIt != m_changedItems.end() )
        return *this; // item has been already modified once

    makeEntry( parent, CHT_MODIFY | aExtraFlags, aCopy );

    return *this;
}


void COMMIT::makeEntry( EDA_ITEM* aItem, CHANGE_TYPE aType, EDA_ITEM* aCopy )
{
    // Expect an item copy if it is going to be modified
    assert( !!aCopy == ( ( aType & CHT_TYPE ) == CHT_MODIFY ) );

    if( m_changedItems.find( aItem ) != m_changedItems.end() )
    {
        eraseIf( m_changes, [aItem] ( const COMMIT_LINE& aEnt ) {
            return aEnt.m_item == aItem;
        } );
    }

    COMMIT_LINE ent;

    ent.m_item = aItem;
    ent.m_type = aType;
    ent.m_copy = aCopy;

    m_changedItems.insert( aItem );
    m_changes.push_back( ent );
}


COMMIT::COMMIT_LINE* COMMIT::findEntry( EDA_ITEM* aItem )
{
    for( COMMIT_LINE& change : m_changes )
    {
        if( change.m_item == aItem )
            return &change;
    }

    return nullptr;
}


CHANGE_TYPE COMMIT::convert( UNDO_REDO_T aType ) const
{
    switch( aType )
    {
    case UR_NEW:
        return CHT_ADD;

    case UR_DELETED:
        return CHT_REMOVE;

    default:
        assert( false );
        // fall through

    case UR_CHANGED:
    case UR_MOVED:
    case UR_MIRRORED_X:
    case UR_MIRRORED_Y:
    case UR_ROTATED:
    case UR_ROTATED_CLOCKWISE:
    case UR_FLIPPED:
        return CHT_MODIFY;
    }
}

