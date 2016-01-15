#include "elastos/droid/systemui/statusbar/policy/AccessibilityContentDescriptions.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


const Int32 AccessibilityContentDescriptions::PHONE_SIGNAL_STRENGTH[5] = {
    SystemUIR::string::accessibility_no_phone,
    SystemUIR::string::accessibility_phone_one_bar,
    SystemUIR::string::accessibility_phone_two_bars,
    SystemUIR::string::accessibility_phone_three_bars,
    SystemUIR::string::accessibility_phone_signal_full
};

const Int32 AccessibilityContentDescriptions::DATA_CONNECTION_STRENGTH[5] = {
    SystemUIR::string::accessibility_no_data,
    SystemUIR::string::accessibility_data_one_bar,
    SystemUIR::string::accessibility_data_two_bars,
    SystemUIR::string::accessibility_data_three_bars,
    SystemUIR::string::accessibility_data_signal_full
};

const Int32 AccessibilityContentDescriptions::WIFI_CONNECTION_STRENGTH[5] = {
    SystemUIR::string::accessibility_no_wifi,
    SystemUIR::string::accessibility_wifi_one_bar,
    SystemUIR::string::accessibility_wifi_two_bars,
    SystemUIR::string::accessibility_wifi_three_bars,
    SystemUIR::string::accessibility_wifi_signal_full
};

const Int32 AccessibilityContentDescriptions::WIMAX_CONNECTION_STRENGTH[5] = {
    SystemUIR::string::accessibility_no_wimax,
    SystemUIR::string::accessibility_wimax_one_bar,
    SystemUIR::string::accessibility_wimax_two_bars,
    SystemUIR::string::accessibility_wimax_three_bars,
    SystemUIR::string::accessibility_wimax_signal_full
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
