/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Chris Pavlina <pavlina.chris@gmail.com>
 * Copyright (C) 2015-2019 KiCad Developers, see change_log.txt for contributors.
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

#ifndef _LIB_CACHE_RESCUE_H_
#define _LIB_CACHE_RESCUE_H_

/* This code handles the case where an old schematic was made before
 * various changes were made, either to KiCad or to the libraries, and
 * the project needs to be recovered. The function of note is a member
 * of SCH_EDIT_FRAME, defined thus:
 *
 * bool SCH_EDIT_FRAME::RescueProject( bool aSilentIfNone );
 *
 * When this is called, a list of problematic components is compiled. If
 * this list is empty, then the function displays a notification and returns
 * (if aSilentIfNone is true, the notification is silenced).
 */

#include <vector>
#include <wx/string.h>
#include <boost/ptr_container/ptr_vector.hpp>

#include <properties.h>

#include <class_libentry.h>
#include <sch_legacy_plugin.h>
#include <class_draw_panel_gal.h>


class LIB_PART;
class SCH_COMPONENT;
class RESCUER;
class SCH_EDIT_FRAME;
class SCH_LEGACY_PLUGIN;
class SCH_SHEET_PATH;


enum RESCUE_TYPE
{
    RESCUE_CONFLICT,
    RESCUE_CASE,
};


class RESCUE_CANDIDATE
{
protected:
    wxString  m_requested_name;
    wxString  m_new_name;
    LIB_PART* m_lib_candidate;

public:
    virtual ~RESCUE_CANDIDATE() {}

    /**
     * Get the name that was originally requested in the schematic
     */
    virtual wxString GetRequestedName() const { return m_requested_name; }

    /**
     * Get the name we're proposing changing it to
     */
    virtual wxString GetNewName() const { return m_new_name; }

    /**
     * Get the part that can be loaded from the project cache, if possible, or
     * else NULL.
     */
    virtual LIB_PART* GetCacheCandidate() const { return NULL; }

    /**
     * Get the part the would be loaded from the libraries, if possible, or else
     * NULL.
     */
    virtual LIB_PART* GetLibCandidate() const { return m_lib_candidate; }

    /**
     * Get a description of the action proposed, for displaying in the UI.
     */
    virtual wxString GetActionDescription() const = 0;

    /**
     * Perform the actual rescue action. If successful, this must log the rescue using
     * RESCUER::LogRescue to allow it to be reversed.
     * @return True on success.
     */
    virtual bool PerformAction( RESCUER* aRescuer ) = 0;
};


class RESCUE_CASE_CANDIDATE : public RESCUE_CANDIDATE
{
public:
    /**
     * Grab all possible RESCUE_CASE_CANDIDATE objects into a vector.
     *
     * @param aRescuer - the working RESCUER instance.
     * @param aCandidates - the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Constructor RESCUE_CANDIDATE
     * @param aRequestedName - the name the schematic asks for
     * @param aNewName - the name we want to change it to
     * @param aLibCandidate - the part that will give us
     */
    RESCUE_CASE_CANDIDATE( const wxString& aRequestedName, const wxString& aNewName,
                           LIB_PART* aLibCandidate );

    RESCUE_CASE_CANDIDATE() { m_lib_candidate = NULL; }

    virtual wxString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;
};


class RESCUE_CACHE_CANDIDATE: public RESCUE_CANDIDATE
{
    LIB_PART* m_cache_candidate;

public:
    /**
     * Grab all possible #RESCUE_CACHE_CANDIDATE objectss into a vector.
     *
     * @param aRescuer - the working RESCUER instance.
     * @param aCandidates - the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Constructor RESCUE_CACHE_CANDIDATE
     * @param aRequestedName - the name the schematic asks for
     * @param aNewName - the name we want to change it to
     * @param aCacheCandidate - the part from the cache
     * @param aLibCandidate - the part that would be loaded from the library
     */
    RESCUE_CACHE_CANDIDATE( const wxString& aRequestedName, const wxString& aNewName,
                            LIB_PART* aCacheCandidate, LIB_PART* aLibCandidate );

    RESCUE_CACHE_CANDIDATE();

    virtual LIB_PART* GetCacheCandidate() const override { return m_cache_candidate; }

    virtual wxString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;
};


class RESCUE_SYMBOL_LIB_TABLE_CANDIDATE : public RESCUE_CANDIDATE
{
    LIB_ID m_requested_id;
    LIB_ID m_new_id;
    LIB_PART* m_cache_candidate;

public:
    /**
     * Grab all possible RESCUE_SYMBOL_LIB_TABLE_CANDIDATE objects into a vector.
     *
     * @param aRescuer - the working RESCUER instance.
     * @param aCandidates - the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Constructor RESCUE_CANDIDATE
     * @param aRequestedName - the name the schematic asks for
     * @param aNewName - the name we want to change it to
     * @param aCacheCandidate - the part from the cache
     * @param aLibCandidate - the part that would be loaded from the library
     */
    RESCUE_SYMBOL_LIB_TABLE_CANDIDATE( const LIB_ID& aRequestedId, const LIB_ID& aNewId,
                                       LIB_PART* aCacheCandidate, LIB_PART* aLibCandidate);

