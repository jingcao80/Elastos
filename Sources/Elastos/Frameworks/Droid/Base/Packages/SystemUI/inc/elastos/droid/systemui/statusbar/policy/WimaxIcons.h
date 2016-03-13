
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
