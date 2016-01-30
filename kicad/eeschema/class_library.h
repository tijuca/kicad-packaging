/**********************************/
/*	Headers fo library definition */
/**********************************/

#ifndef CLASS_LIBRARY_H
#define CLASS_LIBRARY_H

#include "class_libentry.h"


/******************************/
/* Classe to handle a library */
/******************************/

class LibraryStruct
{
public:
    int            m_Type;                  /* type indicator */
    wxString       m_Name;                  /* Short Name of the loaded library (without path). */
    wxString       m_FullFileName;          /* Full File Name (with path) of library. */
    wxString       m_Header;                /* first line of loaded library. */
    int            m_NumOfParts;            /* Number of parts this library has. */
    PriorQue*      m_Entries;               /* Parts themselves are saved here. */
    LibraryStruct* m_Pnext;                 /* Point on next lib in chain. */
    int            m_Modified;              /* flag indicateur d'edition */
    int            m_Size;                  // Size in bytes (for statistics)
    unsigned long  m_TimeStamp;             // Signature temporelle
    int            m_Flags;                 // variable used in some functions
    bool           m_IsLibCache;            /* False for the "standard" libraries,
                                              * True for the library cache */

public:
    LibraryStruct( int type, const wxString& name, const wxString& fullname );
    ~LibraryStruct();
    /**
     * Function SaveLibrary
     * writes the data structures for this object out to 2 file
     * the library in "*.lib" format.
     * the doc file in "*.dcm" format.
     * creates a backup file for each  file (.bak and .bck)
     * @param aFullFileName The full lib filename.
     * @return bool - true if success writing else false.
     */
    bool SaveLibrary( const wxString& aFullFileName );

    bool ReadHeader( FILE* file, int* LineNum );

private:
    bool WriteHeader( FILE* file );
};

#endif  //  CLASS_LIBRARY_H
