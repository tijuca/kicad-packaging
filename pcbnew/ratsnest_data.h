/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2013-2015 CERN
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

/**
 * @file ratsnest_data.h
 * @brief Class that computes missing connections on a PCB.
 */

#ifndef RATSNEST_DATA_H
#define RATSNEST_DATA_H

#include <core/typeinfo.h>
#include <math/box2.h>

#include <deque>
#include <unordered_set>
#include <unordered_map>

#include <ttl/halfedge/hetriang.h>
#include <ttl/halfedge/hetraits.h>

#include <connectivity/connectivity_algo.h>

class BOARD;
class BOARD_ITEM;
class BOARD_CONNECTED_ITEM;
class CN_CLUSTER;
class CN_CONNECTIVITY_ALGO;

struct RN_NODE_OR_FILTER;
struct RN_NODE_AND_FILTER;


/**
 * Class RN_NET
 * Describes ratsnest for a single net.
 */
class RN_NET
{
public:
    ///> Default constructor.
    RN_NET();

    /**
     * Function SetVisible()
     * Sets state of the visibility flag.
     * @param aEnabled is new state. True if ratsnest for a given net is meant to be displayed,
     * false otherwise.
     */
    void SetVisible( bool aEnabled );

    /**
     * Function MarkDirty()
     * Marks ratsnest for given net as 'dirty', i.e. requiring recomputation.
     */
    void MarkDirty()
    {
        m_dirty = true;
    }

    /**
     * Function IsDirty()
     * Returns state of the 'dirty' flag, indicating that ratsnest for a given net is invalid
     * and requires an update.
     * @return True if ratsnest requires recomputation, false otherwise.
     */
    bool IsDirty() const
    {
        return m_dirty;
    }

    /**
     * Function GetUnconnected()
     * Returns pointer to a vector of edges that makes ratsnest for a given net.
     * @return Pointer to a vector of edges that makes ratsnest for a given net.
     */
    const std::vector<CN_EDGE> GetUnconnected() const
    {
        return m_rnEdges;
    }

    /**
     * Function Update()
     * Recomputes ratsnest for a net.
     */
    void Update();
    void Clear();

    void AddCluster( std::shared_ptr<CN_CLUSTER> aCluster );

    unsigned int GetNodeCount() const
    {
        return m_nodes.size();
    }

    /**
     * Function GetNodes()
     * Returns list of nodes that are associated with a given item.
     * @param aItem is an item for which the list is generated.
     * @return List of associated nodes.
     */
    std::list<CN_ANCHOR_PTR> GetNodes( const BOARD_CONNECTED_ITEM* aItem ) const;

    const std::vector<CN_EDGE>& GetEdges() const
    {
        return m_rnEdges;
    }

    /**
     * Function GetAllItems()
     * Adds all stored items to a list.
     * @param aOutput is the list that will have items added.
     * @param aTypes determines the type of added items.
     */
    void GetAllItems( std::list<BOARD_CONNECTED_ITEM*>& aOutput, const KICAD_T aTypes[] ) const;

    /**
     * Function GetClosestNode()
     * Returns a single node that lies in the shortest distance from a specific node.
     * @param aNode is the node for which the closest node is searched.
     */
    const CN_ANCHOR_PTR GetClosestNode( const CN_ANCHOR_PTR& aNode ) const;

    bool NearestBicoloredPair( const RN_NET& aOtherNet, CN_ANCHOR_PTR& aNode1, CN_ANCHOR_PTR& aNode2 ) const;

protected:
    ///> Recomputes ratsnest from scratch.
    void compute();

    ///> Vector of nodes
    std::vector<CN_ANCHOR_PTR> m_nodes;

    ///> Vector of edges that make pre-defined connections
    std::vector<CN_EDGE> m_boardEdges;

    ///> Vector of edges that makes ratsnest for a given net.
    std::vector<CN_EDGE> m_rnEdges;

    ///> Flag indicating necessity of recalculation of ratsnest for a net.
    bool m_dirty;

    class TRIANGULATOR_STATE;

    std::shared_ptr<TRIANGULATOR_STATE> m_triangulator;
};

#endif /* RATSNEST_DATA_H */
