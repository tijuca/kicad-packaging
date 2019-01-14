/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see CHANGELOG.TXT for contributors.
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
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file dialog_hotkey_list.h
 * Hotkey list dialog (as opposed to editor)
 */

#ifndef DIALOG_HOTKEYS_LIST_H
#define DIALOG_HOTKEYS_LIST_H


#include <dialog_shim.h>

// Private forwards
class PANEL_HOTKEYS_EDITOR;


/**
 * A dialog that presents the user with a read-only list of hotkeys and
 * their current bindings.
 */
class DIALOG_LIST_HOTKEYS: public DIALOG_SHIM
{
public:

    /**
     * Construct a hotkey list dialog on the given frame, with a set of hotkeys
     *
     * @param aParent the parent frame
     * @param aDescList the list of hotkey sections (each of which has a list
     * of hotkeys) to display
     */
    DIALOG_LIST_HOTKEYS( EDA_BASE_FRAME* aParent,
        EDA_HOTKEY_CONFIG* aDescList );

protected:

    /**
     * Called on dialog initialisation - inits the dialog's own widgets
     */
    bool TransferDataToWindow() override;

private:

    PANEL_HOTKEYS_EDITOR* m_hk_list;
};

#endif // DIALOG_HOTKEYS_LIST_H