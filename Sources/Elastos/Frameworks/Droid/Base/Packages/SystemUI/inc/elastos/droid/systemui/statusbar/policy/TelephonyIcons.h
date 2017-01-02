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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TELEPHONEYICONS_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TELEPHONEYICONS_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class TelephonyIcons
{
public:
    //***** Signal strength icons

    //GSM/UMTS
    //
    static const Int32 TELEPHONY_SIGNAL_STRENGTH[2][5];

    static const Int32 QS_TELEPHONY_SIGNAL_STRENGTH[2][5];

    static const Int32 TELEPHONY_SIGNAL_STRENGTH_ROAMING[2][5];

    static const Int32 QS_DATA_R[2];

    static const Int32 DATA_SIGNAL_STRENGTH[2][5]; // TELEPHONY_SIGNAL_STRENGTH

    //***** Data connection icons

    //GSM/UMTS
    //
    static const Int32 DATA_G[2][4];

    static const Int32 QS_DATA_G[2];

    static const Int32 DATA_3G[2][4];

    static const Int32 QS_DATA_3G[2];

    static const Int32 DATA_E[2][4];

    static const Int32 QS_DATA_E[2];

    //3.5G
    static const Int32 DATA_H[2][4];

    static const Int32 QS_DATA_H[2];

    //CDMA
    // Use 3G icons for EVDO data and 1x icons for 1XRTT data
    static const Int32 DATA_1X[2][4];

    static const Int32 QS_DATA_1X[2];

    // LTE and eHRPD
    static const Int32 DATA_4G[2][4];

    static const Int32 QS_DATA_4G[2];

    // LTE branded "LTE"
    static const Int32 DATA_LTE[2][4];

    static const Int32 QS_DATA_LTE[2];

    static const Int32 FLIGHT_MODE_ICON;
    static const Int32 ROAMING_ICON;
    static const Int32 ICON_LTE;
    static const Int32 ICON_3G;
    static const Int32 ICON_4G;
    static const Int32 ICON_1X;

    static const Int32 QS_ICON_LTE;
    static const Int32 QS_ICON_3G;
    static const Int32 QS_ICON_4G;
    static const Int32 QS_ICON_1X;

private:
    TelephonyIcons();
    ~TelephonyIcons();
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TELEPHONEYICONS_H__
