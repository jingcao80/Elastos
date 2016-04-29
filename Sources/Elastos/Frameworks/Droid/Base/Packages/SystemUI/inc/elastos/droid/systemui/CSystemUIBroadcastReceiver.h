
#ifndef  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIBROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_CSystemUIBroadcastReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CSystemUIApplication;

/**
 * Application class for SystemUI.
 */
CarClass(CSystemUIBroadcastReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ ISystemUIApplication* host);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CSystemUIApplication* mHost;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIBROADCASTRECEIVER_H__
