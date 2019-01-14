/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014 CERN
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

#ifndef ALIGN_DISTRIBUTE_TOOL_H_
#define ALIGN_DISTRIBUTE_TOOL_H_

#include <tool/tool_interactive.h>
#include <class_board_item.h>
#include <pcb_base_frame.h>

using ALIGNMENT_RECT = std::pair<BOARD_ITEM*, EDA_RECT>;
using ALIGNMENT_RECTS = std::vector<ALIGNMENT_RECT>;

class SELECTION_TOOL;

class ALIGN_DISTRIBUTE_TOOL : public TOOL_INTERACTIVE
{
public:
    ALIGN_DISTRIBUTE_TOOL();
    virtual ~ALIGN_DISTRIBUTE_TOOL();

    /// @copydoc TOOL_INTERACTIVE::Reset()
    void Reset( RESET_REASON aReason ) override {}

    /// @copydoc TOOL_INTERACTIVE::Init()
    bool Init() override;

    /**
     * Function AlignTop()
     * Sets Y coordinate of the selected items to the value of the top-most selected item Y coordinate.
     */
    int AlignTop( const TOOL_EVENT& aEvent );

    /**
     * Function AlignBottom()
     * Sets Y coordinate of the selected items to the value of the bottom-most selected item Y coordinate.
     */
    int AlignBottom( const TOOL_EVENT& aEvent );

    /**
     * Function AlignLeft()
     * Sets X coordinate of the selected items to the value of the left-most selected item X coordinate.
     */
    int AlignLeft( const TOOL_EVENT& aEvent );

    /**
     * Function AlignRight()
     * Sets X coordinate of the selected items to the value of the right-most selected item X coordinate.
     */
    int AlignRight( const TOOL_EVENT& aEvent );

    /**
     * Function AlignCenterX()
     * Sets the x coordinate of the midpoint of each of the selected items to
     * the value of the x coordinate of the center of the middle selected item.
     */
    int AlignCenterX( const TOOL_EVENT& aEvent );

    /**
     * Function AlignCenterX()
     * Sets the y coordinate of the midpoint of each of the selected items to
     * the value of the y coordinate of the center of the middle selected item.
     */
    int AlignCenterY( const TOOL_EVENT& aEvent );

    /**
     * Function DistributeHorizontally()
     * Distributes the selected items along the X axis.
     */
    int DistributeHorizontally( const TOOL_EVENT& aEvent );

    /**
     * Function DistributeVertically()
     * Distributes the selected items along the Y axis.
     */
    int DistributeVertically( const TOOL_EVENT& aEvent );

    ///> Sets up handlers for various events.
    void setTransitions() override;

private:

    /**
     * Function GetSelections()
     * Populates two vectors with the sorted selection and sorted locked items
     * Returns the size of aItems()
     */
    template< typename T >
    size_t GetSelections( ALIGNMENT_RECTS& aItems, ALIGNMENT_RECTS& aLocked, T aCompare );

    template< typename T >
    int selectTarget( ALIGNMENT_RECTS& aItems, ALIGNMENT_RECTS& aLocked, T aGetValue );

    /**
     * Sets X coordinate of the selected items to the value of the left-most selected item X coordinate.
     *
     * NOTE: Uses the bounding box of items, which do not get mirrored even when
     * the view is mirrored!
     */
    int doAlignLeft();

    /**
     * Aligns selected items using the right edge of their bounding boxes to the right-most item
     *
     * NOTE: Uses the bounding box of items, which do not get mirrored even when
     * the view is mirrored!
     */
    int doAlignRight();

    SELECTION_TOOL* m_selectionTool;

    CONTEXT_MENU* m_placementMenu;

    PCB_BASE_FRAME* m_frame;

    /**
     * Check a selection to ensure locks are valid for alignment.
     *
     * This is slightly different from the standard lock checking in that we ignore the lock
     * of the first element in the selection as this is meant to be our anchor.
     * We also check the lock of a pad's parent as we will not move pads independently of
     * the parent module
     */
    int checkLockedStatus( const SELECTION &selection ) const;

    /**
     * Distributes selected items using an even spacing between the centers of their bounding boxes
     *
     * NOTE: Using the centers of bounding box of items can give unsatisfactory visual results since
     * items of differing widths will be placed with different gaps. Is only used if items overlap
     */
    void doDistributeCentersHorizontally( ALIGNMENT_RECTS &itemsToDistribute ) const;

    /**
     * Distributes selected items using an even spacing between the centers of their bounding boxes
     *
     * NOTE: Using the centers of bounding box of items can give unsatisfactory visual results since
     * items of differing widths will be placed with different gaps. Is only used if items overlap
     */
    void doDistributeCentersVertically( ALIGNMENT_RECTS &itemsToDistribute ) const;

    /**
     * Distributes selected items using an even spacing between their bounding boxes
     *
     * NOTE: Using the edges of bounding box of items is only possible if there is enough space
     * between them. If this is not the case, use the center spacing method
     */
    void doDistributeGapsHorizontally( ALIGNMENT_RECTS &itemsToDistribute,
                                       const BOARD_ITEM *lastItem,
                                       int totalGap ) const;

    /**
     * Distributes selected items using an even spacing between their bounding boxes
     *
     * NOTE: Using the edges of bounding box of items is only possible if there is enough space
     * between them. If this is not the case, use the center spacing method
     */
    void doDistributeGapsVertically( ALIGNMENT_RECTS &itemsToDistribute,
                                     const BOARD_ITEM *lastItem,
                                     int totalGap ) const;
};

#endif /* ALIGN_DISTRIBUTE_TOOL_H_ */
