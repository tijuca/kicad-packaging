/********************************************/
/* Definitions for the EESchema program:	*/
/********************************************/

#ifndef CLASS_TEXT_LABEL_H
#define CLASS_TEXT_LABEL_H

#ifndef eda_global
#define eda_global extern
#endif

#include "macros.h"
#include "base_struct.h"

/* Type des labels sur sheet (Labels sur hierarchie) et forme des Global-Labels*/
typedef enum {
    NET_INPUT,
    NET_OUTPUT,
    NET_BIDI,
    NET_TRISTATE,
    NET_UNSPECIFIED,
    NET_TMAX        /* Derniere valeur: fin de tableau */
} TypeSheetLabel;

/* Messages correspondants aux types ou forme des labels */
#ifdef MAIN
const char*        SheetLabelType[] =
{
    "Input",
    "Output",
    "BiDi",
    "3State",
    "UnSpc",
    "?????"
};
#else
extern const char* SheetLabelType[];
#endif

/* Description du graphisme des icones associes aux types des Global_Labels */
#ifdef MAIN
int         TemplateIN_HN[] = { 6, 0, 0, -1, -1, -2, -1, -2, 1, -1, 1, 0, 0 };
int         TemplateIN_HI[] = { 6, 0, 0, 1, 1, 2, 1, 2, -1, 1, -1, 0, 0 };
int         TemplateIN_BOTTOM[] = { 6, 0, 0, 1, -1, 1, -2, -1, -2, -1, -1, 0, 0 };
int         TemplateIN_UP[] = { 6, 0, 0, 1, 1, 1, 2, -1, 2, -1, 1, 0, 0 };

int         TemplateOUT_HN[] = { 6, -2, 0, -1, 1, 0, 1, 0, -1, -1, -1, -2, 0 };
int         TemplateOUT_HI[] = { 6, 2, 0, 1, -1, 0, -1, 0, 1, 1, 1, 2, 0 };
int         TemplateOUT_BOTTOM[] = { 6, 0, -2, 1, -1, 1, 0, -1, 0, -1, -1, 0, -2 };
int         TemplateOUT_UP[] = { 6, 0, 2, 1, 1, 1, 0, -1, 0, -1, 1, 0, 2 };

int         TemplateUNSPC_HN[] = { 5, 0, -1, -2, -1, -2, 1, 0, 1, 0, -1 };
int         TemplateUNSPC_HI[] = { 5, 0, -1, 2, -1, 2, 1, 0, 1, 0, -1 };
int         TemplateUNSPC_BOTTOM[] = { 5, 1, 0, 1, -2, -1, -2, -1, 0, 1, 0 };
int         TemplateUNSPC_UP[] = { 5, 1, 0, 1, 2, -1, 2, -1, 0, 1, 0 };

int         TemplateBIDI_HN[] = { 5, 0, 0, -1, -1, -2, 0, -1, 1, 0, 0 };
int         TemplateBIDI_HI[] = { 5, 0, 0, 1, -1, 2, 0, 1, 1, 0, 0 };
int         TemplateBIDI_BOTTOM[] = { 5, 0, 0, -1, -1, 0, -2, 1, -1, 0, 0 };
int         TemplateBIDI_UP[] = { 5, 0, 0, -1, 1, 0, 2, 1, 1, 0, 0 };

int         Template3STATE_HN[] = { 5, 0, 0, -1, -1, -2, 0, -1, 1, 0, 0 };
int         Template3STATE_HI[] = { 5, 0, 0, 1, -1, 2, 0, 1, 1, 0, 0 };
int         Template3STATE_BOTTOM[] = { 5, 0, 0, -1, -1, 0, -2, 1, -1, 0, 0 };
int         Template3STATE_UP[] = { 5, 0, 0, -1, 1, 0, 2, 1, 1, 0, 0 };

int*        TemplateShape[5][4] =
{
    { TemplateIN_HN,     TemplateIN_UP,     TemplateIN_HI,     TemplateIN_BOTTOM     },
    { TemplateOUT_HN,    TemplateOUT_UP,    TemplateOUT_HI,    TemplateOUT_BOTTOM    },
    { TemplateBIDI_HN,   TemplateBIDI_UP,   TemplateBIDI_HI,   TemplateBIDI_BOTTOM   },
    { Template3STATE_HN, Template3STATE_UP, Template3STATE_HI, Template3STATE_BOTTOM },
    { TemplateUNSPC_HN,  TemplateUNSPC_UP,  TemplateUNSPC_HI,  TemplateUNSPC_BOTTOM  }
};
#else
extern int* TemplateShape[5][4];
#endif

