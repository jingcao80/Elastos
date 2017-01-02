//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
