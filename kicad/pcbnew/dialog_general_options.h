#ifndef __dialog_general_options_h
#define __dialog_general_options_h

#include "dialog_general_options_BoardEditor_base.h"

/***********************************************************************/
class Dialog_GeneralOptions : public DialogGeneralOptionsBoardEditor_base
/***********************************************************************/
{
private:
    WinEDA_PcbFrame* m_Parent;
    BOARD * m_Board;

    void init();

public:
    Dialog_GeneralOptions( WinEDA_PcbFrame* parent );
    ~Dialog_GeneralOptions() {};
    void OnOkClick( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
};


#endif	// __dialog_general_options_h
