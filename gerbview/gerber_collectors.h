/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GERBER_COLLECTORS_H
#define GERBER_COLLECTORS_H

#include <collector.h>

/**
 * Class GERBER_COLLECTOR
 * is intended for use when the right click button is pressed, or when the
 * plain "arrow" tool is in effect.
 */
class GERBER_COLLECTOR : public COLLECTOR
{
protected:
    /**
     * A place to hold collected objects which don't match precisely the search
     * criteria, but would be acceptable if nothing else is found.
     * "2nd" choice, which will be appended to the end of COLLECTOR's prime
     * "list" at the end of the search.
     */
    std::vector<EDA_ITEM*> m_List2nd;


    /**
     * Determines which items are to be collected by Inspect()
     */
    //const COLLECTORS_GUIDE* m_Guide;


    /**
     * The number of items that were originally in the primary list before the
     * m_List2nd was concatenated onto the end of it.
     */
    int m_PrimaryLength;


public:

    /**
     * A scan list for all selectable gerber items
     */
    static const KICAD_T AllItems[];

    GERBER_COLLECTOR()
    {
        //m_Guide = NULL;
        m_PrimaryLength = 0;
        SetScanTypes( AllItems );
    }

    void Empty2nd()
    {
        m_List2nd.clear();
    }

    /*void Append2nd( BOARD_ITEM* item )
    {
        m_List2nd.push_back( item );
    }*/


    /**
     * Function SetGuide
     * records which COLLECTORS_GUIDE to use.
     * @param aGuide Which guide to use in the collection.
     */
    //void SetGuide( const COLLECTORS_GUIDE* aGuide ) { m_Guide = aGuide; }


    /**
     * Function operator[int]
     * overloads COLLECTOR::operator[](int) to return a EDA_ITEM* instead of
     * an EDA_ITEM* type.
     * @param ndx The index into the list.
     * @return EDA_ITEM* - or something derived from it, or NULL.
     */
    EDA_ITEM* operator[]( int ndx ) const
    {
        if( (unsigned)ndx < (unsigned)GetCount() )
            return (EDA_ITEM*) m_List[ ndx ];
        return NULL;
    }

    /**
     * Function GetPrimaryCount
     * @return int - The number if items which met the primary search criteria
     */
    int GetPrimaryCount() { return m_PrimaryLength; }

    /**
     * Function Inspect
     * is the examining function within the INSPECTOR which is passed to the
     * Iterate function.
     *
     * @param testItem An EDA_ITEM to examine.
     * @param testData is not used in this class.
     * @return SEARCH_RESULT - SEARCH_QUIT if the Iterator is to stop the scan,
     *   else SCAN_CONTINUE;
     */
    SEARCH_RESULT Inspect( EDA_ITEM* testItem, void* testData )  override;

    /**
     * Function Collect
     * scans an EDA_ITEM using this class's Inspector method, which does the collection.
     * @param aItem An EDA_ITEM to scan
     * @param aScanList A list of KICAD_Ts with a terminating EOT, that specs
     *  what is to be collected and the priority order of the resultant
     *  collection in "m_List".
     * @param aRefPos A wxPoint to use in hit-testing.
     * @param aGuide The COLLECTORS_GUIDE to use in collecting items.
     */
    void Collect( EDA_ITEM* aItem, const KICAD_T aScanList[],
                 const wxPoint& aRefPos/*, const COLLECTORS_GUIDE& aGuide */);
};

#endif
