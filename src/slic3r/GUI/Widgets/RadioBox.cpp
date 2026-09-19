#include "RadioBox.hpp"

#include "../wxExtensions.hpp"

#include <wx/sizer.h>

#ifdef __WXGTK__
#include <algorithm>
#include <wx/dcmemory.h>
#endif

namespace Slic3r {
namespace GUI {

#ifdef __WXGTK__
static int dip_from_window(wxWindow *win, int value)
{
    return win ? win->FromDIP(value) : value;
}

static wxBitmap make_visible_unchecked_radio_bitmap(wxWindow *win, const wxBitmap &base)
{
    wxBitmap bmp(base);
    const wxSize size = ScalableBitmap::GetBmpSize(bmp);
    wxMemoryDC dc(bmp);

    const int diameter = std::max(dip_from_window(win, 12), std::min(size.x, size.y) - dip_from_window(win, 4));
    const int x = (size.x - diameter) / 2;
    const int y = (size.y - diameter) / 2;

    // Linux/wxGTK native/stock radio artwork may be too dark on manually styled
    // AMS panels; enforce a light unchecked ring so empty slots remain visible.
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxPen(wxColour(0xD0, 0xD0, 0xD0), std::max(1, dip_from_window(win, 2))));
    dc.DrawEllipse(x, y, diameter, diameter);
    dc.SelectObject(wxNullBitmap);
    return bmp;
}
#endif

RadioBox::RadioBox(wxWindow *parent)
    : wxBitmapToggleButton(parent, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , m_on(this, "radio_on", 18)
    , m_off(this, "radio_off", 18)
    , m_ban(this, "radio_ban", 18)
{
    // SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
    if (parent) SetBackgroundColour(parent->GetBackgroundColour());
    // Bind(wxEVT_TOGGLEBUTTON, [this](auto& e) { update(); e.Skip(); });
    SetSize(m_on.GetBmpSize());
    SetMinSize(m_on.GetBmpSize());
    update();
}

void RadioBox::SetValue(bool value)
{
    wxBitmapToggleButton::SetValue(value);
    update();
}

bool RadioBox::GetValue() const
{
    return wxBitmapToggleButton::GetValue();
}


void RadioBox::Rescale()
{
    m_on.msw_rescale();
    m_off.msw_rescale();
    SetSize(m_on.GetBmpSize());
    update();
}

void RadioBox::update() {
    if (IsEnabled())
    {
#ifdef __WXGTK__
        if (!GetValue())
            SetBitmap(make_visible_unchecked_radio_bitmap(this, m_off.bmp()));
        else
            SetBitmap(m_on.bmp());
#else
        SetBitmap((GetValue() ? m_on : m_off).bmp());
#endif
    } else
    {
        SetBitmap(m_ban.bmp());
    }

}

BBLRadioButton::BBLRadioButton(wxWindow *parent, wxWindowID id, const wxString &label,
                               const wxPoint &pos, const wxSize &size, long style)
    : wxPanel(parent, id, pos, size, style)
    , m_enabled_label_colour(parent ? parent->GetForegroundColour() : *wxBLACK)
    , m_disabled_label_colour(wxColour(0xAC, 0xAC, 0xAC))
{
    if (parent) SetBackgroundColour(parent->GetBackgroundColour());

    auto *sizer = new wxBoxSizer(wxHORIZONTAL);
    m_radio = new RadioBox(this);
    m_label = new wxStaticText(this, wxID_ANY, label);
    m_label->SetForegroundColour(m_enabled_label_colour);

    sizer->Add(m_radio, 0, wxALIGN_CENTER_VERTICAL);
    sizer->AddSpacer(FromDIP(6));
    sizer->Add(m_label, 0, wxALIGN_CENTER_VERTICAL);
    SetSizer(sizer);

    // Native wxGTK radio buttons can have poor unchecked contrast on manually
    // styled dark backgrounds, so Bambu-styled dialogs should use RadioBox
    // artwork and route clicks through this wrapper instead of GTK rendering.
    auto click_handler = [this](wxEvent &event) {
        if (!IsEnabled())
            return;
        SetValue(true);
        send_radio_event();
    };
    Bind(wxEVT_LEFT_UP, click_handler);
    m_radio->Bind(wxEVT_TOGGLEBUTTON, [this](wxCommandEvent &) {
        if (!IsEnabled())
            return;
        SetValue(true);
        send_radio_event();
    });
    m_label->Bind(wxEVT_LEFT_UP, click_handler);
}

void BBLRadioButton::SetValue(bool value)
{
    m_radio->SetValue(value);
}

bool BBLRadioButton::GetValue() const
{
    return m_radio->GetValue();
}

bool BBLRadioButton::Enable(bool enable)
{
    const bool result = wxPanel::Enable(enable);
    m_radio->Enable(enable);
    m_label->Enable(enable);
    update_label_colour();
    return result;
}

void BBLRadioButton::SetLabel(const wxString &label)
{
    m_label->SetLabel(label);
}

wxString BBLRadioButton::GetLabel() const
{
    return m_label->GetLabel();
}

bool BBLRadioButton::SetForegroundColour(const wxColour &colour)
{
    const bool result = wxPanel::SetForegroundColour(colour);
    m_enabled_label_colour = colour;
    update_label_colour();
    return result;
}

bool BBLRadioButton::SetFont(const wxFont &font)
{
    const bool result = wxPanel::SetFont(font);
    m_label->SetFont(font);
    return result;
}

void BBLRadioButton::Rescale()
{
    m_radio->Rescale();
    Layout();
}

void BBLRadioButton::send_radio_event()
{
    wxCommandEvent event(wxEVT_RADIOBUTTON, GetId());
    event.SetEventObject(this);
    ProcessWindowEvent(event);
}

void BBLRadioButton::update_label_colour()
{
    m_label->SetForegroundColour(IsEnabled() ? m_enabled_label_colour : m_disabled_label_colour);
}

}
}
