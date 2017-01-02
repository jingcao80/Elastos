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

#include "elastos/droid/systemui/statusbar/policy/WimaxIcons.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Int32 WimaxIcons::WIMAX_SIGNAL_STRENGTH[2][5] = {  // TELEPHONY_SIGNAL_STRENGTH
    {
        R::drawable::stat_sys_signal_0,
        R::drawable::stat_sys_signal_1,
        R::drawable::stat_sys_signal_2,
        R::drawable::stat_sys_signal_3,
        R::drawable::stat_sys_signal_4
    },

    {
        R::drawable::stat_sys_signal_0_fully,
        R::drawable::stat_sys_signal_1_fully,
        R::drawable::stat_sys_signal_2_fully,
        R::drawable::stat_sys_signal_3_fully,
        R::drawable::stat_sys_signal_4_fully
    }
};

const Int32 WimaxIcons::WIMAX_DISCONNECTED = R::drawable::stat_sys_signal_0;

const Int32 WimaxIcons::WIMAX_IDLE = R::drawable::stat_sys_signal_0;

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
