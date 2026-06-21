#include "RadioBox.hpp"

#include "../wxExtensions.hpp"

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

bool RadioBox::GetValue()
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

}
}

