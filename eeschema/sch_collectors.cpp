/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2004-2018 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file sch_collectors.cpp
 */

#include <macros.h>
#include <trace_helpers.h>

#include <sch_sheet_path.h>
#include <transform.h>
#include <sch_collectors.h>
#include <sch_component.h>
#include <sch_line.h>
#include <sch_bus_entry.h>


const KICAD_T SCH_COLLECTOR::AllItems[] = {
    SCH_MARKER_T,
    SCH_JUNCTION_T,
    SCH_NO_CONNECT_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    SCH_LINE_T,
    SCH_BITMAP_T,
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_FIELD_T,
    SCH_COMPONENT_T,
    LIB_PIN_T,
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::AllItemsButPins[] = {
    SCH_MARKER_T,
    SCH_JUNCTION_T,
    SCH_NO_CONNECT_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    SCH_LINE_T,
    SCH_BITMAP_T,
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_FIELD_T,
    SCH_COMPONENT_T,
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::EditableItems[] = {
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_FIELD_T,
    SCH_COMPONENT_T,
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    SCH_BITMAP_T,
    SCH_LINE_T,
    EOT
};

const KICAD_T SCH_COLLECTOR::CmpFieldValueOnly[] = {
    SCH_FIELD_LOCATE_VALUE_T,
    EOT
};

const KICAD_T SCH_COLLECTOR::CmpFieldReferenceOnly[] = {
    SCH_FIELD_LOCATE_REFERENCE_T,
    EOT
};

const KICAD_T SCH_COLLECTOR::CmpFieldFootprintOnly[] = {
        SCH_FIELD_LOCATE_FOOTPRINT_T,
        EOT
};

const KICAD_T SCH_COLLECTOR::CmpFieldDatasheetOnly[] = {
        SCH_FIELD_LOCATE_DATASHEET_T,
        EOT
};


const KICAD_T SCH_COLLECTOR::MovableItems[] = {
    SCH_MARKER_T,
    SCH_JUNCTION_T,
    SCH_NO_CONNECT_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
//    SCH_LINE_T,
    SCH_BITMAP_T,
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_FIELD_T,
    SCH_COMPONENT_T,
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::DraggableItems[] = {
    SCH_JUNCTION_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    SCH_LINE_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_COMPONENT_T,
    SCH_SHEET_T,
    SCH_TEXT_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::RotatableItems[] = {
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_FIELD_T,
    SCH_COMPONENT_T,
    SCH_SHEET_T,
    SCH_BITMAP_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::ParentItems[] = {
    SCH_MARKER_T,
    SCH_JUNCTION_T,
    SCH_NO_CONNECT_T,
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    SCH_LINE_T,
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_COMPONENT_T,
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    SCH_BITMAP_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::ComponentsOnly[] = {
    SCH_COMPONENT_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::SheetsOnly[] = {
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::SheetsAndSheetLabels[] = {
    SCH_SHEET_PIN_T,
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::OrientableItems[] = {
    SCH_BUS_BUS_ENTRY_T,
    SCH_BUS_WIRE_ENTRY_T,
    SCH_COMPONENT_T,
    SCH_BITMAP_T,
    SCH_SHEET_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::CopyableItems[] = {
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_COMPONENT_T,
    EOT
};


const KICAD_T SCH_COLLECTOR::DoubleClickItems[] = {
    SCH_TEXT_T,
    SCH_LABEL_T,
    SCH_GLOBAL_LABEL_T,
    SCH_HIERARCHICAL_LABEL_T,
    SCH_COMPONENT_T,
    SCH_SHEET_T,
    SCH_BITMAP_T,
    SCH_FIELD_T,
    SCH_MARKER_T,
    EOT
};


SEARCH_RESULT SCH_COLLECTOR::Inspect( EDA_ITEM* aItem, void* aTestData )
{
    if( aItem->Type() != LIB_PIN_T && !aItem->HitTest( m_RefPos ) )
        return SEARCH_CONTINUE;

    // Pins have special hit testing requirements that are relative to their parent
    // SCH_COMPONENT item.
    if( aItem->Type() == LIB_PIN_T )
    {
        wxCHECK_MSG( aTestData && ( (EDA_ITEM*) aTestData )->Type() == SCH_COMPONENT_T,
                     SEARCH_CONTINUE, wxT( "Cannot inspect invalid data.  Bad programmer!" ) );

        // Pin hit testing is relative to the components position and orientation in the
        // schematic.  The hit test position must be converted to library coordinates.
        SCH_COMPONENT* component = (SCH_COMPONENT*) aTestData;
        TRANSFORM transform = component->GetTransform().InverseTransform();
        wxPoint position = transform.TransformCoordinate( m_RefPos - component->GetPosition() );

        position.y *= -1;   // Y axis polarity in schematic is inverted from library.

        if( !aItem->HitTest( position ) )
            return SEARCH_CONTINUE;
    }

    Append( aItem );

    return SEARCH_CONTINUE;
}


void SCH_COLLECTOR::Collect( SCH_ITEM* aItem, const KICAD_T aFilterList[],
                             const wxPoint& aPosition )
{
    Empty();        // empty the collection just in case

    SetScanTypes( aFilterList );

    // remember where the snapshot was taken from and pass refPos to the Inspect() function.
    SetRefPos( aPosition );

    EDA_ITEM::IterateForward( aItem, m_inspector, NULL, m_ScanTypes );
}


bool SCH_COLLECTOR::IsCorner() const
{
    if( GetCount() != 2 )
        return false;

    bool is_busentry0 = (dynamic_cast<SCH_BUS_ENTRY_BASE*>( m_List[0] ) != NULL);
    bool is_busentry1 = (dynamic_cast<SCH_BUS_ENTRY_BASE*>( m_List[1] ) != NULL);

    if( (m_List[0]->Type() == SCH_LINE_T) && (m_List[1]->Type() == SCH_LINE_T) )
        return ( ( SCH_LINE* ) m_List[0])->GetLayer() == ( ( SCH_LINE* ) m_List[1])->GetLayer();

    if( (m_List[0]->Type() == SCH_LINE_T) && is_busentry1 )
        return true;

    if( is_busentry0 && (m_List[1]->Type() == SCH_LINE_T) )
        return true;

    return false;
}


bool SCH_COLLECTOR::IsNode( bool aIncludePins ) const
{
    for( size_t i = 0;  i < m_List.size();  i++ )
    {
        SCH_ITEM* item = (SCH_ITEM*) m_List[ i ];
        KICAD_T type = item->Type();

        if( type == SCH_JUNCTION_T )
            continue;

        if( type == SCH_LINE_T )
        {
            if( item->GetLayer() != LAYER_WIRE )
                return false;

            continue;
        }

        if( type == LIB_PIN_T )
        {
            if( !aIncludePins )
                return false;

            continue;
        }

        // Any other item types indicate that this collection is not a node.
        return false;
    }

    return true;
}


bool SCH_COLLECTOR::IsDraggableJunction() const
{
    for( size_t i = 0;  i < m_List.size();  i++ )
        if( ( (SCH_ITEM*) m_List[ i ] )->Type() == SCH_JUNCTION_T )
            return true;

    return false;
}


/**
 * A singleton item of this class is returned for a weak reference that no longer exists.
 * Its sole purpose is to flag the item as having been deleted.
 */
class DELETED_SCH_ITEM : public SCH_ITEM
{
public:
    DELETED_SCH_ITEM() :
        SCH_ITEM( nullptr, NOT_USED )
    {}

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override
    {
        return _( "(Deleted Item)" );
    }
    wxString GetClass() const override
    {
        return wxT( "DELETED_SCH_ITEM" );
    }

    // define pure virtuals:
    wxPoint GetPosition() const override { return wxPoint(); }
    void SetPosition( const wxPoint& ) override {}
    void Draw( EDA_DRAW_PANEL* , wxDC* , const wxPoint& , GR_DRAWMODE , COLOR4D ) override {}

#if defined(DEBUG)
    void Show( int , std::ostream&  ) const override {}
#endif

    void Move( const wxPoint&  ) override {}
    void MirrorY( int  ) override {}
    void MirrorX( int  ) override {}
    void Rotate( wxPoint  ) override {}
};


DELETED_SCH_ITEM g_DeletedSchItem;


SCH_ITEM* SCH_FIND_COLLECTOR::GetItem( int ndx ) const
{
    if( (unsigned)ndx >= (unsigned)GetCount() )
        return NULL;

    // Do not simply return m_List[ ndx ] as it might have been deleted.  Instead
    // treat it as a weak reference and search the sheets for an item with the same
    // pointer value.

    void*     weakRef = m_List[ ndx ];
    SCH_ITEM* item    = &g_DeletedSchItem;

    INSPECTOR_FUNC inspector = [&] ( EDA_ITEM* candidate, void* testData )
    {
        if( (void*) candidate == weakRef )
        {
            item = (SCH_ITEM*) candidate;
            return SEARCH_QUIT;
        }

        return SEARCH_CONTINUE;
    };

    for( unsigned i = 0; i < m_sheetPaths.size(); i++ )
    {
        EDA_ITEM::IterateForward( m_sheetPaths[ i ].LastDrawList(),
                                  inspector, nullptr, SCH_COLLECTOR::AllItems );
    }

    return item;
}


SCH_ITEM* SCH_FIND_COLLECTOR::operator[]( int ndx ) const
{
    return GetItem( ndx );
}


bool SCH_FIND_COLLECTOR::PassedEnd() const
{
    bool retv = false;

    wxUint32 flags = m_findReplaceData.GetFlags();

    if( GetCount() == 0 )
        return true;

    if( !(flags & FR_SEARCH_WRAP) || (flags & FR_SEARCH_REPLACE) )
    {
        if( flags & wxFR_DOWN )
        {
            if( m_foundIndex >= GetCount() )
                retv = true;
        }
        else
        {
            if( m_foundIndex < 0 )
                retv = true;
        }
    }

    return retv;
}


#if defined(DEBUG)

void SCH_FIND_COLLECTOR::dump()
{
    int tmp = m_foundIndex;

    wxLogTrace( traceFindReplace, wxT( "%d items found to replace %s with %s." ),
                GetCount(), GetChars( m_findReplaceData.GetFindString() ),
                GetChars( m_findReplaceData.GetReplaceString() ) );

    for( m_foundIndex = 0;  m_foundIndex < GetCount();  m_foundIndex++ )
        wxLogTrace( traceFindReplace, wxT( "    " ) + GetText( MILLIMETRES ) );

    m_foundIndex = tmp;
}

#endif


void SCH_FIND_COLLECTOR::UpdateIndex()
{
    wxUint32 flags = m_findReplaceData.GetFlags();

    if( flags & wxFR_DOWN )
    {
        if( m_foundIndex < GetCount() )
            m_foundIndex += 1;
        if( (m_foundIndex >= GetCount()) && (flags & FR_SEARCH_WRAP) )
            m_foundIndex = 0;
    }
    else
    {
        if( m_foundIndex >= 0 )
            m_foundIndex -= 1;
        if( (m_foundIndex < 0) && (flags & FR_SEARCH_WRAP) )
            m_foundIndex = GetCount() - 1;
    }
}


SCH_FIND_COLLECTOR_DATA SCH_FIND_COLLECTOR::GetFindData( int aIndex )
{
    wxCHECK_MSG( (unsigned) aIndex < m_data.size(), SCH_FIND_COLLECTOR_DATA(),
                 wxT( "Attempt to get find data outside of list boundary." ) );

    return m_data[ aIndex ];
}


wxString SCH_FIND_COLLECTOR::GetText( EDA_UNITS_T aUnits )
{
    wxCHECK_MSG( (GetCount() != 0) && IsValidIndex( m_foundIndex ), wxEmptyString,
                 wxT( "Cannot get found item at invalid index." ) );

    SCH_FIND_COLLECTOR_DATA data = m_data[ m_foundIndex ];
    EDA_ITEM* foundItem = GetItem( m_foundIndex );

    if( data.GetParent() )
    {
        return wxString::Format( _( "Match %i of %i: %s of %s in sheet %s" ),
                                 m_foundIndex + 1,
                                 GetCount(),
                                 foundItem->GetSelectMenuText( aUnits ),
                                 data.GetParent()->GetSelectMenuText( aUnits ),
                                 data.GetSheetPath() );
    }
    else
    {
        return wxString::Format( _( "Match %i of %i: %s in sheet %s" ),
                                 m_foundIndex + 1,
                                 GetCount(),
                                 foundItem->GetSelectMenuText( aUnits ),
                                 data.GetSheetPath() );
    }
}


EDA_ITEM* SCH_FIND_COLLECTOR::GetItem( SCH_FIND_COLLECTOR_DATA& aData )
{
    if( PassedEnd() )
        return NULL;

    aData = m_data[ m_foundIndex ];
    return GetItem( m_foundIndex );
}


bool SCH_FIND_COLLECTOR::ReplaceItem( SCH_SHEET_PATH* aSheetPath )
{
    if( PassedEnd() )
        return false;

    wxCHECK_MSG( IsValidIndex( m_foundIndex ), false,
                 wxT( "Invalid replace list index in SCH_FIND_COLLECTOR." ) );

    EDA_ITEM* item = GetItem( m_foundIndex );

    bool replaced = item->Replace( m_findReplaceData, aSheetPath );

    return replaced;
}


SEARCH_RESULT SCH_FIND_COLLECTOR::Inspect( EDA_ITEM* aItem, void* aTestData )
{
    wxPoint position;

    if( aItem->Matches( m_findReplaceData, m_currentSheetPath, &position ) )
    {
        if( aItem->Type() == LIB_PIN_T )
        {
            wxCHECK_MSG( aTestData && ( (EDA_ITEM*) aTestData )->Type() == SCH_COMPONENT_T,
                         SEARCH_CONTINUE, wxT( "Cannot inspect invalid data.  Bad programmer!" ) );

            // Pin positions are relative to their parent component's position and
            // orientation in the schematic.  The pin's position must be converted
            // schematic coordinates.
            SCH_COMPONENT* component = (SCH_COMPONENT*) aTestData;
            TRANSFORM transform = component->GetTransform();
            position.y = -position.y;
            position = transform.TransformCoordinate( position ) + component->GetPosition();
        }

        Append( aItem );
        m_data.push_back( SCH_FIND_COLLECTOR_DATA( position,
                                                   m_currentSheetPath->PathHumanReadable(),
                                                   (SCH_ITEM*) aTestData ) );
    }

    return SEARCH_CONTINUE;
}


void SCH_FIND_COLLECTOR::SetReplaceString( const wxString &aReplaceString )
{
    m_findReplaceData.SetReplaceString( aReplaceString );
}


void SCH_FIND_COLLECTOR::Collect( SCH_FIND_REPLACE_DATA& aFindReplaceData,
                                  SCH_SHEET_PATH* aSheetPath )
{
    if( !IsSearchRequired( aFindReplaceData ) && !m_List.empty() && !m_forceSearch )
        return;

    m_findReplaceData = aFindReplaceData;
    Empty();                 // empty the collection just in case
    m_data.clear();
    m_foundIndex = 0;
    m_sheetPaths.clear();
    SetForceSearch( false );

    if( aSheetPath )
    {
        m_currentSheetPath = aSheetPath;
        m_sheetPaths.push_back( *m_currentSheetPath );
        EDA_ITEM::IterateForward( aSheetPath->LastDrawList(), m_inspector, NULL, m_ScanTypes );
    }
    else
    {
        SCH_SHEET_LIST schematic( g_RootSheet );

        for( unsigned i = 0; i < schematic.size(); i++ )
        {
            m_currentSheetPath = &schematic[i];
            m_sheetPaths.push_back( *m_currentSheetPath );
            EDA_ITEM::IterateForward( m_currentSheetPath->LastDrawList(), m_inspector, NULL, m_ScanTypes );
        }
    }

#if defined(DEBUG)
    dump();
#endif

    if( m_List.size() != m_data.size() )
    {
        wxFAIL_MSG( wxT( "List size mismatch." ) );
        m_List.clear();
        m_data.clear();
    }
}


SEARCH_RESULT SCH_TYPE_COLLECTOR::Inspect( EDA_ITEM* aItem, void* testData )
{
    // The Vist() function only visits the testItem if its type was in the
    // the scanList, so therefore we can collect anything given to us here.
    Append( aItem );

    return SEARCH_CONTINUE;
}


void SCH_TYPE_COLLECTOR::Collect( SCH_ITEM* aItem, const KICAD_T aFilterList[] )
{
    Empty();        // empty the collection

    SetScanTypes( aFilterList );

    EDA_ITEM::IterateForward( aItem, m_inspector, NULL, m_ScanTypes );
}