class SCH_TEXT : public SCH_ITEM
    , public EDA_TextStruct
{
public:
    int  m_Layer;
    int  m_Shape;
    bool m_IsDangling;          // TRUE if not connected

public:
    SCH_TEXT( const wxPoint& pos = wxPoint( 0, 0 ), const wxString& text = wxEmptyString,
                    KICAD_T aType = TYPE_SCH_TEXT );
    ~SCH_TEXT() { }

    virtual wxString GetClass() const
    {
        return wxT( "SCH_TEXT" );
    }


    SCH_TEXT*       GenCopy();
    virtual void    Draw( WinEDA_DrawPanel* panel, wxDC* DC, const wxPoint& offset, int draw_mode,
                          int Color = -1 );

    void            SwapData( SCH_TEXT* copyitem );

    void            Place( WinEDA_SchematicFrame* frame, wxDC* DC );

    /** Function HitTest
     * @return true if the point aPosRef is within item area
     * @param aPosRef = a wxPoint to test
     */
    bool HitTest( const wxPoint& aPosRef );

    EDA_Rect        GetBoundingBox();

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool    Save( FILE* aFile ) const;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os );

#endif

};


class SCH_LABEL : public SCH_TEXT
{
public:
    SCH_LABEL( const wxPoint& pos = wxPoint( 0, 0 ), const wxString& text = wxEmptyString );
    ~SCH_LABEL() { }
    virtual void    Draw( WinEDA_DrawPanel* panel, wxDC* DC, const wxPoint& offset, int draw_mode,
                          int Color = -1 );

    virtual wxString GetClass() const
    {
        return wxT( "SCH_LABEL" );
    }

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool    Save( FILE* aFile ) const;
};


class SCH_GLOBALLABEL : public SCH_TEXT
{
public:
    SCH_GLOBALLABEL( const wxPoint& pos = wxPoint( 0, 0 ),
                           const wxString& text = wxEmptyString );
    ~SCH_GLOBALLABEL() { }
    virtual void    Draw( WinEDA_DrawPanel* panel, wxDC* DC, const wxPoint& offset, int draw_mode,
                          int Color = -1 );

    virtual wxString GetClass() const
    {
        return wxT( "SCH_GLOBALLABEL" );
    }


    /** function CreateGraphicShape
      * Calculates the graphic shape (a polygon) associated to the text
      * @param corner_list = coordinates list fill with polygon corners ooordinates (size > 20)
      * @param Pos = Postion of the shape
      * format list is
      * <corner_count>, x0, y0, ... xn, yn
     */
    void CreateGraphicShape( int* corner_list, const wxPoint & Pos );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool    Save( FILE* aFile ) const;

    /** Function HitTest
     * @return true if the point aPosRef is within item area
     * @param aPosRef = a wxPoint to test
     */
    bool HitTest( const wxPoint& aPosRef );

    EDA_Rect        GetBoundingBox();
};



class SCH_HIERLABEL : public SCH_TEXT
{
public:
    SCH_HIERLABEL( const wxPoint& pos = wxPoint( 0, 0 ),
                         const wxString& text = wxEmptyString );
    ~SCH_HIERLABEL() { }
    virtual void    Draw( WinEDA_DrawPanel* panel, wxDC* DC, const wxPoint& offset, int draw_mode,
                          int Color = -1 );

    virtual wxString GetClass() const
    {
        return wxT( "SCH_HIERLABEL" );
    }


    /** function CreateGraphicShape
      * Calculates the graphic shape (a polygon) associated to the text
      * @param corner_list = coordinates list fill with polygon corners ooordinates (size >= 14)
      * @param Pos = Postion of the shape
      * format list is
      * <corner_count>, x0, y0, ... xn, yn
      */
    void CreateGraphicShape( int* corner_list, const wxPoint & Pos );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool    Save( FILE* aFile ) const;

    /** Function HitTest
     * @return true if the point aPosRef is within item area
     * @param aPosRef = a wxPoint to test
     */
    bool HitTest( const wxPoint& aPosRef );

    EDA_Rect        GetBoundingBox();
};

#endif /* CLASS_TEXT_LABEL_H */
