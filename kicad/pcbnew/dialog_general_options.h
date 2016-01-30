#ifndef __dialog_general_options_h
#define __dialog_general_options_h

/***********************************************************************/
class Dialog_GeneralOptions : public DialogGeneralOptionsBoardEditor_base
/***********************************************************************/
{
private:
    WinEDA_PcbFrame* m_Parent;
     wxDC* m_DC;

    void init();

public:
    Dialog_GeneralOptions( WinEDA_PcbFrame* parent, wxDC* DC );
    ~Dialog_GeneralOptions() {};
    void OnOkClick( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
};


class Dialog_Display_Options : public DialogDisplayOptions_base
{
private:
    WinEDA_BasePcbFrame* m_Parent;

    void init();

public:
    Dialog_Display_Options( WinEDA_BasePcbFrame* parent );
    ~Dialog_Display_Options( ) { };
    void OnOkClick( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
};

#endif	// __dialog_general_options_h
