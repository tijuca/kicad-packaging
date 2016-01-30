/*********************************************/
/*	Headers for component library definition */
/*********************************************/

#ifndef CLASS_LIBRARY_H
#define CLASS_LIBRARY_H


#include "class_libentry.h"

/*
 * Component Library version and file header  macros.
 */
#define LIB_VERSION_MAJOR 2
#define LIB_VERSION_MINOR 3

/* Must be the first line of component library (.lib) files. */
#define LIBFILE_IDENT     "EESchema-LIBRARY Version"

#define LIB_VERSION( major, minor ) ( major * 100 + minor )

#define IS_LIB_CURRENT_VERSION( major, minor )              \
    (                                                       \
        LIB_VERSION( major1, minor1 ) ==                    \
        LIB_VERSION( LIB_VERSION_MAJOR, LIB_VERSION_MINOR)  \
    )

/*
 * Library versions 2.3 and lower use the old separate library (.lib) and
 * document (.dcm) files.  Component libraries after 2.3 merged the library
 * and document files into a single library file.  This macro checks if the
 * library version supports the old format
 */
#define USE_OLD_DOC_FILE_FORMAT( major, minor )                 \
    ( LIB_VERSION( major, minor ) <= LIB_VERSION( 2, 3 ) )

/* Must be the first line of component library document (.dcm) files. */
#define DOCFILE_IDENT     "EESchema-DOCLIB  Version 2.0"

#define DOC_EXT           wxT( "dcm" )


/* Helpers for creating a list of component libraries. */
class CMP_LIBRARY;
class wxRegEx;


typedef boost::ptr_vector< CMP_LIBRARY > CMP_LIBRARY_LIST;

extern bool operator<( const CMP_LIBRARY& item1, const CMP_LIBRARY& item2 );


/**
 * Component library object.
 *
 * Component libraries are used to load, save, search, and otherwise manipulate
 * component library files.
 */

class CMP_LIBRARY
{
public:
    int            m_Type;         /* type indicator */
    int            m_Flags;

protected:
    wxFileName     fileName;        /* Library file name. */
    wxDateTime     timeStamp;       /* Library save time and date. */
    int            versionMajor;    /* Library major version number. */
    int            versionMinor;    /* Library minor version number. */
    LIB_ENTRY_LIST entries;         /* Parts themselves are saved here. */
    bool           isCache;         /* False for the "standard" libraries,
                                     * True for the library cache */
    wxString       header;          /* first line of loaded library. */

    static CMP_LIBRARY_LIST libraryList;
    static wxArrayString    libraryListSortOrder;

    friend class CMP_LIB_ENTRY;

private:
    bool           isModified;      /* Library modification status. */

public:
    CMP_LIBRARY( int aType, const wxFileName& aFileName );
    CMP_LIBRARY( int aType, const wxString& aFileName )
    {
        CMP_LIBRARY( aType, wxFileName( aFileName ) );
    }
    ~CMP_LIBRARY();

    /** Modify flags handling:
     */
    void SetModifyFlags( ) { isModified = true; }
    void ClearModifyFlag( ) { isModified = false; }
    bool getModifyFlag( ) { return isModified;}

    /**
     * Save library to file.
     *
     * Prior to component library version 3.0, two files were created.  The
     * component objects are wer as component library (*.lib) files.  The
     * library entry ojbect document strings were save in library document
     * definition (*.dcm) files.  After version component library version 3.0,
     * the document string information is saved as part of the library file.
     * Saving separate document is maintained for backwards compatability.
     * Please note that this behavior may change in the future.  If the
     * component library already exists, it is backup up in file *.bak.
     *
     * @param aFullFileName - The library filename with path.
     * @param aOldDocFormat - Save the document information in a separate
     *                        file if true.  The default is to save as the
     *                        current library file format.
     * @return True if success writing else false.
     */
    bool Save( const wxString& aFullFileName, bool aOldDocFormat = false );

    /**
     * Save library document information to file.
     *
     * If the document definition file* already exists, it is backed up in
     * file *.bck.
     *
     * @param aFullFileName - The library filename with path.
     * @return True if success writing else false.
     */
    bool SaveDocFile( const wxString& aFullFileName );

    /**
     * Load library from file.
     *
     * @param aErrorMsg - Error message if load fails.
     * @return True if load was successful otherwise false.
     */
    bool Load( wxString& aErrorMsg );

    bool LoadDocs( wxString& aErrorMsg );

private:
    bool SaveHeader( FILE* aFile );

    bool LoadHeader( FILE* aFile, int* aLineNum );
    void LoadAliases( LIB_COMPONENT* aComponent );

    /** function RemoveEntryName
     * Remove an /a aName entry from the library list names.
     * Warning: this is a partiel remove, because if aname is an alias
     * it is not removed from its root component.
     * this is for internal use only
     * Use RemoveEntry( CMP_LIB_ENTRY* aEntry ) to remove safely an entry.
     * @param aName - Entry name to remove from library.
     */
    void RemoveEntryName( const wxString& aName );

public:
    /**
     * Get library entry status.
     *
     * @return True if there are no entries in the library.
     */
    bool IsEmpty() const
    {
        return entries.empty();
    }

