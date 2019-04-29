/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#include <unit_test_utils/unit_test_utils.h>

#include <wildcards_and_files_ext.h>


BOOST_AUTO_TEST_SUITE( WildcardFileExt )


/**
 * Data used to construct a simple test of one or more extensions
 * and get a filter string for WX dialogs out
 */
struct ExtWildcardFilterCase
{
    /// The list of exts handled
    std::vector<std::string> m_exts;

    /// Filter for case-insensitive environments
    std::string m_filter_case_insenstive;

    /// Filter for regex-capable environments (case insensitive filter in a
    /// case-sensitive environment)
    std::string m_re_filter;
};

const static std::vector<ExtWildcardFilterCase> ext_wildcard_cases = {
    {
            { "png" },
            " ( *.png)|*.png",
            " ( *.png)|*.[pP][nN][gG]",
    },
    {
            { "png", "gif" },
            " ( *.png *.gif)|*.png;*.gif",
            " ( *.png *.gif)|*.[pP][nN][gG];*.[gG][iI][fF]",
    },
};


static constexpr bool should_use_regex_filters()
{
#ifdef __WXGTK__
    return true;
#else
    return false;
#endif
}


/**
 * Check correct handling of filter strings (as used by WX)
 */
BOOST_AUTO_TEST_CASE( BasicFilter )
{
    for( const auto& c : ext_wildcard_cases )
    {
        const std::string exp_filter =
                should_use_regex_filters() ? c.m_re_filter : c.m_filter_case_insenstive;

        const auto resp = AddFileExtListToFilter( c.m_exts );

        BOOST_CHECK_EQUAL( resp, exp_filter );
    }
}

static constexpr bool should_use_windows_filters()
{
#ifdef __WXMSW__
    return true;
#else
    return false;
#endif
}

BOOST_AUTO_TEST_CASE( AllFilesFilter )
{
    const auto resp = AddFileExtListToFilter( {} );

    const std::string exp_filter = should_use_windows_filters() ? " (*.*)|*.*" : " (*)|*";

    BOOST_CHECK_EQUAL( resp, exp_filter );
}

BOOST_AUTO_TEST_SUITE_END()
