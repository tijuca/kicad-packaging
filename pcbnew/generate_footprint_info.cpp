/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Chris Pavlina <pavlina.chris@gmail.com>
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

#include <generate_footprint_info.h>
#include <kicad_string.h>
#include <class_module.h>
#include <fp_lib_table.h>


static const wxString DescriptionFormat =
    "<b>__NAME__</b>"
    "<br>__DESC__"
    "<hr><table border=0>"
    "__FIELDS__"
    "</table>";

static const wxString KeywordsFormat =
        "<tr>"
        "   <td><b>" + _( "Keywords" ) + "</b></td>"
        "   <td>__KEYWORDS__</td>"
        "</tr>";

static const wxString DocFormat =
        "<tr>"
        "   <td><b>" + _( "Documentation" ) + "</b></td>"
        "   <td><a href=\"__HREF__\">__TEXT__</a></td>"
        "</tr>";


class FOOTPRINT_INFO_GENERATOR
{
    wxString      m_html;
    FP_LIB_TABLE* m_fp_lib_table;
    LIB_ID const  m_lib_id;
    MODULE*       m_module;

public:
    FOOTPRINT_INFO_GENERATOR( FP_LIB_TABLE* aFpLibTable, LIB_ID const& aLibId )
        : m_html( DescriptionFormat ),
          m_fp_lib_table( aFpLibTable ),
          m_lib_id( aLibId ),
          m_module( nullptr )
    { }

    /**
     * Generate the HTML internally.
     */
    void GenerateHtml()
    {
        wxCHECK_RET( m_fp_lib_table, "Footprint library table pointer is not valid" );

        if( !m_lib_id.IsValid() )
            return;

        try
        {
            m_module = m_fp_lib_table->FootprintLoad( m_lib_id.GetLibNickname(),
                                                      m_lib_id.GetLibItemName() );
        }
        catch( const IO_ERROR& ioe )
        {
            wxLogError( wxString::Format( _( "Error loading footprint %s from library %s.\n\n%s" ),
                                          m_lib_id.GetLibItemName().wx_str(),
                                          m_lib_id.GetLibNickname().wx_str(),
                                          ioe.What() ) );
            return;
        }

        if( m_module )
        {
            wxString name = m_lib_id.GetLibItemName();
            wxString desc = m_module->GetDescription();
            wxString keywords = m_module->GetKeywords();
            wxString doc;

            // It is currently common practice to store a documentation link in the description.
            int idx = desc.find( wxT( "http:" ) );

            if( idx >= 0 )
            {
                doc = desc.substr( (unsigned) idx );

                desc = desc.substr( 0, (unsigned) idx );
                desc = desc.Trim( true );

                if( !desc.IsEmpty() && desc.Last() == ',' )
                    desc.RemoveLast( 1 );
            }

            m_html.Replace( "__NAME__", EscapedHTML( name ) );
            m_html.Replace( "__DESC__", EscapedHTML( desc ) );

            wxString keywordsHtml = KeywordsFormat;
            keywordsHtml.Replace( "__KEYWORDS__", EscapedHTML( keywords ) );

            wxString docHtml = DocFormat;
            docHtml.Replace( "__HREF__", EscapedHTML( doc ) );

            if( doc.Length() > 75 )
                doc = doc.Left( 72 ) + wxT( "..." );

            docHtml.Replace( "__TEXT__", EscapedHTML( doc ) );

            m_html.Replace( "__FIELDS__", keywordsHtml + docHtml );
        }
    }

    /**
     * Return the generated HTML.
     */
    wxString GetHtml()
    {
        return m_html;
    }

};


wxString GenerateFootprintInfo( FP_LIB_TABLE* aFpLibTable, LIB_ID const& aLibId )
{
    FOOTPRINT_INFO_GENERATOR gen( aFpLibTable, aLibId );
    gen.GenerateHtml();
    return gen.GetHtml();
}
