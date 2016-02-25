
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

    static const Int32 DATA_SIGNAL_STRENGTH[2][5]; // TELEPHONY_SIGNAL_STRENGTH

    //***** Data connection icons

    //GSM/UMTS
    //
    static const Int32 DATA_G[2][4];

    static const Int32 DATA_3G[2][4];

    static const Int32 DATA_E[2][4];

    //3.5G
    static const Int32 DATA_H[2][4];

    //CDMA
    // Use 3G icons for EVDO data and 1x icons for 1XRTT data
    static const Int32 DATA_1X[2][4];

    // LTE and eHRPD
    static const Int32 DATA_4G[2][4];

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