    /**
     * Get the number of entries in the library.
     *
     * @return The number of component and alias entries.
     */
    int GetCount() const
    {
        return entries.size();
    }

    bool IsModified() const
    {
        return isModified;
    }

    bool IsCache() const { return isCache; }

    void SetModified( void ) { isModified = true; }

    void SetCache( void ) { isCache = true; }

   /**
     * Load a string array with the names of all the entries in this library.
     *
     * @param aNames - String array to place entry names into.
     * @param aSort - Sort names if true.
     * @param aMakeUpperCase - Force entry names to upper case.
     */
    void GetEntryNames( wxArrayString& aNames, bool aSort = true,
                        bool aMakeUpperCase = true );

    /**
     * Load string array with entry names matching name and/or key word.
     *
     * This currently mimics the old behavior of calling KeyWordOk() and
     * WildCompareString().  The names array will be populated with the
     * library entry names that meat the search criteria on exit.
     *
     * @param aNames - String array to place entry names into.
     * @param aNameSearch - Name wild card search criteria.
     * @param aKeySearch - Key word search criteria.
     * @param aSort - Sort names if true.
     */
    void SearchEntryNames( wxArrayString& aNames,
                           const wxString& aNameSearch = wxEmptyString,
                           const wxString& aKeySearch = wxEmptyString,
                           bool aSort = true );

    /**
     * Find components in library by key word regular expression search.
     *
     * @param aNames - String array to place found component names into.
     * @param aRe - Regular expression used to seach component key words.
     * @param aSort - Sort component name list.
     */
    void SearchEntryNames( wxArrayString& aNames, const wxRegEx& aRe,
                           bool aSort = true );

    /**
     * Find entry by name.
     *
     * @param aName - Name of entry, case insensitive.
     * @return Entry if found.  NULL if not found.
     */
    CMP_LIB_ENTRY* FindEntry( const wxChar* aName );

    /**
     * Find entry by /a aName and /a aType.
     *
     * @param aName - Name of entry, case insensitive.
     * @param aType - Type of entry, root or alias.
     * @return Entry if found.  NULL if not found.
     */
    CMP_LIB_ENTRY* FindEntry( const wxChar* aName, LibrEntryType aType );

    /**
     * Find component by /a aName.
     *
     * This is a helper for FindEntry so casting a CMP_LIB_ENTRY pointer to
     * a LIB_COMPONENT pointer is not required.
     *
     * @param aName - Name of component, case insensitive.
     * @return Component if found.  NULL if not found.
     */
    LIB_COMPONENT* FindComponent( const wxChar* aName );

    /**
     * Find alias by /a nName.
     *
     * This is a helper for FindEntry so casting a CMP_LIB_ENTRY pointer to
     * a LIB_ALIAS pointer is not required.
     *
     * @param aName - Name of alias, case insensitive.
     * @return Alias if found.  NULL if not found.
     */
    LIB_ALIAS* FindAlias( const wxChar* aName )
    {
        return (LIB_ALIAS*) FindEntry( aName, ALIAS );
    }

    /**
     * Add a new /a aAlias entry to the library.
     *
     * First check if a component or alias with the same name already exists
     * in the library and add alias if no conflict occurs.  Once the alias
     * is added to the library it is owned by the library.  Deleting the
     * alias pointer will render the library unstable.  Use RemoveEntry to
     * remove the alias from the library.
     *
     * @param aAlias - Alias to add to library.
     * @return True if alias added to library.  False if a conflict exists.
     */
    bool AddAlias( LIB_ALIAS* aAlias );

    /**
     * Add /a aComponent entry to library.
     * Note a component can have an alias list,
     * so these alias will be added in library.
     * Conflicts can happen if aliases are already existing.
     * User is asked to choose what alias is removed (existing, or new)
     * @param aComponent - Component to add.
     * @return Added component if successful.
     */
    LIB_COMPONENT* AddComponent( LIB_COMPONENT* aComponent );

    /**
     * Remove safely an /a aEntry from the library.
     *
     * If the entry is an alias, the alias is removed from the library and from
     * the alias list of the root component.  If the entry is a root component
     * with no aliases, it is removed from the library.  If the entry is a root
     * component with aliases, the root component is renamed to the name of
     * the first alias and the root name for all remaining aliases are updated
     * to reflect the new root name.
     *
     * @param aEntry - Entry to remove from library.
     */
    void RemoveEntry( CMP_LIB_ENTRY* aEntry );

    /**
     * Replace an existing component entry in the library.
     * Note a component can have an alias list,
     * so these alias will be added in library (and previously existing alias removed)
     * @param aOldComponent - The component to replace.
     * @param aNewComponent - The new component.
     */
    LIB_COMPONENT* ReplaceComponent( LIB_COMPONENT* aOldComponent,
                                     LIB_COMPONENT* aNewComponent );

    /**
     * Return the first entry in the library.
     *
     * @return The first entry or NULL if the library has no entries.
     */
    CMP_LIB_ENTRY* GetFirstEntry();

