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

#include "elastos/droid/systemui/statusbar/policy/WifiIcons.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Int32 WifiIcons::WIFI_SIGNAL_STRENGTH[2][5] = {
    {
        R::drawable::stat_sys_wifi_signal_0,
        R::drawable::stat_sys_wifi_signal_1,
        R::drawable::stat_sys_wifi_signal_2,
        R::drawable::stat_sys_wifi_signal_3,
        R::drawable::stat_sys_wifi_signal_4
    },

    {
        R::drawable::stat_sys_wifi_signal_0_fully,
        R::drawable::stat_sys_wifi_signal_1_fully,
        R::drawable::stat_sys_wifi_signal_2_fully,
        R::drawable::stat_sys_wifi_signal_3_fully,
        R::drawable::stat_sys_wifi_signal_4_fully
    }
};

const Int32 WifiIcons::QS_WIFI_SIGNAL_STRENGTH[2][5] = {
    {
        R::drawable::ic_qs_wifi_0,
        R::drawable::ic_qs_wifi_1,
        R::drawable::ic_qs_wifi_2,
        R::drawable::ic_qs_wifi_3,
        R::drawable::ic_qs_wifi_4
    },

    {
        R::drawable::ic_qs_wifi_full_0,
        R::drawable::ic_qs_wifi_full_1,
        R::drawable::ic_qs_wifi_full_2,
        R::drawable::ic_qs_wifi_full_3,
        R::drawable::ic_qs_wifi_full_4
    }
};

const Int32 WifiIcons::WIFI_LEVEL_COUNT = 5;

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
