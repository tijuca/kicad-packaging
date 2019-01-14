/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef FP_TREE_SYNCHRONIZING_ADAPTER_H
#define FP_TREE_SYNCHRONIZING_ADAPTER_H

#include <fp_tree_model_adapter.h>
#include <set>

class FOOTPRINT_EDIT_FRAME;

class FP_TREE_SYNCHRONIZING_ADAPTER : public FP_TREE_MODEL_ADAPTER
{
public:
    static PTR Create( FOOTPRINT_EDIT_FRAME* aFrame, FP_LIB_TABLE* aLibs );

    bool IsContainer( const wxDataViewItem& aItem ) const override;

    void Sync();

    int GetLibrariesCount() const override;

protected:
    FP_TREE_SYNCHRONIZING_ADAPTER( FOOTPRINT_EDIT_FRAME* aFrame, FP_LIB_TABLE* aLibs );

    void updateLibrary( LIB_TREE_NODE_LIB& aLibNode );

    LIB_TREE_NODE::PTR_VECTOR::iterator deleteLibrary( LIB_TREE_NODE::PTR_VECTOR::iterator& aLibNodeIt );

    void GetValue( wxVariant& aVariant, wxDataViewItem const& aItem,
                   unsigned int aCol ) const override;
    bool GetAttr( wxDataViewItem const& aItem, unsigned int aCol,
                  wxDataViewItemAttr& aAttr ) const override;

    FOOTPRINT_EDIT_FRAME*  m_frame;
    std::set<wxString>     m_libMap;   // Set to indicate libraries currently in tree
};

#endif /* FP_TREE_SYNCHRONIZING_ADAPTER_H */