    RESCUE_SYMBOL_LIB_TABLE_CANDIDATE();

    virtual LIB_PART* GetCacheCandidate() const override { return m_cache_candidate; }

    virtual wxString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;
};


class RESCUE_LOG
{
public:
    SCH_COMPONENT*  component;
    wxString        old_name;
    wxString        new_name;
};


class RESCUER
{
protected:
    friend class DIALOG_RESCUE_EACH;

    std::vector<SCH_COMPONENT*> m_components;
    PROJECT* m_prj;
    EDA_DRAW_PANEL_GAL::GAL_TYPE m_galBackEndType;
    SCH_SHEET_PATH* m_currentSheet;

    boost::ptr_vector<RESCUE_CANDIDATE> m_all_candidates;
    std::vector<RESCUE_CANDIDATE*> m_chosen_candidates;

    std::vector<RESCUE_LOG> m_rescue_log;

public:
    RESCUER( PROJECT& aProject, SCH_SHEET_PATH* aCurrentSheet,
             EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackeEndType );

    /**
     * Writes out the rescue library. Called after successful PerformAction()s. If this fails,
     * undo the actions.
     *
     * @return True on success.
     */
    virtual bool WriteRescueLibrary( wxWindow *aParent ) = 0;

    virtual void OpenRescueLibrary() = 0;

    /**
     * Populate the RESCUER with all possible candidates.
     */
    virtual void FindCandidates() = 0;

    virtual void AddPart( LIB_PART* aNewPart ) = 0;

    /**
     * Display a dialog to allow the user to select rescues.
     *
     * @param aAskShowAgain - whether the "Never Show Again" button should be visible
     */
    virtual void InvokeDialog( wxWindow* aParent, bool aAskShowAgain ) = 0;

    /**
     * Filter out duplicately named rescue candidates.
     */
    void RemoveDuplicates();

    /**
     * Returen the number of rescue candidates found.
     */
    size_t GetCandidateCount() { return m_all_candidates.size(); }

    /**
     * Get the number of resuce candidates chosen by the user.
     */
    size_t GetChosenCandidateCount() { return m_chosen_candidates.size(); }

    /**
     * Get the list of symbols that need rescued.
     */
    std::vector<SCH_COMPONENT*>* GetComponents() { return &m_components; }

    /**
     * Return the #SCH_PROJECT object for access to the symbol libraries.
     */
    PROJECT* GetPrj() { return m_prj; }

    /**
     * Used by individual #RESCUE_CANDIDATE objects to log a rescue for undoing.
     */
    void LogRescue( SCH_COMPONENT *aComponent, const wxString& aOldName,
                    const wxString& aNewName );

    /**
     * Perform all chosen rescue actions, logging them to be undone if necessary.
     *
     * @return True on success
     */
    bool DoRescues();

    /**
     * Reverse the effects of all rescues on the project.
     */
    void UndoRescues();

    static bool RescueProject( wxWindow* aParent, RESCUER& aRescuer, bool aRunningOnDemand );
};


class LEGACY_RESCUER : public RESCUER
{
private:
    std::unique_ptr<PART_LIB> m_rescue_lib;

public:
    LEGACY_RESCUER( PROJECT& aProject, SCH_SHEET_PATH* aCurrentSheet,
                    EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackEndType ) :
        RESCUER( aProject, aCurrentSheet, aGalBackEndType )
    {
    }

    virtual void FindCandidates() override;

    virtual void InvokeDialog( wxWindow* aParent, bool aAskShowAgain ) override;

    virtual void OpenRescueLibrary() override;

    virtual bool WriteRescueLibrary( wxWindow *aParent ) override;

    virtual void AddPart( LIB_PART* aNewPart ) override;
};


class SYMBOL_LIB_TABLE_RESCUER : public RESCUER
{
private:
    SCH_PLUGIN::SCH_PLUGIN_RELEASER m_pi;

    std::unique_ptr< PROPERTIES > m_properties;   ///< Library plugin properties

public:
    SYMBOL_LIB_TABLE_RESCUER( PROJECT& aProject, SCH_SHEET_PATH* aCurrentSheet,
                              EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackeEndType );

    virtual void FindCandidates() override;

    virtual void InvokeDialog( wxWindow* aParent, bool aAskShowAgain ) override;

    virtual void OpenRescueLibrary() override;

    virtual bool WriteRescueLibrary( wxWindow* aParent ) override;

    virtual void AddPart( LIB_PART* aNewPart ) override;
};

#endif // _LIB_CACHE_RESCUE_H_
