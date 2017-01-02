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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIMAXICONS_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIMAXICONS_H__

#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class WimaxIcons
{
public:
    static const Int32 WIMAX_SIGNAL_STRENGTH[2][5]; // = TelephonyIcons.DATA_SIGNAL_STRENGTH;

    static const Int32 WIMAX_DISCONNECTED; //WIMAX_SIGNAL_STRENGTH[0][0];

    static const Int32 WIMAX_IDLE;// = WIMAX_DISCONNECTED XXX: unclear if we need a different icon

private:
    WimaxIcons();
    ~WimaxIcons();
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIMAXICONS_H__
