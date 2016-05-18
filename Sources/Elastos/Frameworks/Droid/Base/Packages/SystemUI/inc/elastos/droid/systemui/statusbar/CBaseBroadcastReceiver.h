
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBASEBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBASEBROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CBaseBroadcastReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class BaseStatusBar;

/**
 * The view representing the separation between important and less important notifications
 */
CarClass(CBaseBroadcastReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBaseStatusBar* host);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    BaseStatusBar* mHost;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBASEBROADCASTRECEIVER_H__
