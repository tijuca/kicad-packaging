/*************************************************************************/
/*	classe EQUIPOT: definition des elements relatifs aux equipotentielles */
/*************************************************************************/


/* Representation des descriptions des equipotentielles */

class EQUIPOT : public BOARD_ITEM
{
private:
    int        m_NetCode;       // numero de code interne du net


public:
    wxString   m_Netname;       // nom du net
    int        status;          // no route, hight light...
    int        m_NbNodes;       // nombre de pads appartenant au net
    int        m_NbLink;        // nombre de chevelus
    int        m_NbNoconn;      // nombre de chevelus actifs
    int        m_Masque_Layer;  // couches interdites (bit 0 = layer 0...)
    int        m_Masque_Plan;   // couches mises en plan de cuivre
    int        m_ForceWidth;    // specific width (O = default width)
    LISTE_PAD* m_PadzoneStart;  // pointeur sur debut de liste pads du net
    LISTE_PAD* m_PadzoneEnd;    // pointeur sur fin de liste pads du net
    CHEVELU*   m_RatsnestStart; // pointeur sur debut de liste ratsnests du net
    CHEVELU*   m_RatsnestEnd;   // pointeur sur fin de liste ratsnests du net

    EQUIPOT( BOARD_ITEM* StructFather );
    ~EQUIPOT();

    EQUIPOT*    Next() { return (EQUIPOT*) Pnext; }
    
    /* Effacement memoire de la structure */
    void  UnLink();

    /* Readind and writing data on files */
    int   ReadEquipotDescr( FILE* File, int* LineNum );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */ 
    bool Save( FILE* aFile ) const;
    
    
    /**
     * Function GetNet
     * @return int - the netcode
     */
    int GetNet() const { return m_NetCode; }
    void SetNet( int aNetCode ) { m_NetCode = aNetCode; }
     
    
    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    wxString GetClass() const
    {
        return wxT("NET");
    }

    
#if defined(DEBUG)
    /**
     * Function Show
     * is used to output the object tree, currently for debugging only.
     * @param nestLevel An aid to prettier tree indenting, and is the level 
     *          of nesting of this object within the overall tree.
     * @param os The ostream& to output to.
     */
    virtual void Show( int nestLevel, std::ostream& os );
#endif
    
};
