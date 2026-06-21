#ifndef slic3r_GUI_RADIOBOX_hpp_
#define slic3r_GUI_RADIOBOX_hpp_

#include "../wxExtensions.hpp"

#include <wx/tglbtn.h>
#include <wx/panel.h>
#include <wx/stattext.h>

namespace Slic3r {
namespace GUI {

class RadioBox : public wxBitmapToggleButton
{
public:
    RadioBox(wxWindow *parent);

public:
    void SetValue(bool value) override;
	bool GetValue() const;
    void Rescale();
    bool Disable() {
        return wxBitmapToggleButton::Disable();
    }
    bool Enable(bool enable = true) override {
        return wxBitmapToggleButton::Enable(enable);
    }

private:
    void update();

private:
    ScalableBitmap m_on;
    ScalableBitmap m_off;
    ScalableBitmap m_ban;
};

class BBLRadioButton : public wxPanel
{
public:
    BBLRadioButton(wxWindow *parent, wxWindowID id, const wxString &label,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize, long style = 0);

    void SetValue(bool value);
    bool GetValue() const;
    bool Enable(bool enable = true) override;
    bool Disable() { return Enable(false); }
    void SetLabel(const wxString &label) override;
    wxString GetLabel() const override;
    bool SetForegroundColour(const wxColour &colour) override;
    bool SetFont(const wxFont &font) override;
    void Rescale();

private:
    void send_radio_event();
    void update_label_colour();

    RadioBox *m_radio { nullptr };
    wxStaticText *m_label { nullptr };
    wxColour m_enabled_label_colour;
    wxColour m_disabled_label_colour;
};

}}



#endif // !slic3r_GUI_CheckBox_hpp_
