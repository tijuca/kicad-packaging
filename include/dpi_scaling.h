/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#ifndef DPI_SCALING__H
#define DPI_SCALING__H

#include <wx/config.h>
#include <wx/window.h>

/**
 * Class to handle configuration and automatic determination of the DPI
 * scale to use for canvases. This has several sources and the availability of
 * some of them are platform dependent.
 */
class DPI_SCALING
{
public:
    /**
     * Construct a DPI scale provider.
     *
     * @param aConfig the config store to check for a user value (can be nullptr,
     * in which case on automatically determined values are considered)
     * @param aWindow a WX window to use for automatic DPI determination
     * @return the scaling factor (1.0 = no scaling)
     */
    DPI_SCALING( wxConfigBase* aConfig, const wxWindow* aWindow );

    /**
     * Get the DPI scale from all known sources in order:
     *
     * * user config, if given
     * * user's environment variables, if set and according to platform
     * * WX's internal determination of the DPI scaling (WX > 3.1)
     */
    double GetScaleFactor() const;

    /**
     * Is the current value auto scaled, or is it user-set in the config
     */
    bool GetCanvasIsAutoScaled() const;

    /**
     * Set the common DPI config in a given config object
     *
     * The encoding of the automatic/manual nature of the config is handled internally.
     *
     * @param aAuto   store a value meaning "no user-set scale"
     * @param aValue  the value to store (ignored if aAuto set)
     */
    void SetDpiConfig( bool aAuto, double aValue );

    /*
     * Get the maximum scaling factor that should be presented to the user.
     * This is only advisory, it has no real technical use other than for validation.
     */
    static double GetMaxScaleFactor();

    /*
     * Get the minimum scaling factor that should be presented to the user.
     * This is only advisory, it has no real technical use other than for validation.
     */
    static double GetMinScaleFactor();

    /**
     * Get the "default" scaling factor to use if not other config is available
     */
    static double GetDefaultScaleFactor();

private:
    /**
     * The configuration object to use to get/set user setting. nullptr
     * if only automatic options are wanted
     */
    wxConfigBase* m_config;

    /**
     * The WX window to use for WX's automatic DPI checking
     */
    const wxWindow* m_window;
};

#endif // DPI_SCALING__H