    /**
     * Find next library entry by /a aName.
     *
     * If the name of the entry is the last entry in the library, the first
     * entry in the list is returned.
     *
     * @param aName - Name of current entry.
     * @return Next entry if entry name is found. Otherwise NULL.
     */
    CMP_LIB_ENTRY* GetNextEntry( const wxChar* aName );


    /**
     * Find previous library entry by /a aName.
     *
     * If the name of the entry is the first entry in the library, the last
     * entry in the list is returned.
     *
     * @param aName - Name of current entry.
     * @return Previous entry if entry name is found, otherwise NULL.
     */
    CMP_LIB_ENTRY* GetPreviousEntry( const wxChar* aName );

    /**
     * Return the file name without path or extension.
     *
     * @return Name of library file.
     */
    wxString GetName() const { return fileName.GetName(); }

    /**
     * Return the full file library name with path and extension.
     *
     * @return Full library file name with path and extension.
     */
    wxString GetFullFileName() { return fileName.GetFullPath(); }

    /**
     * Set the component library file name.
     *
     * @param aFileName - New library file name.
     */
    void SetFileName( const wxFileName aFileName )
    {
        if( aFileName != fileName )
            fileName = aFileName;
    }

    /*
     * The following static methods are for manipulating the list of
     * component libraries.  This eliminates the need for yet another
     * global variable ( formerly g_LibraryList ) and gives some measure
     * of safety from abusing the library list.
     */

     /**
     * Test for an existing library.
     *
     * @param aLibptr - aLibptr.
     * @return true found.  false if not found.
     */

    static bool LibraryExists( const CMP_LIBRARY* aLibptr );
    /**
     * Load a component library file.
     *
     * @param aFileName - File name of the component library to load.
     * @param aErrorMsg - Error message if the component library failed to load.
     * @return Library object if library file loaded successfully,
     *         otherwise NULL.
     */
    static CMP_LIBRARY* LoadLibrary( const wxFileName& aFileName,
                                     wxString& aErrorMsg );

    /**
     * Add a compnent library to the library list.
     *
     * @param aFileName - File name object of component library.
     * @param aErrorMsg - Error message if the component library failed to load.
     * @return True if library loaded properly otherwise false.
     */
    static bool AddLibrary( const wxFileName& aFileName, wxString& aErrorMsg );

    /**
     * Insert a compnent library to the library list.
     *
     * @param aFileName - File name object of component library.
     * @param aErrerMsg - Error message if the component library failed to load.
     * @param aIteratir - Iterator to insert library in front of.
     * @return True if library loaded properly otherwise false.
     */
    static bool AddLibrary( const wxFileName& aFileName, wxString& aErrorMsg,
                            CMP_LIBRARY_LIST::iterator& aIterator );

    /**
     * Remove component library from the library list.
     *
     * @param aName - Name of component library to remove.
     */
    static void RemoveLibrary( const wxString& aName );

    /**
     * Find component library by /a aName.
     *
     * @param aName - Library file name without path or extension to find.
     * @return Component library if found, otherwise NULL.
     */
    static CMP_LIBRARY* FindLibrary( const wxString& aName );

    /**
     * Get the list of component library file names without path and extension.
     *
     * @param aSorted - Sort the list of name if true.  Otherwise use the
     *                  library load order.
     * @return The list of library names.
     */
    static wxArrayString GetLibraryNames( bool aSorted = true );

    /**
     * Search all libraries in the list for a component.
     *
     * A component object will always be returned.  If the entry found
     * is an alias.  The root component will be found and returned.
     *
     * @param aCompoentName - Name of component to search for.
     * @param aLibraryName - Name of the library to search for component.
     * @return The component object if found, otherwise NULL.
     */
    static LIB_COMPONENT* FindLibraryComponent( const wxString& aComponentName,
                                                const wxString& aLibraryName = wxEmptyString );

    /**
     * Search all libraries in the list for an entry.
     *
     * The object can be either a component or an alias.
     *
     * @param aEntryName - Name of entry to search for.
     * @param aLibraryName - Name of the library to search.
     * @return The entry object if found, otherwise NULL.
     */
    static CMP_LIB_ENTRY* FindLibraryEntry( const wxString& aEntryName,
                                            const wxString& aLibraryName = wxEmptyString );

    /**
     * Remove all cache libraries from library list.
     */
    static void RemoveCacheLibrary();

    static int GetLibraryCount() { return libraryList.size(); }

    static CMP_LIBRARY_LIST& GetLibraryList()
    {
        return libraryList;
    }

    static void SetSortOrder( const wxArrayString& aSortOrder )
    {
        libraryListSortOrder = aSortOrder;
    }

    static wxArrayString& GetSortOrder( void )
    {
        return libraryListSortOrder;
    }
};


/**
 * Case insensitive library name comparison.
 */
extern bool operator==( const CMP_LIBRARY& aLibrary, const wxChar* aName );
extern bool operator!=( const CMP_LIBRARY& aLibrary, const wxChar* aName );

#endif  //  CLASS_LIBRARY_H
