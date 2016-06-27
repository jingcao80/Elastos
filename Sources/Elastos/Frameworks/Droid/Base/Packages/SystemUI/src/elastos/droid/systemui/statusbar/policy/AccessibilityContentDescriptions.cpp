
#include "elastos/droid/systemui/statusbar/policy/AccessibilityContentDescriptions.h"
#include "../R.h"

using Elastos::Droid::SystemUI::R;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Int32 AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[5] = {
    R::string::accessibility_no_phone,
    R::string::accessibility_phone_one_bar,
    R::string::accessibility_phone_two_bars,
    R::string::accessibility_phone_three_bars,
    R::string::accessibility_phone_signal_full
};

const Int32 AccessibilityContentDescriptions::DATA_CONNECTION_STRENGTH[5] = {
    R::string::accessibility_no_data,
    R::string::accessibility_data_one_bar,
    R::string::accessibility_data_two_bars,
    R::string::accessibility_data_three_bars,
    R::string::accessibility_data_signal_full
};

const Int32 AccessibilityContentDescriptions::WIFI_CONNECTION_STRENGTH[5] = {
    R::string::accessibility_no_wifi,
    R::string::accessibility_wifi_one_bar,
    R::string::accessibility_wifi_two_bars,
    R::string::accessibility_wifi_three_bars,
    R::string::accessibility_wifi_signal_full
};

const Int32 AccessibilityContentDescriptions::WIMAX_CONNECTION_STRENGTH[5] = {
    R::string::accessibility_no_wimax,
    R::string::accessibility_wimax_one_bar,
    R::string::accessibility_wimax_two_bars,
    R::string::accessibility_wimax_three_bars,
    R::string::accessibility_wimax_signal_full
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
